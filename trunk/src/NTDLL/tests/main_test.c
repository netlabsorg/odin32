/* Automatically generated file; DO NOT EDIT!! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "winbase.h"

extern void func_generated(void);
extern void func_large_int(void);
extern void func_rtlstr(void);
extern void func_string(void);

struct test
{
    const char *name;
    void (*func)(void);
};

static const struct test winetest_testlist[] =
{
    { "generated", func_generated },
    { "large_int", func_large_int },
    { "rtlstr", func_rtlstr },
    { "string", func_string },
    { 0, 0 }
};

#define WINETEST_WANT_MAIN
#include "test.h"
