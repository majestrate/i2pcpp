#ifndef LOG_H
#define LOG_H

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

#define I2P_LOG(logger, sev) BOOST_LOG_SEV(logger, sev)

#define I2P_LOG_EP(logger, var) logger.add_attribute("Endpoint", boost::log::attributes::constant<Endpoint>(var))
#define I2P_LOG_RH(logger, var) logger.add_attribute("RouterHash", boost::log::attributes::constant<RouterHash>(var))

#define I2P_LOG_SCOPED_EP(logger, var) BOOST_LOG_SCOPED_LOGGER_ATTR(logger, "Endpoint", boost::log::attributes::constant<Endpoint>(var))
#define I2P_LOG_SCOPED_RH(logger, var) BOOST_LOG_SCOPED_LOGGER_ATTR(logger, "RouterHash", boost::log::attributes::constant<RouterHash>(var))
#define I2P_LOG_SCOPED_TAG(logger, name) BOOST_LOG_SCOPED_LOGGER_TAG(logger, "Scope", name)

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
			static void initialize();
			static void formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s);
	};
}

#endif
