/* $Id: dll-c.c,v 1.1 2002-05-16 11:37:06 bird Exp $
 *
 * Test DLL.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef DEBUG
#define _STR "\"DEBUG\" "
#else
#define _STR ""
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__IBMCPP__) || defined(__IBMC__))

#define DLLENTRYPOINT_CCONV _System
#define DLLENTRYPOINT_NAME  _DLL_InitTerm

#if (__IBMCPP__ == 300) || (__IBMC__ == 300)
void _Optlink __ctordtorInit( void );
#define ctordtorInit()  __ctordtorInit()

void _Optlink __ctordtorTerm( void );
#define ctordtorTerm()  __ctordtorTerm()

#elif (__IBMCPP__ == 360) || (__IBMC__ == 360)
void _Optlink __ctordtorInit( int flag );
#define ctordtorInit()  __ctordtorInit(0)

void _Optlink __ctordtorTerm( int flag );
#define ctordtorTerm()  __ctordtorTerm(0)

#else
#error "Unknown compiler!"
#endif

int  _Optlink _CRT_init(void);
void _Optlink _CRT_term(void);

#elif defined(__WATCOMC__)

#define DLLENTRYPOINT_CCONV _syscall
#define DLLENTRYPOINT_NAME  LibMain

#define ctordtorInit() ((void)0)
#define ctordtorTerm() ((void)0)

#define _CRT_init() ((void)0)
#define _CRT_term() ((void)0)

#endif

#ifdef __cplusplus
}
#endif




/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <math.h>

#ifdef OS2
#include <os2.h>
#define OSCALL APIENTRY
#endif

/**
 * Testprogram.
 * @returns 0 on success.
 */
int OSCALL FOO42(void)
{
    long double lrd = asin(cos(0));
    lrd = lrd;

    #if !defined(DLLENTRYPOINT_NAME)
    puts("DLLInit");
    #endif

    #ifdef __cplusplus
    puts("Hello \"C++\" "_STR"World!");
    #else
    puts("Hello \"C\" "_STR"World!");
    #endif

/*
 * Define assertions.
 */
#ifdef __32BIT__
# ifdef __16BIT__
    puts("error: __16BIT__ && __32BIT__ is defined!");
# else
    puts("__32BIT__");
# endif
#else
# ifdef __16BIT__
    puts("__16BIT__");
# else
    puts("error: __16BIT__ nor __32BIT__ is defined!");
# endif

#endif

#if !defined(OS2) && !defined(WIN32) && !defined(LINUX)
    puts("error: OS is not defined!");
#endif

    return 0;
}


int OSCALL FOONAME(void)
{
    puts("FOOName");
    #if !defined(DLLENTRYPOINT_NAME)
    puts("DLLTerm");
    #endif
    return 0;
}


#if defined(DLLENTRYPOINT_NAME) && defined(OS2)
int DLLENTRYPOINT_CCONV DLLENTRYPOINT_NAME(int hmod, int fFlags)
{
    switch (fFlags)
    {
        case 0:
        {
            ctordtorInit();
            _CRT_init();
            puts("DLLInit");
            break;
        }

        case 1:
        {
            #ifdef __WATCOMC__
            /* probably already terminated by now. */
            #ifdef __16BIT__
            USHORT Dummy;
            #else
            ULONG Dummy;
            #endif
            DosWrite((HFILE)1 /*HF_STDOUT*/, "DLLTerm\r\n", 9, &Dummy);
            #else
            puts("DLLTerm");
            #endif
            ctordtorTerm();
            _CRT_term();
            break;
        }

        default:
            return 0;
    }
    return 1;
}
#endif
