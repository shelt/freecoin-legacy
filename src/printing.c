#include <stdio.h>
#include "shared.h"


void print_version()
{
    printf("Version %d    \n", __VERSION);
};

void print_greeting()
{
    printf("##############\n");
    printf("## freecoin ##\n");
    printf("##############\n");
    print_version();
    
};
