#ifndef C_API_DB_H
#define C_API_DB_H

void *i2p_db_load(char *dbFile);
void i2p_db_create(char *dbFile);
void i2p_db_config_set(void *db, char *name, char *value);
char *i2p_db_config_get(void *db, char *name);
bool i2p_db_router_exists(void *db, char *hash);

#endif
