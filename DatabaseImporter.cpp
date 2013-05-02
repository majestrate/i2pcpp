#include "DatabaseImporter.h"
#include "datatypes/ByteArray.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>

using namespace i2pcpp;
using namespace boost::filesystem;

void DatabaseImporter::importDir(Router & router, std::string dirname)
{
  path netdbDir(dirname);
  std::vector<path> netdbFiles;
  if (exists(netdbDir) && is_directory(netdbDir))
    {
      BOOST_LOG_SEV(router.getLogger(), info) << "Load netdb from " << dirname;
      copy(directory_iterator(netdbDir),directory_iterator(), back_inserter(netdbFiles));
      
      for ( auto f : netdbFiles)
	importFile(router,f.native());

      BOOST_LOG_SEV(router.getLogger(), info) << "Loaded netdb";
	
    }
}

void DatabaseImporter::importFile(Router & router, std::string fname)
{

  std::fstream f;
  path p = path(fname);
  if ( exists(p) && is_regular_file(p) )
    {
      f.open(fname);
      ByteArray ba((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
      f.close();
      router.importRouterInfo(ba);
    }
}
