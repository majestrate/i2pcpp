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
			RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature = ByteArray(40));
			RouterInfo(ByteArrayConstItr &begin, ByteArrayConstItr end);

			ByteArray serialize() const;
			void addAddress(RouterAddress const &address);
			bool verifySignature() const;
			void sign(const Botan::DSA_PrivateKey * const signingKey);

			const RouterAddress& getAddress(const int index) const;
			const RouterIdentity& getIdentity() const;
			const Date& getPublished() const;
			const Mapping& getOptions() const;
			const ByteArray& getSignature() const;

			const std::vector<RouterAddress>::const_iterator begin() const;
			const std::vector<RouterAddress>::const_iterator end() const;

		private:
			ByteArray getSignedBytes() const;

			RouterIdentity m_identity;
			Date m_published;
			std::vector<RouterAddress> m_addresses;
			Mapping m_options;
			ByteArray m_signature;
	};
}

#endif
