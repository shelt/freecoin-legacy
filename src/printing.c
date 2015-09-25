#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "shared.h"


void print_version()
{
    printf("Version %d \n", __VERSION);
};

void print_greeting()
{
    printf("##############\n");
    printf("## freecoin ##\n");
    printf("##############\n");
    print_version();
    
};

void printfv(int verbose, const char *format, ...)
{
    // va_list is a special type that allows hanlding of variable
    // length parameter list
    va_list args;
    va_start(args, format);

    if (verbose > 0)
        vfprintf(stdout, format, args);
}

void die(const char *string)
{
    printf("Fatal: %s\n", string);
    exit(1);
};
