#ifndef FRAGMENTSTATE_H
#define FRAGMENTSTATE_H

#include <list>

#include "FirstFragment.h"
#include "FollowOnFragment.h"

namespace i2pcpp {
	class FragmentState {
		public:
			void setFirstFragment(std::unique_ptr<FirstFragment> ff);
			void addFollowOnFragment(FollowOnFragment fof);

			bool isComplete() const;
			ByteArray compile();
			const std::unique_ptr<FirstFragment>& getFirstFragment() const;

		private:
			uint8_t m_lastFragNum = 0;

			std::unique_ptr<FirstFragment> m_firstFragment = nullptr;
			std::list<FollowOnFragment> m_followOnFragments;
	};
}

#endif
