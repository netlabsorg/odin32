/* $Id: GetDriveType.c,v 1.2 2002-04-16 00:06:24 bird Exp $
 *
 * Test of GetDriveType.c
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <windows.h>

#include <string.h>
#include <stdio.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#define DEFENTRY(def) {#def, def}
static struct
{
    char *  pszDefine;
    UINT    uValue;
}   aConvTab[] =
{
    DEFENTRY(DRIVE_UNKNOWN),
    DEFENTRY(DRIVE_CANNOTDETERMINE),
    DEFENTRY(DRIVE_NO_ROOT_DIR),
    DEFENTRY(DRIVE_DOESNOTEXIST),
    DEFENTRY(DRIVE_REMOVABLE),
    DEFENTRY(DRIVE_FIXED),
    DEFENTRY(DRIVE_REMOTE),
    DEFENTRY(DRIVE_CDROM),
    DEFENTRY(DRIVE_RAMDISK)
};


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

    printf("Syntax: %s <Inputstring>[=DRIVE_<someret>]\n"
           , psz);
    return -1;
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
    int argi;
    int rc = 0;

    /*
     * Check for syntax request.
     */
    if (argc < 2)
        return syntax(argv[0]);

    for (argi = 1; argi < argc; argi++)
        if (    (argv[argi][0] == '-' || argv[argi][0] == '/')
            &&  (argv[argi][1] == '-' || argv[argi][1] == 'h' || argv[argi][1] == 'H')
            )
        return syntax(argv[0]);

    /*
     * Loop thru the arguments.
     */
    for (argi = 1; argi < argc; argi++)
    {
        int         i;
        char        szInput[256];
        char *      pszExpected;
        char *      pszResult = NULL;
        UINT        uResult;

        /* parse the argument */
        strcpy(szInput, argv[argi]);
        pszExpected = strchr(szInput, '=');
        if (pszExpected)
            *pszExpected++ = '\0';

        /* call the function */
        uResult = GetDriveTypeA(szInput);

        /* convert result to define */
        for (i = 0; i < sizeof(aConvTab) / sizeof(aConvTab[0]); i++)
        {
            if (aConvTab[i].uValue == uResult)
            {
                pszResult = aConvTab[i].pszDefine;
                break;
            }
        }

        /* print result */
        if (pszResult)
            printf("GetDriveType(\"%s\") -> %s\n", szInput, pszResult);
        else
            printf("GetDriveType(\"%s\") -> %d\n", szInput, uResult);

        /* check for error if possible */
        if (pszExpected)
        {
            UINT    uValue = ~0;

            for (i = 0; i < sizeof(aConvTab) / sizeof(aConvTab[0]); i++)
            {
                if (!stricmp(aConvTab[i].pszDefine, pszExpected))
                {
                    uValue = aConvTab[i].uValue;
                    break;
                }
            }

            if (uValue != ~0)
            {
                if (uValue == uResult)
                    printf("ok\n", argv[argi]);
                else
                {
                    printf("nok\n", argv[argi]);
                    rc++;
                }
            }
            else
                printf("result '%s' not found\n", pszExpected);
        }
    }

    return rc;
}
