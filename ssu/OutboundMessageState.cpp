#include "OutboundMessageState.h"

#include <botan/auto_rng.h>

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		OutboundMessageState::OutboundMessageState(PeerStatePtr const &ps, I2NP::MessagePtr const &msg) : m_state(ps), m_msg(msg)
		{
			Botan::AutoSeeded_RNG rng;

			rng.randomize((unsigned char *)&m_msgId, sizeof(m_msgId));
		}

		void OutboundMessageState::fragment()
		{
			const ByteArray& data = m_msg->toBytes();
			auto dataItr = data.cbegin();
			auto end = data.cend();

			if(data.size() > 16383 * 127) { /* TODO Exception */ }

			size_t step;
			while(dataItr < end) {
				step = std::min((size_t)(end - dataItr), (size_t)512);

				ByteArray fragment(dataItr, dataItr + step);
				m_fragments.push_back(fragment);
				dataItr += step;
			}
		}

		const std::forward_list<OutboundMessageState::Fragment> OutboundMessageState::getFragments() const
		{
			std::forward_list<OutboundMessageState::Fragment> fragments;

			unsigned char size = m_fragments.size();
			for(unsigned char i = 0; i < size; i++) {
				Fragment f;

				f.msgId = m_msgId;

				f.fragNum = i;

				f.isLast = (i == (size - 1));

				f.data = m_fragments[i];

				fragments.push_front(f);
			}

			return fragments;
		}
	}
}
