#!/bin/sh
#
# build script for python wrapper
#
# author: chi
#
# liecense: public domain
#
# probably should make cmake do this
#

# python include directory
PYINC="/usr/include/python3.3"
# python library directory
PYLIB="/usr/lib"
# botan include directory
BOTANINC="/usr/local/include/botan-1.11"
# botan library directory
BOTANLIB="/usr/local/lib"

# compile :3
g++ -o i2py.so -shared -fPIC i2py.cpp -Wall -std=c++11 -I$PYDIR -I$BOTANINC -lbotan-1.11 -L$BOTANLIB
