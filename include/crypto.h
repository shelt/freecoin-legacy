#ifndef CRYPTO_H
#define CRYPTO_H


typedef struct 
{
   unchar data[64];
   unint datalen;
   unint bitlen[2];
   unint state[8];
} SHA256_CTX;


void sha256_init(SHA256_CTX *ctx);

void sha256_update(SHA256_CTX *ctx, unchar data[], size_t len);

void sha256_state(SHA256_CTX *ctx, unchar state[]);

void sha256_final(SHA256_CTX *ctx, unchar hash[]);

#endif
