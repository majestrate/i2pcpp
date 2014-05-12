#!/usr/bin/env python3
#
# Run like this: 
#
#   LD_LIBRARY_PATH='/path/to/build/directory/out/lib/' ./test.py
#

from i2pcpp.router import Router
import time
import logging

logging.basicConfig(level=logging.INFO)

r = Router(port=11112)
r.import_netdb('/tmp/netDb')
r.start()
try:
    while True:
        time.sleep(1)
finally:
    r.stop()
