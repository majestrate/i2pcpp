#ifndef FRAGMENTHANDLER_H
#define FRAGMENTHANDLER_H

#include <unordered_map>
#include <list>
#include <mutex>

#include "../Log.h"

#include "FragmentState.h"

namespace i2pcpp {
    class RouterContext;

    class FragmentHandler {
        public:
            FragmentHandler(RouterContext &ctx);
            FragmentHandler(const FragmentHandler &) = delete;
            FragmentHandler& operator=(FragmentHandler &) = delete;

            void receiveFragments(std::list<FragmentPtr> fragments);

        private:
            void checkAndFlush(uint32_t msgId);

            RouterContext &m_ctx;

            std::unordered_map<uint32_t, FragmentState> m_states;

            mutable std::mutex m_statesMutex;

            i2p_logger_mt m_log;
    };
}

#endif
