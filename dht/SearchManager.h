#ifndef DHTSEARCHMANAGER_H
#define DHTSEARCHMANAGER_H

#include <mutex>
#include <set>

#include <boost/asio.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/signals2.hpp>

#include "../datatypes/RouterHash.h"

#include "../Log.h"

#include "Kademlia.h"
#include "SearchState.h"

namespace bmi = boost::multi_index;

namespace i2pcpp {
    class RouterContext;

    namespace DHT {
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
                    SearchManager(boost::asio::io_service& ios, RouterContext& ctx);

                    SearchManager(const SearchManager &) = delete;
                    SearchManager& operator=(SearchManager &) = delete;

                    boost::signals2::connection registerSuccess(SuccessSignal::slot_type const &sh);
                    boost::signals2::connection registerFailure(FailureSignal::slot_type const &fh);

                    void createSearch(Kademlia::key_type const &k, Kademlia::result_type const &startingPoints);

                    void connected(RouterHash const rh);
                    void connectionFailure(RouterHash const rh);
                    void searchReply(RouterHash const from, StaticByteArray<32> const query, std::list<RouterHash> const hashes);
                    void databaseStore(RouterHash const from, StaticByteArray<32> const k, bool isRouterInfo);

                private:
                    void timeout(const boost::system::error_code& e, Kademlia::key_type const k);
                    void cancel(Kademlia::key_type const &k);

                    boost::asio::io_service& m_ios;
                    RouterContext& m_ctx;

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
