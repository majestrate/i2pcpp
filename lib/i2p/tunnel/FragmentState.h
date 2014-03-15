#ifndef TUNNELFRAGMENTSTATE_H
#define TUNNELFRAGMENTSTATE_H

#include "FirstFragment.h"
#include "FollowOnFragment.h"

#include <boost/asio.hpp>

#include <list>

namespace i2pcpp {
    namespace Tunnel {
        class FragmentState {
            public:
                /**
                 * Sets the first fragment for this state.
                 */
                void setFirstFragment(std::unique_ptr<FirstFragment> ff);

                /**
                 * Adds a follow on fragment for this state.
                 */
                void addFollowOnFragment(FollowOnFragment fof);

                /**
                 * @return true if all the fragments for a message have been
                 * received.
                 */
                bool isComplete() const;

                /**
                 * Compiles all the received fragments in to one i2pcpp::ByteArray.
                 */
                ByteArray compile();

                /**
                 * Returns the first fragment in this state. This is needed because
                 * the first fragment contains delivery instructions.
                 */
                const std::unique_ptr<FirstFragment>& getFirstFragment() const;

                /**
                 * Sets a timer for this state.
                 */
                void setTimer(std::unique_ptr<boost::asio::deadline_timer> t);

            private:
                uint8_t m_lastFragNum = 0;

                std::unique_ptr<FirstFragment> m_firstFragment = nullptr;
                std::list<FollowOnFragment> m_followOnFragments;

                std::unique_ptr<boost::asio::deadline_timer> m_timer;
        };
    }
}

#endif
