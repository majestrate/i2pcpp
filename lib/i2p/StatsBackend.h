#ifndef STATSBACKEND_H
#define STATSBACKEND_H

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>

#include <mutex>

namespace sinks = boost::log::sinks;

namespace i2pcpp {
    class StatsBackend : public sinks::basic_sink_backend<
                              sinks::combine_requirements<
                                  sinks::synchronized_feeding
                              >::type
                          >
    {
        public:
            void consume(boost::log::record_view const& rec);

            std::pair<uint64_t, uint64_t> getBytesAndReset();

        private:
            std::mutex m_mutex;

            uint64_t m_receivedBytes = 0;
            uint64_t m_sentBytes = 0;
    };
}

#endif
