from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

true = True
false = False
contractAddr = 'lat1gtgk6sela6q9ykypexf289fpkg4waxss3h40nw'
from_address = 'lat1ar0s6re3qpe3rt39523qw4jars6s4sdhak459n'
cabi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"string"},{"name":"arg2","type":"uint32"}],"name":"hello_event","topic":1,"type":"Event"},{"baseclass":[],"fields":[{"name":"head","type":"string"}],"name":"message","type":"struct"},{"baseclass":["message"],"fields":[{"name":"body","type":"string"},{"name":"end","type":"string"}],"name":"my_message","type":"struct"},{"constant":false,"input":[{"name":"one_message","type":"my_message"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"one_message","type":"my_message"},{"name":"a","type":"uint32"},{"name":"b","type":"uint32"}],"name":"add_message","output":"my_message[]","type":"Action"},{"constant":true,"input":[{"name":"name","type":"string"}],"name":"get_message","output":"my_message[]","type":"Action"},{"constant":false,"input":[],"name":"hello_abort","output":"void","type":"Action"},{"constant":false,"input":[],"name":"hello_panic","output":"void","type":"Action"}]


eventCalledAddr = 'lat1npffkce3elp0ug80r85rz36y9gjkgevnculnyn'
eventCalled_abi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"outCaller","topic":1,"type":"Event"},{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"baseclass":[],"fields":[{"name":"i","type":"int32"},{"name":"s","type":"string"}],"name":"MyStruct","type":"struct"},{"constant":false,"input":[{"name":"ms","type":"MyStruct"}],"name":"outCall","output":"int32","type":"Action"}]

outCallerAddr = 'lat1wtmjnquh4y08mpvrn0zd4c7hhyy0g9qpz58q8a'
outCaller_abi = [{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[],"name":"Call","output":"int32","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"}],"name":"setContract","output":"void","type":"Action"},{"constant":true,"input":[],"name":"getContract","output":"string","type":"Action"}]

def testPlatONwasm():
    w3 = Web3(HTTPProvider("http://47.241.69.26:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)
    #tx_hash = hello.functions.add_message([['5'], '6', '7'], 73, 8).transact({'from':from_address,'gas':1500000})
    #tx_events_receipt = platon.waitForTransactionReceipt(tx_hash)
    tx_events_receipt = platon.waitForTransactionReceipt(HexBytes('0x89b8e6bc13c0b48dee24f708ed37671e34c54c6b50206e0d37eab4ba011c1365'))

    #print(tx_events_receipt)

    topic_param = hello.events.hello_event().processReceipt(tx_events_receipt)
    print(topic_param)


def testEventWasm():
    w3 = Web3(HTTPProvider("http://47.241.69.26:6789"))
    platon = PlatON(w3)

    eventCalled = platon.wasmcontract(address=eventCalledAddr, abi=eventCalled_abi,vmtype=1)
    #tx_hash1 = eventCalled.functions.outCall([0, '']).transact({'from':from_address,'gas':1500000})
    #tx_events_receipt1 = platon.waitForTransactionReceipt(tx_hash1)


    outCaller = platon.wasmcontract(address=outCallerAddr, abi=outCaller_abi,vmtype=1)
    tx_hash = outCaller.functions.Call().transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_hash)

    #这里需要使用实际emit事件的合约获取events
    topic_param = eventCalled.events.outCaller().processReceipt(tx_events_receipt)
    print(topic_param)

#testPlatONwasm()
testEventWasm()
