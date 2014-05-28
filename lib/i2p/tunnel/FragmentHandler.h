#ifndef TUNNELFRAGMENTHANDLER_H
#define TUNNELFRAGMENTHANDLER_H

#include "FragmentState.h"

#include <i2pcpp/Log.h>

#include <unordered_map>
#include <mutex>

namespace i2pcpp {
    class RouterContext;

    namespace Tunnel {
        class FragmentHandler {
            public:
                FragmentHandler(boost::asio::io_service &ios, RouterContext &ctx);
                FragmentHandler(const FragmentHandler &) = delete;
                FragmentHandler& operator=(FragmentHandler &) = delete;

                /**
                 * Collects a list of fragments we've received. All fragments go
                 * in to a corresponding i2pcpp::Tunnel::FragmentState based on
                 * message ID. FragmentStates are deleted after two minutes.
                 */
                void receiveFragments(std::list<FragmentPtr> fragments);

            private:
                /**
                 * Checks whether all the fragments for a particular \a msgId have
                 * been received. If so, the fragments are compiled in to a single
                 * byte array, wrapped in an i2pcpp::I2NP::Message, and sent to the
                 * intended destination.
                 */
                void checkAndFlush(uint32_t msgId);

                /**
                 * Erases the i2pcpp::Tunnel::FragmentState for a given \a msgId.
                 */
                void timerCallback(const boost::system::error_code& e, const uint32_t msgId);

                boost::asio::io_service &m_ios;
                RouterContext &m_ctx;

                /// A map of message IDs to fragment states.
                std::unordered_map<uint32_t, FragmentState> m_states;

                mutable std::mutex m_statesMutex;

                i2p_logger_mt m_log;
        };
    }
}

#endif
