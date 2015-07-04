#ifndef SHARED_H
#define SHARED_H

#define unchar unsigned char 
#define unint unsigned int

#define __version 1
#define TX_HEADER_SIZE 8
#define TX_INPUT_BYTESIZE 160  // The bytesize of 1 input.  [IN_TX .. SIG]  32B + 128B
#define TX_OUTPUT_BYTESIZE 132 // The bytesize of 1 output. [PUBKEY .. AMT] 128B + 4B
#define SHA_SIZE 32



#endif