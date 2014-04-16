/**
 * @file SSU.cpp
 * @brief Implements SSU.h
 */
#include "../../include/i2pcpp/transports/SSU.h"

#include "Context.h"
#include "Packet.h"

#include <i2pcpp/datatypes/RouterInfo.h>

#include <i2pcpp/util/make_unique.h>

namespace i2pcpp {
    namespace SSU {
        SSU::SSU(std::shared_ptr<Botan::DSA_PrivateKey> const &dsaPrivKey, RouterIdentity const &ri) :
            Transport()
        {
            m_impl = std::make_unique<Context>(dsaPrivKey, ri);
        }

        SSU::~SSU()
        {
            shutdown();
        }

        void SSU::start(Endpoint const &ep)
        {
            try {
                if(ep.getUDPEndpoint().address().is_v4())
                    m_impl->socket.open(boost::asio::ip::udp::v4());
                else if(ep.getUDPEndpoint().address().is_v6())
                    m_impl->socket.open(boost::asio::ip::udp::v6());

                m_impl->socket.bind(ep.getUDPEndpoint());

                I2P_LOG(m_impl->log, info) << "listening on " << ep;

                m_impl->serviceThread = std::thread([&](){
                    while(1) {
                        try {
                            m_impl->ios.run();
                            break;
                        } catch(std::exception &e) {
                            I2P_LOG(m_impl->log, error) << "exception thrown: " << e.what();
                        }
                    }
                });
            } catch(boost::system::system_error &e) {
                shutdown();
                throw;
            }
        }

        void SSU::connect(RouterInfo const &ri)
        {
            try {
                for(auto a: ri) {
                    if(a.getTransport() == "SSU") {
                        const Mapping& m = a.getOptions();

                        // Skip if the host and port are not valid. TODO Is this the best way?
                        if(!m.getValue("host").size() || !m.getValue("port").size())
                            continue;

                        Endpoint ep(m.getValue("host"), stoi(m.getValue("port")));
                        RouterIdentity id = ri.getIdentity();

                        std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

                        if(m_impl->establishmentManager.stateExists(ep) || m_impl->peers.peerExists(ep))
                            return;

                        m_impl->establishmentManager.createState(ep, id);

                        I2P_LOG_SCOPED_TAG(m_impl->log, "Endpoint", ep);
                        I2P_LOG_SCOPED_TAG(m_impl->log, "RouterHash", id.getHash());
                        I2P_LOG(m_impl->log, debug) << "attempting to establish session";

                        break; // Only connect to the first address
                    }
                }
            } catch(std::exception &e) {
                I2P_LOG(m_impl->log, error) << "exception thrown: " << e.what();
            }
        }

        void SSU::send(RouterHash const &rh, uint32_t msgId, ByteArray const &data)
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            if(m_impl->peers.peerExists(rh)) {
                PeerState ps = m_impl->peers.getPeer(rh);

                m_impl->omf.sendData(ps, msgId, data);
            } else {
                // TODO Exception
            }
        }

        void SSU::disconnect(RouterHash const &rh)
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            if(m_impl->peers.peerExists(rh)) {
                const PeerState& ps = m_impl->peers.getPeer(rh);

                PacketPtr p = PacketBuilder::buildSessionDestroyed(ps.getEndpoint());
                p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
                m_impl->sendPacket(p);

                m_impl->peers.delPeer(rh);
            }
        }

        uint32_t SSU::numPeers() const
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            return m_impl->peers.numPeers();
        }

        bool SSU::isConnected(RouterHash const &rh) const
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            return m_impl->peers.peerExists(rh);
        }

        void SSU::shutdown()
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            for(auto itr = m_impl->peers.cbegin(); itr != m_impl->peers.cend(); ++itr) {
                PacketPtr sdp = PacketBuilder::buildSessionDestroyed(itr->getEndpoint());
                sdp->encrypt(itr->state.getCurrentSessionKey(), itr->state.getCurrentMacKey());
                m_impl->sendPacket(sdp);
            }

            m_impl->ios.stop();
            if(m_impl->serviceThread.joinable()) m_impl->serviceThread.join();
        }
    }
}
