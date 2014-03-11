#include "../../include/i2pcpp/c_api/db.h"

#include "../../include/i2pcpp/Database.h"

#include <string>

using namespace i2pcpp;

void *i2p_db_load(char *dbFile)
{
    Database *db = new (std::nothrow) Database(std::string(dbFile));

    return db;
}

void i2p_db_create(char *dbFile)
{
    Database::createDb(std::string(dbFile));
}

void i2p_db_config_set(void *db, char *name, char *value)
{
    ((Database *)db)->setConfigValue(std::string(name), std::string(value));
}

char *i2p_db_config_get(void *db, char *name)
{
    std::string result = ((Database *)db)->getConfigValue(std::string(name));
    char *buf = new char[result.length() + 1];
    std::memcpy(buf, result.c_str(), sizeof(*buf));

    return buf;
}
