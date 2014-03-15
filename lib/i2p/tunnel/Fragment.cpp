#include "Fragment.h"

#include "FirstFragment.h"
#include "FollowOnFragment.h"

#include <i2pcpp/util/make_unique.h>

#include <botan/auto_rng.h>

namespace i2pcpp {
    namespace Tunnel {
        void Fragment::setMsgId(uint32_t id)
        {
            m_msgId = id;
        }

        uint32_t Fragment::getMsgId() const
        {
            return m_msgId;
        }

        void Fragment::setPayload(ByteArrayConstItr &begin, ByteArrayConstItr end, uint16_t max)
        {
            const uint8_t headerLen = headerSize();
            auto payloadLen = std::distance(begin, end);

            if(headerLen + payloadLen > max) {
                payloadLen = max - headerLen;
                end = begin + payloadLen;
            }

            m_payload.clear();
            m_payload.resize(payloadLen);
            std::copy(begin, end, m_payload.begin());

            begin = end;
        }

        const ByteArray& Fragment::getPayload() const
        {
            return m_payload;
        }

        uint16_t Fragment::size() const
        {
            return headerSize() + m_payload.size();
        }

        std::vector<FragmentPtr> Fragment::fragmentMessage(ByteArray const &data)
        {
            constexpr uint16_t maxSize = 1003;

            std::vector<FragmentPtr> fragments;

            std::unique_ptr<FirstFragment> first = std::make_unique<FirstFragment>();
            if(first->mustFragment(data.size(), maxSize)) {
                first->setFragmented(true);

                uint32_t msgId = 0;
                Botan::AutoSeeded_RNG rng;
                rng.randomize((unsigned char *)&msgId, sizeof(msgId));
                first->setMsgId(msgId);

                auto pos = data.cbegin();
                auto end = data.cend();
                first->setPayload(pos, data.cend(), maxSize);
                fragments.push_back(std::move(first));

                uint8_t fragNum = 1;
                while(pos != end) {
                    std::unique_ptr<FollowOnFragment> followup = std::make_unique<FollowOnFragment>(msgId, fragNum++);
                    followup->setPayload(pos, end, maxSize);

                    if(pos == end)
                        followup->setLast(true);

                    fragments.push_back(std::move(followup));
                }
            } else {
                auto pos = data.cbegin();
                first->setPayload(pos, data.cend(), maxSize);
                fragments.push_back(std::move(first));
            }

            return fragments;
        }

        FragmentPtr Fragment::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            size_t size = std::distance(begin, end);

            if(size < 3)
                throw std::runtime_error("could not parse fragment");

            unsigned char flag = *begin;
            if(flag & 0x80) {
                return std::make_unique<FollowOnFragment>(FollowOnFragment::parse(begin, end));
            } else {
                return std::make_unique<FirstFragment>(FirstFragment::parse(begin, end));
            }
        }
    }
}
