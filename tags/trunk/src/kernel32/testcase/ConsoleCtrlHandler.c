/* $Id: ConsoleCtrlHandler.c,v 1.1 2002-06-26 07:11:02 sandervl Exp $
 *
 * Test of SetConsoleCtrlHandler and GenerateConsoleCtrlEvent.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
//#define HACKING


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>
#include <wincon.h>
#define _WPROCESS_H__                   /* a hack to exclude some odin specific stuff. */
#include <winprocess.h>
#include <thread.h>

#include <process.h>
#include <string.h>
#include <stdio.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
char    aRHOrder[20];
int     iRHOrder = 0;
int     fRemoveAdd = FALSE;             /* Flag which enables removal/add within
                                         * the RemovalHandler#(). I.e. makes them
                                         * really ugly. */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
BOOL WIN32API RemovalHandler0(DWORD dwEvent);
BOOL WIN32API RemovalHandler1(DWORD dwEvent);
BOOL WIN32API RemovalHandler2(DWORD dwEvent);
BOOL WIN32API RemovalHandler3(DWORD dwEvent);
BOOL WIN32API RemovalHandler4(DWORD dwEvent);
BOOL WIN32API RemovalHandler5(DWORD dwEvent);


/**
 * Display syntax
 */
int syntax(const char * arg0)
{
    const char *psz = strrchr(arg0, '\\');
    if (!psz)
        psz = strrchr(arg0, '/');
    else if (strrchr(psz, '/'))
        psz = strrchr(psz, '/');
    if (!psz)
        psz = strrchr(arg0, ':');
    if (!psz)
        psz = arg0;
    else
        psz++;

    printf("Syntax: %s <testcase#>\n"
           "\n"
           "Testcases: (1-9 is non-interactive; 10-19 is interactive)\n"
           "    1 - Watch default action.\n"
           "    2 - Register two handlers and rais Ctrl-C condition.\n"
           "    3 - Register two handlers and rais Ctrl-Break condition.\n"
           , psz);
    return -1;
}


/**
 * Print internal structures.
 */
#ifdef WIN32
    #ifdef __WATCOMC__
    #undef NtCurrentTeb
    __declspec(dllimport) TEB * __cdecl NtCurrentTeb(void);
    #endif
#endif


/**
 * Hacking function.
 */
