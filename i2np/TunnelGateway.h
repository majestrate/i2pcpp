#ifndef I2NPTUNNELGATEWAY_H
#define I2NPTUNNELGATEWAY_H

#include <list>

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        class TunnelGateway : public Message {
            public:
                TunnelGateway(uint32_t const tunnelId, ByteArray const &data);

                uint32_t getTunnelId() const;
                const ByteArray& getData() const;

                static TunnelGateway parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                TunnelGateway() = default;

                ByteArray compile() const;

            private:
                uint32_t m_tunnelId;
                ByteArray m_data;
        };
    }
}

#endif
