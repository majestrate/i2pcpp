/**
 * @file EstablishmentManager.h
 * @brief Defines the i2pcpp::SSU::EstablishmentManager helper class.
 */
#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <i2pcpp/Log.h>

#include <i2pcpp/datatypes/Endpoint.h>
#include <i2pcpp/datatypes/RouterIdentity.h>

#include <botan/dsa.h>

#include <unordered_map>

namespace i2pcpp {
    class UDPTransport;
    class Endpoint;

    namespace SSU {
        class EstablishmentState; typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;

        /**
         * Manages session establishment.
         * @todo Implement indirect establishment.
         */
        class EstablishmentManager {
            public:
                /**
                 * Constructs given a reference to the i2pcpp::SSU::UDPTransport.
                 * @param privKey DSA private key of this router used to create
                 *  the certificate in the SessionConfirmed packet.
                 * @param ri the identity of the router we are establishing a
                 *  session here.
                 */
                EstablishmentManager(UDPTransport &transport, Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri);
                EstablishmentManager(const EstablishmentManager &) = delete;
                EstablishmentManager& operator=(EstablishmentManager &) = delete;

                /**
                 * Creates a state for a given i2pcpp::Endpoint \a ep.
                 * @return the newly created i2pcpp::SSU::EstablishmentState
                 */
                EstablishmentStatePtr createState(Endpoint const &ep);

                /**
                 * Creates a state for a given i2pcpp::Endpoint \a ep.
                 */
                void createState(Endpoint const &ep, RouterIdentity const &ri);

                /**
                 * @return true if there exists a state for the i2pcpp::Endpoint
                 *  given (\a ep)
                 */
                bool stateExists(Endpoint const &ep) const;

                /**
                 * Post a stateChanged task on the boost::asio::io_service.
                 * @param es object of which the state has been changed
                 * @see i2pcpp::SSU::UDPTranport::stateChanged
                 */
                void post(EstablishmentStatePtr const &es);

                /**
                 * Called when the state is changed.
                 * Mainly used for logging purposes.
                 * @see i2pcpp::SSU::UDPTranport::post
                 */
                void stateChanged(EstablishmentStatePtr es);

                /**
                 * @return a pointer to the sate associated with the given
                 *  enpoint \a ep.
                 */
                EstablishmentStatePtr getState(Endpoint const &ep) const;

            private:
                /**
                 * Removes the state associated with the enpoint \a ep.
                 */
                void delState(const Endpoint &ep);

                /**
                 * Called when establishment times out.
                 * Changes the state to EstablishmentState::State::FAILURE.
                 * @see i2pcpp::SSU::EstablishmentManager::createState
                 */
                void timeoutCallback(const boost::system::error_code& e, EstablishmentStatePtr es);

                /**
                 * Sends the first request to initiate a session.
                 * Builds a SessionRequest packet, encrypts it, and sends it.
                 * Changes the state to EstablishmentState::State::REQUEST_SENT.
                 */
                void sendRequest(EstablishmentStatePtr const &state);

                /**
                 * Processes a SessionRequest packet.
                 * Builds a SessionCreated packet, encrypts it, and sends it.
                 * Initializes the state's i2pcpp::SessionKey after computing the
                 *  Diffie-Hellman shared secret.
                 * Changes the state to EstablishmentState::State::CREATED_SENT.
                 */
                void processRequest(EstablishmentStatePtr const &state);

                /**
                 * Processes a SessionCreated packet.
                 * Verfies the DSA certficate in the SessionCreated packet.
                 * If it is correct, creates an i2pcpp::SSU::PeerState object
                 *  and adds it to the i2pcpp::UDPTranport's
                 *  i2pcpp::SSU::PeerStateList.
                 * Builds a SessionConfirmed packet, encrypts it, and sends it.
                 * Changes the state to EstablishmentState::State::CONFIRMED_SENT.
                 */
                void processCreated(EstablishmentStatePtr const &state);

                /**
                 * Proceses a SessionConfirmed packet.
                 * Verfies the DSA cerficate in the SessionConfirmed packet.
                 * If it is correct, creates an i2pcpp::SSU::PeerState object
                 *  and adds it to the i2pcpp::UDPTranport's
                 *  i2pcpp::SSU::PeerStateList.
                 * Deletes the state object.
                 * Invokes the established signal of the i2pcpp::UDPTransport.
                 */
                void processConfirmed(EstablishmentStatePtr const &state);

                UDPTransport &m_transport;

                Botan::DSA_PrivateKey m_privKey;
                RouterIdentity m_identity;

                std::unordered_map<Endpoint, EstablishmentStatePtr> m_stateTable;
                std::unordered_map<Endpoint, std::unique_ptr<boost::asio::deadline_timer>> m_stateTimers;
                /// Mutex object for i2pcpp::SSU::EstablismentManager::m_stateTable
                mutable std::mutex m_stateTableMutex;

                /// Logging object
                i2p_logger_mt m_log;
        };
    }
}

#endif
