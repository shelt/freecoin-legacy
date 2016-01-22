#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

//TODO check for overflow on all maximums in this file

/*******************
** SHARED STRUCTS **
*******************/

// transactions.h
typedef struct _s_M_tx M_tx;

// network.h
typedef struct _s_Network Network;
typedef struct _s_Peer Peer;

// data.h
typedef struct _s_Mempool Mempool;
typedef struct _s_Dbs Dbs;


/******************
** CONFIGURATION **
******************/

/* CLIENT OPTIONS */ // TODO
#define TX_FEE_PER_1K 1
#define SERVER_PORT 30330

/* DEVELOPER OPTIONS */
// Client
#define __VERSION 1
#define VERBOSE 1
// Mining
#define MINING_REWARD 100
#define REASONABLE_CONFIRMATIONS 6
// Peers
#define MAX_CONNECTED_PEERS 8
#define MAX_ADDR_LENGTH 80
// Mempool
#define TX_COUNT_SOFT_LIMIT 500


/******************
** MISCELLANEOUS **
******************/

/* Type aliases */
#define uchar unsigned char
#define uint uint32_t
#define ushort uint16_t

/* db.h (library) aliases */
#define u_long unsigned long
#define u_int unsigned int
#define u_short unsigned short
#define u_char unsigned char

/* Crypto sizes */
#define SIZE_SHA256 32
#define SIZE_RSA1024 128



/*****************
** TRANSACTIONS **
*****************/

/* Maximums */
// 1024*1024
#define MAX_TX_SIZE 1048576

/* Tx general positions */
#define POS_TX_HEADER 0
// Tx body located after header

/* Tx general sizes */
#define SIZE_TX_HEADER 10
#define SIZE_TX_INPUT 290
#define SIZE_TX_OUTPUT 36

/* Tx header element positions */
#define POS_TX_VERSION 0
#define POS_TX_IN_COUNT 2
#define POS_TX_OUT_COUNT 4
#define POS_TX_LOCK_TIME 6
#define POS_TX_BODY 10



/* Tx header element sizes */
#define SIZE_TX_VERSION 2
#define SIZE_TX_IN_COUNT 2
#define SIZE_TX_OUT_COUNT 2
#define SIZE_TX_LOCK_TIME 4

/* Tx-input positions */
#define POS_TX_INPUT_REF_TX 0
#define POS_TX_INPUT_OUT_INDEX 32
#define POS_TX_INPUT_PUBKEY 34
#define POS_TX_INPUT_SIG 162

/* Tx-input sizes */
#define SIZE_TX_INPUT_REF_TX 32
#define SIZE_TX_INPUT_OUT_INDEX 2
#define SIZE_TX_INPUT_PUBKEY 128
#define SIZE_TX_INPUT_SIG 128

/* Tx-output positions */
#define POS_TX_OUT_ADDRESS 0
#define POS_TX_OUT_AMOUNT 32

/* Tx-output sizes */
#define SIZE_TX_OUT_ADDRESS 32
#define SIZE_TX_OUT_AMOUNT 4


/***********
** BLOCKS **
***********/

/* Maximums */
// 1024*1024
#define MAX_BLOCK_SIZE 1048576

/* Block general positions */
#define POS_BLOCK_HEADER 0
// TODO change POS_BLOCK_BODY refs to SIZE_BLOCK_HEADER

/* Block general sizes */
#define SIZE_BLOCK_HEADER 83

/* Block header element positions */
#define POS_BLOCK_VERSION 0
#define POS_BLOCK_TIME 2
#define POS_BLOCK_HEIGHT 6
#define POS_BLOCK_PREV_HASH 10
#define POS_BLOCK_MERKLE_ROOT 42
#define POS_BLOCK_TARGET 74
#define POS_BLOCK_NONCE 75
#define POS_BLOCK_TX_COUNT 79

