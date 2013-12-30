#include "LoggingBackend.h"

#include <boost/log/attributes/value_extraction.hpp>

namespace i2pcpp {
    namespace Control {
        LoggingBackend::LoggingBackend(Server &s) :
            m_server(s) {}

        void LoggingBackend::consume(boost::log::record_view const& rec)
        {
            if(rec.attribute_values().count("sent")) {
                m_sentBytes += boost::log::extract<uint32_t>("sent", rec).get();
            } else if(rec.attribute_values().count("received")) {
                m_receivedBytes += boost::log::extract<uint32_t>("received", rec).get();
            }
        }
    }
}
