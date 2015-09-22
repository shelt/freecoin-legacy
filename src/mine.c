#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "shared.h"
#include "printing.h"



int main(int argc, char **argv)
{
    print_greeting();

    if (argc==1)
        printf("\nType -h for help.\n");
    else
    {
        
    }
    return 0;
}
