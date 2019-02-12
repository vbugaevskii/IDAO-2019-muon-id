#!/usr/bin/python

import sys
import os
import requests
import urllib.request

from multiprocessing.pool import ThreadPool

"""
Ключ можно взять отсюда:
https://oauth.yandex.ru/authorize?response_type=token&client_id={client_id}
{client_id} -- id веб-приложения
"""

with open('yandex_disk_auth.key') as f_key:
    key = next(f_key).rstrip()

r = requests.get("https://cloud-api.yandex.net:443/v1/disk/public/resources",
                 params={"public_key" : "https://yadi.sk/d/pdwdp4Lt5X4DMQ"},
                 headers={"Authorization": "OAuth " + key})

if r.status_code != 200:
    print("Something went wrong", file=sys.stderr)
    sys.exit(1)

items = r.json()
items = items['_embedded']['items']

def download(item):
    urllib.request.urlretrieve(item['file'], os.path.join('data', item['path'].strip('/')))

pool = ThreadPool(10)
pool.map(download, items)
