/* $Id: Zombie.c,v 1.1 2001-02-21 22:29:29 bird Exp $
 *
 * Zombie Process creator
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);

void Zombie1(void);
VOID _System Zombie1Thread(ULONG ulIgnored);

void Zombie2(void);
ULONG _System Zombie2XcptHandler(
    PEXCEPTIONREPORTRECORD                  pReport,
    struct _EXCEPTIONREGISTRATIONRECORD *   pERegRec,
    PCONTEXTRECORD                          pContext,
    PVOID                                   pv);

void Zombie3(void);
VOID _System Zombie3ExitList(ULONG ul);


/** @design Zombie Processes
 * <ul>
 *  <li> 1. Suspend a thread & exit program.
 *  <li> 2. Block in exception handler.
 *  <li> 3. Block in exitlist.
 *  <li>
 * </ul>
 */


int main(int argc, char **argv)
{
    int     iZombie;                   /* Zombie number specified at the commandline.  */


    /*
     * Syntax.
     */
    if (argc != 2)
    {
        syntax();
        return -42;
    }


    /*
     * Invoke the requested zombie creator.
     */
    switch (atoi(argv[1]))
    {
        case 1:
            Zombie1();
            break;

        case 2:
            Zombie2();
            break;

        case 3:
            Zombie3();
            break;

        default:
            fprintf(stderr, "error: invalid zombie type number, %s.\n", argv[1]);
            syntax();
            return -42;
    }


    /*
     * Exit with return code 42 just to distiguish from
     * any returncode set by OS/2 when killed.
     */
    return 42;
}


/**
 * Display program commandline syntax.
 */
static void syntax(void)
{
    printf(
        "Zombie.exe <zombie type no.>\n"
        "   Zombie types: \n"
        "       1 Suspend a thread & exit program.\n"
        "       2 Block in exception handler.\n"
        "       3 Block in exitlist.\n"
        );
}


/**
 * Creates zombie situation 1.
 */
void Zombie1(void)
{
    TID     tid;
    APIRET  rc;

    printf("1 - Suspend a thread & exit program.\n");
    flushall();
    rc = DosCreateThread(&tid, Zombie1Thread, 0, 0, 16384);
    if (rc == NO_ERROR)
    {
        #if 1
        DosSleep(128);              /* let the new thread start first */
        rc = DosSuspendThread(tid);
        if (rc == NO_ERROR)
        {
            DosExit(EXIT_PROCESS, 43);
            printf("!!!internal error!!!\n");
        }
        else
            fprintf(stderr, "DosSuspendThread failed with rc=%d\n", rc);
        #else
        for (;;)
            DosSleep(0);
        #endif
    }
    else
        fprintf(stderr, "DosCreateThread failed with rc=%d\n    ", rc);
}

/**
 * Zombie 1 thread the one we'll supend.
 */
VOID _System Zombie1Thread(ULONG ulIgnored)
{
    #if 1
    printf("Zombie1 Second thread is started.\n");
    for (;;)
        DosSleep(0);
    #else
    APIRET rc;
    printf("Zombie1 Second thread is started.\n");
    rc = DosSuspendThread(1);
    if (rc == NO_ERROR)
    {
        DosExit(EXIT_PROCESS, 43);
        printf("!!!internal error!!!\n");
    }
    else
        fprintf(stderr, "DosSuspendThread failed with rc=%d\n", rc);
    #endif
}


/**
 * Creates zombie situation 2.
 */
void Zombie2(void)
{
    APIRET                      rc;
    EXCEPTIONREGISTRATIONRECORD ERegRec;

    printf("2 - Block in exception handler.\n");
    flushall();

    ERegRec.prev_structure = NULL;
    ERegRec.ExceptionHandler = Zombie2XcptHandler;
    rc = DosSetExceptionHandler(&ERegRec);
    if (rc == NO_ERROR)
    {
        /* make exception - termination exception for example. */
        DosExit(EXIT_PROCESS, 43);
        printf("!!!internal error!!!\n");
    }
}

/**
 * Zombie 2 exceptionhandler - we'll block here.
 */
ULONG _System Zombie2XcptHandler(
    PEXCEPTIONREPORTRECORD                  pReport,
    struct _EXCEPTIONREGISTRATIONRECORD *   pERegRec,
    PCONTEXTRECORD                          pContext,
    PVOID                                   pv)
{
    /* block */
    DosSleep(-1);
}


/**
 * Creates zombie situation 3.
 */
void Zombie3(void)
{
    APIRET rc;

    printf("3 - Block in exitlist.\n");
    flushall();

    rc = DosExitList(EXLST_ADD, Zombie3ExitList);
    if (rc == NO_ERROR)
    {
        DosExit(EXIT_PROCESS, 43);
        printf("!!!internal error!!!\n");
    }
    else
        fprintf(stderr, "DosExistList failed with rc=%d\n");
}


/**
 * Zombie 3 exit list handler - we'l block here.
 */
VOID _System Zombie3ExitList(ULONG ul)
{
    DosSleep(-1);
}
