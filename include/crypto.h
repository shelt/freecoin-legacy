#ifndef CRYPTO_H
#define CRYPTO_H

#include "shared.h"


typedef struct 
{
   uchar data[64];
   uint datalen;
   uint bitlen[2];
   uint state[8];
} SHA256_CTX;


void sha256_init(SHA256_CTX *ctx);

void sha256_update(SHA256_CTX *ctx, uchar data[], size_t len);

void sha256_state(SHA256_CTX *ctx, uchar state[]);

void sha256_final(SHA256_CTX *ctx, uchar hash[]);

#endif
