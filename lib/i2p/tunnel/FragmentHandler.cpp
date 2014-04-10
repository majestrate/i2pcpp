#include "FragmentHandler.h"

#include "FirstFragment.h"

#include "../RouterContext.h"

#include "../i2np/TunnelGateway.h"

#include <i2pcpp/util/make_unique.h>

namespace i2pcpp {
    namespace Tunnel {
        FragmentHandler::FragmentHandler(boost::asio::io_service &ios, RouterContext &ctx) :
            m_ios(ios),
            m_ctx(ctx),
            m_log(boost::log::keywords::channel = "FH") {}

        void FragmentHandler::receiveFragments(std::list<FragmentPtr> fragments)
        {
            I2P_LOG(m_log, debug) << "got " << fragments.size() << " fragments";
            for(auto& f: fragments) {
                uint32_t msgId = f->getMsgId();

                if(typeid(*f) == typeid(FirstFragment)) {
                    I2P_LOG(m_log, debug) << "first fragment";

                    auto ff = std::unique_ptr<FirstFragment>(dynamic_cast<FirstFragment *>(f.release()));
                    if(!ff->isFragmented()) {
                        I2P_LOG(m_log, debug) << "not fragmented";

                        // We received a first fragment with no further fragments -- send it right out
                        I2NP::MessagePtr tg(new I2NP::TunnelGateway(ff->getTunnelId(), ff->getPayload()));
                        m_ctx.getOutMsgDisp().sendMessage(ff->getToHash(), tg);
                    } else {
                        I2P_LOG(m_log, debug) << "fragmented";

                        std::lock_guard<std::mutex> lock(m_statesMutex);

                        auto itr = m_states.find(msgId);
                        if(itr != m_states.end())
                            m_states[msgId].setFirstFragment(std::move(ff));
                        else {
                            FragmentState s;
                            s.setFirstFragment(std::move(ff));

                            auto timer = std::make_unique<boost::asio::deadline_timer>(m_ios, boost::posix_time::time_duration(0, 2, 0));
                            timer->async_wait(boost::bind(&FragmentHandler::timerCallback, this, boost::asio::placeholders::error, msgId));
                            s.setTimer(std::move(timer));

                            m_states[msgId] = std::move(s);
                        }
                    }
                } else {
                    I2P_LOG(m_log, debug) << "follow on fragment";

                    auto fof = std::unique_ptr<FollowOnFragment>(dynamic_cast<FollowOnFragment *>(f.release()));

                    std::lock_guard<std::mutex> lock(m_statesMutex);
                    auto itr = m_states.find(msgId);
                    if(itr != m_states.end())
                        m_states[msgId].addFollowOnFragment(std::move(*fof));
                    else {
                        FragmentState s;
                        s.addFollowOnFragment(std::move(*fof));
                        m_states[msgId] = std::move(s);
                    }
                }

                checkAndFlush(msgId);
            }
        }

        void FragmentHandler::checkAndFlush(uint32_t msgId)
        {
            std::lock_guard<std::mutex> lock(m_statesMutex);

            if(m_states[msgId].isComplete()) {
                I2P_LOG(m_log, debug) << "all fragments received";

                auto& ff = m_states[msgId].getFirstFragment();
                switch(ff->getDeliveryMode()) {
                    case FirstFragment::DeliveryMode::TUNNEL:
                        {
                            I2P_LOG(m_log, debug) << "destination: tunnel";

                            I2NP::MessagePtr tg(new I2NP::TunnelGateway(ff->getTunnelId(), m_states[msgId].compile()));
                            m_ctx.getOutMsgDisp().sendMessage(ff->getToHash(), tg);
                        }

                        break;

                    case FirstFragment::DeliveryMode::ROUTER:
                        {
                            I2P_LOG(m_log, debug) << "destination: router";

                            I2NP::MessagePtr msg = I2NP::Message::fromBytes(msgId, m_states[msgId].compile());
                            if(!msg)
                                throw std::runtime_error("error sending router message as an endpoint");

                            m_ctx.getOutMsgDisp().sendMessage(ff->getToHash(), msg);
                        }

                        break;

                    default:
                        break;
                }

                m_states.erase(msgId);
            }
        }

        void FragmentHandler::timerCallback(const boost::system::error_code& e, const uint32_t msgId)
        {
            std::lock_guard<std::mutex> lock(m_statesMutex);

            m_states.erase(msgId);
        }
    }
}
