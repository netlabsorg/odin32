/* $Id: TstProcessReadWrite.c,v 1.2 2000-12-11 06:53:56 bird Exp $
 *
 * Test program for the ProcessReadWrite API.
 *  The testprogram will spawn a child process which it will interact with.
 *  The interaction is as follows:
 *      - Parent reads dataarea in the datasegemnt and compare it with is's
 *        own duplicate area.
 *      - Parent writes some text data to the child process.
 *      - Parent process reads the text data back again and displays it.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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

#include <string.h>
#include <stdio.h>

#include <win32k.h>

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* The data we read from the child process */
static char szDataRead[] =
    "* Test program for the ProcessReadWrite API.                                 \n"
    "*  The testprogram will spawn a child process which it will interact with.   \n"
    "*  The interaction is as follows:                                            \n"
    "*      - Parent reads dataarea in the datasegemnt and compare it with is's   \n"
    "*        own duplicate area.                                                 \n"
    "*      - Parent writes some text data to the child process.                  \n"
    "*      - Parent process reads the text data back again and displays it.      \n";

/* The data we write to the child process (overwrites szDataRead) */
static char szDataWrite[] =
    " write data - write data - write data - write data - write data - write data \n"
    " write data - write data - write data - write data - write data - write data \n"
    " write data - write data - write data - write data - write data - write data \n"
    " write data - write data - write data - write data - write data - write data \n"
    " write data - write data - write data - write data - write data - write data \n"
    " write data - write data - write data - write data - write data - write data \n";

/* Buffer used to read into */
static char szReadBuffer[1024];


/**
 *
 *
 * @returns
 * @param
 * @equiv
 * @time
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
int main(int argc, char **argv)
{
    RESULTCODES     resc;
    APIRET          rc;
    int             rcTest;
    char            szArg[1024];

    /* Check if we're the child process */
    if (argc == 2 && strcmp(argv[1], "2ndProcess") == 0)
    {
        DosSleep(-1);
        return 0;
    }

    /* initiate Win32k */
    rc = libWin32kInit();
    if (rc != NO_ERROR)
    {
        printf("libWin32kInit failed with rc=%d\n", rc);
        return rc;
    }

    /* Start child process */
    szArg[sprintf(szArg, "%s 2ndProcess", argv[0]) - 11] = '\0';
    rc = DosExecPgm(NULL, 0, EXEC_ASYNC, szArg, NULL, &resc, szArg);
    if (rc != NO_ERROR)
    {
        printf("Failed to spawn child process. DosExecPgm failed with rc=%d\n", rc);
        return rc;
    }
    printf("Created child process with PID = %d\n", resc.codeTerminate);

    /*
     * Use the API.
     */

    /* 1. Read data block from child process. */
    rcTest = W32kProcessReadWrite(resc.codeTerminate, sizeof(szDataRead), szDataRead, szReadBuffer, TRUE);
    if (rcTest == NO_ERROR)
    {
        rcTest = memcmp(szDataRead, szReadBuffer, sizeof(szDataRead));
        if (rcTest == 0)
        {
            printf("Read successfull:\n%.*s\n\n", sizeof(szDataRead), szReadBuffer);

            /* 2. Write datablock to childprocess */
            rcTest = W32kProcessReadWrite(resc.codeTerminate, sizeof(szDataWrite), szDataWrite, szDataRead, FALSE);
            if (rcTest == NO_ERROR)
            {
                printf("Wrote successfully to child\n");
                /* 2b. Read it back again */
                rcTest = W32kProcessReadWrite(resc.codeTerminate, sizeof(szDataWrite), szDataRead, szReadBuffer, TRUE);
                if (rcTest == NO_ERROR)
                {
                    rcTest = memcmp(szDataWrite, szReadBuffer, sizeof(szDataWrite));
                    if (rcTest == 0)
                    {
                        printf("Read successfull the data which we wrote:\n%.*s\n\n", sizeof(szDataWrite), szReadBuffer);
                    }
                    else
                        printf("The data read back was not equal!\n");
                }
                else
                    printf("Read back failed with rc=%d\n", rcTest);
            }
            else
                printf("Failed to write data to process. rc=%d\n", rc);
        }
        else
            printf("The data read was not equal!\n");
    }
    else
        printf("First read failed with rc=%d\n", rcTest);

    /*
     * Negative test... Read data at invalid addresses.
     */
    if (rcTest == NO_ERROR)
    {
        rcTest = W32kProcessReadWrite(resc.codeTerminate, 10, (PVOID)0xffffff00, szReadBuffer, TRUE);
        if (rcTest != NO_ERROR)
        {
            rcTest = W32kProcessReadWrite(resc.codeTerminate, 10, (PVOID)0x00000100, szReadBuffer, TRUE);
            if (rcTest != NO_ERROR)
            {
                rcTest = W32kProcessReadWrite(resc.codeTerminate, 10, (PVOID)0x001f000, szReadBuffer, TRUE); /* shouldn't be anything here! check map file... */
                if (rcTest != NO_ERROR)
                {

                }
                else
                    printf("Error - negative test: read at 0x001f000\n");
            }
            else
                printf("Error - negative test: read at 0x00000100\n");
        }
        else
            printf("Error - negative test: read at 0xffffff00\n");

        rcTest = (rcTest == 0) ?  -1 : 0;
    }

    /*
     * Test result.
     */
    if (rcTest == 0)
    {
        printf(" ... Test ended successfully! ...\n");
    }
    else
        printf(" !!!!!!! Test failed (%3d) !!!!!!!\n", rcTest);

    /*
     * Cleanup.
     */
    /* Kill child. */
    rc = DosKillProcess(DKP_PROCESS,resc.codeTerminate);
    if (rc != NO_ERROR)
        printf("Failed to kill child! rc=%d\n", rc);

    /* Terminate Win32k library. */
    libWin32kTerm();

    return rcTest;
}
