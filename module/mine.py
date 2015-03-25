import hashlib
import struct
from binascii import hexlify,unhexlify
import sys
import math

# My HP Pavlilion laptop can do about 80K hashes/min
# it should take 800K hashes to find a block
# it should take 10 minutes to generate 800K
def mine(target):
    ver = 2
    prev_block = "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717"
    mrkl_root = "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a"
    time_ = 0x53058b35
    target = 9999999999999999999999999999999999999999999999999999999999999999994195203
    

    

    nonce = 0
    while nonce < 0x100000000:
        header =  struct.pack("<L", ver)
        header += unhexlify(prev_block)[::-1]
        header += unhexlify(mrkl_root)[::-1]
        header += struct.pack("<LL", time_, nonce)
        hash = hashlib.sha256(hashlib.sha256(header).digest()).digest()
        print nonce, hexlify(hash)
        if int(hexlify(hash),16) < target:
            return nonce
        nonce += 1
        
mine()