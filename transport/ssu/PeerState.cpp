#include "PeerState.h"

#include <boost/bind.hpp>

#include "../../util/make_unique.h"

#include "../../Log.h"

namespace i2pcpp {
    namespace SSU {
        PeerState::PeerState(Endpoint const &ep, RouterHash const &rh) :
            m_endpoint(ep),
            m_routerHash(rh) {}

        SessionKey PeerState::getCurrentSessionKey() const
        {
            return m_sessionKey;
        }

        SessionKey PeerState::getCurrentMacKey() const
        {
            return m_macKey;
        }

        SessionKey PeerState::getNextSessionKey() const
        {
            return m_nextSessionKey;
        }

        SessionKey PeerState::getNextMacKey() const
        {
            return m_nextMacKey;
        }

        void PeerState::setCurrentSessionKey(SessionKey const &sk)
        {
            m_sessionKey = sk;
        }

        void PeerState::setCurrentMacKey(SessionKey const &mk)
        {
            m_macKey = mk;
        }

        void PeerState::setNextSessionKey(SessionKey const &sk)
        {
            m_nextSessionKey = sk;
        }

        void PeerState::setNextMacKey(SessionKey const &mk)
        {
            m_nextMacKey = mk;
        }

        RouterHash PeerState::getHash() const
        {
            return m_routerHash;
        }

        Endpoint PeerState::getEndpoint() const
        {
            return m_endpoint;
        }
    }
}
