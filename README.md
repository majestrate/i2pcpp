i2pcpp
======

i2p c++ implementation, forked from http://git.repo.i2p/w/i2pcpp.git

This is not the official C++ implementation, it is a modified version that works with GCC and an older version of boost.

The Official Version is on github [here](https://github.com/i2pcpp/i2pcpp)

Dependancies:

* boost 1.54
* sqlite3
* botan-1.11
* gtest (optional)


Building:

    mkdir build
    cmake ..
    make

Running:

    ./i2pinit.py --port $random_port_here --netdb /path/to/existing/i2p/netdb
	./i2p
