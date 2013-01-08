#include "OutboundMessageState.h"

#include <botan/auto_rng.h>

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		OutboundMessageState::OutboundMessageState(I2NP::MessagePtr const &msg) : m_msg(msg)
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

			size_t step, i = 0;
			while(dataItr < end) {
				step = std::min((size_t)(end - dataItr), (size_t)512);

				FragmentPtr f(new Fragment());
				f->msgId = m_msgId;
				f->fragNum = i++;
				f->isLast = (step < 512);
				f->data = ByteArray(dataItr, dataItr + step);

				m_fragments.push_back(f);

				dataItr += step;
			}

			m_fragmentStates.resize(m_fragments.size() * 2);
		}

		const OutboundMessageState::FragmentPtr OutboundMessageState::getNextFragment()
		{
			if(!m_fragments.size())
				fragment();

			unsigned char i = 0, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i))	i += 2;

			if(i >= size) return OutboundMessageState::FragmentPtr();

			return m_fragments[i / 2];
		}

		const OutboundMessageState::FragmentPtr OutboundMessageState::getFragment(const unsigned char fragNum) const
		{
			return m_fragments[fragNum];
		}

		void OutboundMessageState::markFragmentSent(const unsigned char fragNum)
		{
			m_fragmentStates[fragNum * 2] = 1;
		}

		void OutboundMessageState::markFragmentReceived(const unsigned char fragNum)
		{
			m_fragmentStates[(fragNum * 2) + 1] = 1;
		}

		bool OutboundMessageState::allFragmentsSent() const
		{
			unsigned char i = 0, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			return (i >= size);
		}

		bool OutboundMessageState::allFragmentsReceived() const
		{
			unsigned char i = 1, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			return (i >= size);
		}
	}
}
