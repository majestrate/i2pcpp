#ifndef LEASEQUERY_H
#define LEASEQUERY_H

#include <i2pcpp/datatypes/Destination.h>

namespace i2pcpp {
    class RouterContext;
    class LeaseQuerySettings;

    class LeaseQuery {
        public:
            /**
             * Constructor. This will connect the callbacks contained in the LeaseQuerySettings
             * to the DHT signals
             */
            LeaseQuery(RouterContext const &ctx, Destination const &d, LeaseQuerySettings const &lqs);

            /**
             * Initiates a LeaseQuery.
             */
            void begin();
    };
}

#endif
