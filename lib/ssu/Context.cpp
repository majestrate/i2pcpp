#include "Context.h"
#include "Packet.h"

#include "../../include/i2pcpp/transports/SSU.h"

namespace i2pcpp {
    namespace SSU {
        Context::Context(SSU &s, std::shared_ptr<Botan::DSA_PrivateKey> const &dsaPrivKey, RouterIdentity const &ri) :
            self(s),
            establishedSignal(s.m_establishedSignal),
            receivedSignal(s.m_receivedSignal),
            failureSignal(s.m_failureSignal),
            disconnectedSignal(s.m_disconnectedSignal),
            socket(ios),
            peers(*this),
            packetHandler(*this, ri.getHash()),
            establishmentManager(*this, dsaPrivKey, ri),
            ackManager(*this),
            omf(*this),
            log(boost::log::keywords::channel = "SSU") {}

        void Context::sendPacket(PacketPtr const &p)
        {
            ByteArray& pdata = p->getData();
            Endpoint ep = p->getEndpoint();

            socket.async_send_to(
                    boost::asio::buffer(pdata.data(), pdata.size()),
                    ep.getUDPEndpoint(),
                    boost::bind(
                        &Context::dataSent,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        ep.getUDPEndpoint()
                        )
                    );
        }

        void Context::dataReceived(const boost::system::error_code& e, size_t n)
        {
            if(!e && n > 0) {
                Endpoint ep(senderEndpoint);

                I2P_LOG_SCOPED_TAG(log, "Endpoint", ep);
                I2P_LOG(log, debug) << "received " << n << " bytes";
                I2P_LOG(log, debug) << boost::log::add_value("received", (uint64_t)n);

                if(n >= Packet::MIN_PACKET_LEN) {
                    auto p = std::make_shared<Packet>(ep, receiveBuf.data(), n);
                    ios.post(boost::bind(&PacketHandler::packetReceived, &packetHandler, p));
                } else
                    I2P_LOG(log, debug) << "dropping short packet";
                socket.async_receive_from(
                        boost::asio::buffer(receiveBuf.data(), receiveBuf.size()),
                        senderEndpoint,
                        boost::bind(
                            &Context::dataReceived,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred
                            )
                        );
            } else {
                I2P_LOG(log, debug) << "error: " << e.message();
            }
        }

        void Context::dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep)
        {
            I2P_LOG_SCOPED_TAG(log, "Endpoint", Endpoint(ep));
            I2P_LOG(log, debug) << "sent " << n << " bytes";
            I2P_LOG(log, debug) << boost::log::add_value("sent", (uint64_t)n);
        }

        void Context::disconnect(RouterHash const &rh)
        {
            self.disconnect(rh);
        }
    }
}
