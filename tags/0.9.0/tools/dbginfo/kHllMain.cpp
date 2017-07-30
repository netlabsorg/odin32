/* $Id: kHllMain.cpp,v 1.2 2002-04-12 01:40:21 bird Exp $
 *
 * kHllMain - interface to the kHll class.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_TYPES
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>

#include <malloc.h>
#define  free(a)    memset(a, '7', _msize(a))  //overload free for debugging purposes.
#define  malloc(a)  memset(malloc(a), '3', a)  //overload free for debugging purposes.
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include <kTypes.h>
#include <kError.h>
#include <kFileInterfaces.h>
#include <kList.h>
#include <kFile.h>
#include <kFileFormatBase.h>
#include <kFileLX.h>

#include "hll.h"
#include "sym.h"
#include "kHll.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void syntax(void);


/**
 * Debugging entry point for the readLX constructor.
 * It reads this executable and dumps it.
 * @param    argc   Argument count.
 * @param    argv   Argument vector - only the first entry is used.
 */
int main(int argc, char **argv)
{
    kHll *  pHll;
    char *  pszInput;
    int     argi;
    APIRET  rc;

    /*
     * Check argument count.
     */
    if (argc <= 4)
    {
        syntax();
        return -1;
    }


    /*
     * Read input.
     */
    pszInput = argv[2];
    argi = 3;
    switch (argv[1][0])
    {
        case 'l':
        case 'L':
            pHll = kHll::readLX(pszInput);
            break;

        case 'e':
        case 'E':
            try
            {
                kFileLX FileLX(pszInput);
                pHll = kHll::readLXExports(&FileLX);
            }
            catch (int err)
            {
                printf("Failed to read LX file %s (exports).(err=%d)\n",
                       pszInput, argv[argi], err);
                return -2;
            }
            break;

        case 's':
        case 'S':
            try
            {
                kFile File(pszInput);
                kFileLX FileLX(argv[argi]);
                pHll = kHll::readSym(&File, &FileLX);
            }
            catch (int err)
            {
                printf("Failed to read sym file %s or LX reference file %s.(err=%d)\n",
                       pszInput, argv[argi], err);
                return -2;
            }
            argi++;
            break;

        default:
            printf("fatal error: invalid input type '%s'\n", argv[1]);
            return -2;
    }

    /* Check that read was successfull. */
    if (pHll != NULL)
        printf("Successfully read %s\n", pszInput);
    else
    {
        printf("fatal error: failed to read input\n");
        return -4;
    }


    /*
     * produce output
     */
    for (argi = argi; argi + 1 < argc; argi += 2)
    {
        char *pszOutput = argv[argi+1];
        switch (argv[argi][0])
        {
            case 'd':
            case 'D':
            {
                FILE *ph = fopen(pszOutput, "w");
                if (ph != NULL)
                {
                    pHll->dump(ph);
                    fclose(ph);
                    printf("Successfully wrote dump file %s\n", pszOutput);
                }
                else
                    printf("error: failed to open dump file\n");
                break;
            }

            case 'h':
            case 'H':
            {
                if (pHll->write(pszOutput))
                    printf("Successfully wrote raw HLL file %s\n", pszOutput);
                else
                    printf("Failed writing raw HLL file %s\n", pszOutput);
                break;
            }

            case 'i':
            case 'I':
                try
                {
                    kFile kidc(pszOutput, FALSE);
                    pHll->ida(&kidc);
                    printf("Successfully wrote IDC file %s\n", pszOutput);
                }
                catch (int err)
                {
                    printf("Failed writing IDC file %s. err=%d\n", pszOutput, err);
                }
                break;

            case 'l':
            case 'L':
            {
                if ((rc = pHll->writeToLX(pszOutput)) == NO_ERROR)
                    printf("Successfully wrote to LX file %s\n", pszOutput);
                else
                    printf("Failed writing to LX file %s (rc=%d)", pszOutput, rc);
                break;
            }

            case 's':
            case 'S':
                try
                {
                    kFile ksym(pszOutput, FALSE);
                    pHll->writeSym(&ksym);
                    printf("Successfully wrote symbol file %s\n", pszOutput);
                }
                catch (int err)
                {
                    printf("Failed writing symbol file %s. err=%d\n", pszOutput, err);
                }
                break;


            default:
                printf("error: invalid output type '%s'\n", argv[argi]);
                return -2;
        }
    }


    /* cleanup */
    delete (pHll);

    return 0;
}


/**
 * Writes program syntax help.
 */
void syntax(void)
{
    printf(
        "Syntax: kHll.exe <inputtype> <inputfile> [reffile] <outputtype> <outputfile>\n"
        "   (Multiple output pairs are allowed.)\n"
        "\n"
        "   InputType:\n"
        "       l       LX (HLL) debuginfo.\n"
        "       e       LX export table.\n"
        "       s       Symbol files with extra LX reference file [reffile].\n"
        "\n"
        "   OutputType:\n"
        "       i       IDA IDC macro file.\n"
        "       h       Raw HLL file.\n"
        "       l       Add as debuginfo to an LX executable.\n"
        "       s       Symbol file (.SYM).\n"
        "       d       Dump.\n"
        "       \n"
        "(Copyright (c) 2000 knut st. osmundsen)\n"
        );
}

#ifdef __IBMCPP__
#include "klist.cpp"
#endif

