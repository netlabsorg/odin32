/* $Id: StateUpd.cpp,v 1.10 2000-02-12 17:55:02 bird Exp $
 *
 * StateUpd - Scans source files for API functions and imports data on them.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "StateUpd.h"
#include "db.h"



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static FILE  *phLog = NULL;
static FILE  *phSignal = NULL;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static void openLogs(void);
static void closeLogs(void);
static unsigned long processDir(const char *pszDirOrFile, POPTIONS pOptions);
static unsigned long processFile(const char *pszFilename, POPTIONS pOptions);
static unsigned long processAPI(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnHdr(PFNDESC pFnDesc, char **papszLines, int i, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnDcl(PFNDESC pFnDesc, char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnDcl2(PFNDESC pFnDesc, char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static BOOL  isFunction(char **papszLines, int i, POPTIONS pOptions);
static long _System dbNotUpdatedCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
static char *skipInsignificantChars(char **papszLines, int &i, char *psz);
static char *readFileIntoMemory(const char *pszFilename);
static char **createLineArray(char *pszFile);
static char *findEndOfWord(char *psz);
static char *findStartOfWord(char *psz, const char *pszStart);
static char *trim(char *psz);
static void copy(char *psz, int jFrom, int iFrom, int jTo, int iTo, char **papszLines);
static void copy(char *psz, char *pszFrom, int iFrom, char *pszTo, int iTo, char **papszLines);
static char *stristr(const char *pszStr, const char *pszSubStr);


/**
 * Main function.
 * @returns   Number of signals.
 * @param     argc  Argument count.
 * @param     argv  Argument array.
 */
int main(int argc, char **argv)
{
    int            argi;
    BOOL           fFatal = FALSE;
    unsigned long  ulRc = 0;
    char           szDLLName[64];
    OPTIONS        options = {TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, &szDLLName[0], -1};
    unsigned long  ulRc2;
    char          *pszErrorDesc = NULL;
    char          *pszHost     = "localhost";
    char          *pszDatabase = "Odin32";
    char          *pszUser     = "root";
    char          *pszPasswd   = "";
    ULONG          ul1, ul2;
    DosError(0x3);

    /* get dll name from directory */
    ul1 = ul2 = 0;
    DosQueryCurrentDisk(&ul1, &ul2);
    ul2 = sizeof(szDLLName);
    DosQueryCurrentDir(ul1, &szDLLName[0], &ul2);
    if (ul2 != 0)
    {
        if (szDLLName[ul2-1] == '\\' || szDLLName[ul2-1] == '/')
            szDLLName[--ul2] = '\0';
        ul1 = ul2;
        while (ul1 != 0 && szDLLName[ul1-1] != '\\' && szDLLName[ul1-1] != '/')
            ul1--;
        if (ul1 != 0)
            options.pszDLLName = &szDLLName[ul1];
    }
    else
        szDLLName[0] = '\0';


    /**************************************************************************
    * parse arguments.
    * options:  -h or -?         Syntax help.
    *           -ib<[+]|->       Integrity check at start.
    *           -ie<[+]|->       Integrity check at end.
    *           -io              Integrity check only.
    *           -s               Scan subdirectories.
    *           -Old <[+]|->     Old API Style.
    *           -OS2<[+]|->      Removes 'OS2'-prefix from function name.
    *           -COMCTL32<[+]|-> Removes 'COMCTL32'-prefix from function name.
    *           -VERSION<[+]|->  Removes 'VERSION'-prefix from function name.
    *           -Dll:<dllname>   Name of the dll being processed.
    *           -d:<dbname>      Database name
    *           -p:<passwd>      Password
    *           -u:<user>        Userid
    *           -h:<host>        Hostname/IP-address
    **************************************************************************/
    argi = 1;
    while (argi < argc && !fFatal)
    {
        if(argv[argi][0] == '-' || argv[argi][0] == '/')
        {
            switch (argv[argi][1])
            {
                case 'd':
                case 'D':
                    if (strnicmp(argv[argi], "dll:", 4) == 0 )
                        options.pszDLLName = &argv[argi][5];
                    else
                    {
                        if (argv[argi][2] == ':')
                            pszDatabase = &argv[argi][3];
                        else
                            fprintf(stderr, "warning: option '-d:' requires database name.\n");
                    }
                    break;

                case 'h':
                case 'H':
                    if (argv[argi][2] == ':')
                    {
                        pszHost = &argv[argi][3];
                        break;
                    }
                case '?':
                    syntax();
                    return 0;

                case 'i': /* Integrity */
                case 'I':
                    switch (argv[argi][2])
                    {
                        case 'b':
                        case 'B':
                            options.fIntegrityBefore = argv[argi][3] != '-';
                            break;

                        case 'e':
                        case 'E':
                            options.fIntegrityAfter = argv[argi][3] != '-';
                            break;

                        case 'o':
                        case 'O':
                            options.fIntegrityOnly = argv[argi][3] != '-';
                            break;

                        default:
                            fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                            fFatal = TRUE;
                    }
                    break;

                case 'o':
                case 'O':
                    if (stricmp(&argv[argi][1], "OS2") == 0)
                        options.fOS2 = argv[argi][4] != '-';
                    else if (stricmp(&argv[argi][1], "Old") == 0)
                        options.fOld = argv[argi][4] != '-';
                    else
                    {
                        fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                        fFatal = TRUE;
                    }
                    break;

                case 'p':
                case 'P':
                    if (argv[argi][2] == ':')
                        pszPasswd = &argv[argi][3];
                    else
                        fprintf(stderr, "warning: option '-p:' requires password.\n");
                    break;

                case 's':
                case 'S':
                    options.fRecursive = TRUE;
                    break;

                case 'u':
                case 'U':
                    if (argv[argi][2] == ':')
                        pszUser = &argv[argi][3];
                    else
                        fprintf(stderr, "error: option '-u:' requires userid.\n");
                    break;

                default:
                    fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                    fFatal = TRUE;
                    break;
            }
        }
        else
            break; /* files has started */
        argi++;
    }

    if (!fFatal)
    {
        /* open database */
        if (!dbConnect(pszHost, pszUser, pszPasswd, pszDatabase))
        {
            fprintf(stderr,   "Could not connect to database (%s). \n\terror description: %s\n",
                    pszDatabase, dbGetLastErrorDesc());
            return 0x00010001;
        }

        /* open the logs */
        openLogs();

        /* check db integrity */
        if (options.fIntegrityBefore)
        {
            pszErrorDesc = (char*)malloc(1048768); assert(pszErrorDesc != NULL);
            *pszErrorDesc = '\0';
            ulRc2 = dbCheckIntegrity(pszErrorDesc);
            if (ulRc2 != 0)
            {
                fprintf(phSignal, "-,-: integrity errors:\n\t%s\n", pszErrorDesc);
                ulRc += ulRc2 << 16;
            }
            free(pszErrorDesc);
        }

        if (!options.fIntegrityOnly)
        {
            /* find dll */
            options.lDllRefcode = dbGetDll(options.pszDLLName);
            fprintf(phLog, "DLL: refcode=%d, name=%s\n", options.lDllRefcode, options.pszDLLName);

            /* processing */
            if (argv[argi] == NULL || *argv[argi] == '\0')
                ulRc = processDir(".", &options);
            else
                while (argv[argi] != NULL)
                {
                    ulRc += processDir(argv[argi], &options);
                    argi++;
                }

            /* create new history rows */
            pszErrorDesc = (char*)malloc(1048768); assert(pszErrorDesc != NULL);
            *pszErrorDesc = '\0';
            ulRc2 = dbCreateHistory(pszErrorDesc);
            if (ulRc2 != 0)
            {
                fprintf(phSignal, "-,-: errors which occurred while creating history:\n\t%s\n", pszErrorDesc);
                ulRc += ulRc2 << 16;
            }
            free(pszErrorDesc);

            /* check db integrity */
            if (options.fIntegrityAfter)
            {
                pszErrorDesc = (char*)malloc(1048768); assert(pszErrorDesc != NULL);
                *pszErrorDesc = '\0';
                ulRc2 = dbCheckIntegrity(pszErrorDesc);
                if (ulRc2 != 0)
                {
                    fprintf(phSignal, "-,-: integrity errors:\n\t%s\n", pszErrorDesc);
                    ulRc += ulRc2 << 16;
                }
                free(pszErrorDesc);
            }
        }

        /* write status to log */
        ul2 = dbGetNumberOfUpdatedFunction(options.lDllRefcode);
        ul1 = dbCountFunctionInDll(options.lDllRefcode);
        fprintf(phLog, "-------------------------------------------------\n");
        fprintf(phLog, "-------- Functions which was not updated --------\n");
        dbGetNotUpdatedFunction(options.lDllRefcode, dbNotUpdatedCallBack);
        fprintf(phLog, "-------------------------------------------------\n");
        fprintf(phLog, "-------------------------------------------------\n\n");
        fprintf(phLog,"Number of function in this DLL:        %4ld\n", ul1);
        fprintf(phLog,"Number of successfully processed APIs: %4ld (%ld)\n", (long)(0x0000FFFF & ulRc), ul2);
        fprintf(phLog,"Number of signals:                     %4ld\n", (long)(ulRc >> 16));

        /* close the logs */
        closeLogs();

        /* close database */
        dbDisconnect();

        /* warn if error during processing. */
        fprintf(stdout,"Number of function in this DLL:        %4ld\n", ul1);
        fprintf(stdout,"Number of successfully processed APIs: %4ld (%ld)\n", (long)(0x0000FFFF & ulRc), ul2);
        fprintf(stdout,"Number of signals:                     %4ld\n", (long)(ulRc >> 16));
        if ((int)(ulRc >> 16) > 0)
            fprintf(stderr, "Check signal file 'Signals.Log'.\n");
    }

    return (int)(ulRc >> 16);
}



