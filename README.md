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
> Messages can be of fixed or variable size. If they are the latter, some fixed part of their content specifies the size of the following content. All messages have an additional byte prefix specifying their method (not included in method table).

| Name | reject |
| ---- | ---- |
| Value    | 0  |
| Size     | 1B + 2B + <=256B (ERRORTYPE + info_size + info)  |
| Purpose  | Tell a peer that a block/tx/time/alert/version is invalid. Sent in response to various messages.  |
| Content  | ERRORTYPE byte and info about what specifically is invalid.    |

| Name | getnode |
| ---- | ---- |
| Value    | 1  |
| Size     | 2B + 4B (peer_info_t + excludes)  |
| Purpose  | Initializes handshake from client to server. Requests a random peer to connect to. Sent unsolicited.  |
| Content  | Version of sender's protocol implementation; exclusions.  |

| Name | node |
| ---- | ---- |
| Value    | 2  |
| Size     | 0B  |
| Purpose  | Acknowledge client's version during handshake. Tells a client about which node to connect to next. Solicited by a getnode message or sent unsolicited.  |
| Content  | N/A  |

| Name | getblocks |
| ---- | ---- |
| Value    | 3  |
| Size     | 256B + 2B (startblock + block_count)  |
| Purpose  | Request an inv containing startblock and block_count blocks following it (up to 500). Used to update blockchain from a block onward. Sent unsolicited.  |
| Content  | A block hash and a block count.  |

| Name | mempool |
| ---- | ---- |
| Value    | 4  |
| Size     | 0B  |
| Purpose  | Request an inv containing peer's mempool txs. Sent unsolicited.  |
| Content  | N/A  |

| Name | inv |
| ---- | ---- |
| Value    | 5  |
| Size     | 1B + ?*256B (DATATYPE + data_ids[])  |
| Purpose  | Tell peer about blocks or txs that you have. Sent unsolicited or in response to getblocks or mempool.  |
| Content  | List of tx or block hashes that you have in your mempool or blockchain, respectively.  |

| Name | getdata |
| ---- | ---- |
| Value    | 6  |
| Size     | 1B + ?*256B (DATATYPE + data_ids[])  |
| Purpose  | Request full block(s)/tx(s). Sent unsolicited.  |
| Content  | DATATYPE byte and identifying hash(es).  |

| Name | block |
| ---- | ---- |
| Value    | 7  |
| Size     | 80B to 1MB (header + body)  |
| Purpose  | Send a full block to a peer. Solicited by getdata.  |
| Content  | A full block.  |

| Name | tx |
| ---- | ---- |
| Value    | 8  |
| Size     | 10B + ?*160 + ?*132 (header + ins + outs)  |
| Purpose  | Send a full transaction to a peer. Solicited by getdata.  |
| Content  | A full transaction.  |

| Name | alert |
| ---- | ---- |
| Value    | 9  |
| Size     | 1B + 4B + 100B + 1024B (ALERTTYPE + time + msg + sig{ALERTTYPE+time+msg})  |
| Purpose  | Notify entire network about network emergency. Only valid if signed by key at key.shelt.ca.  |
| Content  | Information about the network issue. Depending on ALERTTYPE, the implementation may need to take action (such as not allowing mining).  |

| Name | ping |
| ---- | ---- |
| Value    | 10  |
| Size     | 0B  |
| Purpose  | Verify connectivity of peer. Sent unsolicited.  |
| Content  | N/A  |

| Name | pong |
| ---- | ---- |
| Value    | 11  |
| Size     | 0B  |
| Purpose  | Verify connectivity of self to peer. Solicited by ping. |
| Content  | N/A  |

### Error constants
#### Block-related
| Name | `BLOCK_VERSION_INVALID` |
| ---- | ---- |
| Value    |  |
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
| Value    | 1  |
| Meaning  | Incompatible version.  |
| Content  | N/A  |

| Name | `MESSAGE_MALFORMED` |
| ---- | ---- |
| Value    |   |
| Meaning  | Data within the message was not parsed according to specification. |
| Content  | *`<any>`*  |

| Name | `UNKNOWN_METHOD` |
| ---- | ---- |
| Value    |   |
| Meaning  | First byte represents an unknown protocol method.  |
| Content  | *`<any>`*  |


### DATATYPES
> (used for inv and getdata)

    0 block
    1 transaction



### Procedures

#### Technical description
- Try connecting to known nodes until one works. This is peer<sub>0</sub> .
- During the handshake with peer<sub>n</sub> , ask for a random node they are connected to (excluding yourself,  peer<sub>[0,n‑1]</sub> and any nodes you've failed to connect to) for `(0 <= n <= 8)`.
- If peer<sub>n</sub> for `n>0` has no peers for you, go back to peer<sub>n‑1</sub> and ask for another peer. If peer<sub>n‑1</sub> has another peer for you, connect to it (making it the new peer<sub>n</sub>) and tell the old peer<sub>n</sub> about the new peer<sub>n</sub> .

#### Become a peer client-wise:
- Send server a `<getnode>`
- Recieve a `<node>` from server
- Send a `<getblocks>` containing your latest block
- Connect to the next peer

#### Become a peer server-wise:
- Recieve a `<getnode>` from a client
- Verify the version of the client is acceptable
- Select a random peer that isn't excluded by the `<getnode>` message and send it to the client in a `<node>`
- Send a `<node>` of your new peer to your other peers

#### Receiving `<getnode>`:
- *See above*

#### Receiving `<node>`:
- If you have less than 8 peers, connect to the specified node.

#### Submit transaction(s):
- Send all peers an `<inv>` containing new transaction hash(es)

#### Recieving an `<inv>`:
- If you don't have a transaction or block listed in the `<inv>, do <getdata>` on all missing data.
- Verify the data
- If it's valid, store the data where it belongs (mempool if tx or blockchain if block, then interrupt miner)
- If it isn't valid, send `<reject>`

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


