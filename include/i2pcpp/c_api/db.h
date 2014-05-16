#ifndef C_API_DB_H
#define C_API_DB_H

/**
 * Loads an I2P database.
 * @param dbFile The filename of the database to load.
 * @return A database handle to be used in subsequent functions.
 */
void *i2p_db_load(char *dbFile);

/**
 * Creates a new I2P database.
 */
void i2p_db_create(char *dbFile);

/**
 * Sets the value of the configuration field \a name to \a value.
 */
void i2p_db_config_set(void *db, char *name, char *value);

/**
 * @return the value of the configuration field \a name
 */
char *i2p_db_config_get(void *db, char *name);

/**
 * @param hash 32 byte RouterHash.
 * @return true if the database has RouterInfo for the given
 * \a hash, false otherwise.
 */
bool i2p_db_router_exists(void *db, char *hash);

void * i2p_db_ri_get_rh(void * ri);

void i2p_db_ri_free(void * ri);
void i2p_db_rh_free(void * rh);

void * i2p_db_get_ri_via_hash(void * db, char * c_hash_str);


#endif
