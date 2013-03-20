#include <iostream>

#include "Log.h"
#include "transport/UDPTransport.h"
#include "util/Base64.h"

int main()
{
	using namespace i2pcpp;

	Log::initialize();

	UDPTransport u(SessionKey(Base64::decode("A6DVqs4yCV1s9QalgeB28iiV6341qm88Gblf3-c1SVg=")));
	u.start(Endpoint("127.0.0.1", 12345));

	int x;
	std::cin >> x;

	return 0;
}
