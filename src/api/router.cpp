#include <i2pcpp/router.hpp>
#include <i2pcpp/datatypes/ByteArray.h>
#include <i2pcpp/datatypes/RouterInfo.h>
#include <boost/filesystem.hpp>

#include <fstream>
#include <string>
#include <iostream>

#include <botan/botan.h>

#include "Router.h"

using namespace i2pcpp;
namespace fs = boost::filesystem;

void i2p_init()
{
    Log::initialize();
    Botan::LibraryInitializer init("thread_safe=true");
}

void *i2p_router_new(char *dbFile)
{
    Router *r = new (std::nothrow) Router(std::string(dbFile));

    return r;
}

void i2p_router_free(void *router)
{
    delete (Router *)router;
}

void i2p_db_create(char *dbFile)
{
    Database::createDb(std::string(dbFile));
}

int i2p_db_netdb_populate(void * router, char * directory)
{

    std::vector<RouterInfo> routers;
    std::string dir(directory);
    
    if(fs::exists(dir)) {
        fs::recursive_directory_iterator itr(dir), end;
        while(itr != end) {
            if(is_regular_file(*itr)) {
                std::ifstream f(itr->path().string(), std::ios::binary);
                if(!f.is_open()) continue;
                
                ByteArray info = ByteArray((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                f.close();
                auto begin = info.cbegin();
                auto ri = RouterInfo(begin, info.cend());
                
                if(ri.verifySignature())
                    routers.push_back(ri);
            }
            if(fs::is_symlink(*itr)) itr.no_push();
            
            try {
                ++itr;
            } catch(std::exception &e) {
                itr.no_push();
                continue;
            }
        }
    } else return -1;
  
    ((Router *)router)->importRouter(routers);

    return routers.size();
}

void i2p_db_config_set(void *router, char *name, char *value)
{
    ((Router *)router)->setConfigValue(std::string(name), std::string(value));
}

char *i2p_db_config_get(void *router, char *name)
{
    std::string result = ((Router *)router)->getConfigValue(std::string(name));
    char *buf = new char[result.length() + 1];
    std::memcpy(buf, result.c_str(), sizeof(*buf));

    return buf;
}

void i2p_router_start(void *router)
{
    ((Router *)router)->start();
}

void i2p_router_stop(void *router)
{
    ((Router *)router)->stop();
}