/**
 * Displays syntax.
 */
static void syntax()
{
    printf("\n"
           "StateUpd v%01d.%02d - Odin32 API Database utility\n"
           "----------------------------------------------\n"
           "syntax: StateUpd.exe  [-h|-?] [options] [FileOrDir1 [FileOrDir2 [...]]]\n"
           "\n"
           "    -h or -?      Syntax help. (this)\n"
           "    -ib<[+]|->    Integrity check at start.     default: enabled\n"
           "    -ie<[+]|->    Integrity check at end.       default: enabled\n"
           "    -io           Integrity check only.         default: disabled\n"
           "    -s            Scan subdirectories.          default: disabled\n"
           "    -Old          Use old API style.            default: disabled\n"
           "    -OS2          Ignore 'OS2'-prefix on APIs.  default: disabled\n"
           "    -h:<hostname> Database server hostname.     default: localhost\n"
           "    -u:<username> Username on the server.       default: root\n"
           "    -p:<password> Password.                     default: <empty>\n"
           "    -d:<database> Database to use.              default: Odin32\n"
           "\n"
           "Scans files for API functions. This util will extract data about the API\n"
           "and insert it into the database.\n"
           "\n"
           "If no files are given, then all *.c and *.cpp files will be scanned. (Not correct!)\n"
           "NOTE: When files are given, only *.c and *.cpp files will be scanned.\n"
           "Wildchars are allowed in the file spesifications.\n"
           "\n"
           "Copyright (c) 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)",
           MAJOR_VER, MINOR_VER
           );
}


/**
 * Open logs, StateUpd.log and Signals.log (error log).
 */
static void openLogs(void)
{
    if (phLog == NULL)
    {
        phLog = fopen("StateUpd.Log", "w");
        if (phLog == NULL)
        {
            fprintf(stderr,"error occured while opening log file - will use stderr instead.\n");
            phLog = stderr;
        }
    }

    if (phSignal == NULL)
    {
        phSignal = fopen("Signals.Log", "w");
        if (phSignal == NULL)
        {
            fprintf(stderr,"error occured while opening signal file - will use stdout instead.\n");
            phSignal = stdout;
        }
    }
}


/**
 * Closes the logs.
 */
static void closeLogs(void)
{
    if (phLog != stderr && phLog != NULL)
        fclose(phLog);
    if (phSignal != stdout && phSignal != NULL)
    {
        if (ftell(phSignal) > 0)
            fclose(phSignal);
        else
        {
            fclose(phSignal);
            unlink("Signals.log");
        }
    }
}


/**
 * Processes a file or a subdirectory with files.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     pszDirOrFile  Directory or file, see fFile.
 * @param     pOptions      Pointer to options.
 * @sketch    -0. Determin dir or file.
 *            0. Interpret parameters.
 *            1. Scan current directory for *.cpp and *.c files and process them.
 *            2. If recusion option is enabled:
 *                   Scan current directory for sub-directories, scan them using recursion.
 */
