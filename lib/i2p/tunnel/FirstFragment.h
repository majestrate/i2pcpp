#ifndef TUNNELFIRSTFRAGMENT_H
#define TUNNELFIRSTFRAGMENT_H

#include "Fragment.h"

#include <i2pcpp/datatypes/RouterHash.h>

namespace i2pcpp {
    namespace Tunnel {
        class FirstFragment : public Fragment {
            public:
                /**
                 * Represents the delivery type flag in the delivery instructions
                 * for a tunnel message.
                 */
                enum class DeliveryMode {
                    LOCAL = 0x00,
                    TUNNEL = 0x01,
                    ROUTER = 0x02
                };

                /**
                 * @return a i2pcpp::ByteArray containing the compiled fragment.
                 */
                ByteArray compile() const;

                /**
                 * @return true if headerSize() + \a desiredSize > \a max.
                 */
                bool mustFragment(uint16_t desiredSize, uint16_t max) const;

                /**
                 * Set \a f to false if this fragment is the only fragment
                 * (bit 7 of a first fragment).
                 */
                void setFragmented(bool f);

                /**
                 * @return whether or not this fragment indicates that there will
                 * be follow on fragments (bit 7 of a first fragment).
                 */
                bool isFragmented() const;

                /**
                 * @return the tunnel ID associated with this fragment.
                 */
                uint32_t getTunnelId() const;

                /**
                 * @return the toHash field in the first fragment.
                 */
                const RouterHash& getToHash() const;

                /**
                 * @return the i2pcpp::Tunnel::FirstFragment::DeliveryMode for
                 * this fragment.
                 */
                DeliveryMode getDeliveryMode() const;

                /**
                 * Constructs a i2pcpp::Tunnel::FirstFragment from a pair of
                 * const i2pcpp::ByteArray iterators.
                 */
                static FirstFragment parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            private:
                uint8_t headerSize() const;

                DeliveryMode m_mode = DeliveryMode::LOCAL;
                bool m_fragmented = false;
                uint32_t m_tunnelId;
                RouterHash m_toHash;
        };
    }
}

#endif
