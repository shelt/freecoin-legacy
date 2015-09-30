# freecoin

**freecoin** is a fully-functional cryptocurrency I'm making for fun.

## Background

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Mathematics and cryptographic principles allow a user to prove they were the recipient of a past transaction.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has along with the processing of transactions all happens on a few servers. Though they probably take backups and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works.

## Protocol specification
#####Revision 4 - 9/28/15

> Freecoin uses a session-layer protocol that enables peer-to-peer communication. It is intended to be used specifically for freecoin. 
>
> The freecoin protocol requires a reliable underlying transport layer protocol. This is due to the nature of the exchanged data; info must be able to permeate the network quickly.

### Message content types
> Messages can be of fixed or variable size. If they are the latter, some fixed part of their content specifies the size of the following content. All messages have an additional 3 bytes at the beginning specifying their client version and their content type.

| Name | reject |
| ---- | ---- |
| Value    | 0  |
| Size     | 1B + 2B + <=256B (ERRORTYPE + info_size + info)  |
| Purpose  | Tell a peer that a block/tx/time/alert/version is invalid. Sent in response to various messages.  |
| Content  | ERRORTYPE byte and info about what specifically is invalid.    |

| Name | getnodes |
| ---- | ---- |
| Value    | 1  |
| Size     | 0B  |
| Purpose  | Request a inv containing peers a server has from which you will connect to one. Sent unsolicited.  |
| Content  | N/A  |

| Name | getblocks |
| ---- | ---- |
| Value    | 2  |
| Size     | 256B + 2B (startblock + block_count)  |
| Purpose  | Request an inv containing startblock and block_count blocks following it (up to 500). Used to update blockchain from a block onward. Sent unsolicited.  |
| Content  | A block hash and a block count.  |

| Name | mempool |
| ---- | ---- |
| Value    | 3  |
| Size     | 0B  |
| Purpose  | Request an inv containing peer's mempool txs. Sent unsolicited.  |
| Content  | N/A  |

| Name | inv |
| ---- | ---- |
| Value    | 4  |
| Size     | 1B + 1B + ?*256B (DATATYPE + data_ids_count + data_ids[])  |
| Purpose  | Tell peer about peers, blocks or txs that you have. Sent unsolicited or in response to getblocks, getpeers or mempool.  |
| Content  | List of tx or block hashes that you have in your mempool or blockchain, respectively; or list of peers you are connected to.  |

| Name | getdata |
| ---- | ---- |
| Value    | 5  |
| Size     | 1B + 1B + ?*256B (DATATYPE + data_ids_count + data_ids[])  |
| Purpose  | Request full block(s)/tx(s). Sent unsolicited.  |
| Content  | DATATYPE byte and identifying hash(es).  |

| Name | block |
| ---- | ---- |
| Value    | 6  |
| Size     | 80B to 1MB (header + body)  |
| Purpose  | Send a full block to a peer. Solicited by getdata.  |
| Content  | A full block.  |

| Name | tx |
| ---- | ---- |
| Value    | 7  |
| Size     | 10B + ?*160 + ?*132 (header + ins + outs)  |
| Purpose  | Send a full transaction to a peer. Solicited by getdata.  |
| Content  | A full transaction.  |

| Name | alert |
| ---- | ---- |
| Value    | 8  |
| Size     | 1B + 4B + 100B + 1024B (ALERTTYPE + time + msg + sig{ALERTTYPE+time+msg})  |
| Purpose  | Notify entire network about network emergency. Only valid if signed by key at key.shelt.ca.  |
| Content  | Information about the network issue. Depending on ALERTTYPE, the implementation may need to take action (such as not allowing mining).  |

| Name | ping |
| ---- | ---- |
| Value    | 9  |
| Size     | 0B  |
| Purpose  | Verify connectivity of peer. Sent unsolicited.  |
| Content  | N/A  |

| Name | pong |
| ---- | ---- |
| Value    | 10  |
| Size     | 0B  |
| Purpose  | Verify connectivity of self to peer. Solicited by ping. |
| Content  | N/A  |

### Error constants
#### Block-related
| Name | `BLOCK_VERSION_INVALID` |
| ---- | ---- |
| Value    | 1 |
| Meaning  | Block version is invalid.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_CONFLICT` |
| ---- | ---- |
| Value    |  |
| Meaning  | Block's prev_hash is already in another block.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_TIME_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block's time is before last block.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_MRKROOT_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block's merkle_root does not match body.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_TARGET_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block's target is not correct.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_HASH_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block's hash isn't valid as per the target.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_BODY_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Transaction(s) in body are invalid. No tx-specific reject messages are needed because the offender shouldn't have send such a block anyway.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_OVERFLOW` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block is larger than 1 MB.  |
| Content  | Offending block's hash.  |

| Name | `BLOCK_EXISTS` |
| ---- | ---- |
| Value    |   |
| Meaning  | Block already exists in the blockchain.  |
| Content  | Offending block's hash.  |

#### Transaction-related

