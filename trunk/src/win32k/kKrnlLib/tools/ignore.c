/* $Id: ignore.c,v 1.1 2001-10-14 22:52:34 bird Exp $
 *
 * Ignores lines matching a certain pattern.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define HF_STDIN    0
#define HF_STDOUT   1
#define HF_STDERR   2



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#include <os2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/


int main(int argc, char **argv)
{
    int         rc;
    int         iArgCmd;
    char        szBuffer[4096];
    int         iBuffer;
    ULONG       cchRead;
    BOOL        fIgnore;

    RESULTCODES Res;
    PID         pid;
    HFILE       hStdOut = HF_STDOUT;
    HFILE       hStdErr = HF_STDERR;
    HFILE       hStdOutSave = -1;
    HFILE       hStdErrSave = -1;
    HPIPE       hPipeR = NULLHANDLE;
    HPIPE       hPipeW = NULLHANDLE;

    /*
     * Find the command. (first argument without dash)
     */
    for (iArgCmd = 1; iArgCmd < argc; iArgCmd++)
        if (argv[iArgCmd][0] != '-')
            break;

    /*
     * Fail if no command.
     */
    if (iArgCmd >= argc)
    {
        fprintf(stderr, "Fatal error: no command given\n");
        return 8;
    }

    /*
     * Redirect.
     */
    rc = DosCreatePipe(&hPipeR, &hPipeW, sizeof(szBuffer) - 1);
    if (rc)
    {
        fprintf(stderr, "Internal Error: Failed to create pipe! rc=%d\n", rc);
        return 8;
    }
    DosDupHandle(HF_STDOUT, &hStdOutSave);
    DosDupHandle(HF_STDERR, &hStdErrSave);
    DosDupHandle(hPipeW, &hStdOut);
    DosDupHandle(hPipeW, &hStdErr);

    /*
     * Star child and restore filehandles.
     */
    /*rc = DosExecPgm(szObj, sizeof(szObj), EXEC_ASYNCRESULT,
                    szArg, pJob->JobInfo.szzEnv, &Res, szArg);*/
    rc = spawnvp(P_NOWAIT, argv[iArgCmd], &argv[iArgCmd]);
    pid = (PID)rc;
    DosClose(hStdOut); hStdOut = HF_STDOUT;
    DosClose(hStdErr); hStdErr = HF_STDERR;
    DosDupHandle(hStdOutSave, &hStdOut);
    DosDupHandle(hStdErrSave, &hStdErr);
    DosClose(hStdOutSave);
    DosClose(hStdErrSave);
    DosClose(hPipeW);

    /*
     * Check for errors.
     */
    if ((int)rc < 0)
    {
        fprintf(stderr, "Fatal error: Failed to execute command (%s)\n", argv[iArgCmd]);
        return 8;
    }

    /*
     * Read Output.
     */
    fIgnore = FALSE;
    while ((  (rc = DosRead(hPipeR, &szBuffer[iBuffer], 256, &cchRead)) == NO_ERROR
            || rc == ERROR_MORE_DATA
            )
           && cchRead != 0
           )
    {
        int     i;
        int     iStartLine;
        ULONG   cchWritten;
        cchRead += iBuffer;
        szBuffer[cchRead] = '\0';

        /*
         * Process string.
         */
        for (iStartLine = i = 0; i < cchRead; i++)
        {
            if (fIgnore)
            {   /* currently in ignore mode */
                if (szBuffer[i] == '\r')
                {
                    if (szBuffer[i + 1] == '\n')
                        i++;
                    fIgnore = FALSE;
                }
                else if (szBuffer[i] == '\n')
                    fIgnore = FALSE;

                if (!fIgnore)
                {
                    memmove(&szBuffer[iStartLine], &szBuffer[i + 1], cchRead - i); /* could possibly get away without this move somehow.. not sure if we will gain anything... */
                    cchRead -= i - iStartLine + 1;
                    i = iStartLine - 1;
                }
            }
            else
            {
                /* check for end of line */
                if (szBuffer[i] == '\r' || szBuffer[i] == '\n')
                {
                    if (szBuffer[i] == '\r' && szBuffer[i + 1] == '\n')
                        i++;
                    iStartLine = i + 1;
                }
                else
                {   /* check for ignore pattern */
                    int j;
                    for (j = 1; j < iArgCmd; j++)
                        if (!strnicmp(&argv[j][1], &szBuffer[i], strlen(&argv[j][1])))
                        {
                            fIgnore = TRUE;
                            break;
                        }
                }
            }
        } /* for - scan output */

        /*
         * Write output.
         */
        if (iStartLine > 0)
            DosWrite(HF_STDOUT, &szBuffer[0], iStartLine, &cchWritten);

        /*
         * Move last (unfinished) line to start of buffer.
         */
        if (iStartLine > 0)
            memmove(&szBuffer[0], &szBuffer[iStartLine], cchRead - iStartLine + 1);
        iBuffer = cchRead - iStartLine;

    } /* while - read pipe */

    /* finished reading */
    DosClose(hPipeR);

    /*
     * Write remaining part of buffer.
     */
    if (!fIgnore && iBuffer > 0)
        DosWrite(HF_STDOUT, &szBuffer[0], strlen(szBuffer), &cchRead);

    /*
     * Get result.
     */
    DosWaitChild(DCWA_PROCESS, DCWW_WAIT, &Res, &pid, Res.codeTerminate);
    if (Res.codeTerminate == TC_EXIT)
        return Res.codeResult;
    return Res.codeResult != 0 ? Res.codeResult : 8;
}
