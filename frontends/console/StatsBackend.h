#ifndef STATSBACKEND_H
#define STATSBACKEND_H

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>

#include <mutex>
#include <string>
#include <unordered_map>

namespace sinks = boost::log::sinks;

struct stats_t {
    uint64_t bytes_sent = 0;
    uint64_t bytes_recv = 0;
    uint32_t peer_count = 0;
    uint32_t participating_tunnels = 0;
    std::unordered_map<std::string, uint32_t> i2np_count;
    std::string json();
};

class StatsBackend : public sinks::basic_sink_backend<
                          sinks::combine_requirements<
                              sinks::synchronized_feeding
                          >::type
                      >
{
    public:
        void consume(boost::log::record_view const& rec);

        stats_t getStats();

    private:
        mutable std::mutex m_mutex;
        stats_t m_stats;
};

#endif
