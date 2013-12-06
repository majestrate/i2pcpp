#ifndef FIRSTFRAGMENT_H
#define FIRSTFRAGMENT_H

#include "TunnelFragment.h"

namespace i2pcpp {
	class FirstFragment : public TunnelFragment {
		public:
			enum class DeliveryMode {
				LOCAL = 0x00,
				TUNNEL = 0x01,
				ROUTER = 0x02
			};

			FirstFragment();

			ByteArray compile() const;

			void setFragmented(bool f);

		private:
			uint8_t headerSize() const;

			DeliveryMode m_mode;
			bool m_fragmented;
	};
}

#endif
