#include "RouterInfo.h"

#include <botan/pipe.h>
#include <botan/pubkey.h>
#include <botan/pk_filts.h>
#include <botan/auto_rng.h>

namespace i2pcpp {
	RouterInfo::RouterInfo(ByteArray const &infoBytes) : m_signature(40)
	{
		auto infoItr = infoBytes.cbegin();

		m_identity = RouterIdentity(infoItr);

		m_published = Date(infoItr);

		unsigned char size = *(infoItr++);
		for(int i = 0; i < size; i++)
			m_addresses.push_back(RouterAddress(infoItr));

		infoItr++; // unused peer_size

		m_options = Mapping(infoItr);

		copy(infoItr, infoItr + 40, m_signature.begin());
	}

	ByteArray RouterInfo::getBytes() const
	{
		ByteArray signedBytes = getSignedBytes();
		signedBytes.insert(signedBytes.end(), m_signature.begin(), m_signature.end());

		return signedBytes;
	}

	bool RouterInfo::verifySignature(const Botan::DL_Group &dsaParameters) const
	{
		ByteArray dsaKeyBytes = m_identity.getSigningKey();
		Botan::DSA_PublicKey dsaKey(dsaParameters, Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));
		Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Verifier_Filter(new Botan::PK_Verifier(dsaKey, "Raw"), m_signature.data(), m_signature.size()));
		sigPipe.start_msg();
		sigPipe.write(getSignedBytes());
		sigPipe.end_msg();

		unsigned char verified;
		sigPipe.read(&verified, 1);

		return verified;
	}

	void RouterInfo::sign(const Botan::DSA_PrivateKey * const signingKey)
	{
		Botan::AutoSeeded_RNG rng;
		Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Signer_Filter(new Botan::PK_Signer(*signingKey, "Raw"), rng));

		sigPipe.start_msg();
		sigPipe.write(getSignedBytes());
		sigPipe.end_msg();

		sigPipe.read(m_signature.data(), 40);
	}

	ByteArray RouterInfo::calculateHash(ByteArray const &signedBytes) const
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(signedBytes);

		hashPipe.end_msg();

		ByteArray hash(32);
		hashPipe.read(hash.data(), 32);

		return hash;
	}

	ByteArray RouterInfo::getSignedBytes() const
	{
		ByteArray b;

		const ByteArray& idBytes = m_identity.getBytes();
		const ByteArray& pubBytes = m_published.getBytes();
		const ByteArray& optBytes = m_options.getBytes();

		b.insert(b.end(), idBytes.cbegin(), idBytes.cend());

		b.insert(b.end(), pubBytes.cbegin(), pubBytes.cend());

		b.insert(b.end(), m_addresses.size());
		for(auto a: m_addresses) {
			const ByteArray& addr = a.getBytes();
			b.insert(b.end(), addr.cbegin(), addr.cend());
		}

		b.insert(b.end(), 0x00); // Unused peer_size

		b.insert(b.end(), optBytes.cbegin(), optBytes.cend());

		return b;
	}

}
