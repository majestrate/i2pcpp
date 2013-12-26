#include "Certificate.h"

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
    Certificate::Certificate() :
        m_type(Certificate::Type::NIL) {}

    Certificate::Certificate(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        m_type = (Type)*(begin++);

        if(m_type < (Type)0 || m_type > (Type)4) throw FormattingError();
    
        uint16_t size = (*(begin++) << 8) | *(begin++);
        if((end - begin) < size) throw FormattingError();
        m_payload.resize(size);
        copy(begin, begin + size, m_payload.begin()), begin += size;
    }

    uint16_t Certificate::getLength() const
    {
        return m_payload.size();
    }

    ByteArray Certificate::serialize() const
    {
        ByteArray b(m_payload);
        uint16_t length = m_payload.size();

        b.insert(b.begin(), length);
        b.insert(b.begin(), (length >> 8));
        b.insert(b.begin(), (unsigned char)m_type);

        return b;
    }
}
