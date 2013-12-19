#include "OutboundMessageState.h"

#include "../../Log.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageState::OutboundMessageState(uint32_t msgId, ByteArray const &data) :
			m_msgId(msgId),
			m_data(data) {}

		void OutboundMessageState::fragment()
		{
			constexpr size_t maxFragmentSize = 1424;

			auto dataItr = m_data.cbegin();
			auto end = m_data.cend();

			if(m_data.size() > 16383 * 127)
				throw std::runtime_error("Outbound packet too large");

			size_t step, i = 0;
			while(dataItr < end) {
				step = std::min((size_t)(end - dataItr), maxFragmentSize);

				auto f = std::make_shared<PacketBuilder::Fragment>();
				f->msgId = m_msgId;
				f->fragNum = i++;
				f->isLast = (step < maxFragmentSize);
				f->data = ByteArray(dataItr, dataItr + step);

				m_fragments.push_back(f);

				dataItr += step;
			}

			m_fragmentStates.resize(m_fragments.size() * 2);
		}

		const PacketBuilder::FragmentPtr OutboundMessageState::getNextFragment()
		{
			if(!m_fragments.size())
				fragment();

			unsigned char i = 0, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			if(i >= size) return PacketBuilder::FragmentPtr();

			return m_fragments[i / 2];
		}

		const PacketBuilder::FragmentPtr OutboundMessageState::getNextUnackdFragment() const
		{
			unsigned char i = 1, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			if(i >= size) return PacketBuilder::FragmentPtr();

			return m_fragments[i / 2];
		}

		void OutboundMessageState::markFragmentSent(const uint8_t fragNum)
		{
			if((fragNum * 2) >= m_fragmentStates.size()) return;

			m_fragmentStates[fragNum * 2] = 1;
		}

		void OutboundMessageState::markFragmentAckd(const uint8_t fragNum)
		{
			if((fragNum * 2) >= m_fragmentStates.size()) return;

			m_fragmentStates[(fragNum * 2) + 1] = 1;
		}

		bool OutboundMessageState::allFragmentsSent() const
		{
			unsigned char i = 0, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			return (i >= size);
		}

		bool OutboundMessageState::allFragmentsAckd() const
		{
			unsigned char i = 1, size = m_fragmentStates.size();
			while(i < size && m_fragmentStates.test(i)) i += 2;

			return (i >= size);
		}

		uint32_t OutboundMessageState::getMsgId() const
		{
			return m_msgId;
		}

		void OutboundMessageState::incrementTries()
		{
			m_tries++;
		}

		uint8_t OutboundMessageState::getTries() const
		{
			return m_tries;
		}
	}
}
