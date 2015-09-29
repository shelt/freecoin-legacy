# freecoin

**freecoin** is a fully-functional cryptocurrency I'm making for fun.

## Background

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Mathematics and cryptographic principles allow a user to prove they were the recipient of a past transaction.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has along with the processing of transactions all happens on a few servers. Though they probably take backups and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works.

## Protocol specification

```
************ Freecoin Protocol v1 Specification ************

Messages can be of fixed or variable size. If they are the
latter, some fixed part of their content specifies the
size of the following content.
All messages have an additional byte prefix specifying their
method (not included in method table).

** ERROR CONSTANTS **
Blocks
------------------------------------------------------------
Value   : 
Name    : BLOCK_VERSION_INVALID
Meaning : Block version is invalid
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_CONFLICT
Meaning : Block's prev_hash is already in another block
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_TIME_INVALID
Meaning : Block's time is before last block
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_MRKROOT_INVALID
Meaning : Block's merkle_root does not match body
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_TARGET_INVALID
Meaning : Block's target is not correct
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_HASH_INVALID
Meaning : Block's hash isn't valid as per the target
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_BODY_INVALID
Meaning : Transaction(s) in body are invalid. No tx-
          specific reject messages are needed because the
          offender shouldn't have send such a block anyway.
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_OVERFLOW
Meaning : Block is larger than 1 MB.
Content : Offending block's hash
------------------------------------------------------------
Value   : 
Name    : BLOCK_EXISTS
Meaning : Block already exists in the blockchain.
Content : Offending block's hash
------------------------------------------------------------

Transactions
------------------------------------------------------------
Value   : 
Name    : TX_VERSION_INVALID
Meaning : Transaction's version is invalid
Content : Offending transaction's hash
------------------------------------------------------------
Value   : 
Name    : TX_SIGNATURE_INVALID
Meaning : Transaction input signature is invalid
Content : Offending transaction's hash
------------------------------------------------------------
Value   : 
Name    : TX_REF_USED
Meaning : A transaction input is used already
Content : Offending transaction's hash
------------------------------------------------------------
Value   : 
Name    : TX_REF_NONEXIST
Meaning : A transaction input doesn't exist.
Content : Offending transaction's hash
------------------------------------------------------------
Value   : 
Name    : TX_REF_OVERSPEND
Meaning : A transaction's inputs are smaller than outputs.
Content : Offending transaction's hash
------------------------------------------------------------

Generic
------------------------------------------------------------
Value   : 1
Name    : BAD_VERSION
Meaning : Incompatible version
Content : None
------------------------------------------------------------
Value   : 
Name    : MESSAGE_MALFORMED
Meaning : Data within the message was not parsed according
          to specification.
Content : <any>
------------------------------------------------------------
Value   : 
Name    : UNKNOWN_METHOD
Meaning : First byte represents an unknown protocol method.
Content : <any>
------------------------------------------------------------


** DATATYPES ** (used for inv and getdata)
0 block
1 transaction


** CONTENT-TYPE METHODS **
------------------------------------------------------------
Value   : 0
Name    : reject
Size    : 1B + 2B + <=256B (ERRORTYPE + info_size + info)
Purpose : Tell a peer that a block/tx/time/alert/version is
          invalid. Sent in response to various messages.
Content : ERRORTYPE byte and info about what specifically
          is invalid    
------------------------------------------------------------
Value   : 1
Name    : version
Size    : 2B + 4B (version + time)
Purpose : Initialize handshake from client to server. Sent
          unsolicited.
Content : Version of sender's protocol implementation
------------------------------------------------------------
Value   : 2
Name    : verack
Size    : 0B
Purpose : Acknowledge client's version during handshake.
          Solicited by a version message.
Content : N/A
------------------------------------------------------------
Value   : 3
Name    : addr
Size    : 4B + ?*88B (addr_count + struct peer_info[])
Purpose : Tell peer about active peers. Sent unsolicited and
          during handshake.
Content : List of addresses active <3 hrs ago along with
          respective ports and last_active time.
------------------------------------------------------------
Value   : 4
Name    : getblocks
Size    : 256B + 2B (startblock + block_count)
Purpose : Request an inv containing startblock and 
          block_count blocks following it (up to 500). Used
          to update blockchain from a block onward. Sent
          unsolicited.
Content : A block hash and a block count
------------------------------------------------------------
Value   : 5
Name    : mempool
Size    : 0B
Purpose : Request an inv containing peer's mempool txs. Sent
          unsolicited.
Content : N/A
------------------------------------------------------------
Value   : 6
Name    : inv
Size    : 1B + ?*256B (DATATYPE + data_ids[])
Purpose : Tell peer about blocks or txs that you have. Sent
          unsolicited or in response to getblocks or
          mempool.
Content : List of tx or block hashes that you have in your
          mempool or blockchain, respectively
------------------------------------------------------------
Value   : 7
Name    : getdata
Size    : 1B + ?*256B (DATATYPE + data_ids[])
Purpose : Request full block(s)/tx(s). Sent unsolicited.
Content : DATATYPE byte and identifying hash(es)
------------------------------------------------------------
Value   : 8
Name    : block
Size    : 80B to 1MB (header + body)
Purpose : Send a full block to a peer. Solicited by getdata.
Content : A full block.
------------------------------------------------------------
Value   : 9
Name    : tx
Size    : 10B + ?*160 + ?*132 (header + ins + outs)
Purpose : Send a full transaction to a peer. Solicited by
          getdata.
Content : A full transaction.
------------------------------------------------------------
Value   : 10
Name    : alert
Size    : 1B + 4B + 100B + 1024B
          (ALERTTYPE + time + msg + sig{ALERTTYPE+time+msg})
Purpose : Notify entire network about network emergency.
          Only valid if signed by key at key.shelt.ca.
Content : Information about the network issue. Depending on
          ALERTTYPE, the implementation may need to take
          action (such as not allowing mining).
------------------------------------------------------------
Value   : 11
Name    : ping
Size    : 0B
Purpose : Verify connectivity of peer. Sent unsolicited.
Content : N/A
------------------------------------------------------------
Value   : 12
Name    : pong
Size    : 0B
Purpose : Verify connectivity of self to peer. Solicited by
          ping.
Content : N/A
------------------------------------------------------------


** GENERAL PROCEDURE **

Become a peer client-wise:
- Send server a <version>
- Recieve a <verack> from server if your <version> matched theirs.
- Recieve a <version> from server
- Send server an <addr>. DONE EVERY 24 HOURS TO ALL PEERS
- Send <getblocks> containing your latest block

Become a peer server-wise:
- Recieve a <version> from a client
- Send client a <verack> if your version matches
- Send client a <version>
- Send client an <addr>. DONE EVERY 24 HOURS TO ALL PEERS

Recieving <addr>:
- store addresses you don't have already that are younger than 3 hours.

Recieving <getaddr>:
- Send client active peers from your addresslist

Submit transaction(s):
- Send all peers an <inv> containing new transaction hash(es)

Recieving an <inv>:
- If you don't have a transaction or block listed in the <inv>, do <getdata> on all missing data.
- Verify the data
- If it's valid, store the data where it belongs (mempool if tx or blockchain if block, then interrupt miner)
- If it isn't valid, send <reject>

Recieving a <mempool>
- Send an <inv> of your mempool


Recieving a <tx> (which only happens after a request for it):
- Verify integrity (if invalid, send <reject>)
- If you're working on a block and it contains transaction(s), add it to your local mempool
- Send it in an <inv> to peers

Recieving a <block> (which only happens after a request for it):
- Verify integrity (if invalid, send <reject>)
- Add it to your blockchain
- Send it in an <inv> to peers

Recieving a <getdata>:
- Send the requested data.

Recieving a <mempool>:
- Send your mempool in <inv>

Recieving a <getblocks>:
- Send an <inv> containing first 500 blocks they are missing (starting from the start block
    they specified [likely their latest block])

```


## Source guidelines
* `size_t` should be used when dealing with array sizes UNLESS it is measuring lengths of serialized data (where such a number would need to fit in the specified range).
* Memory allocation (i.e., blocks) for a function's output is always done via implementation. Pointers to the pre-allocated memory block are passed into the function (typically as the last parameter).
* Only include things in the header file when the header file itself needs them.

## Todo

#### Roadmap
* recieve <inv> constantly for next mempool once they are in a confirmed block
* verification - what is signed for txs?
* get rid of memcpys and use loops for endian-safeness
* make all functions that copy data to a ptr also return the ptr
* make all function names include the name of what they do/modify (block, tx)
* update time every few seconds (avg of peers)

#### Misc
* The block header is 78 bytes, and the only dynamic component (the nonce) is the last 4. The first 64B chunk of every hash need not be calculated on each iteration.
* Should serial generation functions which take many arguments instead use structs?
* Document protocol in one place.
* Move tx-specific defines to tx header
