import json
from collections import OrderedDict
from binascii import unhexlify
from time import time
import rsa
import hashlib

from modules.Exception import *
from modules import txgen
from modules import util
from modules import crypto


REWARD_AMOUNT = '50'

class Block(object):
    '''
        Abstract superclass of shared block functions.
    '''
    ############################
    ## INTEGRITY VERIFICATION ##
    ############################
    def verify_block_integrity(self,blockhash,block):
        block_gen_valid(block)
        # Transactions
        for tHash,transaction in block['transactions'].items():
            verify_tx_integrity(tHash,transaction)
            tx_amounts_valid(transaction)
        return True
                    
        # Block
        if gen_block_hash(block) != blockhash:
            raise IntegrityException('False block: ' + blockhash)
        return True
    
    def verify_tx_integrity(self,tHash,transaction):
        if gen_tx_hash(transaction) != tHash:
            raise IntegrityException('False transaction: ' + tHash)
        return True
    
    ######################
    # BLOCK VERIFICATION #
    ######################
    def block_gen_valid(self, block):
        author = block['author']
        found = False
        for tHash,transaction in block['transactions'].items():
            if found == True:
                raise IntegrityException('Multiple coin generations in block ' + blockhash)
            if len(transaction['ins']) == 0:
                assert len(transaction['outs']) == 1
                if transaction['outs']['address'] != author:
                    raise IntegrityException('Gen payout not going to author in block ' + blockhash)
                found = True
        return True
        
    ############################
    # TRANSACTION VERIFICATION #
    ############################
    
    ''' Verify that a transaction is signed. Uses child functions get_tx_h. '''
    def tx_inputs_signed(self,tx):
        for input in tx['ins']:
            msg = tx['time'] + input['tx'] + input['tx_i'] + input['out_i']
            _output = tx['outs'][int(input['out_i'])] 
            msg += _output['address'] + util.padint(_output['amount'])
            try:
                input_trans = self.get_tx_h(input['tx'])
                pubkey = input_trans['outs'][int(input['tx_i'])]['address']
            except TypeError:
                raise NoSuchInputException('Input nonexistant: ' + input['tx'] + "-" + input['tx_i'])
            try:
                rsa.verify(unhexlify(msg), unhexlify(input['sig']), rsa.PublicKey(int(pubkey,16), crypto.PUBLIC_EXPONENT))
            except rsa.pkcs1.VerificationError:
                raise SignatureVerificationException("False signature: " + input['sig'])
        return True
        
    ''' Verify that a transaction input has not been used previously. Uses child functions output_used. '''
    def tx_inputs_usable(self,tx):
        for input in tx['ins']:
            if self.output_used(input['tx'],input['tx_i']):
                raise InvalidTransactionException('Output already used: ' + input['tx'] + '-' + input['tx_i'])
        return True
    
    ''' Get whether or not an output has been used within a block. Called from within child overrides.
        This function searches a specific block, whereas overrides call this one repeatedly.
    '''
    def output_used(self,block,hash,i):
        for tHash,transaction in block['transactions'].items():
            for input in transaction['ins']:
                if input['tx'] == hash and input['tx_i'] == i and (gen_tx_hash(transaction) != tHash):
                    return (tHash,i)
        return False
        
    def tx_amounts_valid(self,tx):
        change = {}
        
        if (len(tx['ins']) == 0) and (len(tx['outs']) == 1):
            # Coin generation
            return -1
        elif len(tx['outs']) > len(tx['ins']):
            raise InvalidTransactionException('More outputs than inputs')
        
        for o_index,output in enumerate(tx['outs']):
            if int(output['amount']) == 0:
                raise InvalidTransactionException('Zero-value output at output ' + str(o_index))
            in_total = 0
            for i_index,input in enumerate(tx['ins']):
                if int(input['out_i']) == o_index:
                    in_tx = self.get_tx_h(input['tx'])
                    in_amount  = int(in_tx['outs'][int(input['tx_i'])]['amount'])
                    in_total += in_amount
                    
                    # Change?
                    if int(output['amount']) < in_amount:
                        in_address = in_tx['outs'][int(input['tx_i'])]['address']
                        change[in_address] = in_amount - int(output['amount'])
            if int(output['amount']) > in_total:
                raise InvalidTransactionException('Overspending at output ' + str(o_index))
        return change
        
        

