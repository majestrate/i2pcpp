#ifndef FOLLOWONFRAGMENT_H
#define FOLLOWONFRAGMENT_H

#include "Fragment.h"

namespace i2pcpp {
	class FollowOnFragment : public Fragment {
		public:
			FollowOnFragment(uint32_t msgId, uint8_t n);

			void setLast(bool isLast);
			bool isLast() const;

			ByteArray compile() const;

			static FollowOnFragment parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

		private:
			uint8_t headerSize() const;

			uint8_t m_fragNum;
			bool m_isLast = false;
	};

	typedef std::unique_ptr<FollowOnFragment> FollowOnFragmentPtr;
}

#endif
