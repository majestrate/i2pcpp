/**
 * @file Log.h
 * @brief Defines the i2pcpp::Log class.
 */
#ifndef LOG_H
#define LOG_H


#ifdef USE_BOOST_LOG
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
#define I2P_LOG_CHANNEL(name) boost::log::keywords::channel = name
#else
#include <string>
#include <iostream>
#define I2P_LOG(logger, sev) logger.get_ostream(sev)
#define I2P_LOG_TAG(logger, name, value) logger.set_tag(name, value)
#define I2P_LOG_SCOPED_TAG(logger, name, value) logger.set_scope(name, value)
#define I2P_LOG_CHANNEL(name) name
#endif

namespace i2pcpp {

    /**
     * Defines the level of severity.
     */
    enum severity_level
    {
        debug,
        info,
        warning,
        error,
        fatal
    };

#ifdef USE_BOOST_LOG
    typedef boost::log::sources::severity_channel_logger_mt<severity_level, std::string> i2p_logger_mt;
#else

    class chi_logger {
        public:
            chi_logger() : chi_logger("?") {}
            chi_logger(std::string const & name);
            void set_tag(std::string const & tag_name, std::string const & tag_value);
            std::ostream & get_ostream(severity_level level);
            void set_scope(std::string const & scope_name, std::string const & scope_value);
            void set_scope(std::string const & scope_name, int64_t scope_value) { set_scope(scope_name, std::to_string(scope_value)); } 

        protected:
            std::string m_tag_name, m_tag_val;
            std::string m_scope_name, m_scope_val;
            std::string m_logger_name;

    };

    typedef chi_logger i2p_logger_mt;

#endif

    /**
     * Provides static utility functions that wrap around boost::log.
     */
    class Log {
        public:
            Log() = delete;

            /**
             * Initializes boost::log.
             */
            static void initialize();

            /**
             * Changes the logfile.
             * @param file the name of the new logfile
             */
            static void logToFile(const std::string &file);

            /**
             * Adds an asynchronous sink to a given logging backend \a backend.
             */
            static void addControlServerSink(boost::shared_ptr<Control::LoggingBackend> backend);

#ifdef USE_BOOST_LOG
            /**
             * Formats a message (given by a boost::log::record_view object \a rec)
             *  and writes it to a given boost::log::formatting_ostream, \a s.
             */
            static void formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s);
#endif
    };

}

#endif
