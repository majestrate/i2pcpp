#include "InboundMessageFragments.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <botan/pipe.h>
#include <botan/filters.h>

namespace i2pcpp {
	namespace SSU {
		void InboundMessageFragments::receiveData(PeerStatePtr const &ps, ByteArray::const_iterator &dataItr)
		{
			std::bitset<8> flag = *(dataItr++);
			std::cerr << "Data flag: " << flag << "\n";

			if(flag[7]) {
				// TODO
				unsigned char numAcks = *(dataItr++);
				while(numAcks--) { dataItr += 4; }
			}

			if(flag[6]) {
				// TODO
				unsigned char numFields = *(dataItr++);
				while(numFields--) {
					dataItr += 4;
					while(*(dataItr++) & 0x80);
				}	
			}

			unsigned char numFragments = *(dataItr++);
			std::cerr << "Number of fragments: " << std::to_string(numFragments) << "\n";

			for(int i = 0; i < numFragments; i++) {
				uint32_t msgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
				std::cerr << "Fragment[" << i << "] Message ID: " << std::hex << msgId << std::dec << "\n";

				uint32_t fragInfo = (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

				uint16_t fragNum = fragInfo >> 17;
				std::cerr << "Fragment[" << i << "] fragment #: " << fragNum << "\n";

				bool isLast = (fragInfo & 0x010000);
				std::cerr << "Fragment[" << i << "] isLast: " << isLast << "\n";

				uint16_t fragSize = fragInfo & ((1 << 14) - 1);
				std::cerr << "Fragment[" << i << "] size: " << fragSize << "\n";

				std::cerr << "Fragment[" << i << "] data: ";
				ByteArray fragData(dataItr, dataItr + fragSize);
				for(auto c: fragData) std::cerr << std::setw(2) << std::setfill('0') << std::hex << (int)c << std::setw(0) << std::dec;
				std::cerr << "\n";

				InboundMessageStatePtr ims = ps->getInboundMessageState(msgId);
				if(!ims) {
					ims = InboundMessageStatePtr(new InboundMessageState(ps->getIdentity().getHash(), msgId));
					ps->addInboundMessageState(ims);
				}

				// TODO Should throw an exception on error
				ims->addFragment(fragNum, fragData, isLast);

				if(*ims) {
					ps->delInboundMessageState(ims->getMsgId());
					m_messageReceiver.addMessage(ims);
				}

				std::cerr << "\n";
			}
		}
	}
}
