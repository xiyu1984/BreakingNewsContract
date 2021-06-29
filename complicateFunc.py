from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

true = True
false = False
contractAddr = 'lat1ktum8z9m9j4pz0l0gytqqrfgt8uv9sxnptxg9d'
from_address = 'lat1ar0s6re3qpe3rt39523qw4jars6s4sdhak459n'
cabi = [{"constant":true,"input":[],"name":"makeCall","output":"int32","type":"Action"},{"constant":false,"input":[{"name":"ele","type":"int32"}],"name":"putElement","output":"void","type":"Action"},{"constant":false,"input":[],"name":"clearElement","output":"void","type":"Action"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"uint32"}],"name":"Add","topic":1,"type":"Event"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"call2","topic":1,"type":"Event"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"put","topic":1,"type":"Event"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"uint32"}],"name":"clear","topic":1,"type":"Event"},{"baseclass":[],"fields":[{"name":"myParams","type":"int32[]"}],"name":"inputParams","type":"struct"},{"constant":false,"input":[{"name":"ipa","type":"inputParams"}],"name":"init","output":"void","type":"Action"},{"constant":true,"input":[],"name":"getParams","output":"int32[]","type":"Action"},{"constant":false,"input":[],"name":"AddCalc","output":"int32","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"makeCall2","output":"int32","type":"Action"}]
nodeIP = "http://47.241.69.26:6789"

def testCall():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    result = hello.functions.getParams().call()
    print(result)

def testMakeCall():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    result = hello.functions.makeCall().call()
    print(result)


def testClear():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_hash = hello.functions.clearElement().transact({'from': from_address, 'gas': 999999})

    tx_receipt = platon.waitForTransactionReceipt(tx_hash)
    #print(tx_receipt)

    topic_param = hello.events.clear().processReceipt(tx_receipt)
    print(topic_param)


def testOps():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_hash = hello.functions.putElement(13).transact({'from': from_address, 'gas': 999999})

    tx_receipt = platon.waitForTransactionReceipt(tx_hash)
    #print(tx_receipt)

    topic_param = hello.events.put().processReceipt(tx_receipt)
    print(topic_param)


def testAdd():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)

    tx_hash = hello.functions.AddCalc().transact({'from': from_address, 'gas': 999999})

    tx_receipt = platon.waitForTransactionReceipt(tx_hash)
    #print(tx_receipt)

    topic_param = hello.events.Add().processReceipt(tx_receipt)
    print(topic_param)

#testOps()
#testCall()
#testClear()
testAdd()
#testMakeCall()
