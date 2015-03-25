from binascii import unhexlify

from Class.Exception import *
from Class.Blockchain import Blockchain
from Class.WorkBlock import WorkBlock
from module import txgen


# UNHEXLIFY: convert hex string to bytes
# HEXLIFY  : convert bytes to hex string


# INIT
bc = Blockchain('block.chain')
prevblock = '0000000000000000000000000000000000000000000000000000000000000000'
miner = '82321151d85f79a9046ed4e73a9715156d22b5e22783d1db8ef78cde98a113b536a52cb70f5851496d8273fa3c09f07c972d37c7cf9977c9e55c61eb0329df7d'
wb = WorkBlock(bc,prevblock,miner)


tx = {
            'time': '1427089580',
            'outs':
            [
                {
                    'amount': '1',
                    'address': '82321151d85f79a9046ed4e73a9715156d22b5e22783d1db8ef78cde98a113b536a52cb70f5851496d8273fa3c09f07c972d37c7cf9977c9e55c61eb0329df7d'
                }
            ],
            'ins':
            
            [
                {
                    'tx':'4b252922a1581c8a6dfd4bba25dd43f3f6bac043b6707d82f471d259f47d1d61',
                    'tx_i':'00',
                    'out_i':'00',
                    'sig':'27f972e4b382c27c2c17c2a8d7c74075e7369d076c7897bbce200609eec2fb3459e9c215bf24fccb6984cfe575003191cb21cc6123f4d1a0343e94abdb6caa66'
                }
            ]
        }

privkey1 = (5083279484401429324073056983841183282431776454681299888105805385648700526596680163,1449864890410299501964644048563276303195782151519820095824366143508776907,5651394174524078934168162938693777262175721225289623885571130003140775246884408047329635271920555263165412437473280575576344843125562713057545608985843097)

privkey2 = (6968223843230798705543442829794906286227291833860491327471375594944479345114184147,1664988739484861807187105005799608620354005132287471160152809096849779613,4819287325787109228139822323811514448951413276363465355747835384658054639311320686678997358399315479692586720056971693513646408262534014599316446871051081)



#wb.verify_tx_integrity('4b252922a1581c8a6dfd4bba25dd43f3f6bac043b6707d82f471d259f47d1d61',tx) #TODO make this implemented in blockchain init

#tx = wb.get_tx_h('4b252922a1581c8a6dfd4bba25dd43f3f6bac043b6707d82f471d259f47d1d61')

# note that at the moment, block.chain hashes need recalculation.
# thusly, signatures do as well.
print(txgen.sign_input(tx,0,privkey2))

try:
    wb.add_tx(tx)
except BlockException as e:
    print("FAILED TO ADD TRANSACTION: " + str(e))


'''
- get transaction
- assert not in newblock
- assert not in blockchain
- assert inputs_useable
- assert inputs_signed
- assert for each out: (ins(with out_i) combined) >= outs amount
-            if greater, return surplus back to sender in a new out
'''
