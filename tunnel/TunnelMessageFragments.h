#ifndef TUNNELMESSAGEFRAGMENTS_H
#define TUNNELMESSAGEFRAGMENTS_H

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	class TunnelMessageFragments {
		public:
			TunnelMessageFragments();
			TunnelMessageFragments(uint32_t const tunnelId, ByteArray const &data);

		private:
			uint32_t m_tunnelId;
	};
}

#endif
