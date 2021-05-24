from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

true = True
false = False
contractAddr = 'lat1kzyglsytjffg8r4hhypnx0uu2azywnp3yutxan'
from_address = 'lat1a3tlqd07aps8tjsegz967gdq686qttk2e2p4kw'
cabi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"Add","topic":1,"type":"Event"},{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[],"name":"makeNumber","output":"void","type":"Action"},{"constant":false,"input":[],"name":"deleteNumber","output":"void","type":"Action"},{"constant":true,"input":[],"name":"get_Numbers","output":"list<int32>","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"int32","type":"Action"}]


def testPlatON():
    #w3 = Web3(WebsocketProvider("ws://47.105.180.114:6790"))
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)
    #w3.personal.unlockAccount(from_address, "platoncross", 999999)
    #block_number = platon.blockNumber
    #print(block_number)
    #print(platon.syncing)
    #print(platon.gasPrice)
    #print(platon.accounts)
    #print(platon.evidences)
    #print(platon.consensusStatus)
    #print(platon.getBalance('lat1a3tlqd07aps8tjsegz967gdq686qttk2e2p4kw'))
    #print(platon.getCode('lat1fp0tmgk2e20765q7zaxmelzfhqsp3w49ghakld'))
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)
    tx_hash0 = hello.functions.makeNumber().transact({'from':from_address,'gas':1500000,})
    tx_hash1 = hello.functions.makeNumber().transact({'from':from_address,'gas':1500000,})
    tx_receipt0 = platon.waitForTransactionReceipt(tx_hash0)
    tx_receipt1 = platon.waitForTransactionReceipt(tx_hash1)

    #print(tx_receipt0)
    #print(tx_receipt1)


def testDelete():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_delete_hash = hello.functions.deleteNumber().transact({'from':from_address,'gas':1500000})


def testCall():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    #result = hello.functions.get_Numbers().call()
    #print(result)

    tx_events_hash = hello.functions.calcAdd(10, 5).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    #print(tx_events_receipt)

    topic_param = hello.events.Add().processReceipt(tx_events_receipt)
    print(topic_param)


def testList():
    a = ['12', '13', '14', ['55'], ['66']]
    for i in a:
        if isinstance(i, list):
            print(i[0])
        else:
            print(i)

def testConnect():
    #w3 = Web3(WebsocketProvider("ws://47.105.180.114:6790"))
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)
    w3.personal.unlockAccount(from_address, "platoncross", 999999)
    block_number = platon.blockNumber
    print(block_number)

#testConnect()

testCall()
