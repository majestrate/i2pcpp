#include "OutboundMessageState.h"

#include <botan/auto_rng.h>

#include "../../Log.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageState::OutboundMessageState(ByteArray const &data) :
			m_data(data)
		{
			Botan::AutoSeeded_RNG rng;

			rng.randomize((unsigned char *)&m_msgId, sizeof(m_msgId));
		}

		void OutboundMessageState::fragment()
		{
			auto dataItr = m_data.cbegin();
			auto end = m_data.cend();

			if(m_data.size() > 16383 * 127) { /* TODO Exception */ }

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

			m_states.resize(m_fragments.size());
		}

		const OutboundMessageState::FragmentPtr OutboundMessageState::getNextFragment()
		{
			if(!m_fragments.size())
				fragment();

			uint8_t nextFrag = m_states.getNextA();
			if(nextFrag >= m_fragments.size()) return OutboundMessageState::FragmentPtr();

			return m_fragments[nextFrag];
		}

		const OutboundMessageState::FragmentPtr OutboundMessageState::getFragment(const uint8_t fragNum) const
		{
			return m_fragments[fragNum];
		}

		void OutboundMessageState::markFragmentSent(const uint8_t fragNum)
		{
			m_states.markA(fragNum);
		}

		void OutboundMessageState::markFragmentAckd(const uint8_t fragNum)
		{
			m_states.markB(fragNum);
		}

		bool OutboundMessageState::allFragmentsSent() const
		{
			return m_states.allA();
		}

		bool OutboundMessageState::allFragmentsAckd() const
		{
			return m_states.allB();
		}
	}
}
