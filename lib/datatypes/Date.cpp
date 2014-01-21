/**
 * @file Date.cpp
 * @brief Implements Date.h.
 */
#include <i2pcpp/datatypes/Date.h>

#include <chrono>

namespace i2pcpp {
    Date::Date() :
        m_value(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
        ) {}

    Date::Date(const uint64_t &value) :
        m_value(value) {}

    Date::Date(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        if(std::distance(begin, end) < 8) throw std::runtime_error("malformed date");

        m_value = 0;

        for(int i = sizeof(uint64_t); i > 0; i--)
            m_value = (m_value << 8) + (*(begin++));
    }

    ByteArray Date::serialize() const
    {
        ByteArray v;

        for(int i = sizeof(uint64_t); i > 0; i--)
            v.push_back((m_value >> ((i - 1) * 8)));

        return v;
    }
}
