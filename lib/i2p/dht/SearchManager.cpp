/**
 * @file SearchManager.cpp
 * @brief Implements SearchManager.h
 */
#include "SearchManager.h"

#include "../RouterContext.h"

#include "../i2np/DatabaseLookup.h"

#include <i2pcpp/util/Base64.h>
#include <i2pcpp/util/make_unique.h>
#include <i2pcpp/datatypes/RouterInfo.h>

namespace i2pcpp {
    namespace DHT {
        SearchManager::SearchManager(boost::asio::io_service &ios, RouterContext &ctx) :
            m_ios(ios),
            m_ctx(ctx),
            m_nlc(m_ios, boost::posix_time::time_duration(5, 0, 0)),
            m_log(I2P_LOG_CHANNEL("SM")) {}

        boost::signals2::connection SearchManager::registerSuccess(SuccessSignal::slot_type const &sh)
        {
            return m_successSignal.connect(sh);
        }

        boost::signals2::connection SearchManager::registerFailure(FailureSignal::slot_type const &fh)
        {
            return m_failureSignal.connect(fh);
        }

        void SearchManager::createSearch(Kademlia::key_type const &k, Kademlia::result_type const &startingPoints)
        {
            // If the key is in the NLC, immediately trigger failure
            if(m_nlc.contains(k)) {
                m_ios.post(boost::bind(boost::ref(m_failureSignal), k));
                return;
            }
            
            std::lock_guard<std::mutex> lock(m_searchesMutex);
            
            // If we're already searching for this key, don't start
            if(m_searches.get<0>().count(k))
                return;

            SearchState ss(k, startingPoints.first->second);

            for(auto it = std::next(startingPoints.first); it != startingPoints.second; ++it)
                ss.addAlternate(it->second);

            m_searches.insert(ss);

            // Start the timeout-timer (2min)
            m_timers[k] = std::make_unique<boost::asio::deadline_timer>(
                m_ios, boost::posix_time::time_duration(0, 1, 0)
            );
            m_timers[k]->async_wait(boost::bind(
                &SearchManager::timeout, this, boost::asio::placeholders::error, k
            ));

            I2P_LOG(m_log, debug) << "created SearchState for "
                                  << Base64::encode(ByteArray(k.cbegin(), k.cend()))
                                  << " starting with " << ss.current;

            m_ctx.getOutMsgDisp().getTransport()->connect(
                m_ctx.getDatabase()->getRouterInfo(ss.current)
            );
        }

        void SearchManager::timeout(const boost::system::error_code& e, Kademlia::key_type const k)
        {
            if(e) // If the timeout was due to an error, ignore it
                return;

            I2P_LOG(m_log, debug) << "timeout for " << Base64::encode(ByteArray(k.cbegin(), k.cend()));

            std::lock_guard<std::mutex> lock(m_searchesMutex);
            cancel(k);
        }

        void SearchManager::cancel(Kademlia::key_type const &k)
        {
            I2P_LOG(m_log, debug) << "cancelling " << Base64::encode(ByteArray(k.cbegin(), k.cend()));
            SearchStateByGoal::iterator itr = m_searches.get<0>().find(k);
            
            if(itr != m_searches.get<0>().end()) {
                m_ios.post(boost::bind(boost::ref(m_failureSignal), k));

                m_searches.get<0>().erase(itr);
                m_timers.erase(k);

                m_nlc.insert(k);
            }
        }

        void SearchManager::connected(RouterHash const rh)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", rh);
            I2P_LOG(m_log, debug) << "connection established";

            std::lock_guard<std::mutex> lock(m_searchesMutex);

