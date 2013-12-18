#include "FragmentState.h"

namespace i2pcpp {
	void FragmentState::setFirstFragment(std::unique_ptr<FirstFragment> ff)
	{
		if(!m_firstFragment)
			m_firstFragment = std::move(ff);
		else
			throw std::runtime_error("multiple first fragments in state");
	}

	void FragmentState::addFollowOnFragment(FollowOnFragment fof)
	{
		uint8_t thisFragNum = fof.getFragNum();
		if(fof.isLast()) {
			if(m_lastFragNum)
				throw std::runtime_error("multiple last fragments with the same fragment number in state");
				m_lastFragNum = thisFragNum;
		}

		if(std::none_of(m_followOnFragments.cbegin(), m_followOnFragments.cend(), [thisFragNum](const FollowOnFragment &x) { return x.getFragNum() == thisFragNum; }))
			m_followOnFragments.push_back(std::move(fof));
	}

	bool FragmentState::isComplete() const
	{
		// We didn't get the first fragment yet
		if(!m_firstFragment)
			return false;

		// We got the first fragment, and we aren't waiting for anything else
		if(!m_firstFragment->isFragmented())
			return true;

		// We ARE waiting for something else, but we haven't gotten anything yet
		if(m_lastFragNum == 0)
			return false;

		// We don't have everything yet and are expecting more
		return (m_followOnFragments.size() == m_lastFragNum);
	}

	ByteArray FragmentState::compile()
	{
		m_followOnFragments.sort([](const FollowOnFragment &f1, const FollowOnFragment &f2) { return f1.getFragNum() < f2.getFragNum(); } );

		ByteArray ret = m_firstFragment->getPayload();
		for(auto& f: m_followOnFragments) {
			ByteArray p = f.getPayload();
			ret.insert(ret.end(), p.cbegin(), p.cend());
		}

		return ret;
	}

	const std::unique_ptr<FirstFragment>& FragmentState::getFirstFragment() const
	{
		return m_firstFragment;
	}
}
