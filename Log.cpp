#include "Log.h"

#include <sstream>
#include <locale>
#ifdef USE_BOOST_LOG
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/support/exception.hpp>
#else
#include <fstream>
#include <mutex>
#endif

#include "datatypes/Endpoint.h"
#include "datatypes/RouterHash.h"

#ifdef USE_BOOST_LOG
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
#endif

namespace i2pcpp {

#ifndef USE_BOOST_LOG
	static std::ostream * log_stream = nullptr;
	static std::mutex stream_mtx;
#endif

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
#ifdef USE_BOOST_LOG
		typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;

		boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();
		backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::log::empty_deleter()));

		boost::shared_ptr<sink_t> sink(new sink_t(backend));
		boost::log::core::get()->add_sink(sink);

		sink->set_filter(expr::attr<severity_level>("Severity") >= debug);
		sink->set_formatter(&Log::formatter);

		boost::log::core::get()->add_global_attribute("Timestamp", attrs::local_clock());
#else
		log_stream = &std::cerr;
#endif
	}

	void Log::logToFile(const std::string &file)
	{
#ifdef USE_BOOST_LOG
		boost::log::core::get()->remove_all_sinks();

		boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
				keywords::file_name = file
		);
		backend->auto_flush(true);

		typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
		boost::shared_ptr<sink_t> sink(new sink_t(backend));
		boost::log::core::get()->add_sink(sink);

		sink->set_filter(expr::attr<severity_level>("Severity") >= debug);
		sink->set_formatter(&Log::formatter);
#else
		std::ofstream * ofs = new std::ofstream();
		ofs->open(file);
		log_stream = ofs;
#endif
	}

#ifdef USE_BOOST_LOG
	void Log::formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s)
	{
		const boost::log::attribute_value_set& attrSet = rec.attribute_values();

		static std::locale loc(std::clog.getloc(), new boost::posix_time::time_facet("%Y-%m-%d %T.%f"));
		std::stringstream ss;
		ss.imbue(loc);
		if(!ss.good()) return;
		ss << boost::log::extract<boost::posix_time::ptime>("Timestamp", rec);
		if(!ss.good()) return;
		s << '[' << ss.str() << ']';

		s << ' ' << boost::log::extract<std::string>("Channel", rec);
		s << '/' << boost::log::extract<severity_level>("Severity", rec);

		if(attrSet.find("Endpoint") != attrSet.end())
			s << " [" << boost::log::extract<Endpoint>("Endpoint", rec) << ']';

		if(attrSet.find("RouterHash") != attrSet.end())
			s << " [" << boost::log::extract<RouterHash>("RouterHash", rec) << ']';

		if(attrSet.find("TunnelId") != attrSet.end())
			s << " [" << boost::log::extract<uint32_t>("TunnelId", rec) << ']';

		s << ": " << rec[expr::smessage];
	}
#else

	chi_logger::chi_logger(std::string const & name) 
	{
		std::lock_guard<std::mutex> lock(stream_mtx);
		m_logger_name = name;
		m_tag_name = "";
		m_tag_val = "";
		m_scope_name = "";
		m_scope_val = "";
	}
		
	void chi_logger::set_tag(std::string const & tag_name, std::string const & tag_val)
	{
		std::lock_guard<std::mutex> lock(stream_mtx);
		m_tag_name = tag_name;
		m_tag_val = tag_val;
	}

	void chi_logger::set_scope(std::string const & scope_name, std::string const & scope_val)
	{
		std::lock_guard<std::mutex> lock(stream_mtx);
		m_scope_name = scope_name;
		m_scope_val = scope_val;
	}

	std::ostream & chi_logger::get_ostream(severity_level level)
	{
		std::lock_guard<std::mutex> lock(stream_mtx);
		std::ostream & stream = *log_stream;
		stream << std::endl;
		stream << m_logger_name << "/" << level << " " << m_scope_val << ": " ;
		return stream << std::flush;
	}

#endif
}
