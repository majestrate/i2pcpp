#include "StatsBackend.h"

#include <boost/log/attributes/value_extraction.hpp>

std::string stats_t::json()
{
    std::string i2np_msgs = "{";
    for ( auto & item : i2np_count ) {
        i2np_msgs += " \"" + item.first + "\" : "+std::to_string(item.second);
        i2np_msgs += ", ";
    }
    i2np_msgs += "\"0\" : 0 } ";

    return ( "{ \"bandwidth\" : [" + 
             std::to_string(bytes_sent) + "," + std::to_string(bytes_recv) + 
             "], \"peers\" : "+std::to_string(peer_count) +
             ", \"i2np\" : " + i2np_msgs + 
             ", \"tunnels\" : { \"participating\" : "+std::to_string(participating_tunnels) + " } "
             "}");
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
    } else if(rec.attribute_values().count("i2np")) {
        std::string msg_type = boost::log::extract<std::string>("i2np", rec).get();
        if (m_stats.i2np_count.find(msg_type) == m_stats.i2np_count.end()) {
            m_stats.i2np_count[msg_type] = 0;
        }
        m_stats.i2np_count[msg_type] += 1;
    } else if(rec.attribute_values().count("tunnel.participating")) {
        m_stats.participating_tunnels = boost::log::extract<uint32_t>("tunnel.participating", rec).get();
    }
}

stats_t StatsBackend::getStats()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    stats_t stats;
    stats.bytes_sent = m_stats.bytes_sent;
    stats.bytes_recv = m_stats.bytes_recv;
    stats.peer_count = m_stats.peer_count;
    stats.participating_tunnels = m_stats.participating_tunnels;
    stats.i2np_count = std::unordered_map<std::string, uint32_t>(m_stats.i2np_count);


    m_stats.bytes_sent = 0;
    m_stats.bytes_recv = 0;
    m_stats.i2np_count.clear();
    
    return stats;
}
