# freecoin

**freecoin** is a cryptocurrency in early development.

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Mathematics and cryptographic principles allow a user to prove they were the recipient of a past transaction.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has along with the processing of transactions all happens on a few servers. Though they probably take backups and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works.

### Source guidelines
* `size_t` should be used when dealing with array sizes.
* Memory allocation (i.e., blocks) for a function's output is always done via implementation. Pointers to the pre-allocated memory block are passed into the function (typically as the last parameter).
* Only include things in the header file when the header file itself needs them.


### To-Do
* Blocks
* Protocol
* What is the input signature a signature of?
* Need to store the tx hash in the block header.
* Better documentation
