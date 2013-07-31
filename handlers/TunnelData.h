#ifndef HANDLERSTUNNELDATA_H
#define HANDLERSTUNNELDATA_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class TunnelData : public Message {
			public:
				TunnelData(RouterContext &ctx);

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

			private:
				i2p_logger_mt m_log;
		};
	}
}

#endif
