/**
 * @file EstablishmentState.h
 * Defines the i2pcpp::SSU::EstablishmentState type.
 */
#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <i2pcpp/datatypes/Endpoint.h>
#include <i2pcpp/datatypes/SessionKey.h>

#include <botan/botan.h>

namespace Botan { class DH_PrivateKey; class DSA_PrivateKey; }

namespace i2pcpp {
    class RouterIdentity;

    namespace SSU {

        /**
         * Stores the state for a particular session establishment operation.
         * @see i2pcpp::SSU::EstablishmentManager
         */
        class EstablishmentState {
            public:
                /**
                 * Constructs. Generates a Diffie-Hellman private key (exponent).
                 * @param dsaKey private key to create a certifcate in the
                 *  SessionCreated and SessionConfirmed messages.
                 * @param myIdentity identity of this router
                 * @param ep enpoint with which we are establishing a session
                 */
                EstablishmentState(std::shared_ptr<const Botan::DSA_PrivateKey> const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep);

                /**
                 * Constructs. Generates a Diffie-Hellman private key (exponent).
                 * @param dsaKey private key to create a certifcate in the
                 *  SessionCreated and SessionConfirmed messages.
                 * @param myIdentity identity of this router
                 * @param theirIdentity identity of router with which we are
                 *  establishing a session
                 */
                EstablishmentState(std::shared_ptr<const Botan::DSA_PrivateKey> const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep, RouterIdentity const &theirIdentity);

                EstablishmentState(EstablishmentState const &state) = delete;
                ~EstablishmentState();

                /**
                 * The direction can be either INBOUND (this router receives a
                 *  request to establish a session) or OUTBOUND (this router
                 *  makes a request to establish a session).
                 */
                enum class Direction {
                    INBOUND,
                    OUTBOUND
                };

                /**
                 * Defines all possible states the establishment can be in.
                 */
                enum class State {
                    UNKNOWN,
                    REQUEST_SENT,
                    REQUEST_RECEIVED,
                    CREATED_SENT,
                    CREATED_RECEIVED,
                    CONFIRMED_SENT,
                    CONFIRMED_RECEIVED,
                    FAILURE
                };

                /**
                 * @return the direction of this establishment operation
                 */
                Direction getDirection() const;

                /**
                 * @return the current state of the operation
                 */
                State getState() const;

                /**
                 * Changes the state to a given value.
                 * @param state the new state
                 */
                void setState(State state);

                /**
                 * @return the initialization vector to be used for AES (mode
                 *  is CBC).
                 */
                Botan::InitializationVector getIV() const;

                /**
                 * Changes the initialization vector to an i2pcpp::ByteArray
                 *  given by its begin and end iterators.
                 */
                void setIV(ByteArrayConstItr iv_begin, ByteArrayConstItr iv_end);

                /**
                 * @return the session key for AES.
                 */
                const SessionKey& getSessionKey() const;

                /**
                 * Changes the session key to a given key.
                 */
                void setSessionKey(SessionKey const &sk);

                /**
                 * @return the key for the keyed-hashed message authentication
                 *  code (HMAC).
                 */
                const SessionKey& getMacKey() const;

                /**
                 * Changes the MAC key to a given key.
                 */
                void setMacKey(SessionKey const &mk);

                /**
                 * @return the endpoint of the router we are establishing
                 *  a connection with
                 */
                const Endpoint& getTheirEndpoint() const;

                /**
                 * Changes the endpoint of this router a given enpoint object.
                 */
                void setMyEndpoint(Endpoint const &ep);

                uint32_t getRelayTag() const;
                void setRelayTag(const uint32_t rt);

                /**
                 * @return the i2pcpp::RouterIdentity of the router we are
                 *  establishing a connection with
                 */
                const RouterIdentity& getTheirIdentity() const;

                /**
                 * Changes the i2pcpp::RouterIdentity of the router we are
                 *  establishing a connection with
                 */
                void setTheirIdentity(RouterIdentity const &ri);

                /**
                 * @return the i2pcpp::RouterIdentity of this router.
                 */
                const RouterIdentity& getMyIdentity() const;

                /**
                 * @return the public DH key of this router.
                 */
                ByteArray getMyDH() const;

                /**
                 * Changes the public DH key of the router we are establishing a
                 *  connection with.
                 */
                void setTheirDH(ByteArrayConstItr dh_begin, ByteArrayConstItr dh_end);

                /**
                 * Sets the timestamp for in the DSA certificate.
                 */
                void setSignatureTimestamp(const uint32_t ts);

                /**
                 * Sets the certifcate to an i2pcpp::ByteArray given iterators
                 *  to its begin and end.
                 */
                void setSignature(ByteArrayConstItr sig_begin, ByteArrayConstItr sig_end);

                /**
                 * Computes the DSA certificate for the SessionCreated message.
                 * Hashing algorithm used is SHA1.
                 * This is the concacenation of (in order):
                 *  their DH public key,
                 *  my DH public key,
                 *  their IP and port,
                 *  my IP and port,
                 *  their relay tag,
                 *  timestamp when certificate was created
                 */
                ByteArray calculateCreationSignature(const uint32_t signedOn);

                /**
                 * Computes the DSA certificate for the SessionConfirmed message.
                 * Hashing algorithm used is SHA1.
                 * This is the concacenation of (in order):
                 *  my DH public key,
                 *  their DH public key,
                 *  my IP and port,
                 *  their IP and port,
                 *  my relay tag,
                 *  timestamp when certificate was created
                 */
                ByteArray calculateConfirmationSignature(const uint32_t signedOn) const;

                /**
                 * Verifies the certificate in the SessionCreated message.
                 * @return true if it is correct, false otherwise
                 */
                bool verifyCreationSignature() const;

                /**
                 * Verifies the certificate in the SessionConfirmation message.
                 * @return true if it is correct, false otherwise
                 */
                bool verifyConfirmationSignature() const;

                /**
                 * Computes the Diffie-Hellman shared secret and stores it in
                 *  i2pcpp::SSU::EstablishmentState::m_dhSecret.
                 */
                void calculateDHSecret();

                /**
                 * @return the DH shared secret as an i2pcpp::ByteArray.
                 * @note should only be called after caluclating the shared secret
                 */
                const ByteArray& getDHSecret() const;

            private:
                /// The current state, by default State::UNKNOWN
                State m_state = State::UNKNOWN;
                /// The direction of establishment
                Direction m_direction;

                /// DSA private key used to create certifcates
                const std::shared_ptr<const Botan::DSA_PrivateKey> m_dsaKey;
                /// Identity of this router
                const RouterIdentity& m_myIdentity;
                /// Endpoint of this router
                Endpoint m_myEndpoint;

                /// IV for AES (CBC mode)
                Botan::InitializationVector m_iv;
                /// Diffie-Hellman private key (exponent)
                Botan::DH_PrivateKey *m_dhKey;
                /// Diffie-Hellman shared secret
                ByteArray m_dhSecret;
                /// AES session key
                SessionKey m_sessionKey;
                /// HMAC key
                SessionKey m_macKey;

                /// Identity of ther router we are establishing session with
                std::shared_ptr<RouterIdentity> m_theirIdentity;
                /// Endpoint of router we are establishing session with
                Endpoint m_theirEndpoint;
                /// Public Diffie-Hellman key of router we are establishing session with
                ByteArray m_theirDH;

                uint32_t m_relayTag;
                uint32_t m_signatureTimestamp;
                ByteArray m_signature;
        };

        typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
    }
}

#endif
