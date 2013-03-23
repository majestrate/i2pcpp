#include "InboundMessageFragments.h"

#include <string>

#include <botan/pipe.h>
#include <botan/filters.h>

#include "../UDPTransport.h"

#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageFragments::InboundMessageFragments(UDPTransport &transport) :
			m_transport(transport) {}

		void InboundMessageFragments::receiveData(PeerStatePtr const &ps, ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			I2P_LOG_TAG(m_transport.getLogger(), "IMF");

			std::bitset<8> flag = *(begin++);

			if(flag[7]) {
				// TODO
				unsigned char numAcks = *(begin++);
				while(numAcks--) { begin += 4; }
			}

			if(flag[6]) {
				// TODO
				unsigned char numFields = *(begin++);
				while(numFields--) {
					begin += 4;
					while(*(begin++) & 0x80);
				}	
			}

			unsigned char numFragments = *(begin++);
			BOOST_LOG_SEV(m_transport.getLogger(), debug) << "number of fragments: " << std::to_string(numFragments);

			for(int i = 0; i < numFragments; i++) {
				uint32_t msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "fragment[" << i << "] message id: " << std::hex << msgId << std::dec;

				uint32_t fragInfo = (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);

				uint16_t fragNum = fragInfo >> 17;
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "fragment[" << i << "] fragment #: " << fragNum;

				bool isLast = (fragInfo & 0x010000);
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "fragment[" << i << "] isLast: " << isLast;

				uint16_t fragSize = fragInfo & ((1 << 14) - 1);
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "fragment[" << i << "] size: " << fragSize;

				ByteArray fragData(begin, begin + fragSize);
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "fragment[" << i << "] data: " << std::string(fragData.cbegin(), fragData.cend());

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
