#include "FollowOnFragment.h"
#include <stdexcept>

namespace i2pcpp {
    FollowOnFragment::FollowOnFragment(uint32_t msgId, uint8_t n) :
        m_fragNum(n)
    {
        m_msgId = msgId;

        if(m_fragNum > 63)
            throw std::runtime_error("too many fragments!");
    }

    void FollowOnFragment::setLast(bool isLast)
    {
        m_isLast = isLast;
    }

    bool FollowOnFragment::isLast() const
    {
        return m_isLast;
    }

    uint8_t FollowOnFragment::getFragNum() const
    {
        return m_fragNum;
    }

    ByteArray FollowOnFragment::compile() const
    {
        ByteArray output;

        unsigned char flag = 0x80;

        flag |= m_fragNum << 1;
        flag |= (unsigned char)m_isLast;
        output.insert(output.end(), flag);

        output.insert(output.end(), m_msgId >> 24);
        output.insert(output.end(), m_msgId >> 16);
        output.insert(output.end(), m_msgId >> 8);
        output.insert(output.end(), m_msgId);

        uint16_t size = (uint16_t)m_payload.size();
        output.insert(output.end(), size >> 8);
        output.insert(output.end(), size);

        output.insert(output.end(), m_payload.cbegin(), m_payload.cend());

        return output;
    }

    FollowOnFragment FollowOnFragment::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        unsigned char flag = *begin++;
        uint8_t fragNum = ((flag & 0x7e) >> 1);
        uint32_t msgId = parseUint32(begin);

        FollowOnFragment fof(msgId, fragNum);
        fof.m_isLast = flag & 0x01;

        uint16_t size = parseUint16(begin);
        if((end - begin) < size)
            throw std::runtime_error("malformed followon fragment");

        fof.m_payload = ByteArray(begin, begin + size);
        begin += size;

        return fof;
    }

    uint8_t FollowOnFragment::headerSize() const
    {
        return 7;
    }
}
