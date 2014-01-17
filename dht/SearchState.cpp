/**
 * @file SearchState.cpp
 * Implements SearchState.h.
 */
#include "SearchState.h"

namespace i2pcpp {
    namespace DHT {
        void PopAlternates::operator()(SearchState &ss) { ss.alternates.pop(); }

        PushAlternates::PushAlternates(RouterHash const &alt) :
            m_alt(alt) {}

        void PushAlternates::operator()(SearchState &ss) { ss.alternates.push(m_alt); }

        InsertTried::InsertTried(RouterHash const &rh) :
            m_rh(rh) {}

        void InsertTried::operator()(SearchState &ss) { ss.tried.insert(m_rh); }

        ModifyState::ModifyState(SearchState::CurrentState const state) :
            m_state(state),
            m_type(UpdateType::STATE) {}

        ModifyState::ModifyState(RouterHash const &next) :
            m_next(next),
            m_type(UpdateType::NEXT) {}

        ModifyState::ModifyState(RouterHash const &current, RouterHash const &exclude) :
            m_current(current),
            m_exclude(exclude),
            m_type(UpdateType::NEW_CONNECTION) {}

        void ModifyState::operator()(SearchState &ss)
        {
            switch(m_type) {
                case UpdateType::STATE:
                    ss.state = m_state;
                    break;

                case UpdateType::NEXT:
                    ss.next = m_next;
                    break;

                case UpdateType::NEW_CONNECTION:
                    ss.current = m_current;
                    ss.state = SearchState::CurrentState::CONNECTING;
                    ss.next = RouterHash();
                    ss.excluded.push_back(m_exclude);
                    ss.alternates.pop();
                    break;
            }
        }
    }
}
