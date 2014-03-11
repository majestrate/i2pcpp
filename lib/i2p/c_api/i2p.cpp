#include "../../../include/i2pcpp/i2p.h"

#include <botan/botan.h>

void i2p_init()
{
    Botan::LibraryInitializer init("thread_safe=true");
}
