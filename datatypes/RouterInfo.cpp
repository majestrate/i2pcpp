#include "RouterInfo.h"

#include <botan/pipe.h>
#include <botan/pubkey.h>
#include <botan/pk_filts.h>

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

	bool RouterInfo::verifySignature(RouterContext const &ctx) const
	{
		ByteArray dsaKeyBytes = m_identity.getSigningKey();
		const Botan::DL_Group& group = ctx.getDSAParameters();
		Botan::DSA_PublicKey dsaKey(group, Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));
		Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Verifier_Filter(new Botan::PK_Verifier(dsaKey, "Raw"), m_signature.data(), m_signature.size()));
		sigPipe.start_msg();
		sigPipe.write(getSignedBytes());
		sigPipe.end_msg();

		unsigned char verified;
		sigPipe.read(&verified, 1);

		return verified;
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

		ByteArray idBytes = m_identity.getBytes();
		ByteArray pubBytes = m_published.getBytes();
		ByteArray optBytes = m_options.getBytes();

		b.insert(b.end(), idBytes.cbegin(), idBytes.cend());

		b.insert(b.end(), pubBytes.cbegin(), pubBytes.cend());

		b.insert(b.end(), m_addresses.size());
		for(auto a: m_addresses) {
			ByteArray addr = a.getBytes();
			b.insert(b.end(), addr.cbegin(), addr.cend());
		}

		b.insert(b.end(), 0x00); // Unused peer_size

		b.insert(b.end(), optBytes.cbegin(), optBytes.cend());

		return b;
	}

}
