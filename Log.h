#ifndef LOG_H
#define LOG_H

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include "control/LoggingBackend.h"

#define I2P_LOG(logger, sev) BOOST_LOG_SEV(logger, sev)
#define I2P_LOG_TAG(logger, name, value) logger.add_attribute(name, boost::log::attributes::make_constant(value))
#define I2P_LOG_SCOPED_TAG(logger, name, value) BOOST_LOG_SCOPED_LOGGER_TAG(logger, name, value)

namespace i2pcpp {
    enum severity_level
    {
        debug,
        info,
        warning,
        error,
        fatal
    };

    typedef boost::log::sources::severity_channel_logger_mt<severity_level, std::string> i2p_logger_mt;

    class Log {
        public:
            Log() = delete;

            static void initialize();
            static void logToFile(const std::string &file);
            static void addControlServerSink(boost::shared_ptr<Control::LoggingBackend> backend);
            static void formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s);
    };
}

#endif
