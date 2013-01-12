#include <iostream>
#include <thread>
#include <csignal>

#include <botan/botan.h>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>

#include "Router.h"

bool keepRunning = true;

void sigint_handler(int sig)
{
	keepRunning = false;
}

int main()
{
	try {
		Botan::LibraryInitializer init("thread_safe=true");

		i2pcpp::Router r("i2p.db");

		r.start();

		enum Command {
			CONNECT,
			DB_LOOKUP,
			VTB,
			QUIT
		};
		std::map<std::string, Command> cmd_map = boost::assign::map_list_of("d", DB_LOOKUP)("q", QUIT)("v", VTB)("c", CONNECT);

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
					r.connect(*tokItr++);
					break;
				case DB_LOOKUP:
					r.databaseLookup(*(tokItr++), *(tokItr++));
					break;
				case VTB:
					while(tokItr != tok.end()) hopList.push_back(*tokItr++);
					r.createTunnel(hopList);
					break;
				case QUIT:
					keepRunning = false;
					break;
			}
		}

		std::cerr << "Shutting down...\n";

		r.stop();
	} catch (std::runtime_error &e) {
		std::cerr << "main thread exception: " << e.what() << "\n";
	}

	return 0;
}
