from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON

true = True
false = False
from_address = 'lat1a3tlqd07aps8tjsegz967gdq686qttk2e2p4kw'

proxyAddr = 'lat1lzrmqva7sqm0c789hrzuf49cdgx5xsz5kfztcr'
proxy_abi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"incalcAdd","topic":1,"type":"Event"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"inmakeSum","topic":1,"type":"Event"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"RegisterContract","output":"bool","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"pair<int32,bool>","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"const_calcAdd","output":"pair<int32,bool>","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"makeSum","output":"pair<int32,bool>","type":"Action"}]

contractAddr = 'lat1c4xxm2vg20yp3tzfyeqekmuh6mu0mgttfenct5'
contract_abi = [{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"proxyAddr","type":"string"}],"name":"RegisterProxy","output":"bool","type":"Action"},{"constant":true,"input":[],"name":"GetProxyAddress","output":"string","type":"Action"},{"constant":true,"input":[],"name":"GetOwnerAddress","output":"string","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"updateContract","output":"bool","type":"Action"},{"constant":false,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"calcAdd","output":"int32","type":"Action"},{"constant":true,"input":[{"name":"a","type":"int32"},{"name":"b","type":"int32"}],"name":"const_calcAdd","output":"int32","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"makeSum","output":"int32","type":"Action"}]

def proxyCall():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=proxyAddr, abi=proxy_abi,vmtype=1)
    rstAdd1 = hello.functions.calcAdd(73, 8).call()
    print(rstAdd1)

    tx_events_hash = hello.functions.calcAdd(73, 8).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)
    rstAdd2 = hello.events.incalcAdd().processReceipt(tx_events_receipt)
    print(rstAdd2)

    rstSum = hello.functions.makeSum([11, 12, 13, 14]).call()
    tx_events_hash_sum = hello.functions.makeSum([11, 12, 13, 14]).transact({'from':from_address,'gas':1500000})
    print(rstSum)

    tx_events_receipt_sum = platon.waitForTransactionReceipt(tx_events_hash_sum)
    rstAdd_sum = hello.events.inmakeSum().processReceipt(tx_events_receipt_sum)
    print(rstAdd_sum[0]['args']['arg1'])
    return


def contract_Call():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=contract_abi,vmtype=1)
    rstAdd = hello.functions.calcAdd(73, 8).call()
    print(rstAdd)

    rstSum = hello.functions.makeSum([11, 12, 13]).call()
    print(rstSum)
    return


proxyCall()
#contract_Call()

