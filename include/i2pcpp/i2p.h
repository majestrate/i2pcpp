#ifndef I2P_H
#define I2P_H

#define I2PAPI_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

#include <i2pcpp/c_api/router.h>
#include <i2pcpp/c_api/db.h>

/**
 * Call this function at the start of your application.
 * It must not be called more than one time.
 */
void i2p_init();

#ifdef __cplusplus
}
#endif

#endif
