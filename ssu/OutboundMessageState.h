#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <mutex>

#include "../datatypes/Endpoint.h"
#include "../i2np/Message.h"

namespace i2pcpp {
	namespace SSU {
		class OutboundMessageState {
			public:
				OutboundMessageState(Endpoint const &ep, I2NP::MessagePtr const &msg) : m_endpoint(ep), m_msg(msg) {}

				//void fragment
				std::mutex& getMutex() { return m_mutex; }

			private:
				Endpoint m_endpoint;
				I2NP::MessagePtr m_msg;
				std::vector<ByteArray> m_fragments;

				std::mutex m_mutex;
		};

		typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
	}
}

#endif
