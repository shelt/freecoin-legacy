#ifndef PRINTING_H
#define PRINTING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include "shared.h"

void print_greeting();
void print_version();
void v_printf(const char *string, ...);
void fatal(const char *string, ...);

#endif
