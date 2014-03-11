#ifndef LOGGER_H
#define LOGGER_H

#include "StatsBackend.h"

#include <i2pcpp/LogLevels.h>

#include <boost/shared_ptr.hpp>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sinks/async_frontend.hpp>

#include <string>

#define I2P_LOG(logger, sev) BOOST_LOG_SEV(logger, sev)
#define I2P_LOG_TAG(logger, name, value) logger.add_attribute(name, boost::log::attributes::make_constant(value))
#define I2P_LOG_SCOPED_TAG(logger, name, value) BOOST_LOG_SCOPED_LOGGER_TAG(logger, name, value)

typedef boost::log::sources::severity_channel_logger_mt<i2pcpp::severity_level, std::string> i2p_logger_mt;

class Logger {
    public:
        static void logToConsole();
        static void logToFile(const std::string &file);

        template<typename T>
        static void addBackendToAsyncSink(boost::shared_ptr<T> backend)
        {
            typedef sinks::asynchronous_sink<T> sink_t;
            boost::shared_ptr<sink_t> sink(new sink_t(backend));
            boost::log::core::get()->add_sink(sink);
        }

    private:
        static void formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s);

        boost::shared_ptr<StatsBackend> m_stats;
};

#endif
