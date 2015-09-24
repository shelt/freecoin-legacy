#ifndef FREECOIN_H
#define FREECOIN_H

unint get_net_time();

void get_latest_block(unchar *blockhash);

unint get_block_num(unchar *blockhash);


//struct_of_blockinfo get_block_info //??

//struct get_tx_info{} //??

unint get_curr_target();


#endif
