/**
 * @file Log.h
 * @brief Defines the i2pcpp::Log class.
 */
#ifndef LOG_H
#define LOG_H

#include "../../include/i2pcpp/LogLevels.h"

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#define I2P_LOG(logger, sev) BOOST_LOG_SEV(logger, sev)
#define I2P_LOG_TAG(logger, name, value) logger.add_attribute(name, boost::log::attributes::make_constant(value))
#define I2P_LOG_SCOPED_TAG(logger, name, value) BOOST_LOG_SCOPED_LOGGER_TAG(logger, name, value)
#define I2P_LOG_CHANNEL(ch) boost::log::keywords::channel = ch

namespace i2pcpp {
    typedef boost::log::sources::severity_channel_logger_mt<severity_level, std::string> i2p_logger_mt;
}

#endif
