#ifndef SHARED_H
#define SHARED_H

// short (2B)
#define __VERSION 1

#define unchar unsigned char
#define unint unsigned int
#define unshort unsigned short

// 1024*1024
#define MAX_BLOCKSIZE 1048576

// The same as sizeof(Tx_header)
#define TX_HEADER_SIZE 10

// The bytesize of 1 input.  [IN_TX .. SIG]  32B + 128B    
#define TX_IN_SIZE 160

// The bytesize of 1 output. [PUBKEY .. AMT] 128B + 4B
#define TX_OUT_SIZE 132

#define SHA256_SIZE 32
#define RSA1024_SIZE 128

#endif



