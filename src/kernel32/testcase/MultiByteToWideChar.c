/* $Id: MultiByteToWideChar.c,v 1.1 2002-06-26 07:11:03 sandervl Exp $
 *
 * Test of MultiByteToWideChar().
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>

#include <process.h>
#include <string.h>
#include <stdio.h>


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
           "Testcases: \n"
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
    int     i,j,k;
    int     iCase;
    int     argi;
    int     rc = 0;

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
        /*
         * Simple testcases.
         */
        case 1:
        {
            #define  CHPAD   0xaf
            #define  WCPAD   0xafaf
            static struct _SimpleTests
            {
                int             iRc;            /* Return code. */
                int             iCodepage;      /* Codepage of the source string. */
                int             fFlags;         /* Flags to pass on. */
                int             cchSrc;         /* srclen = delta + strlen(pszString) */
                const char *    pszSrc;         /* Source string.  */
                int             cwcDst;         /* dstlen = delta + sizeof(awcDst) */
                const WCHAR     awcDst[256];    /* Result string. */
            } aTests[] =
            {
                /* rc   cp    fFlags           srclen  srcstr                            dstlen dststr                                                   */
                {   7, 865,   MB_PRECOMPOSED,     -1,  "›‘†’",                              0,  {WCPAD}},
                {   7, 865,   MB_PRECOMPOSED,     -1,  "›‘†’",                              7,  {0x00f8,0x00e6,0x00e5,0x00d8,0x00c6,0x00c5,0x0000}},
                {   0, 865,   MB_PRECOMPOSED,     -1,  "›‘†’bcdEfghiJklmn",                 6,  {0x00f8,0x00e6,0x00e5,0x00d8,0x00c6,0x00c5}},
                {   3, 865,   MB_PRECOMPOSED,      3,  "›‘†’bcdEfghiJklmn",                 6,  {0x00f8,0x00e6,0x00e5, WCPAD, WCPAD, WCPAD}},
                {  20, 865,   MB_PRECOMPOSED,     20,  "›‘†\0’abcdefghijklmnopqrstuvwxyz",  0,  {WCPAD}},
                {  20, 865,   MB_PRECOMPOSED,     20,  "›‘†\0’abcdefghijklmnopqrstuvwxyz", 20,  {0x00f8,0x00e6,0x00e5,0x0000,0x00d8,0x00c6,0x00c5,0x0061,0x0062,0x0063,
                                                                                                   0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,0x006c,0x006d}},
                {  20, 865,   MB_PRECOMPOSED,     20,  "›‘†\0’abcdefghijklmnopqrstuvwxyz", 21,  {0x00f8,0x00e6,0x00e5,0x0000,0x00d8,0x00c6,0x00c5,0x0061,0x0062,0x0063,
                                                                                                   0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,0x006c,0x006d, WCPAD}},
            };


            /*
             * Loop thru the testcases in aTests.
             */
            for (i = 0; i < sizeof(aTests) / sizeof(aTests[0]); i++)
            {
                WCHAR   awcDst[256];
                int     iApiRc;

                /*
                 * Initialize the buffer with known value.
                 * Print call.
                 * Do call.
                 * Print result.
                 */
                memset(&awcDst[0], CHPAD, sizeof(awcDst));

                printf("%-2d: MultiByteToWideChar(%d, %#x, \"%s\", %d, %#x, %d)",
                       i,
                       aTests[i].iCodepage,
                       aTests[i].fFlags,
                       aTests[i].pszSrc,
                       aTests[i].cchSrc,
                       awcDst,
                       aTests[i].cwcDst);

                iApiRc = MultiByteToWideChar(aTests[i].iCodepage,
                                             aTests[i].fFlags,
                                             aTests[i].pszSrc,
                                             aTests[i].cchSrc,
                                             awcDst,
                                             aTests[i].cwcDst);
                printf(" -> %d\n", iApiRc);

                /*
                 * Check return value.
                 */
                if (iApiRc != aTests[i].iRc)
                {
                    printf("error(%d): bad rc. rc=%d expected=%d.\n", i, iApiRc, aTests[i].iRc);
                    rc++;
                }
                /*else*/
                /*
                 * Check the output string (if any).
                 */
                if (    aTests[i].cwcDst > 0
                    &&  memcmp(&awcDst[0], aTests[i].awcDst, aTests[i].cwcDst * sizeof(WCHAR))
                    )
                {
                    printf("error(%d): result string mismatch.\n", i);
                    for (j = 0; j < aTests[i].cwcDst; j += 10)
                    {
                        printf("actual:  0x%04x", awcDst[j]);
                        for (k = j + 1; k < aTests[i].cwcDst && k - j < 10; k++)
                            printf(",0x%04x", awcDst[k]);
                        printf("\n"
                               "expected:0x%04x", aTests[i].awcDst[k]);
                        for (k = j + 1; k < aTests[i].cwcDst && k - j < 10; k++)
                            printf(",0x%04x", aTests[i].awcDst[k]);
                        printf("\n");
                    }
                }
                /*else*/
                /*
                 * Check the padding of the output buffer.
                 */
                if (aTests[i].cwcDst > 0)
                {
                    const char *pchEnd = (const char*)&awcDst[0] + sizeof(awcDst);
                    const char *pch = (const char*)&awcDst[aTests[i].cwcDst];
                    int         cPaddingErrors = 0;
                    while (pch < pchEnd)
                    {
                        if (*pch != CHPAD)
                        {
                            printf("error(%d) - padding error at buffer offset %#x: 0x%02x != 0x%02x\n",
                                   i, pch - (const char*)&awcDst[0], *pch, CHPAD);
                            if (++cPaddingErrors > 5)
                                break;
                        }
                        pch++;
                    }
                }

            } /* for aTests */
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
