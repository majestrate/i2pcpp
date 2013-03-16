#include "RouterInfo.h"

#include <botan/pipe.h>
#include <botan/pubkey.h>
#include <botan/pk_filts.h>
#include <botan/auto_rng.h>

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	RouterInfo::RouterInfo() {}

	RouterInfo::RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature = ByteArray(40)) :
		m_identity(identity),
		m_published(published),
		m_options(options),
		m_signature(signature) {}

	RouterInfo::RouterInfo(ByteArrayConstItr &begin, ByteArrayConstItr end) :
		m_signature(40)
	{
		m_identity = RouterIdentity(begin, end);

		m_published = Date(begin, end);

		unsigned char size = *(begin++);
		for(int i = 0; i < size; i++)
			m_addresses.push_back(RouterAddress(begin, end));

		begin++; // unused peer_size

		m_options = Mapping(begin, end);

		if((end - begin) < 40) throw FormattingError();
		copy(end - 40, end, m_signature.begin());
	}

	ByteArray RouterInfo::serialize() const
	{
		ByteArray signedBytes = getSignedBytes();
		signedBytes.insert(signedBytes.end(), m_signature.begin(), m_signature.end());

		return signedBytes;
	}

	void RouterInfo::addAddress(RouterAddress const &address)
	{
		m_addresses.push_back(address);
	}

	bool RouterInfo::verifySignature(const Botan::DL_Group &dsaParameters) const
	{
		const ByteArray&& dsaKeyBytes = m_identity.getSigningKey();
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

	const RouterAddress& RouterInfo::getAddress(const int index) const
	{
		return m_addresses[index];
	}

	const RouterIdentity& RouterInfo::getIdentity() const
	{
		return m_identity;
	}

	const Date& RouterInfo::getPublished() const
	{
		return m_published;
	}

	const Mapping& RouterInfo::getOptions() const
	{
		return m_options;
	}

	const ByteArray& RouterInfo::getSignature() const
	{
		return m_signature;
	}

	const std::vector<RouterAddress>::const_iterator RouterInfo::begin() const
	{
		return m_addresses.cbegin();
	}

	const std::vector<RouterAddress>::const_iterator RouterInfo::end() const
	{
		return m_addresses.cend();
	}

	ByteArray RouterInfo::getSignedBytes() const
	{
		ByteArray b;

		const ByteArray&& idBytes = m_identity.serialize();
		const ByteArray&& pubBytes = m_published.serialize();
		const ByteArray&& optBytes = m_options.serialize();

		b.insert(b.end(), idBytes.cbegin(), idBytes.cend());

		b.insert(b.end(), pubBytes.cbegin(), pubBytes.cend());

		b.insert(b.end(), m_addresses.size());
		for(auto& a: m_addresses) {
			const ByteArray&& addr = a.serialize();
			b.insert(b.end(), addr.cbegin(), addr.cend());
		}

		b.insert(b.end(), 0x00); // Unused peer_size

		b.insert(b.end(), optBytes.cbegin(), optBytes.cend());

		return b;
	}

}
