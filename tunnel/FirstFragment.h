#ifndef FIRSTFRAGMENT_H
#define FIRSTFRAGMENT_H

#include "Fragment.h"

#include "../datatypes/RouterHash.h"

namespace i2pcpp {
	class FirstFragment : public Fragment {
		public:
			enum class DeliveryMode {
				LOCAL = 0x00,
				TUNNEL = 0x01,
				ROUTER = 0x02
			};

			FirstFragment() = default;

			ByteArray compile() const;

			bool mustFragment(uint16_t desiredSize, uint16_t max) const;
			void setFragmented(bool f);
			bool isFragmented() const;

			static FirstFragment parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

		private:
			uint8_t headerSize() const;

			DeliveryMode m_mode = DeliveryMode::LOCAL;
			bool m_fragmented = false;
			uint32_t m_tunnelId;
			RouterHash m_toHash;
	};
}

#endif
