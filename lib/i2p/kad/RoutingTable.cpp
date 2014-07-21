#include "RoutingTable.h"

namespace i2pcpp {
    namespace Kad {
        bool RoutingTable::Entry::operator<(const Entry& e) const
        {
            return false;
        }
    }
}
