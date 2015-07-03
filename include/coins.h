typedef struct _Block Block;
void block_init(Block* b);

typedef struct _Transaction Transaction;
void transaction_init(Transaction* b, unsigned int in_count, unsigned int out_count);