/* $Id: clfix.c,v 1.2 2000-11-09 20:42:43 bird Exp $
 *
 * A wrapper program for cl.exe fix will try fix some of the problems
 * we have seen.
 *
 *   syntax: clfix.exe <cl> [cl arguments]
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


int main(int argc, char **argv)
{
    static char     szArgBuffer[32768];
    RESULTCODES     resc;
    int             argi;
    char *          psz;
    HFILE           hFile;
    APIRET          rc;


    /*
     * Check that we have cl.exe at least passed in.
     */
    if (argc < 2)
    {
        PSZ psz =
            "A wrapper program for cl.exe fix will try fix some of the problems\r\n"
            "we have seen.\r\n"
            "\r\n"
            "  syntax: clfix.exe <drive:fullpath\\cl.exe> [cl arguments]\r\n";
        ULONG cch = strlen(psz);
        DosWrite(0, psz, cch, &cch);
        return -1;
    }


    /*
     * First argument
     */
    psz = strcpy(szArgBuffer, argv[1]);
    psz += strlen(psz);
    psz++;


    /* The other arguments. */
    if (argc > 2)
    {
        argi = 2;
        while (argi < argc)
        {
            strcpy(psz, argv[argi]);
            psz += strlen(psz);
            *psz++ = ' ';
            argi++;
        }
        psz[-1] = '\0';
    }
    *psz = '\0';


    /*
     * Set max filehandles
     * � Is this inherited?
     * - Probably not. So we'll have exploit handle inheritance
     *   to fix this.
     */
    DosSetMaxFH(100);

    /*
     * Let us open a high handle number which makes the
     * child process to inherit the maximum number of open files.
     */
    hFile = 99; /* suggest handle number. */
    rc = DosDupHandle(1, &hFile);
    if (rc)
        printf("clfix: DosDupHandle failed with rc=%d\n\n", rc);


    /*
     * Execute argument without any environment.
     *  Some of the internal errors which has occured might be caused by long variables or
     *  generally a big environment block. We'll send in an empty environment block and hope
     *  this will solve the problems.
     */
    if (DosExecPgm(NULL, 0, EXEC_SYNC, szArgBuffer, "\0\0", &resc, szArgBuffer) != NO_ERROR)
    {
        /*complain*/
        return -1;
    }


    /*
     * return result code from cl.exe.
     */
    return resc.codeResult;
}

