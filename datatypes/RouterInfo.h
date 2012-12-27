#ifndef ROUTERINFO_H
#define ROUTERINFO_H

#include <array>
#include <list>

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/dsa.h>

#include "Datatype.h"
#include "Mapping.h"
#include "RouterAddress.h"
#include "RouterIdentity.h"
#include "Date.h"
#include "RouterHash.h"

using namespace std;
using namespace Botan;

namespace i2pcpp {
	class RouterInfo : public Datatype {
		public:
			RouterInfo() {}
			RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature) : m_identity(identity), m_published(published), m_options(options), m_signature(signature) {}

			RouterInfo(ByteArray const &infoBytes)
			{
				auto infoItr = infoBytes.begin();

				m_identity = RouterIdentity(infoItr);

				m_published = Date(infoItr);

				unsigned char size = *(infoItr++);
				for(int i = 0; i < size; i++)
					m_addresses.push_back(RouterAddress(infoItr));

				m_options = Mapping(infoItr);
			}

			ByteArray getBytes() const
			{
				ByteArray signedBytes = getSignedBytes();
				signedBytes.insert(signedBytes.end(), m_signature.begin(), m_signature.end());

				return signedBytes;
			}

			void addAddress(RouterAddress const &address)
			{
				m_addresses.push_back(address);
			}

			bool verifySignature(ByteArray const &signature)
			{
				ByteArray hash = calculateHash(getSignedBytes());
				return false;
			}

			const RouterAddress& getAddress(const int index) const
			{
				return m_addresses[index];
			}

			const RouterIdentity& getIdentity() const
			{
				return m_identity;
			}

		private:
			ByteArray calculateHash(ByteArray const &signedBytes) const
			{
				Pipe hashPipe(new Hash_Filter("SHA-256"));
				hashPipe.start_msg();

				hashPipe.write(signedBytes);

				hashPipe.end_msg();

				ByteArray hash(32);
				hashPipe.read(hash.data(), 32);

				return hash;
			}

			ByteArray getSignedBytes() const
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

			RouterIdentity m_identity;
			Date m_published;
			vector<RouterAddress> m_addresses;
			Mapping m_options;
			ByteArray m_signature;
	};
}

#endif
