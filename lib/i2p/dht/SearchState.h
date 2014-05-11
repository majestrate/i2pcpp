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
        class SearchState {
            friend class ModifyState;

        public:
            /**
             * A search is either in the \a CONNECTING or in the \a LOOKUP_SENT
             *  state.
             */
            enum class CurrentState {
                CONNECTING,
                LOOKUP_SENT
            };

            /**
             * Constructs from a goal and an i2pcpp::RouterHash to contact 
             * @param goal the key to find
             * @param start the first router to contact
             */
            SearchState(const Kademlia::key_type& goal, const RouterHash& start);

            /**
             * Explicity copy constructor to avoid shallow copy of iterator.
             */
            SearchState(const SearchState& ss);

            /**
             * Explicity assignment operator to avoid shallow copy of iterator.
             */
            SearchState& operator=(const SearchState& ss);

            /**
             * Checks whether a given i2pcpp::RouterHash is an unused alternate for
             *  this search operation.
             * @return true if it is an unused alternate, false otherwise
             */
            bool isAlternate(const RouterHash& rh) const;

            /**
             * Chechs whether a given i2pcpp::RouterHash has been tried as an
             *  alternate already.
             * @return true if has been tried, false otherwise
             */
            bool isTried(const RouterHash& rh) const;

            /**
             * Counts the amount of alternates left.
             * @return the amount of alternates that have not yet been contacted
             */
            std::size_t countAlternates() const;

            /**
             * Adds an alternate i2pcpp::RouterHash to the emulated queue.
             * @param rh the i2pcpp::RouterHash to add
             */
            void addAlternate(const RouterHash& rh);

            /**
             * Pops an alternate from the emulated alternates queue.
             * This is done by advancing the \a m_current pointer.
             * If there are no alternates left, nothing happens.
             */
            void popAlternate();

            /**
             * @return the next router hash
             */
            RouterHash getNext() const;

            /**
             * The list of excluded i2pcpp::RouterHash objects.
             */
            std::list<RouterHash> getExcluded() const;

            CurrentState state = CurrentState::CONNECTING;
            RouterHash current;
            Kademlia::key_type goal;
        private:
            std::list<RouterHash> m_excluded;
            std::list<RouterHash> m_alternates;
            std::list<RouterHash>::const_iterator m_current;
        };


        /**
         * Function object to remove the peer at the front of the alternates
         *  queue.
         */
        class PopAlternates {
            public:
                /**
                 * Removes the peer at the front of the alternates queue and
                 *  automatically selects then next alternate as the current
                 *  peer..
                 */
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
                 *  to a specified state.
                 */
                ModifyState(SearchState::CurrentState const state);

                /**
                 * Modifies the state to the "new connection" state.
                 * @param exclude i2pcpp::RouterHash to be excluded in database
                 *  lookups (usually the peer that told this one about \a current)
                 * @note starting a new connection also requires using
                 *  i2pcpp::DHT::PopAlternates()
                 */
                ModifyState(RouterHash const &exclude);

                void operator()(SearchState &ss);

            private:
                UpdateType m_type;

                RouterHash m_exclude;
                SearchState::CurrentState m_state;
        };
    }
}

#endif
