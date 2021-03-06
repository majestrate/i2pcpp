#!/usr/bin/env python3
#
# this script generates initial keys and populates netdb as needed
#

import sqlite3, os
from urllib.request import urlopen as urlget

def get_external_ip():
    print ('get external ip address')
    return urlget('http://ipecho.net/plain').read().strip().decode('ascii')

def has_config(cur,k):
    return cur.execute('SELECT value FROM config WHERE name = ?',(k,)).fetchone() is not None

def put_config(cur,k,v):
    cur.execute('INSERT INTO config ( name, value ) VALUES ( ? , ? )',(k,v))

def init(ndb_dir,ssu_port,db_fname='i2p.db',ssu_ip='0.0.0.0',max_peers=500):

    os.system('./i2p --init --db=%s'%db_fname) # yes i know you can do injection here

    con = sqlite3.connect(db_fname)
    cur = con.cursor()

    if not has_config(cur,'ssu_bind_port'):
        print ('set ssu port to %d'%ssu_port)
        put_config(cur,'ssu_bind_port',ssu_port)

    if not has_config(cur,'ssu_bind_ip'):
        print ('set ssu ip to '+ssu_ip)
        put_config(cur,'ssu_bind_ip',ssu_ip)

    if not has_config(cur,'ssu_external_port'):
        print ('set ssu external port to %d'%ssu_port)
        put_config(cur,'ssu_external_port',ssu_port)    

    if not has_config(cur,'ssu_external_ip'):
        extern_ip = get_external_ip()
        print ('set ssu external ip to '+extern_ip)
        put_config(cur,'ssu_external_ip',extern_ip)
        
    if not has_config(cur,'max_peers'):
        print ('set max peer count to %d'%max_peers)
        put_config(cur,'max_peers',max_peers)
    
    con.commit()
    con.close()
 
    os.system('./i2p --importdir '+ndb_dir)

if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('--port',default=6699,type=int)
    ap.add_argument('--netdb',default=os.path.join(os.environ['HOME'],'.i2p','netDb'))
    
    args = ap.parse_args()

    init(ssu_port=args.port,ndb_dir=args.netdb)
            
