/* $Id: kRx.c,v 1.1 2000-05-10 16:04:30 bird Exp $
 *
 * kRx - Small rexx script interpreter.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_REXXSAA
#define INCL_DOSERRORS

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <rexxsaa.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

/**
 * Main function.
 * @returns   Return code from RexxStart.
 * @param     argc  Argument count.
 * @param     argv  Argument vector.
 *                  All options ('/' or '-' prefixed words) before the
 *                  REXX script name is ignored currently.
 *                  The arguments after the rexx script name are passed
 *                  ont to the rexx script.
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
int main(int argc, char **argv)
{
    static char achArgs[4096];          /* Buffer for arguments. */
    RXSTRING    rxstrArgs;              /* Rexx arguments - one string. */
    int         cArgs;                  /* Number of rexx arguments. */
    SHORT       sRexxRc;                /* Rexx command return code (converted string). */
    RXSTRING    rxstrRexxRc;            /* Rexx command return code string. */
    int         iScriptName;            /* Argument index of the script name. */
    int         argi;                   /* Argument index loop variable. */
    APIRET      rc;                     /* Return code from RexxStart. */


    /*
     * Find Rexx script filename.
     */
    iScriptName = 1;
    while (iScriptName < argc
           && argv[iScriptName][0] == '/'
           && argv[iScriptName][0] == '-'
           )
        iScriptName++;

    /*
     * Did we find it?
     */
    if (iScriptName >= argc)
    {
        fprintf(stderr, "Invalid parameters, script filename is missing\n");
        return -10000;
    }

    /*
     * Create the rexx script arguments.
     */
    argi = iScriptName + 1;
    if (argi < argc)
    {
        char *psz = &achArgs[0];

        while (argi < argc)
        {
            int cch = strlen(argv[argi]);
            memcpy(psz, argv[argi], cch);
            psz += cch;
            *psz++ = ' ';
            argi++;
        }
        *--psz = '\0';
        cArgs = 1;
        MAKERXSTRING(rxstrArgs, &achArgs[0], (int)psz - (int)&achArgs[0]);
    }
    else
    {   /* no arguments */
        cArgs = 0;
        MAKERXSTRING(rxstrArgs, NULL, 0);
    }


    /*
     * Initiate return string.
     */
    MAKERXSTRING(rxstrRexxRc, NULL, 0);

    /*
     * Call RexxStart
     */
    rc = RexxStart(cArgs,               /* Number of arguments. */
                   &rxstrArgs,          /* Pointer to argument array. */
                   argv[iScriptName],   /* Name of REXX script. */
                   NULL,                /* Pointer ot INSTORE? Not used. */
                   "HMM",               /* Hmm? Command env. name */
                   RXCOMMAND,           /* Program called as Command  */
                   NULL, /*??*/         /* EXIT... fixme */
                   &sRexxRc,            /* Rexx program return code. */
                   &rxstrRexxRc);       /* Rexx program return string. */

    /*
     * Debug display result.
     */
    printf(
        "Interpreter rc:    %d\n"
        "Function    rc:    %d\n"
        "Return string:    '%s'\n",
        rc,
        sRexxRc,
        rxstrRexxRc.strptr);


    DosFreeMem(rxstrRexxRc.strptr);     /* Release storage from RexxStart. */

    if (rc == NO_ERROR)
        return sRexxRc;
    return -10002;
}