static unsigned long processDir(const char *pszDirOrFile, POPTIONS pOptions)
{
    unsigned long ulRc = 0; /* high word = #signals, low word = #APIs successfully processed */
    char         szBuf[CCHMAXPATH];
    char         szFileSpec[CCHMAXPATH];
    APIRET       rc;
    FILEFINDBUF3 ffb;
    FILESTATUS3  fs;
    ULONG        ul = 1;
    HDIR         hDir = (HDIR)HDIR_CREATE;
    PSZ          pszDir;
    PSZ          pszFile;
    BOOL         fFile;

    /* -0.*/
    rc = DosQueryPathInfo(pszDirOrFile, FIL_STANDARD, &fs , sizeof(fs));
    fFile = rc == NO_ERROR && (fs.attrFile & FILE_DIRECTORY) != FILE_DIRECTORY;

    /* 0. */
    strcpy(szBuf, pszDirOrFile);
    pszDir = szBuf;
    if (fFile)
    {
        if ((pszFile = strrchr(pszDir, '\\')) != NULL
            || (pszFile = strrchr(pszDir, '/')) != NULL)
            *pszFile++ = '\0';
        else
        {
            pszFile = pszDir;
            pszDir = ".";
        }
    }
    else
    {
        pszFile = NULL;
        ul = strlen(pszDir)-1;
        if (pszDir[ul] == '\\' || pszDir[ul] == '/')
            pszDir[ul] = '\0';
    }


    /* 1. */
    if (fFile)
        strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), pszFile);
    else
        strcat(strcpy(&szFileSpec[0], pszDir), "\\*.c*");
    ul = 1;
    rc = DosFindFirst((PCSZ)&szFileSpec[0], &hDir,
                      FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                      (PVOID)&ffb, sizeof(ffb), &ul, FIL_STANDARD);
    while (rc == NO_ERROR && ul == 1)
    {
        char *psz = strrchr(&ffb.achName[0], '.');
        if (psz != NULL && (!stricmp(psz, ".cpp") || !stricmp(psz, ".c")))
            ulRc += processFile(strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), &ffb.achName[0]), pOptions);

        /* next */
        ul = 1;
        rc = DosFindNext(hDir, &ffb, sizeof(ffb), &ul);
    }
    DosFindClose(hDir);

    /* 2. */
    if (pOptions->fRecursive)
    {
        strcat(strcpy(&szFileSpec[0], pszDir), "\\*");

        hDir = (HDIR)HDIR_CREATE;
        ul = 1; /* important on TVFS, not on HPFS... */
        rc = DosFindFirst((PCSZ)&szFileSpec[0], &hDir,
                          MUST_HAVE_DIRECTORY,
                          (PVOID)&ffb, sizeof(ffb), &ul, FIL_STANDARD);
        while (rc == NO_ERROR && ul == 1)
        {
            if (strcmp(&ffb.achName[0], ".") != 0 && strcmp(&ffb.achName[0], "..") != 0)
            {
                strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), &ffb.achName[0]);
                if (fFile)
                    strcat(strcat(&szFileSpec[0], "\\"), pszFile);
                ulRc += processDir(&szFileSpec[0], pOptions);
            }

            /* next */
            ul = 1;
            rc = DosFindNext(hDir, &ffb, sizeof(ffb), &ul);
        }
        DosFindClose(hDir);
    }

    return ulRc;
}


/**
 * Processes a file.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     pszFilename  Filename
 * @param     pOptions  Pointer to options.
 * @sketch     1. read file into memory.
 *             2. create line array.
 *            (3. simple preprocessing - TODO)
 *             4. scan thru the line array, looking for APIs.
 *                4b. when API is found, process it.
 */
static unsigned long processFile(const char *pszFilename, POPTIONS pOptions)
{
    unsigned long  cSignals = 0;
    unsigned long  cAPIs = 0;
    char          *pszFile;

    fprintf(phLog, "Processing '%s':\n", pszFilename);
    /* 1.*/
    pszFile = readFileIntoMemory(pszFilename);
    if (pszFile != NULL)
    {
        char **papszLines;

        /* 2.*/
        papszLines = createLineArray(pszFile);
        if (papszLines != NULL)
        {
            int i = 0;

            /* 3. - TODO */

            /* 4.*/
            while (papszLines[i] != NULL)
            {
                if (isFunction(papszLines, i, pOptions))
                {
                    unsigned long ulRc;
                    ulRc = processAPI(papszLines, i, i, pszFilename, pOptions);
                    cAPIs += 0x0000ffff & ulRc;
                    cSignals += ulRc >> 16;
                }
                else
                    i++;
            }

            free(papszLines);
        }
        else
        {
            fprintf(phSignal,"%s: error dividing file into lines.\n", pszFilename);
            cSignals++;
        }
        free(pszFile);
    }
    else
    {
        fprintf(phSignal,"%s: error reading file.\n", pszFilename);
        cSignals++;
    }
    fprintf(phLog, "Processing of '%s' is completed.\n\n", pszFilename);


    return (unsigned long)((cSignals << 16) | cAPIs);
}


/**
 * Processes an API function.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     iRet         Index into papszLines. Where to resume search.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions  Pointer to options.
 */
static unsigned long processAPI(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions)
{
    unsigned long ulRc;
    int           j;
    FNDESC        FnDesc;
    memset(&FnDesc, 0, sizeof(FnDesc));

    /* default value */
    FnDesc.lStatus = 99;

    /* precondition: isFunction is true.
     * brief algorithm:
     *  1. Analyse function declaration.
     *  2. Analyse function header.
     *  3. Log data (for debug purpose).
     *  4. Update database
     */

    /* 1.*/
    ulRc = analyseFnDcl(&FnDesc, papszLines, i, iRet, pszFilename, pOptions);
    if (0x0000ffff & ulRc) /* if low word is 0 the fatal */
    {
        unsigned long ulRcTmp;
        char szErrorDesc[2113]; /* due to some limitation in the latest EMX release size is 2112 and not 4096 as initially implemented. */

        /* 2.*/
        ulRcTmp = analyseFnHdr(&FnDesc, papszLines, i, pszFilename, pOptions);
        if (ulRcTmp == ~0UL) /* check for fatal error */
            return (0xffff0000UL & ulRc) + 0x00010000UL;
        ulRc += 0xffff0000UL & ulRcTmp;

        /* 3.*/
        fprintf(phLog, "Name:      '%s'  (refcodes=", FnDesc.pszName);
        for (j = 0; j < FnDesc.cRefCodes; j++)
            fprintf(phLog, j > 0 ? ", %ld" : "%ld", FnDesc.alRefCode[j]);
        fprintf(phLog, ")\n");
        fprintf(phLog, "  Returns: '%s'\n", FnDesc.pszReturnType != NULL ? FnDesc.pszReturnType : "<missing>");
        fprintf(phLog, "  cParams: %2d\n", FnDesc.cParams);
        for (j = 0; j < FnDesc.cParams; j++)
            fprintf(phLog, "  Param %2d: type '%s' %*s name '%s'\n", j, FnDesc.apszParamType[j],
                    (int)(15 - strlen(FnDesc.apszParamType[j])), "", FnDesc.apszParamName[j]);
        fprintf(phLog, "  Status:   %ld - '%s'\n", FnDesc.lStatus, FnDesc.pszStatus != NULL ? FnDesc.pszStatus : "<missing>");
        fprintf(phLog, "  cAuthors: %2d\n", FnDesc.cAuthors);
        for (j = 0; j < FnDesc.cAuthors; j++)
            fprintf(phLog, "  Author %d: '%s'  (refcode=%ld)\n", j, FnDesc.apszAuthor[j], FnDesc.alAuthorRefCode[j]);

        /* 4.*/
        ulRcTmp = dbUpdateFunction(&FnDesc, pOptions->lDllRefcode, &szErrorDesc[0]);
        if (ulRcTmp != 0)
        {
            fprintf(phSignal, "%s,%s: %s\n", pszFilename, FnDesc.pszName, &szErrorDesc[0]);
            ulRc += ulRcTmp << 16;
        }
    }

    return ulRc;
}


