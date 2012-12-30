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

namespace i2pcpp {
	class RouterInfo : public Datatype {
		public:
			RouterInfo() {}
			RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature) : m_identity(identity), m_published(published), m_options(options), m_signature(signature) {}

			RouterInfo(ByteArray const &infoBytes);

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
			ByteArray calculateHash(ByteArray const &signedBytes) const;
			ByteArray getSignedBytes() const;

			RouterIdentity m_identity;
			Date m_published;
			std::vector<RouterAddress> m_addresses;
			Mapping m_options;
			ByteArray m_signature;
	};
}

#endif