class Blockchain(Block):
    '''
        Class for the file-specified blockchain.
        Functions suffixed with _h mean "by hash"
    '''
    def __init__(self,file):
        data = open(file).read()
        self.blockchain = json.loads(data, object_pairs_hook=OrderedDict)
    
    ''' Get a transaction by hash.
            It is inadvisable to use this function when working on a new block
        because the workblock is not searched. Use the WorkBlock functions
        instead (which search both).'''
    def get_tx_h(self,hash):
        for _,block in self.blockchain.items():
            try:
                return block['transactions'][hash]
            except KeyError:
                pass
    
    ''' Get whether or not an output has been used within the blockchain. 
            It is inadvisable to use this function when working on a new block
        because the workblock is not searched. Use the WorkBlock functions
        instead (which search both).
    '''
    def output_used(self,hash,i):
        for _,block in self.blockchain.items():
            r = super(Blockchain,self).output_used(block,hash,i)
            if r:
                return r
        return False
    
class WorkBlock(Block):
    def __init__(self,blockchain,prev_hash,miner_address):
        '''
            Class for the current work block.
            Functions such as "output_used" search both this AND the passed blockchain object.
        '''
        self.BLOCKCHAIN = blockchain.blockchain
        self.hash = ""
        self.block = {} #TODO get from IRC
        self.block['nonce'] = ""
        self.block['prev'] = prev_hash
        self.block['start_time'] = str(int(time()))
        self.block['solve_time'] = ""
        self.block['author'] = miner_address
        self.block['transactions'] = {"0000":{ # Temp value
                "time":str(int(time())),
                'ins':[],
                'outs':[{
                "address":miner_address,
                "amount":REWARD_AMOUNT
                }]
            }
        }
        # Set reward transaction hash
        self.block['transactions'][gen_tx_hash(self.block['transactions']["0000"])] = self.block['transactions']["0000"]
        del self.block['transactions']["0000"]
    
    ''' Get a transaction by hash. '''
    def get_tx_h(self,hash):
        # SEARCH BLOCKCHAIN
        for _,block in self.BLOCKCHAIN.items():
            try:
                return block['transactions'][hash]
            except KeyError:
                pass
        # SEARCH WORKBLOCK
        try:
            return self.block['transactions'][hash]
        except KeyError:
            pass
    
    ''' Get whether or not an output has been used within the new block. '''
    def output_used(self,hash,i):
        # SEARCH BLOCKCHAIN
        for _,block in self.BLOCKCHAIN.items():
            r = super(WorkBlock,self).output_used(block,hash,i)
            if r:
                return r
        # SEARCH WORKBLOCK
        return super(WorkBlock,self).output_used(self.block,hash,i)
        
        
    def add_tx(self,tx):
        hash = gen_tx_hash(tx)
        if self.get_tx_h(hash):
            raise TransactionExistsException("TransactionExistsException")
        self.tx_inputs_usable(tx)
        self.tx_inputs_signed(tx)
        change = self.tx_amounts_valid(tx)
        assert change != -1 # not a gen-transaction
        for address,amount in change.items():
            tx['outs'].append({
                                "address":address,
                                "amount":amount
                              })
        print("DEBUG")
        print(tx)
        #self.verify_tx_integrity() need not be called, because it only verifies the hash at the moment.
        
#TODO create a function get_valid_inputs_for_address(address)
        
            
    def win(self,nonce):
        self.block['nonce'] = nonce
        self.block['solve_time'] = str(int(time()))
        self.hash = gen_block_hash(self.block)
        
        #TODO broadcast new block to IRC



def gen_block_hash(block):
    m = hashlib.sha256()
    
    text = block['prev']
    text += block['start_time']
    text += block['author']
    for t_hash,transactions in block['transactions'].items():
        text += t_hash
        for input in transactions['ins']:
            text += input['tx']
            text += input['tx_i']
            text += input['out_i']
            text += input['sig']
        for output in transactions['outs']:
            text += output['address']
            text += util.padint(_output['amount'])
    text += util.padint(block['nonce'])
    m.update(unhexlify(text))
    return m.hexdigest()

def gen_tx_hash(tx):
    m = hashlib.sha256()
    text = tx['time']
    for input in tx['ins']:
        text += input['tx']
        text += input['tx_i']
        text += input['out_i']
        text += input['sig']
    for output in tx['outs']:
        text += output['address']
        text += util.padint(output['amount'])
        
    m.update(unhexlify(text))
    return m.hexdigest()


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
