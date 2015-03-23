import json
from collections import OrderedDict
from binascii import unhexlify
from time import time
import rsa
import hashlib

from modules.Error import *

PUBLIC_EXPONENT = 65537
REWARD_AMOUNT = '50'

class Block(object):
    '''
        Abstract superclass of shared block functions.
    '''
    ############################
    # TRANSACTION VERIFICATION #
    ############################
    
    ''' Verify that a transaction is signed. Uses child functions get_tx_h. '''
    def tx_inputs_signed(self,tx):
        for input in tx['ins']:
            msg = tx['time'] + input['tx'] + input['tx_i'] + input['out_i']
            try:
                input_trans = self.get_tx_h(input['tx'])
                pubkey = input_trans['outs'][int(input['tx_i'])]['address']
            except TypeError:
                raise NoSuchInputException()
            try:
                rsa.verify(unhexlify(msg), unhexlify(input['sig']), rsa.PublicKey(int(pubkey,16), PUBLIC_EXPONENT))
            except rsa.pkcs1.VerificationError:
                return False
        return True
        
    ''' Verify that a transaction input has not been used previously. Uses child functions output_used. '''
    def tx_inputs_usable(self,tx):
        for input in tx['ins']:
            if self.output_used(input['tx'],input['tx_i']):
                return False
        return True
    
    ''' Get whether or not an output has been used within a block. Called from within child overrides.'''
    def output_used(self,block,hash,i):
        for tHash,transaction in block['transactions'].items():
            for input in transaction['ins']:
                if input['tx'] == hash and input['tx_i'] == i:
                    return (tHash,i)
        return False

class Blockchain(Block):
    '''
        The main Blockchain object.
        Functions suffixed with _h mean "by hash"
    '''
    def __init__(self,file):
        data = open(file).read()
        self.blockchain = json.loads(data, object_pairs_hook=OrderedDict)
    
    ''' Get a transaction by hash. '''
    def get_tx_h(self,hash):
        for _,block in self.blockchain.items():
            for tHash,transaction in block['transactions'].items():
                if tHash == hash:
                    return transaction
    
    ''' Get whether or not an output has been used within the blockchain. '''
    def output_used(self,hash,i):
        for _,block in self.blockchain.items():
            r = super(Blockchain,self).output_used(block,hash,i)
            if r:
                return r
        return False
    
class NewBlock(Block):
    def __init__(self,prev_hash,miner_address):
        self.hash = ""
        self.block = {} #TODO get from IRC
        self.block['nonce'] = ""
        self.block['prev'] = prev_hash
        self.block['start_time'] = str(int(time()))
        self.block['solve_time'] = ""
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
        for tHash,transaction in self.block['transactions'].items():
            if tHash == hash:
                return transaction
    
    ''' Get whether or not an output has been used within the new block. '''
    def output_used(self,hash,i):
        return super(NewBlock,self).output_used(self.block,hash,i)
        
    def add_transaction(self,tx):
        pass
        #TODO
            
    def win(self,nonce):
        self.block['nonce'] = nonce
        self.block['solve_time'] = str(int(time()))
        self.hash = gen_block_hash(self.block)
        
        #TODO broadcast to IRC



def gen_block_hash(block):
    m = hashlib.sha256()
    
    text = block['prev']
    text += block['start_time']
    for t_hash,transactions in block['transactions'].items():
        text += t_hash
        for input in transactions['ins']:
            text += input['tx']
            text += input['tx_i']
            text += input['out_i']
            text += input['sig']
        for output in transactions['outs']:
            text += output['address']
            text += ((int(len(output['amount']))%2)*'0') + output['amount'] # Make number even length (zero-padding)
    text += ((int(len(block['nonce']))%2)*'0') + block['nonce'] # Make number even length (zero-padding)
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
        text += ((int(len(output['amount']))%2)*'0') + output['amount'] # Make number even length (zero-padding)
        
    m.update(unhexlify(text))
    return m.hexdigest()


# UNHEXLIFY: convert hex string to bytes
# HEXLIFY  : convert bytes to hex string
# signature: time + referenced_tx + referenced_tx_index

    
# NOTE: public addresses are: hex(pubkey)[2:]
# INIT
bc = Blockchain('block.chain')

nc = NewBlock('0000000000000000000000000000000000000000000000000000000000000000','82321151d85f79a9046ed4e73a9715156d22b5e22783d1db8ef78cde98a113b536a52cb70f5851496d8273fa3c09f07c972d37c7cf9977c9e55c61eb0329df7d')

tx = {
            'time': '1427089580',
            'outs':
            [
                {
                    'amount': '50',
                    'address': '82321151d85f79a9046ed4e73a9715156d22b5e22783d1db8ef78cde98a113b536a52cb70f5851496d8273fa3c09f07c972d37c7cf9977c9e55c61eb0329df7d'
                }
            ],
            'ins':
            [
                {
                    'tx':'7dfb1032b7c9d57ae5e396c6ec12b3991319a55712d5d93e2c192594ad1f09d6',
                    'tx_i':'00',
                    'out_i':'00',
                    'sig':'aaaa'
                }
            ]
        }


try:
    bc.tx_inputs_usable(tx)
except NoSuchInputException:
    nc.tx_inputs_usable(tx)


print(bc.tx_inputs_signed(tx))
print(nc.tx_inputs_signed(tx))

'''
- get transaction
- assert not in newblock
- assert not in blockchain
- assert inputs_useable
- assert inputs_signed
- assert for each out: (ins(with out_i) combined) >= outs amount
-            if greater, return surplus back to sender in a new out

'''