/* Block header element sizes */
#define SIZE_BLOCK_VERSION 2
#define SIZE_BLOCK_TIME 4
#define SIZE_BLOCK_HEIGHT 4
#define SIZE_BLOCK_PREV_HASH 32
#define SIZE_BLOCK_MERKLE_ROOT 32
#define SIZE_BLOCK_TARGET 1
#define SIZE_BLOCK_NONCE 4
#define SIZE_BLOCK_TX_COUNT 4




/***************
** NETWORKING **
***************/

// 1024*1024*2
#define MAX_MSG_SIZE 2097152

// 01/01/2017 (00:00 GMT)
#define TESTNET_EPOCH 1483228800

// Number of 10M intervals in 1 day
#define RECALC_TARGET_INTERVAL 144

// Ideal interval between block solutions (10 minutes)
#define BLOCK_SOLUTION_INTERVAL 600

/* Times in seconds */
#define SECONDS_IN_20_MINUTES 1200
#define SECONDS_IN_HALF_WEEK 5040
#define SECONDS_IN_8_WEEKS 80640

/* Content type codes */
#define CTYPE_REJECT 0
#define CTYPE_GETBLOCKS 1
#define CTYPE_MEMPOOL 2
#define CTYPE_INV 3
#define CTYPE_GETDATA 4
#define CTYPE_BLOCK 5
#define CTYPE_TX 6
#define CTYPE_PEER 7
#define CTYPE_ALERT 8
#define CTYPE_PING 9
#define CTYPE_PONG 10

/* Data type codes */
#define DTYPE_BLOCK 0
#define DTYPE_TX 1
#define DTYPE_NODE 2

/* Protocol message positions */
#define POS_MSG_VERSION 0
#define POS_MSG_CTYPE 2

/* "Content type"-specific positions */
#define CTYPE_REJECT_POS_ERRORTYPE 0
#define CTYPE_REJECT_POS_INFO_SIZE 1
#define CTYPE_REJECT_POS_INFO 2

#define CTYPE_GETBLOCKS_POS_START_BLOCK 0
#define CTYPE_GETBLOCKS_POS_BLOCK_COUNT 32

#define CTYPE_INV_POS_DATATYPE 0
#define CTYPE_INV_POS_IDS_COUNT 1
#define CTYPE_INV_POS_IDS 2

#define CTYPE_GETDATA_POS_DATATYPE 0
#define CTYPE_GETDATA_POS_IDS_COUNT 1
#define CTYPE_GETDATA_POS_IDS 2

#define CTYPE_BLOCK_POS_SIZE 0
#define CTYPE_BLOCK_POS_BLOCK 4

#define CTYPE_TX_POS_SIZE 0
#define CTYPE_TX_POS_TX 4

#define CTYPE_PEER_POS_PORT 0
#define CTYPE_PEER_POS_ADDRLEN 2
#define CTYPE_PEER_POS_ADDR 4

#define CTYPE_ALERT_POS_TYPE 0
#define CTYPE_ALERT_POS_COMMAND 1
#define CTYPE_ALERT_POS_TIME 2
#define CTYPE_ALERT_POS_SIG 6
#define CTYPE_ALERT_POS_MSG_LEN 38
#define CTYPE_ALERT_POS_MSG 39


/* Error codes */
//#define ERR_UNUSED_CODE 12 TODO
#define ERR_BAD_VERSION 1
#define ERR_MESSAGE_MALFORMED 2
#define ERR_UNKNOWN_CTYPE 3
#define ERR_BLOCK_CONFLICT 4
#define ERR_BLOCK_TIME_INVALID 5
#define ERR_BLOCK_ROOT_INVALID 6
#define ERR_BLOCK_TARGET_INVALID 7
#define ERR_BLOCK_BAD_HASH 8
#define ERR_BLOCK_BAD_BODY 9
#define ERR_BLOCK_OVERFLOW 10
#define ERR_BLOCK_EXISTS 11
#define ERR_TX_BAD_SIG 13
#define ERR_TX_BAD_REF 14
#define ERR_TX_REF_OVERSPEND 15


#endif
