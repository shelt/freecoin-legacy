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
#define MAX_BLOCK_SIZE 1048576
#define MAX_TX_SIZE 1048576


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

#endif
