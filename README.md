# freecoin

**freecoin** is a fully-functional cryptocurrency I'm making for fun.

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Mathematics and cryptographic principles allow a user to prove they were the recipient of a past transaction.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has along with the processing of transactions all happens on a few servers. Though they probably take backups and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works.

### Source guidelines
* `size_t` should be used when dealing with array sizes UNLESS it is measuring lengths of serialized data (where such a number would need to fit in the specified range).
* Memory allocation (i.e., blocks) for a function's output is always done via implementation. Pointers to the pre-allocated memory block are passed into the function (typically as the last parameter).
* Only include things in the header file when the header file itself needs them.

### Todo
* The block header is 78 bytes, and the only dynamic component (the nonce) is the last 4. The first 64B chunk of every hash need not be calculated on each iteration.
* Should serial generation functions which take many arguments instead use structs?
* Document protocol in one place.