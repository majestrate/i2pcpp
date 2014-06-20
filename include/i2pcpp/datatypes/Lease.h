#ifndef LEASE_H
#define LEASE_H

#include "Datatype.h"
#include "Date.h"
#include "RouterHash.h"

namespace i2pcpp {
    class Lease : public Datatype {
        public:
            Lease(ByteArrayConstItr &begin, ByteArrayConstItr end);
            Lease(RouterHash const &gw, uint32_t const &tid, Date const &end);

            RouterHash getGateway() const;
            uint32_t getTunnelId() const;
            Date getEnd() const;

        private:
            RouterHash m_gw;
            uint32_t m_tid;
            Date m_end;
    };
}

#endif
