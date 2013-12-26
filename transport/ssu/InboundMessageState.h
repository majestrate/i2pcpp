/**
 * @file InboundMessageState.h
 * @brief Defines the i2pcpp::SSU::InboundMessageState class. 
 */
#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <list>

#include "../../datatypes/RouterHash.h"

namespace i2pcpp {
    namespace SSU {
        /**
         * Stores the state of a message that has been received 
         *  (partially or funny) or is to be received.
         */
        class InboundMessageState {
            public:
                InboundMessageState(RouterHash const &rh, const uint32_t msgId);

                /**
                 * Adds a fragment to the message we are receiving.
                 * @param fragNum the ID of the fragment
                 * @param data the actual fragment data
                 * @param isLast true indicates that his packet is the last,
                 *  false otherwise
                 * @todo add exceptions
                 */
                void addFragment(const uint8_t fragNum, ByteArray const &data, bool isLast);

                /**
                 * Merges all fragments into a single i2pcpp::ByteArray containing
                 *  all of the message's data. 
                 */
                ByteArray assemble() const;

                /**
                 * @return the i2pcpp::RouterHash of the sending router
                 */
                RouterHash getRouterHash() const;

                /**
                 * @return the ID of the message associated with this
                 *  i2pcpp::SSU::InboundMessageState object.
                 */
                uint32_t getMsgId() const;

                /**
                 * @return true if the message is complete (all fragments
                 *  have been receied), false otherwise.
                 */
                bool allFragmentsReceived() const;

                /**
                 * @return a std::vector<bool> where the element at index i
                 *  is a boolean that indicates whether fragment i has been
                 *  received (true if this is the case). 
                 */
                std::vector<bool> getFragmentsReceived() const;

            private:
                RouterHash m_routerHash; ///< i2pcpp::RouterHash of the sending router

                uint32_t m_msgId; ///< ID of associatedmessage
                bool m_gotLast = false;
                uint8_t m_lastFragment;
                uint32_t m_byteTotal = 0;

                /// Stores pointers to the fragment data
                std::vector<ByteArrayPtr> m_fragments;
        };

        typedef std::shared_ptr<InboundMessageState> InboundMessageStatePtr;
    }
}

#endif
