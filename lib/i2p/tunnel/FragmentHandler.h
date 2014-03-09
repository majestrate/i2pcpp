#ifndef FRAGMENTHANDLER_H
#define FRAGMENTHANDLER_H

#include "../Log.h"

#include "FragmentState.h"

#include <unordered_map>

namespace i2pcpp {
    class RouterContext;

    class FragmentHandler {
        public:
            FragmentHandler(boost::asio::io_service &ios, RouterContext &ctx);
            FragmentHandler(const FragmentHandler &) = delete;
            FragmentHandler& operator=(FragmentHandler &) = delete;

            void receiveFragments(std::list<FragmentPtr> fragments);

        private:
            void checkAndFlush(uint32_t msgId);
            void timerCallback(const boost::system::error_code& e, const uint32_t msgId);

            RouterContext &m_ctx;
            boost::asio::io_service &m_ios;

            std::unordered_map<uint32_t, FragmentState> m_states;

            mutable std::mutex m_statesMutex;

            i2p_logger_mt m_log;
    };
}

#endif
