#include "Manager.h"

#include "InboundTunnel.h"
#include "OutboundTunnel.h"
#include "Message.h"
#include "Fragment.h"
#include "FragmentHandler.h"

#include "../RouterContext.h"

#include "../i2np/VariableTunnelBuild.h"
#include "../i2np/VariableTunnelBuildReply.h"
#include "../i2np/TunnelData.h"
#include "../i2np/TunnelGateway.h"

#include <i2pcpp/util/make_unique.h>
#include <i2pcpp/datatypes/RouterInfo.h>

#include <botan/auto_rng.h>

namespace i2pcpp {
    namespace Tunnel {
        Manager::Manager(boost::asio::io_service &ios, RouterContext &ctx) :
            m_ios(ios),
            m_ctx(ctx),
            m_fragmentHandler(ios, ctx),
            m_graceful(false),
            m_timer(m_ios, boost::posix_time::time_duration(0, 0, 1)),
            m_log(boost::log::keywords::channel = "TM") {}

        void Manager::begin()
        {
            m_timer.async_wait(boost::bind(&Manager::callback, this, boost::asio::placeholders::error));
        }

        void Manager::receiveRecords(uint32_t const msgId, std::list<BuildRecordPtr> records)
        {
            bool tunnelSuccess = false;
            TunnelPtr t;
            /* First check to see if we have a pending tunnel for this msgId */
            {
                std::lock_guard<std::mutex> lock(m_pendingMutex);
                auto itr = m_pending.find(msgId);
                if(itr != m_pending.end()) {
                    t = itr->second;

                    if(t->getState() != Tunnel::State::REQUESTED) {
                        I2P_LOG(m_log, debug) << "found Tunnel with matching tunnel ID, but was not requested";
                        // reject
                        return;
                    }
                    
                    t->handleResponses(records);
                    if(t->getState() == Tunnel::State::OPERATIONAL) {
                        I2P_LOG(m_log, debug) << "tunnel is operational";
                        
                        std::lock_guard<std::mutex> lock(m_tunnelsMutex);
                        m_tunnels[t->getTunnelId()] = std::move(t);
                        
                        tunnelSuccess = true;
                    } else {
                        I2P_LOG(m_log, debug) << "failed to build tunnel";
                    }

                    m_pending.erase(itr);
                    /*
                     * call tunnel build hooks
                     */
                    auto tunnelId = t->getTunnelId();
                    if (tunnelSuccess) {
                        m_ios.post(boost::bind(&Manager::onTunnelBuildSuccess, this, tunnelId));
                    } else {
                        m_ios.post(boost::bind(&Manager::onTunnelBuildFailure, this, tunnelId));
                    }
                }
                return;
            }

            /* If we don't, then check to see if any of the records have a truncated
             * hash that matches ours.
             */
            RouterHash myHash = m_ctx.getIdentity()->getHash();
            StaticByteArray<16> myTruncatedHash;
            std::copy(myHash.cbegin(), myHash.cbegin() + 16, myTruncatedHash.begin());

            auto itr = std::find_if(records.begin(), records.end(), [myTruncatedHash](BuildRecordPtr const &r) { return (myTruncatedHash == r->getHeader()); });
            if(itr != records.end()) {
                I2P_LOG(m_log, debug) << "found BRR with our identity";

                /* We found a record that belongs to us. Let's decrypt and parse it. */
                auto req = std::make_shared<BuildRequestRecord>(**itr);
                req->decrypt(m_ctx.getEncryptionKey());
                req->parse();

                std::lock_guard<std::mutex> lock(m_participatingMutex);
                if(m_participating.count(req->getTunnelId()) > 0) {
                    I2P_LOG(m_log, debug) << "rejecting tunnel participation request: tunnel ID in use";
                    // reject
                    return;
                }

                auto timer = std::make_unique<boost::asio::deadline_timer>(m_ios, boost::posix_time::time_duration(0, 10, 0));
                timer->async_wait(boost::bind(&Manager::timerCallback, this, boost::asio::placeholders::error, true, req->getTunnelId()));

                auto p = std::make_pair(req, std::move(timer));
                m_participating[req->getTunnelId()] = std::move(p);

                /* Now we generate a SUCCESS reponse which will get sent to the next hop in the chain. */
                BuildResponseRecordPtr resp;
                resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::Reply::SUCCESS);
                resp->compile();
                *itr = std::move(resp); // This replaces our request record with the response in-place.

                for(auto& x: records)
                    x->encrypt(req->getReplyIV(), req->getReplyKey());

                /* If we're the endpoint, wrap the records in a Tunnel Gateway message before
                 * sending it to the next hop.
                 */
                if(req->getType() == BuildRequestRecord::Type::ENDPOINT) {
                    I2P_LOG(m_log, debug) << "forwarding BRRs to IBGW: " << req->getNextHash() << ", tunnel ID: " << req->getNextTunnelId() << ", nextMsgId: " << req->getNextMsgId();

                    I2NP::MessagePtr vtbr(new I2NP::VariableTunnelBuildReply(req->getNextMsgId(), records));
                    I2NP::MessagePtr tg(new I2NP::TunnelGateway(req->getNextTunnelId(), vtbr->toBytes()));
                    m_ctx.getOutMsgDisp().sendMessage(req->getNextHash(), tg);
                } else {
                    I2P_LOG(m_log, debug) << "forwarding BRRs to next hop: " << req->getNextHash() << ", tunnel ID: " << req->getNextTunnelId() << ", nextMsgId: " << req->getNextMsgId();

                    I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(req->getNextMsgId(), records));
                    m_ctx.getOutMsgDisp().sendMessage(req->getNextHash(), vtb);
                }
            }
        }

        void Manager::receiveGatewayData(RouterHash const from, uint32_t const tunnelId, ByteArray const data)
        {
            I2P_LOG_SCOPED_TAG(m_log, "TunnelId", tunnelId);
            I2P_LOG(m_log, debug) << "received " << data.size() << " bytes of gateway data";

            {
                std::lock_guard<std::mutex> lock(m_participatingMutex);
                auto itr = m_participating.find(tunnelId);
                if(itr != m_participating.end()) {
                    BuildRequestRecordPtr hop = itr->second.first;

                    if(hop->getType() != BuildRequestRecord::Type::GATEWAY) {
                        I2P_LOG(m_log, debug) << "data is for a tunnel which is not a gateway, dropping";
                        return;
                    }

                    I2P_LOG(m_log, debug) << "data is for a known tunnel, encrypting and forwarding";
                    SessionKey k1 = hop->getTunnelIVKey();
                    Botan::SymmetricKey ivKey(k1.data(), k1.size());
                    SessionKey k2 = hop->getTunnelLayerKey();
                    Botan::SymmetricKey layerKey(k2.data(), k2.size());

                    auto fragments = Fragment::fragmentMessage(data);
                    I2P_LOG(m_log, debug) << "we have " << fragments.size() << " fragments";

                    for(auto& f: fragments) {
                        I2P_LOG(m_log, debug) << "fragment: " << f->compile();

                        std::list<FragmentPtr> x;
                        x.push_back(std::move(f)); // This may or may not be unsafe

                        Message msg(x);
                        msg.compile();
                        msg.encrypt(ivKey, layerKey);
                        I2NP::MessagePtr td(new I2NP::TunnelData(hop->getNextTunnelId(), msg.getEncryptedData()));
                        m_ctx.getOutMsgDisp().sendMessage(hop->getNextHash(), td);
                    }

                    return;
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_tunnelsMutex);
                auto itr = m_tunnels.find(tunnelId);
                if(itr != m_tunnels.end()) {
                    TunnelPtr t = itr->second;

                    if(t->getDirection() == Tunnel::Direction::INBOUND && t->getState() == Tunnel::State::OPERATIONAL) {
                        I2P_LOG(m_log, debug) << "data is for one of our own tunnels, recirculating";
                        std::shared_ptr<InboundTunnel> ibt = std::dynamic_pointer_cast<InboundTunnel>(t);

                        m_ios.post(boost::bind(&InboundMessageDispatcher::messageReceived, boost::ref(m_ctx.getInMsgDisp()), from, 0, data));
                    }
                }
            }
        }

        void Manager::receiveData(RouterHash const from, uint32_t const tunnelId, StaticByteArray<1024> const data)
        {
            std::lock_guard<std::mutex> lock(m_participatingMutex);

            I2P_LOG_SCOPED_TAG(m_log, "TunnelId", tunnelId);
            I2P_LOG(m_log, debug) << "received " << data.size() << " bytes of tunnel data";

            auto itr = m_participating.find(tunnelId);
            if(itr != m_participating.end()) {
                I2P_LOG(m_log, debug) << "data is for a known tunnel";

                BuildRequestRecordPtr hop = itr->second.first;

                SessionKey k1 = hop->getTunnelIVKey();
                Botan::SymmetricKey ivKey(k1.data(), k1.size());

                SessionKey k2 = hop->getTunnelLayerKey();
                Botan::SymmetricKey layerKey(k2.data(), k2.size());

                Message msg(data);
                msg.encrypt(ivKey, layerKey);

                switch(hop->getType()) {
                    case BuildRequestRecord::Type::PARTICIPANT:
                        {
                            I2P_LOG(m_log, debug) << "we are a participant, forwarding";

                            I2NP::MessagePtr td(new I2NP::TunnelData(hop->getNextTunnelId(), msg.getEncryptedData()));
                            m_ctx.getOutMsgDisp().sendMessage(hop->getNextHash(), td);
                        }

                        break;

                    case BuildRequestRecord::Type::ENDPOINT:
                        {
                            I2P_LOG(m_log, debug) << "we are an endpoint, sending to fragment handler";

                            m_fragmentHandler.receiveFragments(msg.parse());
                        }

                        break;

                    default:
                        break;
                }
            } else
                I2P_LOG(m_log, debug) << "data is for an unknown tunnel, dropping";
        }

        void Manager::timerCallback(const boost::system::error_code &e, bool participating, uint32_t tunnelId)
        {
            if(participating) {
                std::lock_guard<std::mutex> lock(m_participatingMutex);
                m_participating.erase(tunnelId);
            } else {
                std::lock_guard<std::mutex> lock(m_tunnelsMutex);
                m_tunnels.erase(tunnelId);
            }
        }

        void Manager::tunnelBuildExpireCallback(const boost::system::error_code & e, uint32_t tunnelId)
        {
            I2P_LOG(m_log, info) << "tunnel build with tunnelId " << std::to_string(tunnelId) << " timed out";
            m_ios.post(boost::bind(&Manager::onTunnelBuildTimeout, this, tunnelId));
            
        }

        void Manager::callback(const boost::system::error_code &e)
        {
            auto count = getParticipatingTunnelCount();
            I2P_LOG(m_log, debug) << "we have " << std::to_string(count) << " participating tunnels";
            I2P_LOG(m_log, debug) << boost::log::add_value("tunnel.participating", (uint32_t) count);

            if ( count == 0 && m_graceful ) { 
                I2P_LOG(m_log, info) << "no more participating tunnels, we can now die";
                return;
            }

            
            m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 5));
            m_timer.async_wait(boost::bind(&Manager::callback, this, boost::asio::placeholders::error));
        
        }

        void Manager::createTunnel()
        {
            I2P_LOG(m_log, debug) << "creating tunnel";
            std::vector<RouterIdentity> hops = { m_ctx.getProfileManager().getPeer().getIdentity() };
            /*
            auto z = std::make_shared<InboundTunnel>(m_ctx.getIdentity()->getHash());
            auto t = std::make_shared<OutboundTunnel>(hops, m_ctx.getIdentity()->getHash(), z->getTunnelId());
            //auto t = std::make_shared<InboundTunnel>(m_ctx.getIdentity().getHash(), hops);
            m_tunnels[t->getTunnelId()] = z;
            m_pending[t->getNextMsgId()] = t;

            I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRecords()));
            m_ctx.getOutMsgDisp().sendMessage(t->getDownstream(), vtb);
            */
        }

        uint32_t Manager::buildIBTunnel(std::vector<RouterIdentity> & hops)
        {
            I2P_LOG(m_log, info) << "build OB tunnel";

            auto my_hash = m_ctx.getIdentity()->getHash();
            auto tun = std::make_shared<InboundTunnel>(my_hash ,hops);
            {
                std::lock_guard<std::mutex> lock_tunnels(m_tunnelsMutex);
                m_tunnels[tun->getTunnelId()] = tun;
            }
            {
                std::lock_guard<std::mutex> lock_pending(m_pendingMutex);
                m_pending[tun->getTunnelId()] = tun;
            }

            auto tunnelId = tun->getTunnelId();
            
            I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(tun->getRecords()));
            m_ctx.getOutMsgDisp().sendMessage(tun->getDownstream(), vtb);
            
            // bug?
            boost::asio::deadline_timer tunnel_expire_timer(m_ios, boost::posix_time::time_duration(0,0,60));
            tunnel_expire_timer.async_wait(boost::bind(&Manager::tunnelBuildExpireCallback, this, boost::asio::placeholders::error, tunnelId));
            
            return tun->getTunnelId();
            
        }

        uint32_t Manager::buildOBTunnel(std::vector<RouterIdentity> & hops, RouterHash const & reply, uint32_t reply_tunnel_id )
        {
            I2P_LOG(m_log, info) << "build OB tunnel";

            auto tun = std::make_shared<OutboundTunnel>(hops, reply, reply_tunnel_id);
            {
                std::lock_guard<std::mutex> lock_tunnels(m_tunnelsMutex);
                m_tunnels[tun->getTunnelId()] = tun;
            }
            {
                std::lock_guard<std::mutex> lock_pending(m_pendingMutex);
                m_pending[tun->getTunnelId()] = tun;
            }
    
            auto tunnelId = tun->getTunnelId();

            I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(tun->getRecords()));
            m_ctx.getOutMsgDisp().sendMessage(tun->getDownstream(), vtb);
        
            // bug?
            boost::asio::deadline_timer tunnel_expire_timer(m_ios, boost::posix_time::time_duration(0,0,60));
            tunnel_expire_timer.async_wait(boost::bind(&Manager::tunnelBuildExpireCallback, this, boost::asio::placeholders::error, tunnelId));
            
            return tun->getTunnelId();
        }
        
        uint32_t Manager::getParticipatingTunnelCount()
        {
            std::lock_guard<std::mutex> lock_participating(m_participatingMutex);
            return m_participating.size();
        }
        
        void Manager::gracefulShutdown()
        {
            m_graceful = true;
        }
        
    }
}
