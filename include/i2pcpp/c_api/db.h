#ifndef C_API_DB_H
#define C_API_DB_H

void *i2p_db_load(char *dbFile);
void i2p_db_create(char *dbFile);
void i2p_db_config_set(void *router, char *name, char *value);
char *i2p_db_config_get(void *router, char *name);

#endif
