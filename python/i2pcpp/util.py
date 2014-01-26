from urllib.request import urlopen



def get_external_ip(url='http://ipecho.net/plain'):
    f = urlopen(str(url))
    d = f.read()
    f.close()
    return d.decode('utf-8')
