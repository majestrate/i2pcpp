#ifndef C_API_TUNNEL_H
#define C_API_TUNNEL_H

#include <stdint.h>

// self
typedef void (*__tunnel_build_hook)(struct i2p_tunnel_t *) i2p_tunnel_build_hook_t;
// self, data, size
typedef int (*__tunnel_data_hook)(struct i2p_tunnel_t *, void*,  size_t) i2p_data_hook_t;

// 
struct i2p_tunnel_t {
    
    // do not touch
    void * _private;
    
    // build callbacks
    i2p_tunnel_build_hook_t timeout;
    i2p_tunnel_build_hook_t success;
    i2p_tunnel_build_hook_t reject;
    
    // Outbound tunnels Only (optional)
    i2p_data_hook_t send_raw;
    
    // Inbound tunnels only (optional)
    i2p_data_hook_t got_raw;
    
    // NULL terminated list of RouterIdentities
    // NULL for 0 hops
    void ** hops_ri;
    
    // if rh_reply_to is not NULL this is an Outbound Tunnel
    void * rh_reply_to;
    // must be set for Outbound tunnel
    uint32_t reply_tunnel;
};

/**
 * allocate and initialize struct i2p_tunnel_t
 * all fields except _private set to NULL
 */
struct i2p_tunnel_t * i2p_tunnel_new(void * router);

/**
 * build a tunnel
 * all appropriate memebers of the tunnel must be setp
 */
void i2p_tunnel_build(struct i2p_tunnel_t * tunnel);

uint32_t i2p_tunnel_get_id(struct i2p_tunnel_t * tunnel);

void i2p_tunnel_destroy(struct i2p_tunnel * tunnel);

void * i2p_tunnel_get_via_id(void * router, uint32_t tunnelId);

#endif