#ifndef I2NPVARIABLETUNNELBUILD_H
#define I2NPVARIABLETUNNELBUILD_H

#include <list>

#include "../datatypes/BuildRecord.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        class VariableTunnelBuild : public Message {
            public:
                VariableTunnelBuild(std::list<BuildRecordPtr> const &buildRecords);
                VariableTunnelBuild(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

                std::list<BuildRecordPtr> getRecords() const;

                static VariableTunnelBuild parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                VariableTunnelBuild() = default;

                ByteArray compile() const;

            private:
                std::list<BuildRecordPtr> m_buildRecords;
        };
    }
}

#endif
