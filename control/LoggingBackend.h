#ifndef CONTROLLOGGINGBACKEND_H
#define CONTROLLOGGINGBACKEND_H

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>

namespace sinks = boost::log::sinks;

namespace i2pcpp {
    namespace Control {
        class Server;

        class LoggingBackend : public sinks::basic_sink_backend<
                                  sinks::combine_requirements<
                                      sinks::synchronized_feeding
                                  >::type
                              >
        {
            public:
                LoggingBackend(Server &s);

                void consume(boost::log::record_view const& rec);

            private:
                Server& m_server;

                uint64_t m_receivedBytes = 0;
                uint64_t m_sentBytes = 0;
        };
    }
}

#endif
