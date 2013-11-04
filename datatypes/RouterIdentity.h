#ifndef ROUTERIDENTITY_H
#define ROUTERIDENTITY_H

#include <array>

#include "Datatype.h"
#include "RouterHash.h"
#include "Certificate.h"

namespace i2pcpp {
	class RouterIdentity : public Datatype {
		public:
			RouterIdentity(ByteArrayConstItr &begin, ByteArrayConstItr end);
			RouterIdentity(ByteArray const &encryptionKey, ByteArray const &signingKey, Certificate const &certificate);

			ByteArray serialize() const;
			ByteArray getEncryptionKey() const;
			ByteArray getSigningKey() const;
			RouterHash getHash() const;
			const Certificate& getCertificate() const;

		private:
			std::array<unsigned char, 256> m_encryptionKey;
			std::array<unsigned char, 128> m_signingKey;
			Certificate m_certificate;

			mutable bool m_hashed = false;
			mutable RouterHash m_hash;
	};
}

#endif
