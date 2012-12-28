#include "Router.h"

#include <iostream>
#include <thread>

#include <botan/botan.h>

int main()
{
	try {
		Botan::LibraryInitializer init;

		i2pcpp::Router r("i2p.db");

		r.start();

		std::this_thread::sleep_for(std::chrono::seconds(5));

		std::cerr << "Shutting down...\n";
		r.stop();
	} catch (std::runtime_error &e) {
		std::cerr << "main thread exception: " << e.what() << "\n";
	}

	return 0;
}
