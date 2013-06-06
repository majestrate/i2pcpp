#include <iostream>
#include <fstream>
#include <map>

#include <botan/botan.h>

#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>

#include "Log.h"
#include "Router.h"

bool keepRunning = true;

void sigint_handler(int sig)
{
	keepRunning = false;
}

int main()
{
	using namespace i2pcpp;

	Log::initialize();
	i2p_logger_mt lg(boost::log::keywords::channel = "M");

	Botan::LibraryInitializer init("thread_safe=true");

	i2pcpp::Router r("i2p.db");
	try {
	  r.importNetDb("netDb");
	} 
	catch(std::exception & what) {
	  BOOST_LOG_SEV(lg, error) << "netDb import Error: "<< what.what();
	  return 1;
	}
	r.start();
	enum Command {
		CONNECT,
		DB_LOOKUP,
		EXPORT_INFO,
		IMPORT_INFO,
		IMPORT_DIR,
		SEND,
		BUILD_INBOUND,
		QUIT
	};
	std::map<std::string, Command> cmd_map = boost::assign::map_list_of("lookup", DB_LOOKUP)("quit", QUIT)("connect", CONNECT)("export", EXPORT_INFO)("import", IMPORT_INFO)("importdir", IMPORT_DIR)("send", SEND)("ibt", BUILD_INBOUND);


	while(keepRunning) {
		std::string str;
		std::getline(std::cin, str);

		if(!str.length()) continue;

		boost::char_separator<char> sep(" ");
		boost::tokenizer<boost::char_separator<char>> tok(str, sep);
		auto tokItr = tok.begin();

		std::map<std::string, Command>::const_iterator cmdItr = cmd_map.find(*tokItr);
		++tokItr;

		if(cmdItr == cmd_map.cend()) continue;

		Command cmd = cmdItr->second;

		ByteArray info;
		std::fstream f;
		std::list<std::string> hopList;
		std::string s;

		switch(cmd) {
			case CONNECT:
				r.connect(*tokItr++);
				break;

			case DB_LOOKUP:
				//r.databaseLookup(*(tokItr++), *(tokItr++));
				break;

			case EXPORT_INFO:
				f.open(*tokItr++, std::ios::out | std::ios::binary);
				info = r.getRouterInfo();
				f.write((char *)info.data(), info.size());
				f.close();
				break;

			case IMPORT_INFO:
				f.open(*tokItr++, std::ios::in | std::ios::binary);
				info = i2pcpp::ByteArray((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
				f.close();
				r.importRouterInfo(info);
				break;

			case IMPORT_DIR:
				{
					namespace fs = boost::filesystem;

					fs::path dir = *(tokItr++);
					if(!fs::exists(dir)) {
						I2P_LOG(lg, error) << "directory does not exist";
						break;
					}

					fs::recursive_directory_iterator itr(dir), end;
					while(itr != end) {
						if(is_regular_file(*itr)) {
							f.open(itr->path().string());
							info = i2pcpp::ByteArray((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
							f.close();
							I2P_LOG(lg, debug) << "importing " << itr->path().string();
							r.importRouterInfo(info);
						}

						if(fs::is_symlink(*itr)) itr.no_push();

						try {
							++itr;
						} catch(std::exception &e) {
							itr.no_push();
							continue;
						}
					}
				}
				break;

			case SEND:
				r.sendRawData(*tokItr++, *tokItr++);
				break;

			case BUILD_INBOUND:
				r.createTunnel(true);
				break;

			case QUIT:
				keepRunning = false;
				break;
		}
	}

	I2P_LOG(lg, debug) << "shutting down...";

	r.stop();

	return 0;
}
