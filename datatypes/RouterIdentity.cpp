#include "RouterIdentity.h"

#include <algorithm>

#include <botan/pipe.h>
#include <botan/lookup.h>

#include <string>

namespace i2pcpp {
	RouterIdentity::RouterIdentity(ByteArray const &publicKey, ByteArray const &signingKey, Certificate const &certificate) : m_certificate(certificate)
	{
		copy(publicKey.begin(), publicKey.end(), m_publicKey.begin());
		copy(signingKey.begin(), signingKey.end(), m_signingKey.begin());
	}

	RouterIdentity::RouterIdentity(ByteArray::const_iterator &idItr) : m_certificate(Certificate(idItr))
	{
		copy(idItr, idItr + 256, m_publicKey.begin()), idItr += 256;
		copy(idItr, idItr + 128, m_signingKey.begin()), idItr += 128;
	}

	ByteArray RouterIdentity::getBytes() const
	{
		ByteArray b, cert;

		cert = m_certificate.getBytes();

		b.insert(b.end(), m_publicKey.begin(), m_publicKey.end());
		b.insert(b.end(), m_signingKey.begin(), m_signingKey.end());
		b.insert(b.end(), cert.begin(), cert.end());

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
