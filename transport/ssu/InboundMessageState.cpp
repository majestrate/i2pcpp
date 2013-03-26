#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageState::InboundMessageState(RouterHash const &rh, const uint32_t msgId) :
			m_routerHash(rh),
			m_msgId(msgId) {}

		void InboundMessageState::addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast)
		{
			if(m_gotLast && fragNum > m_lastFragment)
				return; // TODO Exception -- trying to give us a fragment greater than last

			if(fragNum < m_fragments.size() && m_fragments[fragNum])
				return; // TODO Exception -- already got thsi fragment

			if(isLast) {
				m_gotLast = true;
				m_lastFragment = fragNum;
			}

			if(m_fragments.size() < fragNum + 1)
				m_fragments.resize(fragNum + 1);

			auto f = std::make_shared<ByteArray>(data);
			m_fragments[fragNum] = f;

			m_byteTotal += data.size();
		}

		ByteArray InboundMessageState::assemble() const
		{
			ByteArray dst(m_byteTotal);

			auto itr = dst.begin();
			for(auto fp: m_fragments)
			{
				if(fp) {
					copy(fp->cbegin(), fp->cend(), itr);
					itr += fp->size();
				}
			}

			return dst;
		}

		RouterHash InboundMessageState::getRouterHash() const
		{
			return m_routerHash;
		}

		uint32_t InboundMessageState::getMsgId() const
		{
			return m_msgId;
		}

		bool InboundMessageState::allFragmentsReceived() const
		{
			if(!m_gotLast) return false;

			for(auto f: m_fragments)
				if(!f)
					return false;

			return true;
		}

		std::vector<bool> InboundMessageState::getFragmentsReceived() const
		{
			std::vector<bool> v(m_fragments.size());

			for(int i = 0; i < m_fragments.size(); i++)
				v[i] = (m_fragments[i] != ByteArrayPtr());

			return v;
		}
	}
}
