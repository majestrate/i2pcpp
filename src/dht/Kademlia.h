/**
 * @file Kademlia.h
 * @brief Defines the i2pcpp::DHT::Kademlia class.
 */
#ifndef DHTKADEMLIA_H
#define DHTKADEMLIA_H

#include <map>
#include <memory>

#include <boost/functional/hash.hpp>

#include <i2pcpp/datatypes/StaticByteArray.h>
#include <i2pcpp/datatypes/RouterHash.h>

#define KEY_SIZE 32
#define NUM_BUCKETS (KEY_SIZE * 8)
#define K_VALUE 20

namespace i2pcpp {
    namespace DHT {
        /**
         * This type implements the local DHT functionality, according to the
         *  Kademlia model.
         * It stores a number K-Buckets, which contain i2pcpp::RouterHash
         *  objects, the values that will be looked up.
         * The goal of the structure is to find peers nearest to a given key.
         * Nearest is, as is typical for Kademia, defined in terms of the
         *  exclusive or operation.
         * This is used  when a floodfill router tries to reply to a lookup request.
         * @note every bit of the key represents one K-Bucket, the prefix
         *  optimization is not used
         */
        class Kademlia {
            public:
                /**
                 * Keys are i2pcpp::StaticByteArray of \a KEY_SIZE bytes.
                 * Specifically, they are SHA-256 hashes of 32B binary keys
                 *  appended with an 8B ASCII string representing the UTC
                 *  date.
                 * With "binary keys" i2pcpp::RouterHash objects are meant.
                 * @note these "routing keys" are only used locally for distance
                 *  calculation
                 */
                typedef StaticByteArray<KEY_SIZE> key_type;

                /**
                 * Values are SHA256 hashes.
                 * For i2pcpp::LeaseSet objects these are destinations.
                 * For i2pcpp::RouterInfo objects these are router hashes.
                 */
                typedef StaticByteArray<32> value_type;

                /**
                 * Stores the values, the size_t object is used to store the
                 *  bucket id.
                 */
                typedef std::multimap<size_t, value_type> map_type;

                /**
                 * The result type is a pair of iterators that define the range
                 *  of a K-bucket.
                 */
                typedef std::pair<map_type::const_iterator, map_type::const_iterator> result_type;

                /**
                 * Constructs an i2pcpp::DHT::Kademlia from a reference to a key.
                 * @note generally this is taken as i2pcpp::RouterHash of this router
                 */
                Kademlia(key_type const &reference);

                /**
                 * Inserts a value into to the DHT, into the correct k bucket
                 *  for the given key.
                 * @param k the key for the new value to be stored
                 * @param v the value to be stored
                 */
                void insert(key_type const &k, value_type const &v);

                /**
                 * Erases a value from the DHT.
                 * @param itr an iterator to the item to be erased
                 */
                void erase(map_type::const_iterator itr);

                /**
                 * @return the range associated with the K-bucket that contains
                 *  the given key \a k
                 */
                result_type find(key_type const &k) const;

                /**
                 * @return the range associated with the K-bucket that contains
                 *  the given key \a k
                 * @param count the maximum amount of entries to be returned
                 * @note if less than count entries are available, as many entries
                 *  as possible are returned
                 */
                result_type find(key_type const &k, std::size_t count) const;
                
                /**
                 * Changes the reference key. This is used to find the bucket
                 *  associated with a given key.
                 * @param reference the new reference key
                 * @see i2pcpp::DHT::Kademlia::getBucket
                 */
                void setReference(key_type const &reference);

                /**
                 * Makes a key from an i2pcpp::RouterHash.
                 * This is done by taking the SHA-256 of the given value
                 *  appending with the UTC data as an ASCII string.
                 * The format for that s yyyyMMdd.
                 */
                static key_type makeKey(value_type const &rh);

            private:
                /**
                 * Finds the K-Bucked associated with a given key.
                 * First, computes the exclusive or distance between the key to
                 *  find and the reference key.
                 * A guess is made for the bucket id, being the last bucked.
                 * Then, for each byte of the distance:
                 * If it is zero, move 8 buckets towards the left
                 * Otherwise, calculate the amount of low-bits set to 0 and
                 *  move that many buckets to the left.
                 * @return the K-Bucked id for a given key
                 * @note the amount of bits set to 0 is calucated as
                 *  8 - log2(b), where the logarithm denotes the highest
                 *  bit that is set
                 */
                size_t getBucket(key_type const &k) const;

                key_type m_ref;
                map_type m_table;
        };

        typedef std::shared_ptr<Kademlia> KademliaPtr;
    }
}

#endif
