#ifndef FOLLOWONFRAGMENT_H
#define FOLLOWONFRAGMENT_H

#include "TunnelFragment.h"

namespace i2pcpp {
	class FollowOnFragment : public TunnelFragment {
		public:
			FollowOnFragment(uint32_t msgId, uint8_t n);

			void setLast(bool isLast);

			ByteArray compile() const;

		private:
			uint8_t headerSize() const;

			uint8_t m_fragNum;
			bool m_isLast = false;
	};
}

#endif
