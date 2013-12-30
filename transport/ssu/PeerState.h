/**
 * @file PeerState.h
 * @brief Defines the i2pcpp::SSU::PeerState type.
 */
#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include <boost/asio.hpp>

#include "../../datatypes/RouterHash.h"
#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"

namespace i2pcpp {
    namespace SSU {
        /**
         * Stores the state of a particular peer.
         */
        class PeerState {
            public:
                /**
                 * Constructs given the endpoint of the peer and its
                 *  i2pcpp::RouterHash.
                 */
                PeerState(Endpoint const &ep, RouterHash const &rh);

                /**
                 * @return the session key, used for AES-256
                 */
                SessionKey getCurrentSessionKey() const;

                /**
                 * @return the HMAC key
                 */
                SessionKey getCurrentMacKey() const;

                /**
                 * @return the pending AES-256 key during rekeying. 
                 */
                SessionKey getNextSessionKey() const;

                /**
                 * @return the pending HMAC key during rekeying. 
                 */
                SessionKey getNextMacKey() const;

                /**
                 * Sets the current AES-256 session key to \a sk.
                 */
                void setCurrentSessionKey(SessionKey const &sk);

                /**
                 * Sets the current HMAC key to \a mk.
                 */
                void setCurrentMacKey(SessionKey const &mk);

                /**
                 * Sets the pending AES-256 session key to \a sk.
                 */
                void setNextSessionKey(SessionKey const &sk);
                
                /**
                 * Sets the pending HMAC key to \a mk.
                 */
                void setNextMacKey(SessionKey const &mk);

                /**
                 * @return the i2cpp::RouterHash associated with this peer.
                 */
                RouterHash getHash() const;

                /**
                 * @return the i2cpp::Endpoint associated with this peer.
                 */
                Endpoint getEndpoint() const;

            private:
                Endpoint m_endpoint;
                RouterHash m_routerHash;

                SessionKey m_sessionKey;
                SessionKey m_macKey;
                SessionKey m_nextSessionKey;
                SessionKey m_nextMacKey;
        };

        typedef std::shared_ptr<PeerState> PeerStatePtr;
    }
}

#endif
