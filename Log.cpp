#include "Log.h"

#include "datatypes/Endpoint.h"

namespace i2pcpp {
	std::ostream& operator<< (std::ostream& strm, severity_level level)
	{
		static const char* strings[] =
		{
			"debug",
			"info",
			"warning",
			"error",
			"fatal"
		};

		if(static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
			strm << strings[level];
		else
			strm << static_cast<int>(level);

		return strm;
	}

	void Log::initialize()
	{
		namespace keywords = boost::log::keywords;
		namespace attrs = boost::log::attributes;
		namespace sinks = boost::log::sinks;
		namespace expr = boost::log::expressions;

		typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;

		boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();
		backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::log::empty_deleter()));

		boost::shared_ptr<sink_t> sink(new sink_t(backend));
		boost::log::core::get()->add_sink(sink);

		sink->set_filter(expr::attr<severity_level>("Severity") >= debug);
		sink->set_formatter(expr::stream
				<< '['
				<< expr::format_date_time<boost::posix_time::ptime>("Timestamp", "%Y-%m-%d %T.%f")
				<< "] "
				<< expr::attr<attrs::current_thread_id::value_type>("ThreadID")
				<< ' '
				<< expr::attr<std::string>("Channel")
				<< '/'
				<< expr::attr<severity_level>("Severity")
				<< expr::if_(expr::has_attr<Endpoint>("Endpoint"))
					[
						expr::stream
						<< " ["
						<< expr::attr<Endpoint>("Endpoint")
						<< "]"
					]
				<< ": "
				<< expr::message
			);

		boost::log::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
		boost::log::core::get()->add_global_attribute("Timestamp", attrs::local_clock());

		// TODO Figure out how to format scopes properly
		boost::log::core::get()->add_global_attribute("Scope", attrs::named_scope());
	}
}
