# freecoin

**freecoin** is a lightweight cryptocurrency implementation with a focus on portability and comprehension.

#### Transaction process

- get_transaction
- assert not in newblock
- assert not in blockchain
- assert inputs_useable
- assert inputs_signed
- assert for each out: (ins(with out_i) combined) >= out amount
 - if greater, return surplus back to sender in a new out


#### To-Do
0.0.1

* IRC transaction submission listening
* IRC block submission and listening
* Transaction adding after assertions
* Divide main.py into respective files
* Address generation
 * NOTE: public addresses are: hex(pubkey)[2:]
* Automatic target calculation
