#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <forward_list>


#include "../i2np/Message.h"
#include "../datatypes/ByteArray.h"

#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class OutboundMessageState {
			public:
				struct Fragment {
					bool isLast;
					uint32_t msgId;
					uint8_t fragNum;
					ByteArray data;
				};

				OutboundMessageState(PeerStatePtr const &ps, I2NP::MessagePtr const &msg);

				void fragment();
				const PeerStatePtr getPeerState() const { return m_state; }
				const std::forward_list<Fragment> getFragments() const;

			private:
				PeerStatePtr m_state;
				uint32_t m_msgId;
				I2NP::MessagePtr m_msg;
				std::vector<ByteArray> m_fragments;
		};

		typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
	}
}

#endif
