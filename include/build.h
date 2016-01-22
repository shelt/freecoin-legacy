#ifndef BUILD_H
#define BUILD_H

#include "data.h"
#include "shared.h"

uchar build_block_list(Dbs *dbs, uchar *startblock, uchar req_blocks, uchar *dest);

uchar build_mempool_list(Dbs *dbs, uchar *dest);

uchar build_missing_blocks_list(Dbs *dbs, uchar *list, uchar count, uchar *dest);



#endif
