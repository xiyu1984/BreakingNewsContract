from client_sdk_python import Web3, HTTPProvider, WebsocketProvider
from client_sdk_python.eth import PlatON
from client_sdk_python.utils import contracts

false = False
true = True
instanceABI = [{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"int32"}],"name":"addCallResult","topic":1,"type":"Event"},{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"eles","type":"int32[]"}],"name":"testAddFromGeneralProxy","output":"bool","type":"Action"}]
instanceContractAddr = 'lat1dgcfjgtdw56rjg7fa90gwzg0fdw9lyrs3kka28'

clientAccount = 'lat1ar0s6re3qpe3rt39523qw4jars6s4sdhak459n'

proxyABI= [{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"contractAddr","type":"string"},{"name":"params","type":"uint8[]"}],"name":"generalCall","output":"bool","type":"Action"}]
proxyContractAddr = 'lat1assxpver7n0a4tldhuyhpdlxt2qmdtngsetaem'

devIP = 'http://47.241.69.26:6789'
mainIP = 'http://192.168.1.49:6789'


def find_and_cheke_fn_abi(abi, fn_name):
    for i in abi:
        if i['name'] == fn_name:
            i['inputs'] = i.pop('input')
            #i['outputs'] = i.pop('output')
            return i
    return None

def generalProxy():
    w3 = Web3(HTTPProvider(devIP))
    platon = PlatON(w3)
    hello = platon.wasmcontract(address=proxyContractAddr, abi=proxyABI, vmtype=1)

    instanceCalled = platon.wasmcontract(address=instanceContractAddr, abi=instanceABI,vmtype=1)

    #这里需使用实例的fn_abi进行打包（本例中为AddressParam::testAddFromGeneralProxy接口）
    fn_abi = find_and_cheke_fn_abi(instanceABI, 'testAddFromGeneralProxy')
    print(fn_abi)
    if not fn_abi is None:
        params = contracts.encode_abi(w3, fn_abi, [[11, 12, 13]], vmtype=1, data=None, setabi=instanceABI)
        print(params)
        print(int(params, 16))
        print(Web3.toBytes(int(params, 16)))

        tx_events_hash = hello.functions.generalCall(instanceContractAddr, int(params, 16)).transact({'from':clientAccount,'gas':1500000})
        tx_events_receipt = platon.waitForTransactionReceipt(tx_events_hash)

        #使用instance的wasmcontract实例来捕获事件
        topic_param = instanceCalled.events.addCallResult().processReceipt(tx_events_receipt)
        print(topic_param)
        

generalProxy()
