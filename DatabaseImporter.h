#ifndef DATABASEIMPORTER_H
#define DATABASEIMPORTER_H
#include "Router.h"
#include <iostream>

namespace i2pcpp
{
  class DatabaseImporter
  {
  public:
    static void importDir(Router & router, std::string dirname);
    static void importFile(Router & router, std::string fname);

  };
}
#endif

