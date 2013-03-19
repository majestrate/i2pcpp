#include <iostream>

#include "transport/UDPTransport.h"

int main()
{
	using namespace i2pcpp;
	UDPTransport u;
	u.start(Endpoint("127.0.0.1", 12345));
	int x;
	std::cin >> x;
	return 0;
}
