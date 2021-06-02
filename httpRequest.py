from client_sdk_python import Web3, HTTPProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

import requests
import json
import time
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


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
    cb = platon.getBalance('lat1zqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqzsjx8h7') * 1e-18
    print(cb)
    print(np.log10(cb))

    ccode = platon.getCode('lat1zqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqzsjx8h7')
    print(len(ccode))

def platonContractOwner():
    w3 = Web3(HTTPProvider("http://192.168.1.49:6789"))
    platon = PlatON(w3)

    false = False
    true = True

    contractAddr = 'lat1fg3u5qy8rnu7gryjznnuvj8fewnvp9efj3zew0'
    cabi = [{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"owner","type":"address"},{"indexed":true,"internalType":"address","name":"spender","type":"address"},{"indexed":false,"internalType":"uint256","name":"value","type":"uint256"}],"name":"Approval","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"previousOwner","type":"address"},{"indexed":true,"internalType":"address","name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":false,"internalType":"uint256","name":"value","type":"uint256"}],"name":"Transfer","type":"event"},{"constant":true,"inputs":[{"internalType":"address","name":"owner","type":"address"},{"internalType":"address","name":"spender","type":"address"}],"name":"allowance","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"spender","type":"address"},{"internalType":"uint256","name":"amount","type":"uint256"}],"name":"approve","outputs":[{"internalType":"bool","name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[{"internalType":"address","name":"account","type":"address"}],"name":"balanceOf","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"uint256","name":"amount","type":"uint256"}],"name":"burn","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"decimals","outputs":[{"internalType":"uint8","name":"","type":"uint8"}],"payable":false,"stateMutability":"pure","type":"function"},{"constant":true,"inputs":[],"name":"isOwner","outputs":[{"internalType":"bool","name":"","type":"bool"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"uint256","name":"amount","type":"uint256"}],"name":"mint","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"name","outputs":[{"internalType":"string","name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"owner","outputs":[{"internalType":"address","name":"","type":"address"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[],"name":"renounceOwnership","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"symbol","outputs":[{"internalType":"string","name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"totalSupply","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"recipient","type":"address"},{"internalType":"uint256","name":"amount","type":"uint256"}],"name":"transfer","outputs":[{"internalType":"bool","name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"sender","type":"address"},{"internalType":"address","name":"recipient","type":"address"},{"internalType":"uint256","name":"amount","type":"uint256"}],"name":"transferFrom","outputs":[{"internalType":"bool","name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[{"internalType":"address","name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"}]

    hello = platon.contract(address=contractAddr, abi=cabi)
    result = hello.functions.owner().call()
    print(type(result[0]), result[0])
    print(Web3.isAddress(result[0]))
    print(result[0])
    print(platon.getTransactionCount(result[0]))


def makeAnalisis():
    df_Addr_Value = pd.read_csv('./PlatON/data/latAVmain 1622429578.csv')

    print('total: ', len(df_Addr_Value))
    accounts = df_Addr_Value[df_Addr_Value['value'] > (1e-6)]['value']
    accounts = accounts.apply(np.log10)
    plt.hist(accounts, bins=1000)
    print('delete <= 1e-6: ', len(accounts))

    spaceL = [-6, 0, 2, np.log10(150), 3, 4, 5, 6, 7, 8, 9, 10]
    #spaceL = np.power(10, spaceL)
    #print(spaceL)
    areas = pd.cut(accounts.values, spaceL)
    value_counts = areas.value_counts()
    #print(value_counts.index)
    #print(value_counts.values)
    print(value_counts)

    plt.show()

#getPlatON_Addr()
#testPD()
#platonBanlenceCalc()
#platonContractOwner()
makeAnalisis()
