/**
 * @file SearchManager.h
 * @brief Defines the i2pcpp::SearchManager class.
 */
#ifndef DHTSEARCHMANAGER_H
#define DHTSEARCHMANAGER_H

#include <mutex>

#include "../Log.h"

#include <i2pcpp/datatypes/RouterHash.h>

#include <boost/asio.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/signals2.hpp>

#include <i2pcpp/datatypes/RouterHash.h>

#include "../Log.h"

#include "Kademlia.h"
#include "SearchState.h"
#include "NegativeLookupCache.h"

namespace bmi = boost::multi_index;

namespace i2pcpp {
    class RouterContext;

    namespace DHT {
            /**
             * Helper class used to manage netDB search operations.
             */
            class SearchManager {
                private:
                    typedef boost::signals2::signal<void(const Kademlia::key_type, const Kademlia::value_type)> SuccessSignal;
                    typedef boost::signals2::signal<void(const Kademlia::key_type)> FailureSignal;

                    typedef boost::multi_index_container<
                        SearchState,
                        bmi::indexed_by<
                            bmi::hashed_unique<
                                BOOST_MULTI_INDEX_MEMBER(SearchState, Kademlia::key_type, goal)
                            >,
                            bmi::hashed_non_unique<
                                BOOST_MULTI_INDEX_MEMBER(SearchState, RouterHash, current)
                            >,
                            bmi::hashed_non_unique<
                                BOOST_MULTI_INDEX_MEMBER(SearchState, RouterHash, next)
                            >
                        >
                    > SearchStateContainer;
                    typedef SearchStateContainer::nth_index<0>::type SearchStateByGoal;
                    typedef SearchStateContainer::nth_index<1>::type SearchStateByCurrent;
                    typedef SearchStateContainer::nth_index<2>::type SearchStateByNext;

                public:
                    /**
                     * Constructs from a reference to the i2pcpp::RouterContext.
                     */
                    SearchManager(boost::asio::io_service &ios, RouterContext& ctx);

                    SearchManager(const SearchManager &) = delete;
                    SearchManager& operator=(SearchManager &) = delete;

                    /**
                     * Registers a signal handler for the signal that is triggered
                     *  when a lookup succeeds.
                     */
                    boost::signals2::connection registerSuccess(SuccessSignal::slot_type const &sh);

                    /**
                     * Registers a signal handler for the signal that is triggered
                     *  when a lookup fails.
                     */
                    boost::signals2::connection registerFailure(FailureSignal::slot_type const &fh);

                    /**
                     * Creates a new i2pcpp::DHT::SearchState to track the status.
                     * Starts connecting to the first closest peer. If the
                     *  connection is successful, this will result in a call to
                     *  SearchManager::connected. Times out after 2 minutes.
                     * @param k the key to lookup
                     * @param startingPoints the closest peers, that will be
                     *  contacted in order until the key is found
                     * @note if the key is already being searched for, a new
                     *  search operation will not be started
                     */
                    void createSearch(Kademlia::key_type const &k, Kademlia::result_type const &startingPoints);

                    /**
                     * Called when we have established a connection with a node.
                     * @param rh the i2pcpp::RouterHash of the router we
                     *  are now connected with
                     */
                    void connected(RouterHash const rh);

                    /**
                     * Called when a connection with a router has failed.
                     * Tries to connect to the next router in the given list of
                     *  closest nodes.
                     * @param rh the i2pcpp::RouterHash of the router we
                     *  connected to
                     */
                    void connectionFailure(RouterHash const rh);

                    /**
                     * Called upon receival of a database search reply message.
                     * This is the resonpose to a failed lookup.
                     * @param from routing hash of the sending router
                     * @param hashes std::list of i2pcpp::RouterHash objects
                     *  contained in this message
                     */
                    void searchReply(RouterHash const from, StaticByteArray<32> const query, std::list<RouterHash> const hashes);

                    /**
                     * Called upon receival of a database store message.
                     * This is the response to a succesful lookup.
                     * @param from routing hash of the sending router
                     * @param k the returned data
                     * @param isRouter true if \a k is an i2pcpp::RouterInfo
                     *  structure, false otherwise
                     */
                    void databaseStore(RouterHash const from, StaticByteArray<32> const k, bool isRouterInfo);

                private:
                    /**
                     * Called when a lookup operation times out.
                     * Cancels the search operation for \a k.
                     * @param k key that was being looked up
                     */
                    void timeout(const boost::system::error_code& e, Kademlia::key_type const k);

                    /**
                     * Cancels the search operation for a key \a k.
                     * This is done when the search failed.
                     * As a consequence an entry is added to the NLC.
                     */
                    void cancel(Kademlia::key_type const &k);

                    boost::asio::io_service& m_ios;
                    RouterContext& m_ctx;
                    NegativeLookupCache m_nlc;

                    std::map<Kademlia::key_type, std::unique_ptr<boost::asio::deadline_timer>> m_timers;

                    SuccessSignal m_successSignal;
                    FailureSignal m_failureSignal;

                    SearchStateContainer m_searches;
                    mutable std::mutex m_searchesMutex;

                    i2p_logger_mt m_log;
            };
    }
}

#endif
