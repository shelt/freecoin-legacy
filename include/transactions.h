#define uchar unsigned char


typedef struct _Header_tx Header_tx;

void generate_transaction(Header_tx* header, uchar* ins, uchar* outs, uint in_count, uint out_count, uchar* tx);

void generate_merkle_root(Transaction* txs, unsigned int tx_count, uchar hash[]);