#include "InboundMessageFragments.h"

#include <botan/pipe.h>
#include <botan/filters.h>

using namespace Botan;

namespace i2pcpp {
	namespace SSU {
		void InboundMessageFragments::receiveData(ByteArray::const_iterator &dataItr, PeerStatePtr const &ps)
		{
			bitset<8> flag = *(dataItr++);
			cerr << "Data flag: " << flag << "\n";

			if(flag[7]) {
				// Handle explicit ACKs
			}

			if(flag[6]) {
				// Handle ACK bitfields
			}

			unsigned char numFragments = *(dataItr++);
			cerr << "Number of fragments: " << to_string(numFragments) << "\n";

			Pipe hexPipe(new Hex_Encoder, new DataSink_Stream(cerr));

			for(int i = 0; i < numFragments; i++) {
				unsigned long msgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
				cerr << "Fragment[" << i << "] Message ID: " << msgId << "\n";

				unsigned long fragInfo = (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

				unsigned short fragNum = fragInfo >> 17;
				cerr << "Fragment[" << i << "] fragment #: " << fragNum << "\n";

				bool isLast = (fragInfo & 0x010000) >> 16;
				cerr << "Fragment[" << i << "] isLast: " << isLast << "\n";

				unsigned short fragSize = fragInfo & 0x003fff;
				cerr << "Fragment[" << i << "] size: " << fragSize << "\n";

				cerr << "Fragment[" << i << "] data: ";
				ByteArray fragData(dataItr, dataItr + fragSize);
				hexPipe.start_msg(); hexPipe.write(fragData); hexPipe.end_msg();
				cerr << "\n";

				auto fragDataItr = fragData.cbegin();

				InboundMessageStatePtr ims = ps->getInboundMessageState(msgId);
				if(!ims) {
					ims = InboundMessageStatePtr(new InboundMessageState(msgId));
					ps->addInboundMessageState(ims);
				}

				ims->addFragment(fragNum, fragData);

				cerr << "\n";
			}
		}
	}
}
