#include <iostream>
#include <map>

#include <botan/botan.h>

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
	i2p_logger_mt lg(boost::log::keywords::channel = "Main");

	Botan::LibraryInitializer init("thread_safe=true");

	i2pcpp::Router r("i2p.db");
	r.start();

	enum Command {
		CONNECT,
		DB_LOOKUP,
		BUILD_INBOUND,
		QUIT
	};
	std::map<std::string, Command> cmd_map = boost::assign::map_list_of("lookup", DB_LOOKUP)("quit", QUIT)("inbound", BUILD_INBOUND)("connect", CONNECT);

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

		std::list<std::string> hopList;
		switch(cmd) {
			case CONNECT:
				//r.connect(*tokItr++);
				break;
			case DB_LOOKUP:
				//r.databaseLookup(*(tokItr++), *(tokItr++));
				break;
			case BUILD_INBOUND:
				while(tokItr != tok.end()) hopList.push_back(*tokItr++);
				//r.createTunnel(hopList);
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
