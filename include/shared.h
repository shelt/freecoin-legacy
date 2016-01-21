#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

// transactions.h
typedef struct _s_M_tx M_tx;

// network.h
typedef struct _s_Network Network;
typedef struct _s_Peer Peer;

// data.h
typedef struct _s_Mempool Mempool;
typedef struct _s_Dbs Dbs;


/* CLIENT OPTIONS */
#define TX_FEE_PER_1K 1
#define SERVER_PORT 30330

/* DEVELOPER OPTIONS */
#define __VERSION 1
#define VERBOSE 1
#define MINING_REWARD 100
#define REASONABLE_CONFIRMATIONS 6

#define MAX_PEERS 8

/* UNSIGNED TYPES */
#define uchar unsigned char
#define uint uint32_t
#define ushort uint16_t

// 1024*1024
//TODO check for overvlow on all 4 of these limits
#define MAX_BLOCK_SIZE 1048576
#define MAX_TX_SIZE 1048576
#define MAX_ADDR_LENGTH 80
#define MAX_CONNECTED_PEERS 10

// For the mempool. It likely won't contain more than this.
// TODO check for overflow on this limit
#define TX_COUNT_SOFT_LIMIT 500

/* CRYPTO SPECIFICATIONS */
#define SIZE_SHA256 32
#define SIZE_RSA1024 128


/* Various net specifications */
#define MAX_MSG_SIZE 1024*1024*2

#define TESTNET_EPOCH 1451606400
#define MAX_GETBLOCKS 500
#define MAX_INV_COUNT 255

// Number of 10M intervals in 1 day
#define RECALC_TARGET_INTERVAL 144

// Ideal interval between block solutions (10 minutes)
#define BLOCK_SOLUTION_INTERVAL 600

/* Times */
#define SECONDS_IN_20_MINUTES 1200
#define SECONDS_IN_HALF_WEEK 5040
#define SECONDS_IN_8_WEEKS 80640


/* db.h typedefs */
#define u_long unsigned long
#define u_int unsigned int
#define u_short unsigned short
#define u_char unsigned char
#endif





/*****************
** TRANSACTIONS **
*****************/

#define POS_TX_VERSION 0
#define POS_TX_IN_COUNT 2
#define POS_TX_OUT_COUNT 4
#define POS_TX_LOCK_TIME 6
#define POS_TX_BODY 10

#define SIZE_TX_VERSION 2
#define SIZE_TX_IN_COUNT 2
#define SIZE_TX_OUT_COUNT 2
#define SIZE_TX_LOCK_TIME 4

#define SIZE_TX_HEADER 10
#define SIZE_TX_INPUT 290
#define SIZE_TX_OUTPUT 36

#define POS_TX_INPUT_REF_TX 0
#define POS_TX_INPUT_OUT_INDEX 32
#define POS_TX_INPUT_PUBKEY 34
#define POS_TX_INPUT_SIG 162

#define SIZE_TX_INPUT_REF_TX 32
#define SIZE_TX_INPUT_OUT_INDEX 2
#define SIZE_TX_INPUT_PUBKEY 128
#define SIZE_TX_INPUT_SIG 128

#define POS_TX_OUT_ADDRESS 0
#define POS_TX_OUT_AMOUNT 32

#define SIZE_TX_OUT_ADDRESS 32
#define SIZE_TX_OUT_AMOUNT 4

/***********
** BLOCKS **
***********/

#define POS_BLOCK_VERSION 0
#define POS_BLOCK_TIME 2
#define POS_BLOCK_HEIGHT 6
#define POS_BLOCK_PREV_HASH 10
#define POS_BLOCK_MERKLE_ROOT 42
#define POS_BLOCK_TARGET 74
#define POS_BLOCK_NONCE 75
#define POS_BLOCK_TX_COUNT 79
#define POS_BLOCK_BODY 83

#define SIZE_BLOCK_VERSION 2
#define SIZE_BLOCK_TIME 4
#define SIZE_BLOCK_HEIGHT 4
#define SIZE_BLOCK_PREV_HASH 32
#define SIZE_BLOCK_MERKLE_ROOT 32
#define SIZE_BLOCK_TARGET 1
#define SIZE_BLOCK_NONCE 4
#define SIZE_BLOCK_TX_COUNT 4

#define SIZE_BLOCK_HEADER 83


/***************
** NETWORKING **
***************/

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

#define DTYPE_BLOCK 0
#define DTYPE_TX 1
#define DTYPE_PEER 2

#define ERR_UNUSED 12

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
