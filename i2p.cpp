#include <iostream>
#include <thread>
#include <csignal>

#include <botan/botan.h>

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

		std::signal(SIGINT, &sigint_handler);

		r.start();

		while(keepRunning) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		std::cerr << "Shutting down...\n";

		r.stop();

	} catch (std::runtime_error &e) {
		std::cerr << "main thread exception: " << e.what() << "\n";
	}

	return 0;
}
