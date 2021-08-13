from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from client_sdk_python import utils
from client_sdk_python.utils import contracts

import json

breakingNewsContractAddr = 'lat17005cansj4xwrdfcxttz003tg0m0drqugwm670'
#breakingNewsContractAddr = 'lat1xezn8pp6vfaju8f47zh4dcxgt0nmqqhj9x703e'
clientAccount = 'lat1ar0s6re3qpe3rt39523qw4jars6s4sdhak459n'
devIP = 'http://47.241.69.26:6789'


def BNtest():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)
    #print(breakingNews_abi)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    print(hello.functions.getOwner().call())

    tx_receipt = hello.functions.createNews('hello', 'world is changing', [], '2021.8.13').transact({'from':clientAccount,'gas':1500000})
    receipt_info = platon.waitForTransactionReceipt(tx_receipt)
    print(receipt_info)

    #print(hello.functions.getNews().call())


def addViewpoint():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    tx_receipt = hello.functions.createViewPoint(1, 'world is not changing', ['no image'], False, '2021.8.13').transact({'from':clientAccount,'gas':1500000})
    receipt_info = platon.waitForTransactionReceipt(tx_receipt)
    print(receipt_info)


def likeNews():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    tx_receipt = hello.functions.likeNews(1).transact({'from':clientAccount,'gas':1500000})
    receipt_info = platon.waitForTransactionReceipt(tx_receipt)
    print(receipt_info)

def dislikeNews():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    tx_receipt = hello.functions.dislikeNews(1).transact({'from':clientAccount,'gas':1500000})
    receipt_info = platon.waitForTransactionReceipt(tx_receipt)
    print(receipt_info)


def simpleGet():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    print(hello.functions.getNews().call())


def clearData():
    fp = open('./PlatON/config/BreakingNews.abi.json')
    breakingNews_abi = json.load(fp)

    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=breakingNewsContractAddr, abi=breakingNews_abi, vmtype=1)

    tx_receipt = hello.functions.clear().transact({'from':clientAccount,'gas':1500000})
    receipt_info = platon.waitForTransactionReceipt(tx_receipt)
    print(receipt_info)


#BNtest()
simpleGet()
#addViewpoint()
#likeNews()
#dislikeNews()
#clearData()