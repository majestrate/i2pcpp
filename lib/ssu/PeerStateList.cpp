/**
 * @file PeerStateList.cpp
 * Implements PeerStateList.h
 */
#include "PeerStateList.h"

#include "../../include/i2pcpp/transports/SSU.h"

#include <i2pcpp/util/make_unique.h>

namespace i2pcpp {
    namespace SSU {
        PeerStateList::PeerStateList(SSU &transport) :
            m_transport(transport) {}

        void PeerStateList::addPeer(PeerState ps)
        {
            PeerStateContainer psc(ps);

            auto timer = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 20, 0));
            timer->async_wait(boost::bind(&PeerStateList::timerCallback, this, boost::asio::placeholders::error, ps.getHash()));
            psc.timer = std::move(timer);

            auto itr = m_container.get<1>().find(ps.getHash());
            if(itr == m_container.get<1>().end())
                m_container.insert(std::move(psc));
            else
                m_container.get<1>().replace(itr, std::move(psc));
        }

        PeerState PeerStateList::getPeer(Endpoint const &ep)
        {
            auto itr = m_container.get<0>().find(ep);
            if(itr == m_container.get<0>().end())
                throw std::runtime_error("record not found");

            return itr->state;
        }

        PeerState PeerStateList::getPeer(RouterHash const &rh)
        {
            auto itr = m_container.get<1>().find(rh);
            if(itr == m_container.get<1>().end())
                throw std::runtime_error("record not found");

            return itr->state;
        }

        void PeerStateList::delPeer(Endpoint const &ep)
        {
            m_container.get<0>().erase(ep);
        }

        void PeerStateList::delPeer(RouterHash const &rh)
        {
            m_container.get<1>().erase(rh);
        }

        bool PeerStateList::peerExists(Endpoint const &ep) const
        {
            return (m_container.get<0>().count(ep) > 0);
        }

        bool PeerStateList::peerExists(RouterHash const &rh) const
        {
            return (m_container.get<1>().count(rh) > 0);
        }

        void PeerStateList::resetPeerTimer(RouterHash const &rh)
        {
            auto itr = m_container.get<1>().find(rh);
            if(itr == m_container.get<1>().end())
                throw std::runtime_error("record not found");

            itr->timer->expires_at(itr->timer->expires_at() + boost::posix_time::time_duration(0, 20, 0));
            itr->timer->async_wait(boost::bind(&PeerStateList::timerCallback, this, boost::asio::placeholders::error, rh));
        }

        uint32_t PeerStateList::numPeers() const
        {
            return m_container.size();
        }

        PeerStateList::const_iterator PeerStateList::cbegin() const
        {
            return m_container.get<0>().cbegin();
        }

        PeerStateList::const_iterator PeerStateList::cend() const
        {
            return m_container.get<0>().cend();
        }

        void PeerStateList::timerCallback(const boost::system::error_code& e, RouterHash const rh)
        {
            if(!e)
                m_transport.disconnect(rh);
        }

        std::mutex& PeerStateList::getMutex() const
        {
            return m_mutex;
        }
    }
}
