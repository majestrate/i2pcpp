#include "PeerStateList.h"

namespace i2pcpp {
	namespace SSU {
		void PeerStateList::addRemotePeer(PeerStatePtr const &ps)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);
			m_remotePeers[ps->getEndpoint()] = ps;
			m_remotePeersByHash[ps->getIdentity().getHash()] = ps;
		}

		PeerStatePtr PeerStateList::getRemotePeer(Endpoint const &ep) const
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				ps = itr->second;

			return ps;
		}

		PeerStatePtr PeerStateList::getRemotePeer(RouterHash const &rh) const
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeersByHash.find(rh);
			if(itr != m_remotePeersByHash.end())
				ps = itr->second;

			return ps;
		}

		void PeerStateList::delRemotePeer(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				m_remotePeersByHash.erase(itr->second->getIdentity().getHash());

			m_remotePeers.erase(ep);
		}

		void PeerStateList::delRemotePeer(RouterHash const &rh)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			auto itr = m_remotePeersByHash.find(rh);
			if(itr != m_remotePeersByHash.end())
				m_remotePeers.erase(itr->second->getEndpoint());

			m_remotePeersByHash.erase(rh);
		}

		std::unordered_map<RouterHash, PeerStatePtr>::const_iterator PeerStateList::begin() const
		{
			return m_remotePeersByHash.cbegin();
		}

		std::unordered_map<RouterHash, PeerStatePtr>::const_iterator PeerStateList::end() const
		{
			return m_remotePeersByHash.cend();
		}
	}
}
