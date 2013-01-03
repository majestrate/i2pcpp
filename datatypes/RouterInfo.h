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
			RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature = ByteArray(40)) : m_identity(identity), m_published(published), m_options(options), m_signature(signature) {}
			RouterInfo(ByteArray const &infoBytes);

			ByteArray getBytes() const;
			void addAddress(RouterAddress const &address)	{	m_addresses.push_back(address);	}
			bool verifySignature(const Botan::DL_Group &dsaParameters) const;
			void sign(const Botan::DSA_PrivateKey * const signingKey);

			const RouterAddress& getAddress(const int index) const { return m_addresses[index];	}
			const RouterIdentity& getIdentity() const	{	return m_identity; }
			const Date& getPublished() const { return m_published; }
			const Mapping& getOptions() const { return m_options; }
			const ByteArray& getSignature() const { return m_signature; }

			const std::vector<RouterAddress>::const_iterator begin() const { return m_addresses.cbegin(); }
			const std::vector<RouterAddress>::const_iterator end() const { return m_addresses.cend(); }

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
