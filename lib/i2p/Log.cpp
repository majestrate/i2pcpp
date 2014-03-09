/**
 * @file Log.cpp
 * @brief Implements Log.h
 */
#include "Log.h"

#include <sstream>
#include <locale>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/support/exception.hpp>

#include <i2pcpp/datatypes/Endpoint.h>
#include <i2pcpp/datatypes/RouterHash.h>

namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

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
}
