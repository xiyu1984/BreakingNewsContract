from client_sdk_python import Web3, HTTPProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

import requests
import json
import time
import pandas as pd


def getPlatON_Addr():
    pageNo = 1
    pageSize = 500

    addrsList = []

    mheaders = {'content-type':'application/json'}
    murl = 'http://157.90.253.30:81/platon-api/address/addressValueAllCols'

    while True:
        mdata = json.dumps({'pageNo': pageNo, 'pageSize': pageSize})

        cur = int(round(time.time()))
        print(cur)
        req = requests.post(url=murl, headers=mheaders, data=mdata)
        cur = int(round(time.time()))
        print(cur)

        if req.status_code == 200:
            req = json.loads(req.text)
            addrs = req['data']['result']
            if len(addrs) > 0:
                aList = [[addr['address'], addr['balance']* 1e-18] for addr in addrs]
                addrsList += aList
            else:
                break
        else:
            break
        
        pageNo += 1
        #if pageNo % 10 == 0:
        now = int(round(time.time()*1000))
        now02 = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(now/1000))
        totalPage = float(req['data']['totalPages'])
        print('*******process:', pageNo, 'Total: ', totalPage, '. rate: ', pageNo / totalPage * 100, '%. Time: ', now02, '********')


    df = pd.DataFrame(addrsList, columns=['address', 'value'])

    now03 = int(round(time.time()))
    filename = './PlatON/data/latAVmain {}.csv'.format(now03)
    df.to_csv(filename)

    print(len(addrsList))

    #pageNo = 100000
    #mdata = json.dumps({'pageNo': pageNo, 'pageSize': pageSize})
    #req = requests.post(url=murl, headers=mheaders, data=mdata)
    #print(len(json.loads(req.text)['data']['result']))

def testPD():
    a = [['a', 1], ['b', 2], ['c', 3]]
    df = pd.DataFrame(a, columns=['address', 'value'])
    print(df)

    now03 = int(round(time.time()))
    filename = './PlatON/data/latAV {}.csv'.format(now03)
    df.to_csv(filename)

    now0xx = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(1621781965000/1000))
    print(now0xx)


def platonBanlenceCalc():
    w3 = Web3(HTTPProvider("http://192.168.1.49:6789"))
    platon = PlatON(w3)
    #print(platon.blockNumber)
    cb = platon.getBalance('lat1258ehcq73ux7wqsuurtke9lss9h4prck4ld34x') * 1e-18
    print(cb)


#getPlatON_Addr()
#getPlatON_Addr2()
#testPD()
platonBanlenceCalc()
