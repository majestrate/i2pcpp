#ifndef LOG_H
#define LOG_H

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/log/expressions/message.hpp>
#include <boost/log/expressions/formatters/named_scope.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>

#define I2P_LOG_EP(logger, var) BOOST_LOG_SCOPED_LOGGER_ATTR(logger, "Endpoint", boost::log::attributes::constant<Endpoint>(var))

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
	};
}

#endif
