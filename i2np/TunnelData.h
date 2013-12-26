#ifndef I2NPTUNNELDATA_H
#define I2NPTUNNELDATA_H

#include "../datatypes/StaticByteArray.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        class TunnelData : public Message {
            public:
                TunnelData(uint32_t const tunnelId, ByteArray const &data);

                uint32_t getTunnelId() const;
                const StaticByteArray<1024>& getData() const;

                static TunnelData parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                TunnelData() = default;

                ByteArray compile() const;

            private:
                uint32_t m_tunnelId;
                StaticByteArray<1024> m_data;
        };
    }
}

#endif