#ifdef HACKING
static void printflags(const char *psz)
{
    TEB * pTEB = NtCurrentTeb();

    printf("printflags %s\n", psz);
    if ((unsigned)pTEB->process >= 0x10000 && (unsigned)pTEB->process < 0xc0000000)
    {
        ENVDB *pEDB = pTEB->process->env_db;
        if ((unsigned)pEDB >= 0x10000 && (unsigned)pEDB < 0xc0000000)
        {
            printf("pEDB->inherit_console= 0x%08x\n", pEDB->inherit_console);
            printf("pEDB->break_type     = 0x%08x\n", pEDB->break_type);
            printf("pEDB->break_sem      = 0x%08x\n", pEDB->break_sem);
            printf("pEDB->break_event    = 0x%08x\n", pEDB->break_event);
            printf("pEDB->break_thread   = 0x%08x\n", pEDB->break_thread);
            printf("pEDB->break_handlers = 0x%08x\n", pEDB->break_handlers);
            printf("pEDB->section        = 0x%08x\n", pEDB->section);

            fflush(stdout);
#if 0
            printf("pTEB=%x pTEB->process=%x\n", pTEB, pTEB->process);
            printf("pEDB->break_type[0]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[0]); fflush(stdout);
            printf("pEDB->break_type[1]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[1]); fflush(stdout);
            printf("pEDB->break_type[2]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[2]); fflush(stdout);
            printf("pEDB->break_type[3]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[3]); fflush(stdout);
            printf("pEDB->break_type[4]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[4]); fflush(stdout);
            printf("pEDB->break_type[5]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[5]); fflush(stdout);
            printf("pEDB->break_type[6]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[6]); fflush(stdout);
            printf("pEDB->break_type[7]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[7]); fflush(stdout);
            printf("pEDB->break_type[8]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[8]); fflush(stdout);
            printf("pEDB->break_type[9]  = 0x%08x\n", ((PDWORD)pEDB->break_type)[9]); fflush(stdout);
#endif
        }
        else
            printf("pTEB->process->env_db = %x\n", pEDB);
    }
    else
        printf("pTEB->process = %x\n", pTEB->process);
}
#else
static void printflags(const char *psz)
{
    psz = psz;
    return;
}
#endif


/**
 * Test handler
 */
BOOL WIN32API MyHandler(DWORD dwCtrlType)
{
    printflags("MyHandler");
    switch (dwCtrlType)
    {
        case CTRL_C_EVENT:
            printf("dwCtrlType=%d CTRL_C_EVENT\n", dwCtrlType);
            break;
        case CTRL_BREAK_EVENT:
            printf("dwCtrlType=%d CTRL_BREAK_EVENT\n", dwCtrlType);
            break;
        case CTRL_CLOSE_EVENT:
            printf("dwCtrlType=%d CTRL_CLOSE_EVENT\n", dwCtrlType);
            break;
        case CTRL_LOGOFF_EVENT:
            printf("dwCtrlType=%d CTRL_LOGOFF_EVENT\n", dwCtrlType);
            break;
        case CTRL_SHUTDOWN_EVENT:
            printf("dwCtrlType=%d CTRL_SHUTDOWN_EVENT\n", dwCtrlType);
            break;

        default:
            printf("dwCtrlType=%d\n", dwCtrlType);
    }
    return TRUE;
}


/**
 * Testing of linking and removal of entries while in the handler.
 */
BOOL WIN32API RemovalHandler0(DWORD dwEvent)
{
    printf("RemovalHandler0 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler0, FALSE))
            printf("remove of 0 ok");
        else
            printf("remove of 0 nok! lasterr=%d", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 0;
    return FALSE;
}
BOOL WIN32API RemovalHandler1(DWORD dwEvent)
{
    printf("RemovalHandler1 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler3, TRUE))
            {
            if (SetConsoleCtrlHandler(RemovalHandler3, FALSE))
                printf("add & remove of 3 ok");
            else
                printf("remove of 3 nok! lasterr=%d", GetLastError());
            }
        else
            printf("add of 3 nok! lasterr=%d", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 1;
    return FALSE;
}

BOOL WIN32API RemovalHandler2(DWORD dwEvent)
{
    printf("RemovalHandler2 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler2, FALSE))
            printf("remove of 2 ok");
        else
            printf("remove of 2nok! lasterr=%d", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 2;
    return FALSE;
}

BOOL WIN32API RemovalHandler3(DWORD dwEvent)
{
    printf("RemovalHandler3 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler3, FALSE))
            printf("remove of 3 ok");
        else
            printf("remove of 3 nok! lasterr=%d", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 3;
    return TRUE;
}

BOOL WIN32API RemovalHandler4(DWORD dwEvent)
{
    printf("RemovalHandler4 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler4, TRUE))
            printf("add of 4 ok");
        else
            printf("add of 4 nok! lasterr=%d", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 4;
    return FALSE;
}

BOOL WIN32API RemovalHandler5(DWORD dwEvent)
{
    printf("RemovalHandler5 ");
    if (fRemoveAdd)
    {
        if (   SetConsoleCtrlHandler(RemovalHandler5, FALSE)
            && SetConsoleCtrlHandler(RemovalHandler5, FALSE)
            && SetConsoleCtrlHandler(RemovalHandler5, FALSE))
            printf("remove of 5 three times ok\n");
        else
            printf("remove of 5 three times nok! lasterr=%d\n", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 5;
    return FALSE;
}

BOOL WIN32API RemovalHandler6(DWORD dwEvent)
{
    printf("RemovalHandler6 ");
    if (fRemoveAdd)
    {
        if (SetConsoleCtrlHandler(RemovalHandler6, FALSE))
            printf("remove of 6 ok\n");
        else
            printf("remove of 6 nok! lasterr=%d\n", GetLastError());
    }
    printf("\n");
    aRHOrder[iRHOrder++] = 6;
    return TRUE;
}




/**
 *
 * @returns 0 on success.
 *          non-zero on failure.
 * @param   argv    A
 * @status  completely implemented.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @remark
 */
int main(int argc, char **argv)
{
    int     iCase;                      /* testcase number. */
    int     argi;
    int     rc = 0;
    char    szCmd[512];

    /*
     * Check for syntax request.
     */
    if (argc != 2 || atol(argv[1]) == 0)
        return syntax(argv[0]);

    for (argi = 1; argi < argc; argi++)
        if (    (argv[argi][0] == '-' || argv[argi][0] == '/')
            &&  (argv[argi][1] == '-' || argv[argi][1] == 'h' || argv[argi][1] == 'H')
            )
        return syntax(argv[0]);

    iCase = atol(argv[1]);
    switch (iCase)
    {
        case 1:
        case 2:
        case 3:
            {
            int pid;
            sprintf(szCmd, "%d", atol(argv[1])*100);
            printflags("Initial");
            if (SetConsoleCtrlHandler(NULL, TRUE))
                printflags("After Set(NULL,TRUE) -> ok");
            else
                printflags("After Set(NULL,TRUE) -> nok");
            pid = spawnl(P_WAIT, argv[0], argv[0], szCmd, NULL);
            if (SetConsoleCtrlHandler(NULL, TRUE))
                printflags("After Set(NULL,TRUE) -> ok");
            else
                printflags("After Set(NULL,TRUE) -> nok");
            rc = wait(&pid);
            printf("exit rc=%d\n", rc);
            break;
            }

        case 1*100:
            printf("GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) rc=%d\n",
                   GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0));
            break;

        case 2*100:
            printflags("Initial");
            if (SetConsoleCtrlHandler(MyHandler, TRUE))
                printflags("After Set(MyHandler,FALSE) -> ok");
            else
                printflags("After Set(MyHandler,FALSE) -> nok");
            //printf("GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) rc=%d\n",
            //       GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0));
            Sleep(10000);
            printflags("end");
            break;

        case 3*100:
            printf("GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0) rc=%d\n",
                   GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0));
            printf("fatal error: Invalid testcase number (%d).\n", atol(argv[1]));
            break;

        /*
         * Negative testing of GenerateConsoleCtrlEvent().
         */
        case 20:
        {
            int     i;
            struct _NegEvent
            {
                DWORD   dwEvent;
                DWORD   dwPidGroup;
                DWORD   dwLastError;
            }  aEvents[] =
            {
                    {CTRL_CLOSE_EVENT/*2*/,          0,         ERROR_MR_MID_NOT_FOUND},
                    {CTRL_LOGOFF_EVENT/*5*/,         0,         ERROR_MR_MID_NOT_FOUND},
                    {CTRL_SHUTDOWN_EVENT/*6*/,       0,         ERROR_MR_MID_NOT_FOUND},
                    {3,                              0,         ERROR_MR_MID_NOT_FOUND},
                    {4,                              0,         ERROR_MR_MID_NOT_FOUND},
                    {7,                              0,         ERROR_MR_MID_NOT_FOUND},
                    {-1,                             0,         ERROR_MR_MID_NOT_FOUND},
                    {-2,                             0,         ERROR_MR_MID_NOT_FOUND},
                    {CTRL_C_EVENT,          0xdeadbeef,         -1}, /* this is weird! */
                    {CTRL_BREAK_EVENT,      0xdeadbeef,         -1}, /* this is weird! */
                    {CTRL_LOGOFF_EVENT/*5*/,        -1,         ERROR_MR_MID_NOT_FOUND},
                    {CTRL_SHUTDOWN_EVENT/*6*/,      -1,         ERROR_MR_MID_NOT_FOUND},
                    {CTRL_SHUTDOWN_EVENT/*6*/,      -1,         ERROR_MR_MID_NOT_FOUND},
                    {3,                             -1,         ERROR_MR_MID_NOT_FOUND},
                    {4,                             -1,         ERROR_MR_MID_NOT_FOUND},
                    {7,                             -1,         ERROR_MR_MID_NOT_FOUND},
                    {-1,                            -1,         ERROR_MR_MID_NOT_FOUND},
                    {-2,                            -1,         ERROR_MR_MID_NOT_FOUND}
            };

            for (i = 0; i < sizeof(aEvents) / sizeof(aEvents[0]); i++)
            {
                BOOL fRc;
                DWORD dwLastError;
                SetLastError(-1);
                fRc = GenerateConsoleCtrlEvent(aEvents[i].dwEvent, aEvents[i].dwPidGroup);
                if (fRc != 0 && fRc != 1)
                    printf("Warning: GenerateConsoleCtrlEvent(%d, %d) returned %d\n", aEvents[i].dwEvent, aEvents[i].dwPidGroup, fRc);
                if (aEvents[i].dwLastError == -1 ? !fRc : fRc)
                {
                    printf("error: GenerateConsoleCtrlEvent(%d, %d) returned %d\n", aEvents[i].dwEvent, aEvents[i].dwPidGroup, fRc);
                    rc++;
                }

                dwLastError = GetLastError();
                if (dwLastError != aEvents[i].dwLastError)
                {
                    printf("error: GenerateConsoleCtrlEvent(%d, %d) LastError=%d expected=%d\n",
                           aEvents[i].dwEvent, aEvents[i].dwPidGroup, dwLastError, aEvents[i].dwLastError);
                    rc++;
                }
            }
            break;
        }


        /*
         * Negative testing of SetConsoleCtrlHandler().
         */
        case 21:
        {
            int     i;
            struct _NegHandler
            {
                DWORD   dwHandler;
                DWORD   dwAdd;
                DWORD   dwLastError;
            }       aHandlers[] =
            {
                    {0,                     0,         -1},                         /* 0 */
                    {0,                     0,         -1},                         /* 1 */
                    {(DWORD)&main,          0,         ERROR_INVALID_PARAMETER},    /* 2 */
                    {0xffffffff,            0,         ERROR_INVALID_PARAMETER},    /* 3 */
                    {0,                     1,         -1},                         /* 4 */
                    {0,                     1,         -1},                         /* 5 */
                    {0xffffffff,            1,         -1},                         /* 6 */
                    {(DWORD)&main,          1,         -1},                         /* 7 */
                    {(DWORD)&main,          0,         -1},                         /* 8 */
                    {(DWORD)&main, 0xdeadbeef,         -1},                         /* 9 */
                    {(DWORD)&main,          1,         -1},                         /*11 */
                    {(DWORD)&main,          0,         -1},                         /*12 */
                    {(DWORD)&main,          0,         -1},                         /*13 */
                    {(DWORD)&main,          0,         ERROR_INVALID_PARAMETER},    /*14 */
                    {0,            0xdeadbeef,         -1},                         /*15 */
            };

            for (i = 0; i < sizeof(aHandlers) / sizeof(aHandlers[0]); i++)
            {
                BOOL fRc;
                DWORD dwLastError;

                SetLastError(-1);
                fRc = SetConsoleCtrlHandler((PHANDLER_ROUTINE)(void*)aHandlers[i].dwHandler, aHandlers[i].dwAdd);
                if (fRc != 0 && fRc != 1)
                    printf("Warning-%d: SetConsoleCtrlHandler(%d, %d) returned %d\n", i, aHandlers[i].dwHandler, aHandlers[i].dwAdd, fRc);
                if (aHandlers[i].dwLastError == -1 ? !fRc : fRc)
                {
                    printf("error-%d: SetConsoleCtrlHandler(%d, %d) returned %d\n", i, aHandlers[i].dwHandler, aHandlers[i].dwAdd, fRc);
                    rc++;
                }

                dwLastError = GetLastError();
                if (dwLastError != aHandlers[i].dwLastError)
                {
                    printf("error-%d: SetConsoleCtrlHandler(%d, %d) LastError=%d expected=%d\n",
                           i, aHandlers[i].dwHandler, aHandlers[i].dwAdd, dwLastError, aHandlers[i].dwLastError);
                    rc++;
                }
            }

            break;
        }


        /*
         * Check how a removal of one self from inside a handler works.
         */
        case 24:
            fRemoveAdd = TRUE;
        case 23:
        case 22:
        {
            if (    SetConsoleCtrlHandler(RemovalHandler6, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler0, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler2, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler4, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler5, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler5, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler5, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler0, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler1, TRUE)
                &&  SetConsoleCtrlHandler(RemovalHandler0, TRUE)
                )
            {
                if (iCase == 23)
                {
                    if (!SetConsoleCtrlHandler(RemovalHandler0, FALSE))
                    {
                        printf("Failed to remove hanlder 0. lasterr=%d\n", GetLastError());
                        rc++;
                    }
                }

                if (GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0))
                {
                    char aRHOrder22[20] = {0,1,0,5,5,5,4,2,0,6,-1,-1};
                    char aRHOrder23[20] = {0,1,0,5,5,5,4,2,6,-1,-1};
                    char aRHOrder24[20] = {0,0,0,1,5,1,1,4,2,6,-1,-1};

                    Sleep(1000);        /* This is really asyncronus events apparently :/ */
                    if (    (iCase == 22 && iRHOrder == 10 && !memcmp(aRHOrder, aRHOrder22, iRHOrder))
                        ||  (iCase == 23 && iRHOrder == 9  && !memcmp(aRHOrder, aRHOrder23, iRHOrder))
                        ||  (iCase == 24 && iRHOrder == 10 && !memcmp(aRHOrder, aRHOrder24, iRHOrder))
                        )
                    {
                        rc = rc;        /* nop */
                    }
                    else
                    {
                        int     i;
                        char *  paRHOrderExpected;
                        int     cRHOrderExpected;

                        /*
                         * Failed - give details.
                         */
                        printf("error: all handlers wasn't called correctly.\n");
                        printf("actual   - #=%-2d order=", iRHOrder);
                        for (i = 0; i < iRHOrder; i++)
                            printf("%d ", (int)aRHOrder[i]);
                        switch (iCase)
                        {
                            case 22:    paRHOrderExpected = &aRHOrder22[0]; cRHOrderExpected = 10; break;
                            case 23:    paRHOrderExpected = &aRHOrder23[0]; cRHOrderExpected = 9; break;
                            case 24:    paRHOrderExpected = &aRHOrder24[0]; cRHOrderExpected = 10; break;
                        }
                        printf("\n"
                               "expected - #=%-2d order=", cRHOrderExpected);
                        for (i = 0; i < cRHOrderExpected; i++)
                            printf("%d ", (int)paRHOrderExpected[i]);
                        printf("\n");
                        rc++;
                    }
                }
                else
                {
                    printf("error: GenerateConsoleCtrlEvent failed. lasterr=%d\n", GetLastError());
                    rc++;
                }
            }
            else
            {
                printf("error: Adding of RemovalHandler failed. lasterr=%d\n", GetLastError());
                rc++;
            }
            break;
        }



        default:
            printf("fatal error: Invalid testcase number (%d).\n", atol(argv[1]));
            rc = 16;
    }

    printf(" . %d Errors .\n", rc);
    fflush(stdout);                     /* odin32 bug! */
    return rc;
}
