#include "InboundMessageFragments.h"

#include <string>
#include <bitset>
#include <iomanip>

#include <botan/pipe.h>
#include <botan/filters.h>

#include "../../exceptions/FormattingError.h"

#include "../UDPTransport.h"

#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageFragments::InboundMessageFragments(UDPTransport &transport) :
			m_transport(transport),
			m_log(boost::log::keywords::channel = "IMF") {}

		void InboundMessageFragments::receiveData(PeerStatePtr const &ps, ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			if((end - begin) < 1) throw FormattingError();
			std::bitset<8> flag = *(begin++);

			if(flag[7]) {
				if((end - begin) < 1) throw FormattingError();
				unsigned char numAcks = *(begin++);
				if((end - begin) < (numAcks * 4)) throw FormattingError();

				while(numAcks--) {
					uint32_t msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
					std::lock_guard<std::mutex> lock(ps->getMutex());

					ps->delOutboundMessageState(msgId);
				}
			}

			if(flag[6]) {
				std::lock_guard<std::mutex> lock(ps->getMutex());

				unsigned char numFields = *(begin++);
				while(numFields--) {
					uint32_t msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);

					OutboundMessageStatePtr oms = ps->getOutboundMessageState(msgId);

					uint8_t byteNum = 0;
					do {
						uint8_t byte = *begin;
						for(int i = 6, j = 0; i >= 0; i--, j++) {
							if(byte & (1 << i)) {
								if(oms)
									oms->markFragmentAckd((byteNum * 7) + j);
							}
						}

						byteNum++;
					} while(*(begin++) & (1 << 7));

					if(oms && oms->allFragmentsAckd())
						ps->delOutboundMessageState(msgId);
				}
			}

			if((end - begin) < 1) throw FormattingError();
			unsigned char numFragments = *(begin++);
			I2P_LOG(m_log, debug) << "number of fragments: " << std::to_string(numFragments);

			// keep alive
			if (numFragments == 0) {
				I2P_LOG(m_log, debug) << "got keepalive";
			}

			for(int i = 0; i < numFragments; i++) {
				if((end - begin) < 7) throw FormattingError();
				uint32_t msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
				I2P_LOG(m_log, debug) << "fragment[" << i << "] message id: " << std::hex << msgId << std::dec;

				uint32_t fragInfo = (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);

				uint16_t fragNum = fragInfo >> 17;
				I2P_LOG(m_log, debug) << "fragment[" << i << "] fragment #: " << fragNum;

				bool isLast = (fragInfo & 0x010000);
				I2P_LOG(m_log, debug) << "fragment[" << i << "] isLast: " << isLast;

				uint16_t fragSize = fragInfo & ((1 << 14) - 1);
				I2P_LOG(m_log, debug) << "fragment[" << i << "] size: " << fragSize;

				if((end - begin) < fragSize) throw FormattingError();
				ByteArray fragData(begin, begin + fragSize);
				std::stringstream s;
				s << std::setw(2) << std::setfill('0') << std::hex;
				for(auto c: fragData) s << (int)c;
				I2P_LOG(m_log, debug) << "fragment[" << i << "] data: " << s.str();

				std::lock_guard<std::mutex> lock(ps->getMutex());
				InboundMessageStatePtr ims = ps->getInboundMessageState(msgId);
				if(!ims) {
					ims = std::make_shared<InboundMessageState>(ps->getIdentity().getHash(), msgId);
					ps->addInboundMessageState(ims);
				}

				// TODO Should throw an exception on error
				ims->addFragment(fragNum, fragData, isLast);

				if(ims->allFragmentsReceived()) {
					const ByteArray data = ims->assemble();
					if(data.size())
						m_transport.post(boost::bind(boost::ref(m_transport.m_receivedSignal), ims->getRouterHash(), data));
				}
			}
		}
	}
}
