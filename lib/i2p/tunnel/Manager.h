#ifndef TUNNELMANAGER_H
#define TUNNELMANAGER_H

#include "Tunnel.h"
#include "FragmentHandler.h"

#include <i2pcpp/Log.h>

#include <i2pcpp/datatypes/BuildRecord.h>
#include <i2pcpp/datatypes/BuildRequestRecord.h>
#include <i2pcpp/datatypes/BuildResponseRecord.h>

#include <boost/asio.hpp>

#include <mutex>
#include <unordered_map>

namespace i2pcpp {
    class RouterContext;

    namespace Tunnel {
        class Manager {
            public:
                Manager(boost::asio::io_service &ios, RouterContext &ctx);
                Manager(const Manager &) = delete;
                Manager& operator=(Manager &) = delete;

                void begin();

                /**
                 * Collects build records that are received. Automatically forwards
                 * the records to the next hop, if necessary. If the records are a
                 * result of a tunnel we created, the tunnel's status is set
                 * accordingly.
                 */
                void receiveRecords(uint32_t const msgId, std::list<BuildRecordPtr> records);

                /**
                 * Checks to see if \a tunnelId is a valid, established tunnel. If so,
                 * \a data is fragmented in to on first fragment and zero or more
                 * follow on fragments. All the fragments are then sent one by one to
                 * the next hop in the tunnel.
                 */
                void receiveGatewayData(RouterHash const from, uint32_t const tunnelId, ByteArray const data);

                /**
                 * Checks to see if the \a tunnelId is valid. If we are a participatory
                 * tunnel, the \a data is merely forwarded to the next hop. If we are
                 * an endpoint, the \a data is sent to the i2pcpp::Tunnel::FragmentHandler
                 * for further processing.
                 */
                void receiveData(RouterHash const from, uint32_t const tunnelId, StaticByteArray<1024> const data);

            private:
                /**
                 * Deletes the \a tunnelId.
                 */
                void timerCallback(const boost::system::error_code &e, bool participating, uint32_t tunnelId);
                void callback(const boost::system::error_code &e);
                void createTunnel();

                boost::asio::io_service &m_ios;
                RouterContext &m_ctx;

                std::unordered_map<uint32_t, TunnelPtr> m_pending;
                std::unordered_map<uint32_t, TunnelPtr> m_tunnels;
                std::unordered_map<uint32_t, std::pair<BuildRequestRecordPtr, std::unique_ptr<boost::asio::deadline_timer>>> m_participating;

                mutable std::mutex m_pendingMutex;
                mutable std::mutex m_tunnelsMutex;
                mutable std::mutex m_participatingMutex;

                FragmentHandler m_fragmentHandler;

                boost::asio::deadline_timer m_timer;

                i2p_logger_mt m_log;
        };
    }
}

#endif
