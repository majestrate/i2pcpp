#ifndef I2P_H
#define I2P_H

#define I2PAPI_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

#include <i2pcpp/c_api/router.h>
#include <i2pcpp/c_api/db.h>

void i2p_init();

#ifdef __cplusplus
}
#endif

#endif
