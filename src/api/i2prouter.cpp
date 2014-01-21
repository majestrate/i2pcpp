#include <i2pcpp/router.hpp>

#include <string>

#include "Router.h"

void *i2p_router_new(char *dbFile)
{
    i2pcpp::Router *r = new (std::nothrow) i2pcpp::Router(std::string(dbFile));

    return r;
}
