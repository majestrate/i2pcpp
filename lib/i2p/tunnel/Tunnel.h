#ifndef TUNNELTUNNEL_H
#define TUNNELTUNNEL_H

#include "../Log.h"

#include <i2pcpp/datatypes/BuildRequestRecord.h>

#include <boost/asio.hpp>

#include <list>

namespace i2pcpp {
    namespace Tunnel {
        class Tunnel {
            public:
                /**
                 * Represents a tunnel direction.
                 */
                enum class Direction {
                    INBOUND,
                    OUTBOUND
                };

                /**
                 * Represents the state of a tunnel we've
                 * created.
                 */
                enum class State {
                    REQUESTED,
                    OPERATIONAL,
                    FAILED
                };

                virtual ~Tunnel() {}

                virtual Direction getDirection() const = 0;

                /**
                 * @return the state of the tunnel.
                 */
                State getState() const;

                /**
                 * @return this tunnel's ID.
                 */
                uint32_t getTunnelId() const;

                /**
                 * @return the build request records which are used to construct
                 * this tunnel.
                 */
                std::list<BuildRecordPtr> getRecords() const;

                /**
                 * @return the i2pcpp::RouterHash of the peer to whom the build
                 * records should be sent to first.
                 */
                RouterHash getDownstream() const;

                /**
                 * @return the next message ID of the tunnel.
                 */
                uint32_t getNextMsgId() const;

                /**
                 * Attempts to decrypt the received build record replies and
                 * set the tunnel state accordingly.
                 */
                void handleResponses(std::list<BuildRecordPtr> const &records);

                /**
                 * Sets a timer on the tunnel (for creation timeout).
                 */
                void setTimer(std::unique_ptr<boost::asio::deadline_timer> t);

            protected:
                Tunnel() = default;

                /**
                 * Encrypts this tunnel's build records according to the spec.
                 */
                void secureRecords();

                std::list<BuildRecordPtr> m_hops;
                State m_state = State::REQUESTED;
                uint32_t m_tunnelId;
                uint32_t m_nextMsgId;

                std::unique_ptr<boost::asio::deadline_timer> m_timer;

                i2p_logger_mt m_log;
        };

        typedef std::shared_ptr<Tunnel> TunnelPtr;
    }
}

#endif
