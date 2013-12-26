#include "RouterAddress.h"

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
    RouterAddress::RouterAddress(int cost, Date const &expiration, std::string const &transport, Mapping const &options) :
        m_cost(cost),
        m_expiration(expiration),
        m_transport(transport),
        m_options(options) {}

    RouterAddress::RouterAddress(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        m_cost = *(begin++);
        m_expiration = Date(begin, end);

        unsigned char size = *(begin++);
        if((end - begin) < size) throw FormattingError();
        m_transport = std::string(begin, begin + size);
        begin += size;

        m_options = Mapping(begin, end);
    }

    ByteArray RouterAddress::serialize() const
    {
        ByteArray b;

        const ByteArray&& expiration = m_expiration.serialize();
        const ByteArray&& options = m_options.serialize();

        b.insert(b.end(), m_cost);
        b.insert(b.end(), expiration.cbegin(), expiration.cend());
        b.insert(b.end(), m_transport.size());
        b.insert(b.end(), m_transport.cbegin(), m_transport.cend());
        b.insert(b.end(), options.cbegin(), options.cend());

        return b;
    }

    const unsigned char RouterAddress::getCost() const
    {
        return m_cost;
    }

    const Date& RouterAddress::getExpiration() const
    {
        return m_expiration;
    }

    const std::string& RouterAddress::getTransport() const
    {
        return m_transport;
    }

    const Mapping& RouterAddress::getOptions() const
    {
        return m_options;
    }
}
