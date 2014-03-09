#ifndef TUNNEL_H
#define TUNNEL_H

#include "../Log.h"

#include <i2pcpp/datatypes/BuildRequestRecord.h>

#include <boost/asio.hpp>

#include <list>

namespace i2pcpp {
    class Tunnel {
        public:
            enum class Direction {
                INBOUND,
                OUTBOUND
            };

            enum class State {
                REQUESTED,
                OPERATIONAL,
                FAILED
            };

            virtual ~Tunnel() {}

            virtual Direction getDirection() const = 0;

            State getState() const;
            uint32_t getTunnelId() const;
            std::list<BuildRecordPtr> getRecords() const;
            RouterHash getDownstream() const;
            uint32_t getNextMsgId() const;
            void handleResponses(std::list<BuildRecordPtr> const &records);

            void setTimer(std::unique_ptr<boost::asio::deadline_timer> t);

        protected:
            Tunnel() = default;
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

#endif
