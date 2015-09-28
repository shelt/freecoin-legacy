#ifndef SHARED_H
#define SHARED_H

/* CONFIG OPTIONS */
// short (2B)
#define __VERSION 1
#define MINING_REWARD 100

// Number of 10M intervals in 1 day
#define RECALC_TARGET_INTERVAL 144

#define MAX_PEERS 8

/* UNSIGNED TYPES */
#define unchar unsigned char
#define unint unsigned int
#define unshort unsigned short

/* BLOCK SPECIFICATIONS */
#define BLOCK_HEADER_SIZE 78

// 1024*1024
#define MAX_BLOCKSIZE 1048576

/* TX SPECIFICATIONS */

// The same as sizeof(Tx_header)
#define TX_HEADER_SIZE 10

// The bytesize of 1 input.  [IN_TX .. SIG]  32B + 128B    
#define TX_IN_SIZE 160

// The bytesize of 1 output. [PUBKEY .. AMT] 128B + 4B
#define TX_OUT_SIZE 132

/* CRYPTO SPECIFICATIONS */
#define SHA256_SIZE 32
#define RSA1024_SIZE 128

#endif



