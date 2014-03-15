#ifndef TUNNELFRAGMENT_H
#define TUNNELFRAGMENT_H

#include "../i2np/Message.h"

#include <i2pcpp/datatypes/ByteArray.h>

#include <list>
#include <memory>

namespace i2pcpp {
    namespace Tunnel {
        class Fragment {
            public:
                virtual ~Fragment() {}

                /**
                 * Sets the message ID for the fragment.
                 */
                void setMsgId(uint32_t id);

                /**
                 * Gets the message ID for the fragment.
                 */
                uint32_t getMsgId() const;

                /**
                 * Copies \a max bytes from the pair of const i2pcpp::ByteArray
                 * iterators and sets them as the payload.
                 */
                void setPayload(ByteArrayConstItr &begin, ByteArrayConstItr end, uint16_t max);

                /**
                 * @return the payload of the fragment.
                 */
                const ByteArray& getPayload() const;

                /**
                 * @return the size of the fragment, including the header.
                 */
                uint16_t size() const;

                virtual ByteArray compile() const = 0;

                /**
                 * Fragments a complete array of \a data in to the corresponding
                 * i2pcpp::Tunnel::FirstFragment and any additional
                 * i2pcpp::Tunnel::FollowOnFragments.
                 */
                static std::vector<std::unique_ptr<Fragment>> fragmentMessage(ByteArray const &data);

                /**
                 * Parses the data at the iterator, creating a
                 * i2pcpp::Tunnel::FirstFragment or i2pcpp::Tunnel::FollowOnFragment
                 * as necessary.
                 */
                static std::unique_ptr<Fragment> parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                uint32_t m_msgId = 0;
                ByteArray m_payload;

            private:
                virtual uint8_t headerSize() const = 0;
        };

        typedef std::unique_ptr<Fragment> FragmentPtr;
    };
}

#endif
