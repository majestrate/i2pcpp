/**
 * @file SearchState.h
 * Defines the i2pcpp::DHT::SearchState structure as well a number of utility
 *  function objects for use by the modify member function of boost multi-index
 *  container.
 */
#ifndef DHTSEARCHSTATE_H
#define DHTSEARCHSTATE_H

#include <memory>
#include <list>
#include <queue>
#include <set>

#include <boost/asio.hpp>

#include <i2pcpp/datatypes/RouterHash.h>

#include "Kademlia.h"

namespace i2pcpp {
    namespace DHT {

        /**
         * Defines the state of a search operation.
         */
        struct SearchState {
            /**
             * A search is either in the \a CONNECTING or in the \a LOOKUP_SENT
             *  state.
             */
            enum class CurrentState {
                CONNECTING,
                LOOKUP_SENT
            };

            Kademlia::key_type goal;
            RouterHash current;
            RouterHash next;
            std::list<RouterHash> excluded;
            std::queue<RouterHash> alternates;
            std::set<RouterHash> tried;
            CurrentState state = CurrentState::CONNECTING;
        };

        /**
         * Function object to remove the peer at the front of the alternates
         *  queue.
         */
        class PopAlternates {
            public:
                void operator()(SearchState &ss);
        };

        /**
         * Function object to add a peer to the alternates queue.
         */
        class PushAlternates {
            public:
                /**
                 * @param alt the i2pcpp::RouterHash of the peer to add as an
                 *  alternate
                 **/
                PushAlternates(RouterHash const &alt);

                void operator()(SearchState &ss);

            private:
                RouterHash m_alt;
        };

        /**
         * Function object to insert a peer in the set of i2pcpp::RouterHash
         *  objects that we have tried to connect to in order to preform a
         *  lookup.
         */
        class InsertTried {
            public:
                /**
                 * @rh i2pcpp::RouterHash of the peer to insert
                 */
                InsertTried(RouterHash const &rh);

                void operator()(SearchState &ss);

            private:
                RouterHash m_rh;
        };

        /**
         * Function object to modify a given state
         */
        class ModifyState {
            private:
                /**
                 * The type of update to take place. It can be either a state
                 *  replacement, the usage of the next alternate or a new connect
                 */
                enum class UpdateType {
                    STATE,
                    NEXT,
                    NEW_CONNECTION
                };

            public:
                /**
                 * Changes the i2pcpp::DHT::SearchState::CurrentState object
                 *  of the state.
                 */
                ModifyState(SearchState::CurrentState const state);

                /**
                 * Modifies the state to the "next" state.
                 */
                ModifyState(RouterHash const &next);

                /**
                 * Modifies the state to the "new connection" state.
                 * @param current the i2pcpp::RouterHash with which a connection
                 *  will be established
                 * @param exclude i2pcpp::RouterHash to be excluded in database
                 *  lookups (usually the peer that told this one about \a current)
                 */
                ModifyState(RouterHash const &current, RouterHash const &exclude);

                void operator()(SearchState &ss);

            private:
                UpdateType m_type;

                RouterHash m_current;
                RouterHash m_exclude;
                RouterHash m_next;
                SearchState::CurrentState m_state;
        };
    }
}

#endif
