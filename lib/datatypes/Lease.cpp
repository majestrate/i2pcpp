#include <i2pcpp/datatypes/Lease.h>

namespace i2pcpp {
    Lease::Lease(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        if(std::distance(begin, end) != 44)
            throw std::runtime_error("malformed lease");

        std::copy(begin, begin + 32, m_gw.begin());
        begin += 32;

        m_tid = parseUint32(begin);

        m_end = Date(begin, end);
    }

    Lease::Lease(RouterHash const &gw, uint32_t const &tid, Date const &end) :
    m_gw(gw),
    m_tid(tid),
    m_end(end) {}

    ByteArray Lease::serialize() const
    {
        return ByteArray();
    }

    RouterHash Lease::getGateway() const
    {
        return m_gw;
    }

    uint32_t Lease::getTunnelId() const
    {
        return m_tid;
    }

    Date Lease::getEnd() const
    {
        return m_end;
    }
}
