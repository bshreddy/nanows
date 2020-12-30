#!/usr/bin/python3

import os, socket, threading, json, re, argparse, signal, sys
from functools import lru_cache
from datetime import datetime

parser = argparse.ArgumentParser(description=('    Run a local HTTP Web Server.\n\n    '
                                'NOTE: Use only for educational purpose. DONOT USE IN PRODUCTION'),
                                formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--host', dest='host', default='0.0.0.0',
                    help=('Specifies the client address the server responds to. '
                          '(default: "0.0.0.0"). Use "0.0.0.0" to respond to all clients'))
parser.add_argument('--port', dest='port', default='8080',
                    help=('Specifies the port on which server listens to requests.'
                          '(default: 8080).'))
group = parser.add_mutually_exclusive_group()
group.add_argument('--no-cache', dest='no_cache', action='store_true',
                   help='Disables LRU Caching of Files. (equivalent to --cache=0).')
group.add_argument('--cache', dest='cache_size', default=1_048_576,
                   help='Change the Max Size (in bytes) of LRU Cache (default: 1,048,576).')
args = parser.parse_args()

SERVER_NAME = 'ElServePie/0.1'
BUF_SIZE = 1024
SITE_DIR = 'sites'
ERROR_SIRE_DIR = 'err_site'
DEFAULT_PAGE = 'index.html'
LRU_CACHE_SIZE = 0 if args.no_cache else args.cache_size

with open('mimetypes.json') as mimitypes_file: content_types = json.load(mimitypes_file)
def now_strtime(): return datetime.now().strftime('%m/%d/%Y, %H:%M:%S')
def content_type_for_url(url):
    try: return content_types[os.path.splitext(url)[1]]
    except: return 'application/octet-stream'

def parse_request(req):
    req = req.strip().split('\r\n')
    req_headers = req[1:]
    req_headers = [re.findall('^([^ ]+):(.+)$', req_header)[0] for req_header in req_headers]
    req_headers = {x[0]: x[1].strip() for x in req_headers}
    return (tuple(req[0].split(' ')), req_headers)

@lru_cache(LRU_CACHE_SIZE)
def read_page(url, site_dir = SITE_DIR):
    with open(os.path.join(site_dir, url), 'br') as f:
        data = f.read()
    return data

def get_response(url, http_method, req_headers):
    data = b''
    url = DEFAULT_PAGE if url == '/' else url[1:]
    url += '.html' if os.path.splitext(url)[1] == '' else ''

    if http_method == 'GET':
        try:
            data = read_page(url)
            status_code = '200 OK'
        except:
            status_code = '404 Not Found'
            data = read_page('404.html', site_dir = ERROR_SIRE_DIR)
    else:
        status_code = '405 Method Not Allowed'
        data = read_page('405.html', site_dir = ERROR_SIRE_DIR)
    
    res_headers = {
        'Server': SERVER_NAME,
        'Content-Type': content_type_for_url(url),
        'Content-Length': len(data)
    }
    return (status_code, data, res_headers)

def handle_request(conn, c_addr):
    with conn:
        req, req_headers = parse_request(conn.recv(BUF_SIZE).decode('utf-8'))
        (http_method, url, http_ver) = req
        
        (status_code, data, res_headers) = get_response(url, http_method, req_headers)
        res_headers = "\n".join([f'{key}: {val}' for key, val in res_headers.items()])
        res = bytearray(f'{http_ver} {status_code}\n{res_headers}\n\n', encoding='utf8')
        conn.sendall(res+data)
        
        print(f'[{now_strtime()}] ({c_addr[0]}:{c_addr[1]}) {req}... {status_code}')

def shutdown(signal, frame):
    print("\nStoping Server.....")
    sys.exit(0)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, shutdown)
    
    os.system('clear')
    with socket.socket() as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((args.host, int(args.port)))
        sock.listen()

        print('Server Started...')
        print(f'Listening on http:/{args.host}:{args.port}.')
        print('Press Ctrl+C to exit.', end='\n\n')

        while True:
            conn, c_addr = sock.accept()
            thread = threading.Thread(target=handle_request,
                                      args=(conn, c_addr),
                                      daemon=True)
            thread.start()
