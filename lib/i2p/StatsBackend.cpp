#include "StatsBackend.h"

#include <boost/log/attributes/value_extraction.hpp>

namespace i2pcpp {
    void StatsBackend::consume(boost::log::record_view const& rec)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(rec.attribute_values().count("sent")) {
            m_sentBytes += boost::log::extract<uint64_t>("sent", rec).get();
        } else if(rec.attribute_values().count("received")) {
            m_receivedBytes += boost::log::extract<uint64_t>("received", rec).get();
        }
    }

    std::pair<uint64_t, uint64_t> StatsBackend::getBytesAndReset()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::pair<uint64_t, uint64_t> p = {m_sentBytes, m_receivedBytes};
        m_sentBytes = m_receivedBytes = 0;
        return p;
    }
}