/**
 * Analyses the function declaration.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     iRet         Index into papszLines. Where to start searching again.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions     Pointer to options.
 */
static unsigned long analyseFnDcl(PFNDESC pFnDesc, char **papszLines, int i, int &iRet,
                                  const char *pszFilename, POPTIONS pOptions)
{
    static long     lPrevFnDll = -1L; /* fix for duplicate dlls */
    unsigned long   ulRc;
    FNFINDBUF       FnFindBuf;
    long            lFn = 0;

    /* brief algorithm:
     * 1. read function declaration using analyseFnDcl2.
     * 2. apply name rules.
     * 3. do a database lookup on the name.
     *  3b. if more that one match, write a signal. (TODO: a simple fix is done, but there are holes.)
     */

    /* 1. */
    ulRc = analyseFnDcl2(pFnDesc, papszLines, i, iRet, pszFilename, pOptions);
    if (ulRc != 1)
        return ulRc;

    /* 2. */
    if (pOptions->fOS2 && strncmp(pFnDesc->pszName, "OS2", 3) == 0)
        pFnDesc->pszName += 3;
    else if (pOptions->fCOMCTL32 && strncmp(pFnDesc->pszName, "COMCTL32", 8) == 0)
        pFnDesc->pszName += 8;
    else if (pOptions->fVERSION && strncmp(pFnDesc->pszName, "VERSION", 7) == 0)
        pFnDesc->pszName += 7;
    else if (pOptions->fOld)
        pFnDesc->pszName += 3;

    /* 3. */
    if (!dbFindFunction(pFnDesc->pszName, &FnFindBuf, pOptions->lDllRefcode))
    {
        fprintf(phSignal, "%s, %s: error occured while reading from database, %s\n",
                pszFilename, pFnDesc->pszName, dbGetLastErrorDesc());
        return 0x00010000;
    }

    pFnDesc->cRefCodes = 0;
    if (FnFindBuf.cFns != 0)
    {
        if (pOptions->lDllRefcode < 0)
        {
            if (FnFindBuf.cFns > 1)
            {
                fprintf(phSignal, "%s: unknown dll and more than two occurences of this function!\n", pszFilename);
                return 0x00010000;
            }
            pOptions->lDllRefcode = FnFindBuf.alDllRefCode[0];
            fprintf(phLog, "DllRef = %d\n", pOptions->lDllRefcode);
        }

        for (lFn = 0; lFn < FnFindBuf.cFns; lFn++)
            pFnDesc->alRefCode[pFnDesc->cRefCodes++] = FnFindBuf.alRefCode[lFn];

        if (pFnDesc->cRefCodes == 0)
            fprintf(phLog, "%s was not an API in this dll(%d)!\n", pFnDesc->pszName, pOptions->lDllRefcode);
    }
    else
        fprintf(phLog, "%s was not an API\n", pFnDesc->pszName);

    ulRc = pFnDesc->cRefCodes;
    return ulRc;
}



/**
 * Analyses the function declaration.
 * No DB lockup or special function type stuff, only ODINFUNCTION is processed.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     iRet         Index into papszLines. Where to start searching again.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions     Pointer to options.
 */
static unsigned long analyseFnDcl2(PFNDESC pFnDesc, char **papszLines, int i, int &iRet,
                                   const char *pszFilename, POPTIONS pOptions)
{
    /** @sketch
     * 1. find the '('
     * 2. find the word ahead of the '(', this is the function name.
     * 2a. class test.
     * 3. find the closing ')'
     * 4. copy the parameters, which is between the two '()'
     * 5. format the parameters
     */

    int     iFn, iP1, iP2, j, c;
    char *  pszFn, *pszFnEnd, *pszP1, *pszP2;
    char *  psz, *pszEnd;
    int     cArgs;
    char *  apszArgs[30];

    /* 1.*/
    iP1 = i;
    while (papszLines[iP1] != NULL
           && (pszP1 = strchr(papszLines[iP1], '(')) == NULL)
        iP1++;
    if (papszLines[iP1] == NULL)
    {
        fprintf(phSignal, "%d: oops! didn't find end of function!, %d\n", pszFilename, __LINE__);
        iRet = iP1;
        return 0x00010000;
    }

    /* 2. */
    iFn = iP1;
    if (papszLines[iFn] != pszP1)
        pszFn = pszP1 - 1;
    else
    {
        pszFn = papszLines[--iFn];
        pszFn += strlen(pszFn);
    }
    while (iFn >= i && *pszFn == ' ')
    {
        if (pszFn != papszLines[iFn])
            pszFn--;
        else
        {
            pszFn = papszLines[--iFn];
            pszFn += strlen(pszFn);
        }
    }
    if (*pszFn == ' ' || *pszFn == '\0')
    {
        fprintf(phSignal, "%s: internal error!, %d\n", pszFilename, __LINE__);
        iRet = iP1;
        return 0x00010000;
    }
    pszFnEnd = pszFn;
    pszFn = findStartOfWord(pszFn, papszLines[i]);

    /* 2a. */
    psz = pszFn;
    while (psz >= papszLines[i] && *psz == ' ')
        psz--;
    if (psz > papszLines[i] && *psz == ':')
    {
        while (psz >= papszLines[i] && *psz == ' ')
            psz--;
        if (psz > papszLines[i] && *psz == ':')
        {
            while (psz >= papszLines[i] && *psz == ' ')
                psz--;
            if (psz > papszLines[i])
                pszFn = findStartOfWord(psz, papszLines[i]);
            else
                fprintf(phLog, "%.*s: class name is not at same line as the ::\n", pszFnEnd - psz-1, psz+1);
        }
        else
        {
            fprintf(phLog, "%.*s: invalid class '::'\n", pszFnEnd - psz, psz);
            return 0;
        }
    }

    /* 3. */
    c = 1;
    iP2 = iP1;
    pszP2 = pszP1 + 1;
    while (c > 0)
    {
        if (*pszP2 == '(')
            c++;
        else if (*pszP2 == ')')
            if (--c == 0)
                break;
        if (*pszP2++ == '\0')
            if ((pszP2 = papszLines[++iP2]) == NULL)
                break;
    }

    iRet = iP2 + 1; //assumes: only one function on a single line!

    /* 4. */
    psz = pFnDesc->szFnDclBuffer;
    copy(pFnDesc->szFnDclBuffer, pszP1, iP1, pszP2, iP2, papszLines);
    pszEnd = psz + strlen(psz) + 1;

    /* 5.*/
    cArgs = 0;
    if (stricmp(psz, "(void)") != 0 && strcmp(psz, "()") != 0 && strcmp(psz, "( )"))
    {
        char *pszC;
        pszC = trim(psz+1);
        c = 1;
        while (*pszC != '\0')
        {
            apszArgs[cArgs] = pszC;
            while (*pszC != ',' && c > 0 && *pszC != '\0')
            {
                if (*pszC == '(')
                    c++;
                else if (*pszC == ')')
                    if (--c == 0)
                        break;
                pszC++;
            }
            *pszC = '\0';
            trim(apszArgs[cArgs++]);

            /* next */
            pszC = trim(pszC + 1);
        }
    }

    /* 6. */
    if (strnicmp(pszFn, "ODINFUNCTION", 12) == 0)
    {
        if (cArgs < 2)
        {
            fprintf(phSignal, "%s: Invalid ODINFUNCTION function too few parameters!\n", pszFilename);
            return 0x00010000;
        }
        /* return type */
        pFnDesc->pszReturnType = apszArgs[0];

        /* function name */
        pFnDesc->pszName = apszArgs[1];

        /* arguments */
        j = 2;
        pFnDesc->cParams = 0;
        while (j+1 < cArgs)
        {
            pFnDesc->apszParamType[pFnDesc->cParams] = apszArgs[j];
            pFnDesc->apszParamName[pFnDesc->cParams] = apszArgs[j+1];
            pFnDesc->cParams++;
            j += 2;
        }
    }
    else
    {
        /* return type - not implemented TODO/FIXME! */
        *pszEnd = '\0';
        copy(pszEnd, papszLines[i], i, pszFn-1, iFn, papszLines);
        pFnDesc->pszReturnType = *pszEnd == '\0' ? NULL : pszEnd;
        pszEnd = strlen(pszEnd) + pszEnd + 1;
        *pszEnd = '\0';

        /* function name */
        if (pFnDesc->pszReturnType != NULL
            && stristr(pFnDesc->pszReturnType, "cdecl") != NULL)
        {   /* cdecl function is prefixed with an '_' */
            strcpy(pszEnd, "_");
            strncat(pszEnd + 1, pszFn, pszFnEnd - pszFn+1);
        }
        else
            strncat(pszEnd, pszFn, pszFnEnd - pszFn+1);
        pFnDesc->pszName = pszEnd;
        pszEnd = strlen(pszEnd) + pszEnd + 1;
        *pszEnd = '\0';


        /* arguments */
        pFnDesc->cParams = cArgs;
        for (j = 0; j < cArgs; j++)
        {
            if ((psz = strchr(apszArgs[j], ')')) == NULL)
            {
                pFnDesc->apszParamName[j] = findStartOfWord(apszArgs[j] + strlen(apszArgs[j]) - 1,
                                                            apszArgs[j]);
                pFnDesc->apszParamName[j][-1] = '\0';
                pFnDesc->apszParamType[j] = trim(apszArgs[j]);
            }
            else
            {
                char *pszP2 = psz;
                psz = findStartOfWord(psz-1, apszArgs[j]);
                strncat(pszEnd, psz, pszP2 - psz);

                pFnDesc->apszParamName[j] = pszEnd;
                memset(psz, ' ', pszP2 - psz);
                pFnDesc->apszParamType[j] = trim(apszArgs[j]);

                pszEnd = strlen(pszEnd) + pszEnd + 1;
                *pszEnd = '\0';
            }
        }
    }
    pOptions = pOptions;
    return 0x00000001;
}


