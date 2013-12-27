#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include <mutex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include "InboundMessageState.h"
#include "PeerState.h"

#include "../../datatypes/ByteArray.h"

#include "../../Log.h"

namespace bmi = boost::multi_index;

namespace i2pcpp {
    class UDPTransport;

    namespace SSU {
        class InboundMessageFragments {
            friend class AcknowledgementManager;

            public:
                InboundMessageFragments(UDPTransport &transport);
                InboundMessageFragments(const InboundMessageFragments &) = delete;
                InboundMessageFragments& operator=(InboundMessageFragments &) = delete;

                void receiveData(RouterHash const &rh, ByteArrayConstItr &begin, ByteArrayConstItr end);

            private:
                UDPTransport &m_transport;

                void addState(const uint32_t msgId, const RouterHash &rh, InboundMessageState ims);
                void delState(const uint32_t msgId);
                void timerCallback(const boost::system::error_code& e, const uint32_t msgId);
                void checkAndPost(const uint32_t msgId, InboundMessageState const &ims);

                struct ContainerEntry {
                    ContainerEntry(InboundMessageState ims);
                    ContainerEntry(ContainerEntry &&) = default;
                    ContainerEntry& operator=(ContainerEntry &&) = default;

                    uint32_t msgId;
                    RouterHash hash;
                    InboundMessageState state;
                    std::unique_ptr<boost::asio::deadline_timer> timer;
                };

                class AddFragment {
                    public:
                        AddFragment(const uint8_t fragNum, ByteArray const &data, bool isLast);

                        void operator()(ContainerEntry &ce);

                    private:
                        uint8_t m_fragNum;
                        ByteArray m_data;
                        bool m_isLast;
                };

                typedef boost::multi_index_container<
                    ContainerEntry,
                    bmi::indexed_by<
                        bmi::hashed_unique<
                            bmi::member<ContainerEntry, uint32_t, &ContainerEntry::msgId>
                        >,
                        bmi::ordered_non_unique<
                            bmi::member<ContainerEntry, RouterHash, &ContainerEntry::hash>
                        >
                    >
                > StateContainer;

                StateContainer m_states;

                mutable std::mutex m_mutex;

                i2p_logger_mt m_log;
                // TODO Decaying bloom filter
        };
    }
}

#endif
