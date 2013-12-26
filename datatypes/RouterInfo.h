/**
 * @file RouterInfo.h
 * @brief Defines the i2pcpp::RouterInfo type.
 */
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

    /**
     * Bundles all information that a router wants to publish. It is stored in
     *  the network database.
     */
    class RouterInfo : public Datatype {
        public:
            /*
             * @param identity the unique router identity structure
             * @param published date of publishing
             * @param options a mapping of options
             * @param 40 byte signature (by router identity who published it)
             */
            RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature = ByteArray(40));

            /**
             * Constructs from iterators to begin and end of an i2pcpp::ByteArray.
             */
            RouterInfo(ByteArrayConstItr &begin, ByteArrayConstItr end);

            ByteArray serialize() const;

            /**
             * Adds an address to the router info datatype.
             */
            void addAddress(RouterAddress const &address);

            /**
             * Verifies the (DSA) signature using the RI's public signing key.
             * @return true if it is correct, false otherwise
             * @note hashing algorithm is SHA1
             */
            bool verifySignature() const;

            /**
             * Creates the (DSA) signature using the given \a signingKey.
             * @note hashing algorithm is SHA1
             */
            void sign(std::shared_ptr<const Botan::DSA_PrivateKey> signingKey);

            /**
             * @return the i2pcpp::RouterAddress at a given \a index
             */
            const RouterAddress& getAddress(const int index) const;

            /**
             * @return the i2pcpp::RouterIdentity
             */
            const RouterIdentity& getIdentity() const;

            /**
             * @return the date of publishing
             */
            const Date& getPublished() const;

            /**
             * @return the options mapping
             */
            const Mapping& getOptions() const;

            /**
             * @return the signature (by the router identity who published it).
             */
            const ByteArray& getSignature() const;

            /**
             * @return an iterator the begin of the underlying
             *  std::vector<RouterAddress>.
             */
            const std::vector<RouterAddress>::const_iterator begin() const;
              /**
             * @return an iterator the end of the underlying
             *  std::vector<RouterAddress>.
             */
            const std::vector<RouterAddress>::const_iterator end() const;

        private:
            /**
             * @return the serialized version of the i2pcpp::RouterAddress but
             *  without the signature
             */
            ByteArray getSignedBytes() const;

            RouterIdentity m_identity;
            Date m_published;
            std::vector<RouterAddress> m_addresses;
            Mapping m_options;
            ByteArray m_signature;
    };
}

#endif
