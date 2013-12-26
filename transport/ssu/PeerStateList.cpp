#include "PeerStateList.h"

#include "../UDPTransport.h"

namespace i2pcpp {
    namespace SSU {
        PeerStateList::PeerStateList(UDPTransport &transport) :
            m_transport(transport) {}

        void PeerStateList::addPeer(PeerState ps)
        {
            auto itr = m_container.get<1>().find(ps.getHash());
            if(itr == m_container.get<1>().end())
                m_container.insert(std::move(ps));
            else
                m_container.get<1>().replace(itr, std::move(ps));
        }

        PeerState PeerStateList::getPeer(Endpoint const &ep)
        {
            auto itr = m_container.get<0>().find(ep);
            if(itr == m_container.get<0>().end())
                throw std::runtime_error("record not found");

            return *itr;
        }

        PeerState PeerStateList::getPeer(RouterHash const &rh)
        {
            auto itr = m_container.get<1>().find(rh);
            if(itr == m_container.get<1>().end())
                throw std::runtime_error("record not found");

            return *itr;
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

        void PeerStateList::timerCallback(const boost::system::error_code& e, RouterHash const &rh)
        {
            if(!e) {
                std::lock_guard<std::mutex> lock(m_mutex);
                delPeer(rh);
            }
        }

        std::mutex& PeerStateList::getMutex() const
        {
            return m_mutex;
        }
    }
}
