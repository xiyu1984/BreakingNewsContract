from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from hexbytes import HexBytes

true = True
false = False
contractAddr = 'lat1gtgk6sela6q9ykypexf289fpkg4waxss3h40nw'
from_address = 'lat1a3tlqd07aps8tjsegz967gdq686qttk2e2p4kw'
cabi = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"string"},{"name":"arg2","type":"uint32"}],"name":"hello_event","topic":1,"type":"Event"},{"baseclass":[],"fields":[{"name":"head","type":"string"}],"name":"message","type":"struct"},{"baseclass":["message"],"fields":[{"name":"body","type":"string"},{"name":"end","type":"string"}],"name":"my_message","type":"struct"},{"constant":false,"input":[{"name":"one_message","type":"my_message"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"one_message","type":"my_message"},{"name":"a","type":"uint32"},{"name":"b","type":"uint32"}],"name":"add_message","output":"my_message[]","type":"Action"},{"constant":true,"input":[{"name":"name","type":"string"}],"name":"get_message","output":"my_message[]","type":"Action"},{"constant":false,"input":[],"name":"hello_abort","output":"void","type":"Action"},{"constant":false,"input":[],"name":"hello_panic","output":"void","type":"Action"}]


def testPlatONwasm():
    w3 = Web3(HTTPProvider("http://47.105.180.114:6789"))
    platon = PlatON(w3)

    hello = platon.wasmcontract(address=contractAddr, abi=cabi,vmtype=1)
    tx_hash = hello.functions.add_message([['5'], '6', '7'], 73, 8).transact({'from':from_address,'gas':1500000})
    tx_events_receipt = platon.waitForTransactionReceipt(tx_hash)

    #print(tx_events_receipt)

    topic_param = hello.events.hello_event().processReceipt(tx_events_receipt)
    print(topic_param)


testPlatONwasm()
