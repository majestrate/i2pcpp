#include "RouterIdentity.h"

#include <algorithm>

#include <botan/pipe.h>
#include <botan/lookup.h>

#include <string>

namespace i2pcpp {
	RouterIdentity::RouterIdentity(ByteArray const &encryptionKey, ByteArray const &signingKey, Certificate const &certificate) : m_certificate(certificate)
	{
		copy(encryptionKey.begin(), encryptionKey.end(), m_encryptionKey.begin());
		copy(signingKey.begin(), signingKey.end(), m_signingKey.begin());
	}

	RouterIdentity::RouterIdentity(ByteArray::const_iterator &idItr)
	{
		copy(idItr, idItr + 256, m_encryptionKey.begin()), idItr += 256;
		copy(idItr, idItr + 128, m_signingKey.begin()), idItr += 128;
		m_certificate = Certificate(idItr);
	}

	ByteArray RouterIdentity::getBytes() const
	{
		ByteArray b;

		const ByteArray&& cert = m_certificate.getBytes();

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

		hashPipe.write(getBytes());

		hashPipe.end_msg();

		RouterHash hash;
		hashPipe.read(hash.data(), 32);

		return hash;
	}

	std::string RouterIdentity::getHashEncoded() const
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"), new Botan::Base64_Encoder);

		hashPipe.start_msg();

		hashPipe.write(getBytes());

		hashPipe.end_msg();

		std::string encoded = hashPipe.read_all_as_string(0);
		replace(encoded.begin(), encoded.end(), '+', '-');
		replace(encoded.begin(), encoded.end(), '/', '~');

		return encoded;
	}
}
