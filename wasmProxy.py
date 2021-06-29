from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON

true = True
false = False
from_address = 'lat1ar0s6re3qpe3rt39523qw4jars6s4sdhak459n'

proxyAddr = 'lat1cjm72p0p6p99g8rmd2hmwg3nrqpam2a9pcaklr'
proxy_abi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"incalcAdd","topic":1,"type":"Event"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"inmakeSum","topic":1,"type":"Event"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"RegisterContract","output":"bool","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"pair<int32,bool>","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"const_calcAdd","output":"pair<int32,bool>","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"makeSum","output":"pair<int32,bool>","type":"Action"}]

contractAddr = 'lat1wcup0qmwtl09nawc5xjdkcmrcnh0f4a9v0sxh8'
contract_abi = [{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"proxyAddr","type":"string"}],"name":"RegisterProxy","output":"bool","type":"Action"},{"constant":true,"input":[],"name":"GetProxyAddress","output":"string","type":"Action"},{"constant":true,"input":[],"name":"GetOwnerAddress","output":"string","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"updateContract","output":"bool","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"int32","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"const_calcAdd","output":"int32","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"makeSum","output":"int32","type":"Action"}]

contractAddr_2 = 'lat1um4c0s739hxtsxula76aqf56w8k4l3cnh8w2gx'
contract_2_abi = [{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"proxyAddr","type":"string"}],"name":"RegisterProxy","output":"bool","type":"Action"},{"constant":true,"input":[],"name":"GetProxyAddress","output":"string","type":"Action"},{"constant":true,"input":[],"name":"GetOwnerAddress","output":"string","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"updateContract","output":"bool","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"int32","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"const_calcAdd","output":"int32","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"makeSum","output":"int32","type":"Action"}]
nodeIP = "http://47.241.69.26:6789"
#nodeIP = 'https://openapi.platon.network/rpc'


def proxyCall():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=proxyAddr, abi=proxy_abi,vmtype=1)

    tx_events_hash = hello.functions.calcAdd(73, 8).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    rstAdd2 = hello.events.incalcAdd().processReceipt(tx_events_receipt)
    print('***********************calcAdd: ')
    print(rstAdd2)

    tx_events_hash_sum = hello.functions.makeSum([11, 12, 13]).transact({'from':from_address,'gas':1500000})
    tx_events_receipt_sum = platon.waitForTransactionReceipt(tx_events_hash_sum)
    rstAdd_sum = hello.events.inmakeSum().processReceipt(tx_events_receipt_sum)
    print('')
    print('***********************makeSum: ')
    print(rstAdd_sum[0]['args']['arg1'])
    return


def contract_1_Call():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=contract_abi,vmtype=1)
    rstAdd = hello.functions.calcAdd(73, 8).call()
    print(rstAdd)

    rstConst_Add = hello.functions.const_calcAdd(100, 99).call()
    print(rstConst_Add)

    rstSum = hello.functions.makeSum([11, 12, 13]).call()
    print(rstSum)
    return

def contract_2_Call():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr_2, abi=contract_2_abi,vmtype=1)
    rstAdd = hello.functions.calcAdd(73, 8).call()
    print(rstAdd)

    rstConst_Add = hello.functions.const_calcAdd(100, 99).call()
    print(rstConst_Add)

    rstSum = hello.functions.makeSum([11, 12, 13]).call()
    print(rstSum)
    return


def contract_1_to_2():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr, abi=contract_abi,vmtype=1)

    print(hello.functions.GetOwnerAddress().call())
    print(hello.functions.GetProxyAddress().call())

    tx_events_hash = hello.functions.updateContract(contractAddr_2).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    print(tx_events_receipt)


def c2_registerProxy():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr_2, abi=contract_2_abi,vmtype=1)

    print(hello.functions.GetProxyAddress().call())

    tx_events_hash = hello.functions.RegisterProxy(proxyAddr).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    print(tx_events_receipt)

    print(hello.functions.GetProxyAddress().call())


def contract_2_to_1():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=contractAddr_2, abi=contract_2_abi,vmtype=1)

    print(hello.functions.GetOwnerAddress().call())
    print(hello.functions.GetProxyAddress().call())

    tx_events_hash = hello.functions.updateContract(contractAddr).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    print(tx_events_receipt)


def whichContract():
    w3 = Web3(HTTPProvider(nodeIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=proxyAddr, abi=proxy_abi,vmtype=1)

    rst = hello.functions.const_calcAdd(73, 8).call()
    print(rst)


proxyCall()
#contract_1_Call()
#contract_2_Call()
#whichContract()
#contract_1_to_2()
#contract_2_to_1()
#c2_registerProxy()
