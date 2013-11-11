#ifndef TUNNELMESSAGE_H
#define TUNNELMESSAGE_H

#include <array>
#include <list>

#include <botan/symkey.h>

#include "../datatypes/ByteArray.h"
#include "../datatypes/StaticByteArray.h"
#include "../i2np/Message.h"

namespace i2pcpp {
	class TunnelMessage {
		public:
			TunnelMessage() {}
			TunnelMessage(StaticByteArray<1024, true> const &data);
			TunnelMessage(std::list<ByteArrayPtr> const &fragments);

			static std::list<ByteArrayPtr> fragment(I2NP::MessagePtr const &msg);
			void encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey);
			ByteArray compile() const;

		private:
			StaticByteArray<16, true> m_iv;
			StaticByteArray<1008, true> m_data;
	};
}

#endif
