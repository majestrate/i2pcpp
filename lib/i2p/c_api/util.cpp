#include "util.h"

using namespace i2pcpp;

RouterHash i2p_util_to_routerhash(char *hash)
{
    RouterHash rh;

    memcpy(rh.data(), hash, rh.size());
    return rh;
}
