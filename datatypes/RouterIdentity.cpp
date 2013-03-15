#include "RouterIdentity.h"

#include <algorithm>

#include <botan/pipe.h>
#include <botan/lookup.h>

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	RouterIdentity::RouterIdentity() {}

	RouterIdentity::RouterIdentity(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		if((end - begin) < 256 + 128) throw FormattingError();
		copy(begin, begin + 256, m_encryptionKey.begin()), begin += 256;
		copy(begin, begin + 128, m_signingKey.begin()), begin += 128;
		m_certificate = Certificate(begin, end);
	}

	RouterIdentity::RouterIdentity(ByteArray const &encryptionKey, ByteArray const &signingKey, Certificate const &certificate) :
		m_certificate(certificate)
	{
		copy(encryptionKey.begin(), encryptionKey.end(), m_encryptionKey.begin());
		copy(signingKey.begin(), signingKey.end(), m_signingKey.begin());
	}

	ByteArray RouterIdentity::serialize() const
	{
		ByteArray b;

		const ByteArray&& cert = m_certificate.serialize();

		b.insert(b.end(), m_encryptionKey.begin(), m_encryptionKey.end());
		b.insert(b.end(), m_signingKey.begin(), m_signingKey.end());
		b.insert(b.end(), cert.begin(), cert.end());

		return b;
	}

	ByteArray RouterIdentity::getEncryptionKey() const 
	{
		ByteArray b(256);
		copy(m_encryptionKey.begin(), m_encryptionKey.end(), b.begin());
		return b;
	}

	ByteArray RouterIdentity::getSigningKey() const
	{
		ByteArray b(128);
		copy(m_signingKey.begin(), m_signingKey.end(), b.begin());
		return b;
	}

	RouterHash RouterIdentity::getHash() const
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(serialize());

		hashPipe.end_msg();

		RouterHash hash;
		hashPipe.read(hash.data(), 32);

		return hash;
	}

	std::string RouterIdentity::getHashEncoded() const
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"), new Botan::Base64_Encoder);

		hashPipe.start_msg();

		hashPipe.write(serialize());

		hashPipe.end_msg();

		std::string encoded = hashPipe.read_all_as_string(0);
		replace(encoded.begin(), encoded.end(), '+', '-');
		replace(encoded.begin(), encoded.end(), '/', '~');

		return encoded;
	}

	const Certificate& RouterIdentity::getCertificate() const
	{
		return m_certificate;
	}
}
