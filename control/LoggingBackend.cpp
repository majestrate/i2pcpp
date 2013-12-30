#include "LoggingBackend.h"

#include "Server.h"

#include <boost/log/attributes/value_extraction.hpp>

namespace i2pcpp {
    namespace Control {
        void LoggingBackend::consume(boost::log::record_view const& rec)
        {
            if(rec.attribute_values().count("sent")) {
                m_sentBytes += boost::log::extract<uint64_t>("sent", rec).get();
            } else if(rec.attribute_values().count("received")) {
                m_receivedBytes += boost::log::extract<uint64_t>("received", rec).get();
            }
        }

        std::pair<uint64_t, uint64_t> LoggingBackend::getBytesAndReset()
        {
            std::pair<uint64_t, uint64_t> p = {m_sentBytes, m_receivedBytes};
            m_sentBytes = m_receivedBytes = 0;
            return p;
        }
    }
}
