#include "Logger.h"

#include <i2pcpp/datatypes/Endpoint.h>
#include <i2pcpp/datatypes/RouterHash.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <sstream>
#include <locale>

namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

using namespace i2pcpp;

namespace i2pcpp {
    std::ostream& operator<<(std::ostream& strm, severity_level level)
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
}

Logger::Logger()
{
    m_stats = boost::make_shared<StatsBackend>();

    typedef sinks::asynchronous_sink<StatsBackend> stats_sink_t;
    boost::shared_ptr<stats_sink_t> statsSink(new stats_sink_t(m_stats));
    boost::log::core::get()->add_sink(statsSink);
}

void Logger::logToConsole()
{
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;

    boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();
    backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::empty_deleter()));

    boost::shared_ptr<sink_t> sink(new sink_t(backend));
    boost::log::core::get()->add_sink(sink);
    sink->set_filter(expr::attr<severity_level>("Severity") >= debug);
    sink->set_formatter(&Logger::formatter);

    boost::log::core::get()->add_global_attribute("Timestamp", attrs::local_clock());
}

void Logger::logToFile(const std::string &file)
{
    boost::log::core::get()->remove_all_sinks();

    boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(keywords::file_name = file);
    backend->auto_flush(true);

    typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(backend));
    boost::log::core::get()->add_sink(sink);

    sink->set_filter(expr::attr<severity_level>("Severity") >= debug);
    sink->set_formatter(&Logger::formatter);
}

void Logger::formatter(boost::log::record_view const &rec, boost::log::formatting_ostream &s)
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