/**
 * Analyses the function header.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions     Pointer to options.
 * @sketch    1. Search backwards (start at i-1) for a comment or code.
 *            2. If comment: (else fail)
 *                2a. find start and end of comment.
 *                2b. check for function header characteristics
 *                        - lots of '*'s.
 *                        - fields 'Status', 'Author' and 'Name'
 *                    or if SDS, check for:
 *                        - at least two '*'s at the begining.
 *                        - fields '@status' and/or '@author'
 *                2c. check that content of the 'Name' field matches (not SDS)
 *                2d. read the status and author fields.
 * @remark    Supports both types of function headers, Odin32-common and SDS.
 */
static unsigned long analyseFnHdr(PFNDESC pFnDesc, char **papszLines, int i, const char *pszFilename, POPTIONS pOptions)
{
    int   iStatus, iAuthor, iName, iStart, iEnd;
    int   j, jStart;
    int   fFound;
    int   fSDS = 0;
    char *psz, *pszB;
    char *pszAuthor = NULL;
    unsigned long ulRc = 0x00000001;

    pOptions = pOptions;

    if (i < 0) /* parameter validation */
        return 0;

    /* 1. + 2a.*/
    iEnd = i-1; /* find end */
    j = strlen(papszLines[iEnd]);
    j -= j > 0 ? 1 : 0;
    fFound = 0;
    while (iEnd >= 0 && i - iEnd  < 7 && papszLines[iEnd][j] != '}' &&
           !(fFound = (papszLines[iEnd][j] == '*' && papszLines[iEnd][j+1] == '/')))
        if (j-- == 0)
            if (iEnd-- > 0)
            {
                j = strlen(papszLines[iEnd]);
                j -= j > 0 ? 1 : 0;
            }
    if (!fFound) /* fail if not found */
        return 0;

    iStart = iEnd; /* find start */
    if (j < 2)
        j -= (j = strlen(papszLines[--iStart])) > 1 ? 2 : j;
    else
        j -= 2;
    fFound = 0;
    while (iStart >= 0 &&
           !(fFound = (papszLines[iStart][j] == '/' && papszLines[iStart][j+1] == '*')))
        if (j-- == 0)
            if (iStart-- > 0)
            {
                j = strlen(papszLines[iStart]);
                j -= j > 1 ? 2 : j;
            }

    if (!fFound) /* fail if not found */
        return 0;
    jStart = j;

    /* 2b.*/
    if (strncmp(&papszLines[iStart][jStart], "/**", 3) != 0) /* checks that there are more than one star at start of comment */
        return 0;

    iName = iStart; /* Odin32 common */
    while (iName <= iEnd &&
           stristr(papszLines[iName], "* Name") == NULL)
        iName++;
    iStatus = iStart;
    while (iStatus <= iEnd &&
           stristr(papszLines[iStatus], "* Status") == NULL)
        iStatus++;
    iAuthor = iStart;
    while (iAuthor <= iEnd &&
           stristr(papszLines[iAuthor], "* Author") == NULL)
        iAuthor++;

    if (!(iName <= iEnd || iStatus <= iEnd || iAuthor <= iEnd)) /* if not found try SDS */
    {
        iStatus = iStart;
        while (iStatus <= iEnd &&
               stristr(papszLines[iStatus], "@status") == NULL)
            iStatus++;
        iAuthor = iStart;
        while (iAuthor <= iEnd &&
               stristr(papszLines[iAuthor], "@author") == NULL)
            iAuthor++;
        if (!(iStatus <= iEnd || iAuthor <= iEnd))
            return 0;
        fSDS = 1;
    }

    /* 2c.*/
    if (iName <= iEnd && strstr(papszLines[iName], pFnDesc->pszName) == NULL)
        fprintf(phLog, "Warning: a matching function name is not found in the name Field\n");

    /* 2d.*/
    pszB = &pFnDesc->szFnHdrBuffer[0];
    if (!fSDS)
    {   /* Odin32 common */
        if (iStatus <= iEnd) /* Status */
        {
            psz = stristr(papszLines[iStatus], "* Status") + sizeof("* Status") - 1;
            while (*psz != '\0' && (*psz == ' ' || *psz == ':'))
                psz++;
            strcpy(pszB, psz);
            trim(pszB);
            if (*pszB != '\0' && pszB[strlen(pszB)-1] == '*') /* just in case some have an right hand '*' column */
            {
                pszB[strlen(pszB)-1] = '\0';
                trim(pszB);
            }
            pFnDesc->pszStatus = pszB;
            pszB += strlen(pszB) + 1;
        }

        if (iAuthor <= iEnd) /* Author(s) */
        {
            pszAuthor = pszB;
            psz = stristr(papszLines[iAuthor], "* Author") + sizeof("* Author") - 1;
            do
            {
                while (*psz != '\0' && (*psz == ' ' || *psz == ':'))
                    psz++;
                strcpy(pszB, psz);
                trim(pszB);
                if (*pszB != '\0' && pszB[strlen(pszB)-1] == '*') /* just in case some have an right hand '*' column */
                {
                    pszB[strlen(pszB)-1] = '\0';
                    trim(pszB);
                }
                if (*pszB != '\0' && pszB[strlen(pszB)-1] != ',')
                    strcat(pszB, ",");
                pszB += strlen(pszB);

                /* next */
                psz = papszLines[++iAuthor] + 1;
                j = 0;
                while (*psz == ' ' && j++ < 5) psz++;
                if (*psz == '*')
                    psz++;
            } while (iAuthor < iEnd && *psz == ' ');
            pszB++;
        }
    }
    else
    {   /* SDS */
        if (iStatus <= iEnd)
        {
            psz = stristr(papszLines[iStatus], "@status") + sizeof("@status");
            while (*psz == ' ')
                psz++;
            strcpy(pszB, psz);
            trim(pszB);
            pszB += strlen(pszB) + 1;
        }

        if (iAuthor <= iEnd)
        {
            pszAuthor = pszB;
            psz = stristr(papszLines[iAuthor], "@author") + sizeof("@author");
            do
            {
                while (*psz == ' ')
                    psz++;
                strcpy(pszB, psz);
                trim(pszB);
                if (*pszB != '\0' && pszB[strlen(pszB)-1] != ',')
                    strcat(pszB, ",");
                pszB += strlen(pszB) + 1;

                /* next */
                psz = papszLines[++iAuthor] + 1;
                j = 0;
                while (*psz == ' ' && j++ < 5) psz++;
                if (*psz == '*')
                    psz++;
            } while (iAuthor <= iEnd && (*psz == ' ' || *psz == '@'));
            pszB++;
        }
    }

    /* Status - refcodes are hardcoded here! */
    if (pFnDesc->pszStatus != NULL && *pFnDesc->pszStatus != '\0')
    {
        if (strstr(pFnDesc->pszStatus, "STUB") != NULL || *pFnDesc->pszStatus == '1')
            pFnDesc->lStatus = 1; /* STUB */
        else if (stristr(pFnDesc->pszStatus, "Partially") != NULL || *pFnDesc->pszStatus == '2' || *pFnDesc->pszStatus == '3')
        {
            if (stristr(pFnDesc->pszStatus, "Tested") == NULL || *pFnDesc->pszStatus == '2')
                pFnDesc->lStatus = 2; /* STUB */
            else
                pFnDesc->lStatus = 3; /* STUB */
            if (stristr(pFnDesc->pszStatus, "Open32") != NULL
                || *pFnDesc->pszStatus == '5' || *pFnDesc->pszStatus == '7')
                pFnDesc->lStatus += 4;
        }
        else if (stristr(pFnDesc->pszStatus, "Completely") != NULL || *pFnDesc->pszStatus == '4' || *pFnDesc->pszStatus == '5')
        {
            if (stristr(pFnDesc->pszStatus, "Tested") == NULL || *pFnDesc->pszStatus == '4')
                pFnDesc->lStatus = 4; /* STUB */
            else
                pFnDesc->lStatus = 5; /* STUB */
            if (stristr(pFnDesc->pszStatus, "Open32") != NULL
                || *pFnDesc->pszStatus == '8' || *pFnDesc->pszStatus == '9')
                pFnDesc->lStatus += 4;
        }
        else
        {
            fprintf(phSignal, "%s, %s: '%s' is not a valid status code.\n",
                    pszFilename, pFnDesc->pszName, pFnDesc->pszStatus);
            ulRc += 0x00010000;
        }
    }

    /* Author */
    if (pszAuthor)
    {   /* author1, author2, author3 */
        j = 0;
        psz = trim(pszAuthor);
        pFnDesc->cAuthors = 0;
        while (*psz != '\0' && pFnDesc->cAuthors < (int)(sizeof(pFnDesc->apszAuthor) / sizeof(pFnDesc->apszAuthor[0])))
        {
            char *pszBr1 = strchr(psz, '[');
            char *pszBr2 = strchr(psz, ']');
            char *pszComma;

            /* remove '[text]' text - this is usualy used for time/date */
            if (pszBr1 != NULL && pszBr2 != NULL && pszBr1 < pszBr2)
                while (pszBr1 <= pszBr2)
                    *pszBr1++ = ' ';

            /* terminate string */
            pszComma = strchr(psz, ',');
            if (pszComma != NULL)
            {
                pszAuthor = pszComma + 1;
                *pszComma = '\0';
            }

            pFnDesc->apszAuthor[pFnDesc->cAuthors]      = trim(psz);
            pFnDesc->alAuthorRefCode[pFnDesc->cAuthors] =
                dbFindAuthor(pFnDesc->apszAuthor[pFnDesc->cAuthors]);

            if (pFnDesc->alAuthorRefCode[pFnDesc->cAuthors] == -1)
            {
                fprintf(phSignal, "%s, %s: author '%s' is not recognized.\n", pszFilename, pFnDesc->pszName,
                        pFnDesc->apszAuthor[pFnDesc->cAuthors]);
                ulRc += 0x00010000;
            }

            /* next */
            pFnDesc->cAuthors++;
            psz = trim(pszAuthor);
        }
    }

    return ulRc;
}


