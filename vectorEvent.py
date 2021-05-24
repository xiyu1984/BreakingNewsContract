from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

true = True
false = False
contractAddr = 'lat1ut68npjuq5uxa9ulj5mr0v07l9re5dwzz978zr'
from_address = 'lat1a3tlqd07aps8tjsegz967gdq686qttk2e2p4kw'
cabi = [{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32[]"}],"name":"clear","topic":1,"type":"Event"},{"constant":true,"input":[],"name":"getParams","output":"int32[]","type":"Action"},{"constant":false,"input":[{"name":"ele","type":"int32"}],"name":"putElement","output":"void","type":"Action"},{"constant":false,"input":[],"name":"clearElement","output":"void","type":"Action"}]


def testOps():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_hash = hello.functions.putElement(15).transact({'from': from_address, 'gas': 999999})

    tx_receipt = platon.waitForTransactionReceipt(tx_hash)


def testCall():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    result = hello.functions.getParams().call()
    print(result)


def testClear():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_hash = hello.functions.clearElement().transact({'from': from_address, 'gas': 999999})

    tx_receipt = platon.waitForTransactionReceipt(tx_hash)
    #print(tx_receipt)

    topic_param = hello.events.clear().processReceipt(tx_receipt)
    print(topic_param)


def testGetCode():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)
    print(platon.getCode('lat1mrugy45vlj3cvf8qps3pgz349rw5wtkyn7k7zr'))


#testCall()
#testClear()
#testOps()
testGetCode()

