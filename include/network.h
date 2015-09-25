#ifndef NETWORK_H
#define NETWORK_H

typedef struct 
{
   unchar ip[11];
   unint port;
} Address;


unint get_net_time();

#endif
