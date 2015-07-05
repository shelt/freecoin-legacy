#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>
#include "shared.h"

/*
    **Transactions** are generated in an instant. Data is given and a block of data [header+body]
    is received as output. This is done using generate_transaction().
    Unlike transactions, **blocks** are constantly changing until they are solved.

    time should be updated every 3 seconds.
    merkle_root should be updated when a new tx is added.
    Nonce is updated after each solve attempt.
    
    **txs should be initialized to very large to accommodate new tx pointers.
    Obviously, the block body should be shrunk for data transmission.
*/

void generate_block(unchar version, unint time, unchar *prev_block_hash, *merkle_root, unint nonce, // Header variables
                    unchar** txs, size_t tx_count);                                                 // Body variables

#endif