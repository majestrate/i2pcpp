#include "../../../include/i2pcpp/c_api/router.h"

#include "../../../include/i2pcpp/Router.h"
#include "../../../include/i2pcpp/Database.h"

#include <string>

using namespace i2pcpp;

void *i2p_router_new(void *db)
{
    std::shared_ptr<Database> db_ptr((Database *)db);
    Router *r = new (std::nothrow) Router(db_ptr);

    return r;
}

void i2p_router_free(void *router)
{
    delete (Router *)router;
}

void i2p_router_start(void *router)
{
    ((Router *)router)->start();
}

void i2p_router_stop(void *router)
{
    ((Router *)router)->stop();
}
