#include "InboundMessageFragments.h"

#include <string>

#include <botan/pipe.h>
#include <botan/filters.h>

namespace i2pcpp {
	namespace SSU {
		void InboundMessageFragments::begin()
		{
			m_messageReceiverThread = std::thread(&InboundMessageFragments::startMessageReceiver, this);
		}

		void InboundMessageFragments::join()
		{
			m_messageReceiver.notify();
			m_messageReceiverThread.join();
		}

		void InboundMessageFragments::receiveData(ByteArray::const_iterator &dataItr, PeerStatePtr const &ps)
		{
			std::bitset<8> flag = *(dataItr++);
			std::cerr << "Data flag: " << flag << "\n";

			if(flag[7]) {
				// Handle explicit ACKs
			}

			if(flag[6]) {
				// Handle ACK bitfields
			}

			unsigned char numFragments = *(dataItr++);
			std::cerr << "Number of fragments: " << std::to_string(numFragments) << "\n";

			Botan::Pipe hexPipe(new Botan::Hex_Encoder, new Botan::DataSink_Stream(std::cerr));

			for(int i = 0; i < numFragments; i++) {
				unsigned long msgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
				std::cerr << "Fragment[" << i << "] Message ID: " << msgId << "\n";

				unsigned long fragInfo = (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

				unsigned short fragNum = fragInfo >> 17;
				std::cerr << "Fragment[" << i << "] fragment #: " << fragNum << "\n";

				bool isLast = (fragInfo & 0x010000) >> 16;
				std::cerr << "Fragment[" << i << "] isLast: " << isLast << "\n";

				unsigned short fragSize = fragInfo & 0x003fff;
				std::cerr << "Fragment[" << i << "] size: " << fragSize << "\n";

				std::cerr << "Fragment[" << i << "] data: ";
				ByteArray fragData(dataItr, dataItr + fragSize);
				hexPipe.start_msg(); hexPipe.write(fragData); hexPipe.end_msg();
				std::cerr << "\n";

				InboundMessageStatePtr ims = ps->getInboundMessageState(msgId);
				if(!ims) {
					ims = InboundMessageStatePtr(new InboundMessageState(ps->getIdentity().getHash(), msgId));
					ps->addInboundMessageState(ims);
				}

				std::lock_guard<std::mutex> lock(ims->getMutex());
				bool fragOK = ims->addFragment(fragNum, fragData, isLast);

				if(!fragOK)
					std::cerr << "InboundMessageFragments: BAD FRAGMENT!\n";

				if(ims->isComplete())
					m_messageReceiver.addMessage(ims);

				std::cerr << "\n";
			}
		}

		void InboundMessageFragments::startMessageReceiver()
		{
			try {
				m_messageReceiver.run();
			} catch(std::exception &e) { // TODO Do this for real
				std::cerr << "MessageReceiver exception: " << e.what() << "\n";
			}
		}
	}
}
