#include <iostream>
#include <fstream>
#include <map>

#include <botan/botan.h>

#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>

#include "Log.h"
#include "Router.h"
#include "DatabaseImporter.h"

bool keepRunning = true;

void sigint_handler(int sig)
{
	keepRunning = false;
}

int main()
{
	using namespace i2pcpp;

	Log::initialize();
	i2p_logger_mt lg(boost::log::keywords::channel = "Main");

	Botan::LibraryInitializer init("thread_safe=true");

	i2pcpp::Router r("i2p.db");
	i2pcpp::DatabaseImporter::importDir(r,"netdb");
	r.start();
	enum Command {
		CONNECT,
		DB_LOOKUP,
		EXPORT_INFO,
		IMPORT_INFO,
		SEND,
		BUILD_INBOUND,
		QUIT
	};
	std::map<std::string, Command> cmd_map = boost::assign::map_list_of("lookup", DB_LOOKUP)("quit", QUIT)("connect", CONNECT)("export", EXPORT_INFO)("import", IMPORT_INFO)("send", SEND)("ibt", BUILD_INBOUND);

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

	BOOST_LOG_SEV(lg, debug) << "shutting down...";

	r.stop();

	return 0;
}
