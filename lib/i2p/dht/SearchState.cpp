/**
 * @file SearchState.cpp
 * Implements SearchState.h.
 */
#include "SearchState.h"

namespace i2pcpp {
    namespace DHT {

        SearchState::SearchState(const Kademlia::key_type& goal, const RouterHash& start)
            : goal(goal), current(start), m_excluded(), m_alternates(),
              m_current(m_alternates.begin()) 
        {

        }

        bool SearchState::isAlternate(const RouterHash& rh) const
        {
            return std::find(m_current, m_alternates.end(), rh) != m_alternates.end();
        }

        bool SearchState::isTried(const RouterHash& rh) const
        {
            return std::find(
                m_alternates.begin(), std::prev(m_current), rh
            ) != m_alternates.end();
        }

        std::size_t SearchState::countAlternates() const
        {
            return std::distance(m_current, m_alternates.end());
        }

        void SearchState::addAlternate(const RouterHash& rh)
        {
            m_alternates.push_back(rh);
        }

        void SearchState::popAlternate()
        {
            ++m_current;
            current = *m_current;
        }

        RouterHash SearchState::getNext() const
        {
            return *std::next(m_current);
        }

        std::list<RouterHash> SearchState::getExcluded() const
        {
            return m_excluded;
        }

        void PopAlternates::operator()(SearchState &ss)
        {
            ss.popAlternate();
        }

        PushAlternates::PushAlternates(RouterHash const &alt) :
            m_alt(alt) {}

        void PushAlternates::operator()(SearchState &ss)
        {
            ss.addAlternate(m_alt);
        }

        ModifyState::ModifyState(SearchState::CurrentState const state) :
            m_state(state),
            m_type(UpdateType::STATE) {}

        ModifyState::ModifyState(RouterHash const &exclude) :
            m_exclude(exclude),
            m_type(UpdateType::NEW_CONNECTION) {}

        void ModifyState::operator()(SearchState &ss)
        {
            switch(m_type) {
                case UpdateType::STATE:
                    ss.state = m_state;
                    break;

                case UpdateType::NEW_CONNECTION:
                    ss.state = SearchState::CurrentState::CONNECTING;
                    ss.m_excluded.push_back(m_exclude);
                    ss.popAlternate();
                    break;
            }
        }
    }
}
