/**
 * @file InboundMessageFragments.h
 * @brief Defines the InboundMessageFragments class.
 */
#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include "../../Log.h"

#include "InboundMessageState.h"

#include <i2pcpp/datatypes/ByteArray.h>

#include <boost/asio.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <mutex>

namespace bmi = boost::multi_index;

namespace i2pcpp {
    class UDPTransport;

    namespace SSU {
        /**
         * Manages (fragments) of messages received by this router.
         */
        class InboundMessageFragments {
            friend class AcknowledgementManager;

            public:
                /**
                 * Constructs from a reference the the i2pcpp::UDPTranport class.
                 */
                InboundMessageFragments(UDPTransport &transport);
                InboundMessageFragments(const InboundMessageFragments &) = delete;
                InboundMessageFragments& operator=(InboundMessageFragments &) = delete;

                /**
                 * Parses raw, just received, data (given by iterators to the
                 *  begin and end of an i2pcpp::ByteArray).
                 * Extracts the flag (first byte) from the data message, and
                 *  based on this does:
                 * (flag 7) remove states in i2pcpp::SSU::OutboundMessageFragements for
                 *  all of the exlplicitly ACK'd messages;
                 * (flag 6) marks all the framgments of a message ACK'd based on
                 *  the bitfields given for each msgId.
                 * Then reads the number of fragments (1B) and reads that many
                 *  fragments, consisting of a msgId (4B), fragment info (3B) and
                 *  the actual data. InboundMessageFragments::checkAndPost and
                 *  InboundMessageFragments::addState.
                 * @param rh i2pcpp::RouterHash of the sending router
                 * @param begin iterator to the begin of the received data
                 * @param end iterator to the end of the received data
                 */
                void receiveData(RouterHash const &rh, ByteArrayConstItr &begin, ByteArrayConstItr end);

            private:
                UDPTransport &m_transport;

                /**
                 * Adds a new i2pcpp::SSU::InboundMessageState to the state container
                 *  InboundMessageFragment::m_states.
                 */
                void addState(const uint32_t msgId, const RouterHash &rh, InboundMessageState ims);

                /**
                 * Removes a state for the message given by \a msgId from the state
                 *  container InboundMessageFragment::m_states.
                 */
                void delState(const uint32_t msgId);

                /**
                 * Called when the timer's deadline ends. If no error occured, removes
                 *  the state given by \a msgId from the state container.
                 */
                void timerCallback(const boost::system::error_code& e, const uint32_t msgId);

                /**
                 * Checks whether all fragements for a given state \a ims have
                 *  been receieved and, if so, posts to the IO service that the
                 *  received signal (in i2pcpp::UDPTransport) should be invoked.
                 */
                void checkAndPost(const uint32_t msgId, InboundMessageState const &ims);

                /**
                 * Defines the structure used for an entry in the
                 *  InboundMessageFragments::StateContainer.
                 */
                struct ContainerEntry {
                    ContainerEntry(InboundMessageState ims);
                    ContainerEntry(ContainerEntry &&) = default;
                    ContainerEntry& operator=(ContainerEntry &&) = default;

                    uint32_t msgId;
                    RouterHash hash;
                    InboundMessageState state;
                    std::unique_ptr<boost::asio::deadline_timer> timer;
                };

                /**
                 * Function object to add new fragments to the
                 *  InboundMessageFragments::StateContainer.
                 */
                class AddFragment {
                    public:
                        AddFragment(const uint8_t fragNum, ByteArray const &data, bool isLast);

                        /**
                         * Adds the fragment to the state of the given
                         *  InboundMessageFragments::ContainerEntry.
                         */
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
