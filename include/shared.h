#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>
/* CLIENT OPTIONS */
#define TX_FEE_PER_1K 1

/* DEVELOPER OPTIONS */
#define __VERSION 1
#define VERBOSE 1
#define MINING_REWARD 100

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


/* VARIOUS NET SPECIFICATIONS */
#define TESTNET_EPOCH 1451606400
#define MAX_GETBLOCKS 500

// Number of 10M intervals in 1 day
#define RECALC_TARGET_INTERVAL 144

// Ideal interval between block solutions (10 minutes)
#define BLOCK_SOLUTION_INTERVAL 600


/* TIMES */
#define SECONDS_IN_20_MINUTES 1200
#define SECONDS_IN_HALF_WEEK 5040
#define SECONDS_IN_8_WEEKS 80640


/* db.h typedefs */
#define u_long unsigned long
#define u_int unsigned int
#define u_short unsigned short
#define u_char unsigned char
#endif
