#ifndef ROUTERIDENTITY_H
#define ROUTERIDENTITY_H

#include <array>

#include "Datatype.h"
#include "Certificate.h"
#include "ByteArray.h"
#include "RouterHash.h"

namespace i2pcpp {
	class RouterIdentity : public Datatype {
		public:
			RouterIdentity() {};
			RouterIdentity(ByteArray::const_iterator &idItr);
			RouterIdentity(ByteArray const &encryptionKey, ByteArray const &signingKey, Certificate const &certificate);

			ByteArray getBytes() const;
			RouterHash getHash() const;
			std::string getHashEncoded() const;
			ByteArray getEncryptionKey() const;
			ByteArray getSigningKey() const;
			const Certificate& getCertificate() const { return m_certificate; }

		private:
			std::array<unsigned char, 256> m_encryptionKey;
			std::array<unsigned char, 128> m_signingKey;
			Certificate m_certificate;
	};
}

#endif
