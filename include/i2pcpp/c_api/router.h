#ifndef C_API_ROUTER_H
#define C_API_ROUTER_H

void *i2p_router_new(char *dbFile);
void i2p_router_free(void *router);
void i2p_router_start(void *router);
void i2p_router_stop(void *router);

#endif
