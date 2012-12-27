#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

namespace i2pcpp {
	namespace SSU {
		class PacketHandler;

		class MessageReceiver {
			public:
				MessageReceiver(PacketHandler &handler) : m_handler(handler) {}

				void run();

			private:
				PacketHandler& m_handler;
		};
	}
}

#endif
