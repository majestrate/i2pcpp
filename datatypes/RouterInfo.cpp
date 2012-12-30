#include "RouterInfo.h"

namespace i2pcpp {
	RouterInfo::RouterInfo(ByteArray const &infoBytes)
	{
		auto infoItr = infoBytes.cbegin();

		m_identity = RouterIdentity(infoItr);

		m_published = Date(infoItr);

		unsigned char size = *(infoItr++);
		for(int i = 0; i < size; i++)
			m_addresses.push_back(RouterAddress(infoItr));

		m_options = Mapping(infoItr);
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

		b.insert(b.end(), idBytes.begin(), idBytes.end());

		b.insert(b.end(), pubBytes.begin(), pubBytes.end());

		b.insert(b.end(), m_addresses.size());
		for(auto a: m_addresses) {
			ByteArray addr = a.getBytes();
			b.insert(b.end(), addr.begin(), addr.end());
		}

		b.insert(b.end(), 0x00);

		b.insert(b.end(), optBytes.begin(), optBytes.end());

		return b;
	}

}
