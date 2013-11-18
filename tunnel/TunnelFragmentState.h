#ifndef TUNNELFRAGMENTSTATE_H
#define TUNNELFRAGMENTSTATE_H

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	class TunnelFragmentState {
		public:
			TunnelFragmentState(uint32_t const msgId);

			void addFragment(ByteArray const &frag);

		private:
			uint32_t m_msgId;
	};
}

#endif
