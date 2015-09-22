#include "shared.h"
#include <stdio.h>

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