/**
 * Checks if there may be an function starting at the current line.
 * @returns   TRUE if API found, else FALSE.
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     pOptions     Pointer to options.
 */
static BOOL isFunction(char **papszLines, int i, POPTIONS pOptions)
{
    if (!pOptions->fOld)
    {   /* new API naming style */
        /* brief algorithm:
         *  check that line don't start with '\\', '{' or '}'
         *  search for '('.
         *  if found then do
         *      find c-word previous to '('
         *      if found then do
         *          check that the following condition are true:
         *            1. word is not 'for', 'while', 'do', 'if', 'else' or 'switch'
         *            2. first significant char after '(' should not be a '*'. (Fix for functionnames in function header, "BackupRead(".)
         *            3. find the matching ')' and check that the first significant char after it is '{'.
         *          if 1, 2 and 3 are all true return true
         *  return false.
         *
         *  Note, for 2: spaces, newlines and comments are ignored, all other chars are significant.
         */
        char *pszP1 = papszLines[i];

        while (*pszP1 != '\0' && *pszP1 == ' ')
            pszP1--;
        if (*pszP1 != '\0' && *pszP1 != '\\' && pszP1[1] != '\\'
            && *pszP1 != '{' && *pszP1 != '}')
        {
            pszP1 = strchr(papszLines[i], '(');
            if (pszP1 != NULL && pszP1 > papszLines[i])
            {
                int  cchFnName  = 0;
                char *pszFnName = pszP1 - 1;

                while (pszFnName - cchFnName > papszLines[i] && pszFnName[cchFnName] == ' ')
                    cchFnName--, pszFnName--;

                pszFnName = findStartOfWord(pszFnName, papszLines[i]);
                cchFnName += pszP1 - pszFnName;
                if (cchFnName >= 0)
                {
                    /* 1. */
                    if (
                        strncmp(pszFnName, "for", cchFnName) != 0 &&
                        strncmp(pszFnName, "while", cchFnName) != 0 &&
                        strncmp(pszFnName, "do", cchFnName) != 0    &&
                        strncmp(pszFnName, "if", cchFnName) != 0    &&
                        strncmp(pszFnName, "else", cchFnName) != 0  &&
                        strncmp(pszFnName, "switch", cchFnName) != 0
                        )
                    {
                        /* 2. */
                        int   j = i;
                        char *psz = skipInsignificantChars(papszLines, j, pszP1+1);
                        if (psz != NULL && *psz != '*')
                        {
                            char *pszB = pszP1 + 1; /*'{'*/
                            int   c = 1;

                            /* 3. */
                            while (c > 0)
                            {
                                if (*pszB == '(')
                                    c++;
                                else if (*pszB == ')')
                                    if (--c == 0)
                                        break;
                                if (*pszB++ == '\0')
                                    if ((pszB = papszLines[++i]) == NULL)
                                        break;
                            }
                            if (pszB != NULL)
                            {
                                pszB = skipInsignificantChars(papszLines, i, pszB+1);
                                if (pszB != NULL && *pszB == '{')
                                {
                                    fprintf(phLog, "Function found: %.*s\n", cchFnName, pszFnName);
                                    return TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {   /* old API naming style */
        char *pszOS2;

        /* brief algorithm:
         *  search for function prefix, 'OS2'.
         *  if found then do
         *      check that the following condition are true:
         *         1. char before 'OS2' is either start-of-line (no char), space or '*'.
         *         2. first significant char after the 'OS2' prefixed word is a '('.
         *         3. find the matching ')' and check that the first significant char after it is '{'.
         *      if 1,2 and 3 all are true return true
         *  return false.
         *
         *  Note, for 2 and 3 spaces, newlines and comments are ignored, all other chars are significant.
         */
        pszOS2 = strstr(papszLines[i], "OS2");
        if (pszOS2 != NULL)
        {
            /* 1.*/
            if (pszOS2 == papszLines[i] || pszOS2[-1] == ' ' || pszOS2[-1] == '*')
            {
                char *pszP1; /*'('*/
                int  cchFnName;

                /* 2. */
                pszP1 = findEndOfWord(pszOS2);
                cchFnName = pszP1 - pszOS2;
                pszP1 = skipInsignificantChars(papszLines, i, pszP1);

                if (pszP1 != NULL && *pszP1 == '(')
                {
                    char *pszB = pszP1 + 1; /*'{'*/
                    int   c = 1;

                    /* 3. */
                    while (c > 0)
                    {
                        if (*pszB == '(')
                            c++;
                        else if (*pszB == ')')
                            if (--c == 0)
                                break;
                        if (*pszB++ == '\0')
                            if ((pszB = papszLines[++i]) == NULL)
                                break;
                    }
                    if (pszB != NULL)
                    {
                        pszB = skipInsignificantChars(papszLines, i, pszB+1);
                        if (pszB != NULL && *pszB == '{')
                        {
                            fprintf(phLog, "Possible API: %.*s\n", cchFnName, pszOS2);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}



/**
 * Callback function for the dbGetNotUpdatedFunction routine.
 *
 */
static long _System dbNotUpdatedCallBack(const char *pszValue, const char *pszFieldName, void *pvUser)
{
    static i = 0;
    switch (i++)
    {
        case 0:
            fprintf(phLog, "%s", pszValue);
            break;
        case 1:
            fprintf(phLog, "(%s)", pszValue);
            break;
        case 2: /* updated */
            fprintf(phLog, " %s=%s", pszFieldName, pszValue);
            break;
        case 3: /* aliasfn */
            fprintf(phLog, " %s=%s", pszFieldName, pszValue);
            break;
        case 4:
            if (pszValue != NULL)
                fprintf(phLog, " --> %s.", pszValue);
            break;
        case 5:
            if (pszValue != NULL)
                fprintf(phLog, "%s", pszValue);
            break;
        case 6:
            if (pszValue != NULL)
                fprintf(phLog, "(%s)", pszValue);
            break;

        default:
            i = 0;
            fprintf(phLog, "\n");
    }

    if (stricmp(pszFieldName, "last") == 0)
    {
        i = 0;
        fprintf(phLog, "\n");
    }

    pvUser = pvUser;
    return 0;
}


/**
 * Skips insignificant chars (spaces, new-lines and comments)
 * @returns   pointer to new string posision. NULL if end of file.
 * @param     papszLines  Pointer to line table.
 * @param     i           Index into line table. (current line)
 * @param     psz         Pointer where to start (within the current line).
 */
static char *skipInsignificantChars(char **papszLines, int &i, char *psz)
{
    BOOL fComment = *psz == '/' && psz[1]  == '*';

    while (fComment || *psz == ' ' || *psz == '\0' || (*psz == '/' && psz[1] == '/'))
    {
        if (*psz == '\0' || (*psz == '/' && psz[1] == '/' && !fComment))
        {
            if ((psz = papszLines[++i]) == NULL)
                break;
        }
        else
            psz++;

        if (fComment)
        {
            if (!(fComment = *psz != '*' || psz[1] != '/'))
                psz += 2;
            else
                continue;
        }

        if ((fComment = *psz == '/' && psz[1]  == '*') == TRUE)
            psz += 1 + (psz[2] != '*'); // don't add two when there is a possible end comment following.
    }

    return psz;
}


/**
 * Reads a file into memory.
 * @returns   Pointer to file. This should be freed using 'free' when processing
 *                             the file is not needed.
 * @param     pszFilename  Name of file to read.
 * @remark    Current implementation reads the file as a binary file.
 */
static char *readFileIntoMemory(const char *pszFilename)
{
    char *pszFile = NULL;
    int   cbFile = 0; /* don't have to initialized, but it removes gcc warning (release) */
    FILE *phFile;

    phFile = fopen(pszFilename, "rb");
    if (phFile != NULL)
    {
        if (!fseek(phFile, 0, SEEK_END)
            && (cbFile = (int)ftell(phFile)) > 0
            && !fseek(phFile, 0, SEEK_SET)
            )
        {
            pszFile = (char*)malloc(cbFile + 1);
            if (pszFile != NULL)
            {
                #if 1
                    memset((void*)pszFile, 0, cbFile + 1);
                    if (fread((void*)pszFile, 1, cbFile, phFile) == 1)
                    {
                        free(pszFile);
                        pszFile = NULL;
                    }
                #else
                    int   cLines = 0;
                    int   cch = 0;
                    char *psz = pszFile;

                    while (!feof(phFile) && cch < cbFile &&
                           fgets(psz, cbFile - cch, phFile) != NULL)
                    {
                        int cchLine;
                        cch += cchLine = strlen(psz);
                        psz += cchLine;
                        cLines++;
                    }

                    /* error check */
                    if (cch > cbFile || !feof(phFile))
                    {
                        free(pszFile);
                        pszFile = NULL;
                    }
                    else
                        *psz = '\0';
                #endif
            }
        }
        fclose(phFile);
    }

    return pszFile;
}


/**
 * Creates an array of lines from a "memory" file. The last entry in the array contains NULL.
 * @returns   Pointer to the array of lines.
 * @param     pszFile  Pointer to "memory" file.
 */
static char **createLineArray(char *pszFile)
{
    char *psz = pszFile;
    char **papszLines = NULL;
    int  cLines = 0;

    while (*psz != '\0')
    {
        if (*psz == '\r')
        {
            if (psz[1] == '\n')
                psz++;
            cLines++;
        } else if (*psz == '\n')
            cLines++;
        psz++;
    }
    fprintf(phLog, "%d lines\n", cLines);

    papszLines = (char**)calloc(cLines + 1, sizeof(char *));
    if (papszLines != NULL)
    {
        cLines = 0;
        psz = pszFile;
        while (*psz != '\0')
        {
            if (*psz == '\r')
            {
                if (psz[1] == '\n')
                    *psz++ = '\0';
                papszLines[cLines++] = psz + 1;
                *psz = '\0';
            } else if (*psz == '\n')
            {
                *psz = '\0';
                papszLines[cLines++] = psz + 1;
            }
            psz++;
        }
        papszLines[cLines] = NULL; /* Strictly, this is not needed as we use calloc. */
    }


    return papszLines;
}


/** first char after word */
static char *findEndOfWord(char *psz)
{

    while (*psz != '\0' &&
            (
              (*psz >= 'A' && *psz <= 'Z') || (*psz >= 'a' && *psz <= 'z')
              ||
              (*psz >= '0' && *psz <= '9')
              ||
              *psz == '_'
            )
          )
        ++psz;
    return psz;
}


/** staring char of word */
static char *findStartOfWord(char *psz, const char *pszStart)
{
    char *pszR = psz;
    while (psz >= pszStart &&
            (
                 (*psz >= 'A' && *psz <= 'Z')
              || (*psz >= 'a' && *psz <= 'z')
              || (*psz >= '0' && *psz <= '9')
              || *psz == '_'
             )
          )
        pszR = psz--;
    return pszR;
}


static char *trim(char *psz)
{
    int i;
    if (psz == NULL)
        return NULL;
    while (*psz == ' ' || *psz == '\t')
        psz++;
    i = strlen(psz) - 1;
    while (i >= 0 && (psz[i] == ' ' || *psz == '\t'))
        i--;
    psz[i+1] = '\0';
    return psz;
}


/* copy: remove remarks, and unneeded spaces, ensuring no space after '(',
 *       ensuring space after '*', ensuring no space before ',' and ')'.
 */

static void copy(char *psz, char *pszFrom, int iFrom, char *pszTo, int iTo, char **papszLines)
{
    copy(psz, pszFrom - papszLines[iFrom], iFrom, pszTo - papszLines[iTo], iTo, papszLines);
}


static void copy(char *psz, int jFrom, int iFrom, int jTo, int iTo, char **papszLines)
{
    char chPrev = '\n';
    int i, j;
    int fComment = 0;

    i = iFrom;
    j = jFrom;
    while (i < iTo || (i == iTo && j <= jTo))
    {
        if (papszLines[i][j] != '\0' &&
            !(papszLines[i][j] == '/' && papszLines[i][j+1] == '/')  /* '//' coments */
            )
        {
            /* copy char ? */
            if (!fComment)
            {
                fComment = papszLines[i][j] == '/' && papszLines[i][j+1] == '*';
                if (!fComment && !(chPrev == ' ' && papszLines[i][j] == ' ') /* only one space char */
                    && !(chPrev == '(' && papszLines[i][j] == ' ') /* no space after '(' */
                    )
                {
                    if (chPrev == ' ' && (papszLines[i][j] == ',' || papszLines[i][j] == ')'))
                        psz[-1] = papszLines[i][j]; /* no space before ',' and ')' */
                    else
                    {
                        chPrev = *psz++ = papszLines[i][j];
                        if (chPrev == '*')                  /* ensure ' ' after '*' */
                            chPrev = *psz++ = ' ';
                    }
                }
            }
            else
                if ((fComment = papszLines[i][j] != '*' || papszLines[i][j+1] != '/') == 0)
                    j++;
            j++;
        }
        else
        {
            /* next */
            j = 0;
            i++;
            if (chPrev != ' ' && chPrev != '(')
                chPrev = *psz++ = ' ';
        }
    }
    *psz = '\0';
}


/**
 * Upcases a char.
 * @returns   Upper case of the char given in ch.
 * @param     ch  Char to capitalize.
 */
inline char upcase(char ch)
{
    return ch >= 'a' && ch <= 'z' ?  (char)(ch - ('a' - 'A')) : ch;
}


/**
 * Searches for a substring in a string.
 * @returns   Pointer to start of substring when found, NULL when not found.
 * @param     pszStr     String to be searched.
 * @param     pszSubStr  String to be searched.
 * @remark    Depends on the upcase function.
 */
static char *stristr(const char *pszStr, const char *pszSubStr)
{
    int cchSubStr = strlen(pszSubStr);
    int i = 0;

    while (*pszStr != '\0' && i < cchSubStr)
    {
        i = 0;
        while (i < cchSubStr && pszStr[i] != '\0' &&
               (upcase(pszStr[i]) == upcase(pszSubStr[i])))
            i++;
        pszStr++;
    }

    return (char*)(*pszStr != '\0' ? pszStr - 1 : NULL);
}

