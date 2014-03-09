#ifndef FRAGMENTSTATE_H
#define FRAGMENTSTATE_H

#include "FirstFragment.h"
#include "FollowOnFragment.h"

#include <boost/asio.hpp>

#include <list>

namespace i2pcpp {
    class FragmentState {
        public:
            void setFirstFragment(std::unique_ptr<FirstFragment> ff);
            void addFollowOnFragment(FollowOnFragment fof);

            bool isComplete() const;
            ByteArray compile();
            const std::unique_ptr<FirstFragment>& getFirstFragment() const;

            void setTimer(std::unique_ptr<boost::asio::deadline_timer> t);

        private:
            uint8_t m_lastFragNum = 0;

            std::unique_ptr<FirstFragment> m_firstFragment = nullptr;
            std::list<FollowOnFragment> m_followOnFragments;

            std::unique_ptr<boost::asio::deadline_timer> m_timer;
    };
}

#endif
