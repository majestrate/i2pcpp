#ifndef C_API_ROUTER_H
#define C_API_ROUTER_H

/**
 * Creates a new router instance.
 * @param an i2pcpp::Database handle returned from i2p_db_load.
 */
void *i2p_router_new(void *db);

/**
 * Destroys a router instance.
 */
void i2p_router_free(void *router);

/**
 * Starts a router instance.
 */
void i2p_router_start(void *router);

/**
 * Stops a router instance.
 */
void i2p_router_stop(void *router);

#endif
