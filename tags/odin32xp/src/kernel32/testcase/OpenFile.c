/* $Id: OpenFile.c,v 1.1 2003-05-06 10:11:43 sandervl Exp $
 *
 * Testcase for OpenFile (only OF_PARSE right now).
 *
 * Copyright (c) 2003 InnoTek Systemberatung GmbH
 * Author: knut st. osmundsen <bird@anduin.net>
 *
 * Project Odin Software License can be found in LICENSE.TXT.
 *
 */

#include <windows.h>

#include <string.h>
#include <stdio.h>
#include <direct.h>

#if 0 /*defined(__WATCOMC__)*/
#define dprintf(a) printf a
#else
#define dprintf(a) do { } while(0)
#endif

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
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

    printf("Syntax: %s <testcase #>\n"
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
    int         argi;
    int         rc = 0;
    HFILE       hFile;
    char        szFilename[_MAX_PATH];
    OFSTRUCT    of;
    int         iTestcase;
    int         iLastErr;
    const char *psz;
    
    /*
     * Check for syntax request.
     */
    if (argc < 2 || atoi(argv[1]) == 0)
        return syntax(argv[0]);

    for (argi = 1; argi < argc; argi++)
        if (    (argv[argi][0] == '-' || argv[argi][0] == '/')
            &&  (argv[argi][1] == '-' || argv[argi][1] == 'h' || argv[argi][1] == 'H')
            )
        return syntax(argv[0]);

    iTestcase = atoi(argv[1]);
    dprintf(("debug: testcase %d\n", iTestcase));
    switch (iTestcase)
    {
        case 1:
        {
            /*
             * OF_PARSE: Non existing file
             */
            SetLastError(0xdeadeeee);
            hFile = OpenFile(psz = ".\\notexisting.file.which.we.are.very.sure.doesn't.exist", &of, OF_PARSE);
            iLastErr = GetLastError();
            strcat(_getcwd(szFilename, sizeof(szFilename)), psz + 1);
            dprintf(("non-exitsing:   hFile=%x LastError=%d\n"
                     "  of.cBytes=%d\n"
                     "  of.fFixedDisk=%d\n"
                     "  of.nErrCode=%d\n"
                     "  of.Reserved1=%#x\n"
                     "  of.Reserved2=%#x\n"
                     "  of.szPathName=%s\n"
                     ,
                     hFile,
                     iLastErr,
                     (int)of.cBytes,
                     (int)of.fFixedDisk,
                     (int)of.nErrCode,
                     (int)of.Reserved1, /* watcom sdk */
                     (int)of.Reserved2,
                     of.szPathName));
            if (iLastErr != 0)
            {
                printf("error: Expected lasterror equals 0 not %d.\n", iLastErr);
                rc++;
            }
            if (hFile != 0)
            {
                printf("error: Expected hFile (return) equals 0 not %#x.\n", hFile);
                rc++;
            }
            if (stricmp(of.szPathName, szFilename))
            {
                printf("error: szPathName differes from our expectations.\n"
                       "       of.szPathName=%s\n"
                       "       expected=%s\n",
                       of.szPathName, szFilename);
                rc++;
            }
            if (of.cBytes != 136)
            {
                printf("error: Expected of.cBytes equals 136 not %d\n", of.cBytes);
                rc++;
            }
            if (of.nErrCode != 0)
            {
                printf("error: Expected of.nErrCode equals 0 not %d\n", of.nErrCode);
                rc++;
            }
            break;
        }

        case 2:
        {
            /*
             * OF_PARSE: Existing file.
             */
            SetLastError(0xdeadeeee);
            hFile = OpenFile(argv[0], &of, OF_PARSE);
            iLastErr = GetLastError();
            dprintf(("non-exitsing:   hFile=%x LastError=%d\n"
                     "  of.cBytes=%d\n"
                     "  of.fFixedDisk=%d\n"
                     "  of.nErrCode=%d\n"
                     "  of.Reserved1=%#x\n"
                     "  of.Reserved2=%#x\n"
                     "  of.szPathName=%s\n"
                     ,
                     hFile,
                     iLastErr,
                     (int)of.cBytes,
                     (int)of.fFixedDisk,
                     (int)of.nErrCode,
                     (int)of.Reserved1,
                     (int)of.Reserved2,
                     of.szPathName));
            if (iLastErr != 0)
            {
                printf("error: Expected lasterror equals 0 not %d.\n", iLastErr);
                rc++;
            }
            if (hFile != 0)
            {
                printf("error: Expected hFile (return) equals 0 not %#x.\n", hFile);
                rc++;
            }
            if (!strstr(of.szPathName, argv[0])) /* actually expects strcmp, but I fear hitting other odin bugs */
            {
                printf("error: Expected to find the filename in the of structure.\n", 
                       "       of.szPathName=%s\n", of.szPathName);
                rc++;
            }
            if (of.cBytes != 136)
            {
                printf("error: Expected of.cBytes equals 136 not %d\n", of.cBytes);
                rc++;
            }
            if (of.nErrCode != 0)
            {
                printf("error: Expected of.nErrCode equals 0 not %d\n", of.nErrCode);
                rc++;
            }
            break;
        }

        case 3:
        {
            /*
             * OF_PARSE: Non existing directory
             */
            SetLastError(0xdeadeeee);
            hFile = OpenFile(psz = ".\\notexisting.dir.which.we.are.very.sure.doesn't.exist\\myfile", &of, OF_PARSE);
            iLastErr = GetLastError();
            strcat(_getcwd(szFilename, sizeof(szFilename)), psz + 1);
            dprintf(("non-exitsing:   hFile=%x LastError=%d\n"
                     "  of.cBytes=%d\n"
                     "  of.fFixedDisk=%d\n"
                     "  of.nErrCode=%d\n"
                     "  of.Reserved1=%#x\n"
                     "  of.Reserved2=%#x\n"
                     "  of.szPathName=%s\n"
                     ,
                     hFile,
                     iLastErr,
                     (int)of.cBytes,
                     (int)of.fFixedDisk,
                     (int)of.nErrCode,
                     (int)of.Reserved1, /* watcom sdk */
                     (int)of.Reserved2,
                     of.szPathName));
            if (iLastErr != 0)
            {
                printf("error: Expected lasterror equals 0 not %d.\n", iLastErr);
                rc++;
            }
            if (hFile != 0)
            {
                printf("error: Expected hFile (return) equals 0 not %#x.\n", hFile);
                rc++;
            }
            if (stricmp(of.szPathName, szFilename))
            {
                printf("error: szPathName differes from our expectations.\n"
                       "       of.szPathName=%s\n"
                       "       expected=%s\n",
                       of.szPathName, szFilename);
                rc++;
            }
            if (of.cBytes != 136)
            {
                printf("error: Expected of.cBytes equals 136 not %d\n", of.cBytes);
                rc++;
            }
            if (of.nErrCode != 0)
            {
                printf("error: Expected of.nErrCode equals 0 not %d\n", of.nErrCode);
                rc++;
            }
            break;
        }

        case 4:
        {
            /*
             * OF_PARSE: Non existing file which are searched for.
             */
            SetLastError(0xdeadeeee);
            hFile = OpenFile(psz = "notexisting.file.which.we.are.very.sure.doesn't.exist", &of, OF_PARSE);
            iLastErr = GetLastError();
            strcat(strcat(_getcwd(szFilename, sizeof(szFilename)), "\\"), psz);
            dprintf(("non-exitsing:   hFile=%x LastError=%d\n"
                     "  of.cBytes=%d\n"
                     "  of.fFixedDisk=%d\n"
                     "  of.nErrCode=%d\n"
                     "  of.Reserved1=%#x\n"
                     "  of.Reserved2=%#x\n"
                     "  of.szPathName=%s\n"
                     ,
                     hFile,
                     iLastErr,
                     (int)of.cBytes,
                     (int)of.fFixedDisk,
                     (int)of.nErrCode,
                     (int)of.Reserved1, /* watcom sdk */
                     (int)of.Reserved2,
                     of.szPathName));
            if (iLastErr != 0)
            {
                printf("error: Expected lasterror equals 0 not %d.\n", iLastErr);
                rc++;
            }
            if (hFile != 0)
            {
                printf("error: Expected hFile (return) equals 0 not %#x.\n", hFile);
                rc++;
            }
            if (stricmp(of.szPathName, szFilename))
            {
                printf("error: szPathName differes from our expectations.\n"
                       "       of.szPathName=%s\n"
                       "       expected=%s\n",
                       of.szPathName, szFilename);
                rc++;
            }
            if (of.cBytes != 136)
            {
                printf("error: Expected of.cBytes equals 136 not %d\n", of.cBytes);
                rc++;
            }
            if (of.nErrCode != 0)
            {
                printf("error: Expected of.nErrCode equals 0 not %d\n", of.nErrCode);
                rc++;
            }
            break;
        }

        case 5:
        {
            /*
             * OF_PARSE | OF_EXIST | OF_CREATE: Non existing file which are searched for.
             *  (This should only do OF_PARSE)
             */
            SetLastError(0xdeadeeee);
            hFile = OpenFile(psz = "notexisting.file.which.we.are.very.sure.doesn't.exist", &of, OF_PARSE | OF_EXIST);
            iLastErr = GetLastError();
            strcat(strcat(_getcwd(szFilename, sizeof(szFilename)), "\\"), psz);
            dprintf(("non-exitsing:   hFile=%x LastError=%d\n"
                     "  of.cBytes=%d\n"
                     "  of.fFixedDisk=%d\n"
                     "  of.nErrCode=%d\n"
                     "  of.Reserved1=%#x\n"
                     "  of.Reserved2=%#x\n"
                     "  of.szPathName=%s\n"
                     ,
                     hFile,
                     iLastErr,
                     (int)of.cBytes,
                     (int)of.fFixedDisk,
                     (int)of.nErrCode,
                     (int)of.Reserved1, /* watcom sdk */
                     (int)of.Reserved2,
                     of.szPathName));
            if (iLastErr != 0)
            {
                printf("error: Expected lasterror equals 0 not %d.\n", iLastErr);
                rc++;
            }
            if (hFile != 0)
            {
                printf("error: Expected hFile (return) equals 0 not %#x.\n", hFile);
                rc++;
            }
            if (stricmp(of.szPathName, szFilename))
            {
                printf("error: szPathName differes from our expectations.\n"
                       "       of.szPathName=%s\n"
                       "       expected=%s\n",
                       of.szPathName, szFilename);
                rc++;
            }
            if (of.cBytes != 136)
            {
                printf("error: Expected of.cBytes equals 136 not %d\n", of.cBytes);
                rc++;
            }
            if (of.nErrCode != 0)
            {
                printf("error: Expected of.nErrCode equals 0 not %d\n", of.nErrCode);
                rc++;
            }
            break;
        }


        default:
        {
            printf("error: Testcase #%d does not exist.\n", iTestcase);
            rc++;
        }
    }
    

    return rc;
}

