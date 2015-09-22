#ifndef SHARED_H

#define SHARED_H

#define __VERSION "0.1a"

#define unchar unsigned char
#define unint unsigned int
#define unshort unsigned short

// The same as sizeof(Tx_header)
#define TX_HEADER_SIZE 10

// The bytesize of 1 input.  [IN_TX .. SIG]  32B + 128B    
#define TX_INPUT_BYTESIZE 160

// The bytesize of 1 output. [PUBKEY .. AMT] 128B + 4B
#define TX_OUTPUT_BYTESIZE 132

#define SHA256_SIZE 32
#define RSA1024_SIZE 128

#endif
