#ifndef TUNNELFOLLOWONFRAGMENT_H
#define TUNNELFOLLOWONFRAGMENT_H

#include "Fragment.h"

namespace i2pcpp {
    namespace Tunnel {
        class FollowOnFragment : public Fragment {
            public:
                /**
                 * Constructs a follow on fragment from a message ID and \a n,
                 * a fragment number.
                 */
                FollowOnFragment(uint32_t msgId, uint8_t n);

                /**
                 * Set to true is this is the last fragment in a tunnel message
                 * (bit 0 of frag field).
                 */
                void setLast(bool isLast);

                /**
                 * @return whether this is the last fragment in a tunnel message
                 * (bit 0 of frag field).
                 */
                bool isLast() const;

                /**
                 * @return the fragment number associated with this fragment.
                 */
                uint8_t getFragNum() const;

                /**
                 * Compiles the class to a i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

                /**
                 * Constructs a i2pcpp::Tunnel::FollowOnFragment from a pair of
                 * const i2pcpp::ByteArray iterators.
                 */
                static FollowOnFragment parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            private:
                uint8_t headerSize() const;

                uint8_t m_fragNum;
                bool m_isLast = false;
        };

        typedef std::unique_ptr<FollowOnFragment> FollowOnFragmentPtr;
    }
}

#endif