            if(m_searches.get<1>().count(rh)) {
                I2P_LOG(m_log, debug) << "found RouterHash in pending search table (after connected)";

                SearchStateByCurrent::iterator itr = m_searches.get<1>().find(rh);
                const SearchState& ss = *itr;

                if(ss.state == SearchState::CurrentState::CONNECTING) {
                    I2P_LOG(m_log, debug) << "found good SearchState, sending DatabaseLookup";

                    m_searches.get<1>().modify(
                        itr, ModifyState(SearchState::CurrentState::LOOKUP_SENT)
                    );

                    I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(
                        ss.goal, m_ctx.getIdentity()->getHash(), 0, ss.getExcluded()
                    ));
                    m_ctx.getOutMsgDisp().sendMessage(rh, dbl);
                }
            }
        }

        void SearchManager::connectionFailure(RouterHash const rh)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", rh);
            I2P_LOG(m_log, debug) << "connection failed";


            std::lock_guard<std::mutex> lock(m_searchesMutex);

            if(!m_searches.get<1>().count(rh))
                return; // Not looking for this router hash

            SearchStateByCurrent::iterator itr = m_searches.get<1>().find(rh);
            const SearchState& ss = *itr;

            if(!ss.countAlternates()) {
                I2P_LOG(m_log, debug) << "connection failed and there are no more alternates,"
                                         "search failed";
                return cancel(ss.goal);
            }

            m_searches.get<1>().modify(itr, PopAlternates());
            while(!m_ctx.getDatabase()->routerExists(ss.current)) {
                I2P_LOG(m_log, debug) << "could not try alternate " << ss.current
                                      << " because it doesn't exist";

                // Skip this bad alternate
                // If no alternates are left, stop the lookup
                if(ss.countAlternates())
                    m_searches.get<1>().modify(itr, PopAlternates());
                else {
                    I2P_LOG(m_log, debug) << "no more alternates left, search failed";

                    return cancel(ss.goal);
                }
            }

            // Exclude bad router and start new connection
            m_searches.get<1>().modify(itr, ModifyState(rh));
            I2P_LOG(m_log, debug) << "connecting to alternate " << ss.current;

            m_ctx.getOutMsgDisp().getTransport()->connect(
                m_ctx.getDatabase()->getRouterInfo(ss.current)
            );
           
        }

        void SearchManager::searchReply(RouterHash const from, StaticByteArray<32> const query, std::list<RouterHash> const hashes)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received search reply";

            std::lock_guard<std::mutex> lock(m_searchesMutex);

            if(!m_searches.get<0>().count(query))
                return; // Not looking for this router hash

            I2P_LOG(m_log, debug) << "found RouterHash in pending search table (after search reply)";

            SearchStateByGoal::iterator itr = m_searches.get<0>().find(query);
            const SearchState& ss = *itr;
            
            if(ss.state == SearchState::CurrentState::LOOKUP_SENT) {
                for(const auto& h: hashes) // Add new alternates
                    m_searches.get<0>().modify(itr, PushAlternates(h));

                if(!ss.countAlternates()) {
                    I2P_LOG(m_log, debug) << "no more alternates left, search failed";
                    return cancel(ss.goal);
                }
                
                // Select the next alternate
                m_searches.get<0>().modify(itr, PopAlternates());
                m_searches.get<0>().modify(itr, ModifyState(from));

                if(!m_ctx.getDatabase()->routerExists(ss.current)) {
                    I2P_LOG(m_log, debug) << "received unknown peer hash " << ss.current
                                          << ", asking for its RouterInfo";

                    I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(
                        ss.current, m_ctx.getIdentity()->getHash(), 0, ss.getExcluded()
                    ));
                    m_ctx.getOutMsgDisp().sendMessage(from, dbl);
                } else {
                    I2P_LOG(m_log, debug) << "received known peer hash " << ss.current
                                          << ", connecting";
                    m_ctx.getOutMsgDisp().getTransport()->connect(
                        m_ctx.getDatabase()->getRouterInfo(ss.current)
                    );
                }
                
            }
            
        }

        void SearchManager::databaseStore(RouterHash const from, StaticByteArray<32> const k, bool isRouterInfo)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received DatabaseStore";

            std::lock_guard<std::mutex> lock(m_searchesMutex);

            if(m_searches.get<0>().count(k)) {
                I2P_LOG(m_log, debug) << "received DatabaseStore for our goal, terminating search";

                SearchStateByGoal::iterator itr = m_searches.get<0>().find(k);
                const SearchState& ss = *itr;

                if(isRouterInfo)
                    m_ios.post(boost::bind(
                        boost::ref(m_successSignal),
                        ss.goal,
                        m_ctx.getDatabase()->getRouterInfo(k).getIdentity().getHash()
                    ));

                m_searches.get<0>().erase(itr);
                m_timers.erase(k);

                return;
            }

            if(!isRouterInfo)
                return; // don't handle LS (yet)

            if(m_searches.get<1>().count(from)) {
                I2P_LOG(m_log, debug) << "found Routerhash in pending search table"
                                         "(after DatabaseStore)";

                SearchStateByCurrent::iterator itr = m_searches.get<1>().find(from);
                const SearchState& ss = *itr;

                if(ss.current == k) {
                    I2P_LOG(m_log, debug) << "stored hash is one we're waiting for, connecting";

                    m_ctx.getOutMsgDisp().getTransport()->connect(
                        m_ctx.getDatabase()->getRouterInfo(k)
                    );
                } else {
                    I2P_LOG(m_log, debug) << "stored hash is not one we're waiting for";
                }
            }
            
        }
    }
}
