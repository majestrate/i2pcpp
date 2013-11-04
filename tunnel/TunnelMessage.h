#ifndef TUNNELMESSAGE_H
#define TUNNELMESSAGE_H

#include <array>

#include <botan/symkey.h>

#include "datatypes/StaticByteArray.h"

namespace i2pcpp {
	class TunnelMessage {
		public:
			TunnelMessage(StaticByteArray<1024, true> const &data);

			void encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey);
			ByteArray compile() const;

		private:
			StaticByteArray<16, true> m_iv;
			StaticByteArray<1008, true> m_data;
	};
}

#endif
