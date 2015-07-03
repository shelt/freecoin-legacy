#define uchar unsigned char // 8-bit byte
#define uint unsigned int // 32-bit word

typedef struct _SHA256_CTX SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);

void sha256_update(SHA256_CTX *ctx, uchar data[], uint len);

void sha256_final(SHA256_CTX *ctx, uchar hash[]);