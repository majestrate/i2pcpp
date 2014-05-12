#include "StatsBackend.h"

#include <boost/log/attributes/value_extraction.hpp>

std::string stats_t::json()
{
    return ( "{ \"bandwidth\" : [" + 
             std::to_string(bytes_sent) + "," + std::to_string(bytes_recv) + 
             "], \"peers\" : "+std::to_string(peer_count)+" }");
}

void StatsBackend::consume(boost::log::record_view const& rec)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(rec.attribute_values().count("sent")) {
        m_stats.bytes_sent += boost::log::extract<uint64_t>("sent", rec).get();
    } else if(rec.attribute_values().count("received")) {
        m_stats.bytes_recv += boost::log::extract<uint64_t>("received", rec).get();
    } else if(rec.attribute_values().count("peers")) {
        m_stats.peer_count = boost::log::extract<uint32_t>("peers", rec).get();
    }
}

stats_t StatsBackend::getStats()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    stats_t stats;
    stats.bytes_sent = m_stats.bytes_sent;
    stats.bytes_recv = m_stats.bytes_recv;
    stats.peer_count = m_stats.peer_count;
    m_stats.bytes_sent = 0;
    m_stats.bytes_recv = 0;
    return stats;
}
