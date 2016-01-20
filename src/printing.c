#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
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

void v_printf(const char *format, ...)
{
    if (VERBOSE)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
    }
}

void die(const char *string, ...)
{
    va_list args;
    va_start(args, string);

    printf("Fatal: ");
    vfprintf (stdout, string, args);
    printf("\n");
    
    exit(1);
};