| Name | `TX_VERSION_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Transaction's version is invalid.  |
| Content  | Offending transaction's hash.  |

| Name | `TX_SIGNATURE_INVALID` |
| ---- | ---- |
| Value    |   |
| Meaning  | Transaction input signature is invalid.  |
| Content  | Offending transaction's hash.  |

| Name | `TX_REF_USED` |
| ---- | ---- |
| Value    |   |
| Meaning  | A transaction input is used already.  |
| Content  | Offending transaction's hash.  |

| Name | `TX_REF_NONEXIST` |
| ---- | ---- |
| Value    |   |
| Meaning  | A transaction input doesn't exist.  |
| Content  | Offending transaction's hash.  |

| Name | `TX_REF_OVERSPEND` |
| ---- | ---- |
| Value    |   |
| Meaning  | A transaction's inputs are smaller than outputs.  |
| Content  | Offending transaction's hash  |

#### Generic

| Name | `BAD_VERSION` |
| ---- | ---- |
| Value    |   |
| Meaning  | Incompatible version.  |
| Content  | N/A  |

| Name | `MESSAGE_MALFORMED` |
| ---- | ---- |
| Value    |   |
| Meaning  | Data within the message was not parsed according to specification. |
| Content  | *`<any>`*  |

| Name | `UNKNOWN_CTYPE` |
| ---- | ---- |
| Value    |   |
| Meaning  | First byte represents an unknown protocol CTYPE.  |
| Content  | *`<any>`*  |


### DATATYPES
> (used for inv and getdata)

    0 blocks
    1 transactions
    2 peers



### Procedures

#### Technical description
- Try connecting to known nodes until one works. This is peer<sub>0</sub> .
- Ask peer<sub>n</sub> what peers they are connected to. Select a node randomly from this pool (excluding yourself,  peer<sub>[0,n‑1]</sub> and any nodes you've failed to connect to) for `(0 <= n < 8)`. This is peer<sub>n+1</sub>.
- If peer<sub>n</sub> for `n>0` has less than 8 peers, send them a list of all your peers.
- If peer<sub>n</sub> for `n>0` has no nodes that are valid to you (according to above exclusions), select another node from the same pool you selected peer<sub>n</sub> from.

#### Become a peer client-wise:
- Send server a `<getnodes>`
- Recieve an `<inv>` from server
- Select a random node from the `<inv>` that is valid (according to above exclusions).
- Connect to that node.
- *see above*

#### Become a peer server-wise:
- Recieve a `<getnodes>` from a client
- Send an `<inv>` of all your peers (ideally excluding them).

#### Receiving `<getnodes>`:
- Send a <inv> containing ALL of your peers.

#### Submit transaction(s):
- Send all peers an `<inv>` containing new transaction hash(es)

#### Recieving an `<inv>` (containing a tx or block):
- If you don't have a transaction or block listed in the `<inv>, do <getdata>` on all missing data.
- Verify the data
- If it's valid, store the data where it belongs (mempool if tx or blockchain if block, then interrupt miner)
- If it isn't valid, send `<reject>`

#### Recieving an `<inv>` (containing nodes):
- If you have less than 8 peers, choose a node randomly from this list (excluding yourself,  peer<sub>[0,n‑1]</sub> and any nodes you've failed to connect to).
- *See above*

#### Recieving a `<mempool>`
- Send an `<inv>` of your mempool


#### Recieving a `<tx>` (which only happens after a request for it):
- Verify integrity (if invalid, send `<reject>`)
- If you're working on a block and it contains transaction(s), add it to your local mempool
- Send it in an `<inv>` to peers

#### Recieving a `<block>` (which only happens after a request for it):
- Verify integrity (if invalid, send `<reject>`)
- Add it to your blockchain
- Send it in an `<inv>` to peers

#### Recieving a `<getdata>`:
- Send the requested data.

#### Recieving a `<mempool>`:
- Send your mempool in `<inv>`

#### Recieving a `<getblocks>`:
- Send an `<inv>` containing first 500 blocks they are missing (starting from the start block
    they specified [likely their latest block])


## Source guidelines
* `size_t` should be used when dealing with array sizes UNLESS it is measuring lengths of serialized data (where such a number would need to fit in the specified range).
* Memory allocation (i.e., blocks) for a function's output is always done via implementation. Pointers to the pre-allocated memory block are passed into the function (typically as the last parameter).
* Only include things in the header file when the header file itself needs them.

## Todo

### Roadmap
* recieve `<inv>` constantly for next mempool once they are in a confirmed block
* verification - what is signed for txs?
* get rid of memcpys and use loops for endian-safeness
* make all functions that copy data to a ptr also return the ptr
* make all function names include the name of what they do/modify (block, tx)
* update time every few seconds (avg of peers)

### Misc
* The block header is 78 bytes, and the only dynamic component (the nonce) is the last 4. The first 64B chunk of every hash need not be calculated on each iteration.
* Should serial generation functions which take many arguments instead use structs?
* Document protocol in one place.
* Move tx-specific defines to tx header


