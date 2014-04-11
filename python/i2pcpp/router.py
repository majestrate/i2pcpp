#
# -*- coding: utf-8 -*-
#
__doc__ = """
router.py -- i2p router interface

wraps c++ router implementation via ctypes

"""

import ctypes
import logging
import os

from . import util

_libi2p = ctypes.CDLL('libi2p.so')
_libi2p.i2p_init()        

def _c_str(s):
    return ctypes.c_char_p(str(s).encode('ascii'))

class Router:
    
    _log = logging.getLogger('I2PRouter')

    def __init__(self,ext_ip=None,port=11110,min_peers=50,fname='i2p.db'):
        if ext_ip is None:
            ext_ip = util.get_external_ip()
        config = {
            'ssu_external_ip' : ext_ip,
            'ssu_bind_ip' : '0.0.0.0',
            'ssu_external_port': port,
            'ssu_bind_port': port,
            'min_peers' : min_peers,
            'control_server': 0,
        }
        self._state = 'initialized'
        self._router = 0

        if not os.path.exists(fname):
            self._log.info('create db at '+fname)
            _libi2p.i2p_db_create(_c_str(fname))

        self._router = _libi2p.i2p_router_new(_c_str(fname))
        if self._router == 0:
            raise Exception('i2p router was null')
        self._log.info('created router at %x'%self._router)

        d = dict(config)
        self._log.info('setting config options')
        for k in d:
            v = d[k]
            self._log.debug('set config %s=%s' % (k,v))
            _libi2p.i2p_db_config_set(self._router_p(),_c_str(k),_c_str(v))


    def import_netdb(self,directory):
        if os.path.exists(directory):
            self._log.info('load netdb from %s'%directory)
            imported = _libi2p.i2p_db_netdb_populate(self._router,_c_str(directory))
            if imported == -1:
                raise Exception('did not import routers from '+directory)
            self._log.info('loaded %d routers'%imported)

    def start(self):
        self._log.info('starting i2p router')
        self._state = 'started'
        _libi2p.i2p_router_start(self._router_p())

    def _router_p(self):
        return ctypes.c_void_p(self._router)

    def stop(self):
        self._log.info('stopping i2p router')
        _libi2p.i2p_router_stop(self._router_p())
        self._state = 'stopped'

    def __del__(self):
        if hasattr(self,'_state') and self._state is not 'stopped':
            self.stop()
        if hasattr(self,'_router') and self._router != 0:
            _libi2p.i2p_router_free(self._router_p())
