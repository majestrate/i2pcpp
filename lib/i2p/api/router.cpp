#include <i2pcpp/router_c.h>

#include <string>

#include <botan/botan.h>

#include "Router.h"

using namespace i2pcpp;

void i2p_init()
{
    Botan::LibraryInitializer init("thread_safe=true");
}

void *i2p_router_new(char *dbFile)
{
    Router *r = new (std::nothrow) Router(std::string(dbFile));

    return r;
}

void i2p_router_free(void *router)
{
    delete (Router *)router;
}

void i2p_db_create(char *dbFile)
{
    Database::createDb(std::string(dbFile));
}

void i2p_db_config_set(void *router, char *name, char *value)
{
    ((Router *)router)->setConfigValue(std::string(name), std::string(value));
}

char *i2p_db_config_get(void *router, char *name)
{
    std::string result = ((Router *)router)->getConfigValue(std::string(name));
    char *buf = new char[result.length() + 1];
    std::memcpy(buf, result.c_str(), sizeof(*buf));

    return buf;
}

void i2p_router_start(void *router)
{
    ((Router *)router)->start();
}

void i2p_router_stop(void *router)
{
    ((Router *)router)->stop();
}
