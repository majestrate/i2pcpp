#ifndef TUNNELMESSAGE_H
#define TUNNELMESSAGE_H

#include <array>

#include <botan/symkey.h>

#include "datatypes/ByteArray.h"

namespace i2pcpp {
	class TunnelMessage {
		public:
			TunnelMessage(std::array<unsigned char, 1024> const &data);

			void encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey);
			ByteArray compile() const;

		private:
			std::array<unsigned char, 16> m_iv;
			std::array<unsigned char, 1008> m_data;
	};
}

#endif
