/* $Id: StateUpd.cpp,v 1.3 1999-12-02 14:29:14 bird Exp $ */
/*
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
static unsigned long processDir(char *pszDir, BOOL fFile, POPTIONS pOptions);
static unsigned long processFile(const char *pszFilename, POPTIONS pOptions);
static unsigned long processAPI(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnHdr(PFNDESC pFnDesc, char **papszLines, int i, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnDcl(PFNDESC pFnDesc, char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static BOOL  isFunction(char **papszLines, int i, POPTIONS pOptions);
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
    OPTIONS        options = {TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE};
    unsigned long  ulRc2;
    char          *pszErrorDesc = NULL;
    char          *pszHost     = "localhost";
    char          *pszDatabase = "Odin32";
    char          *pszUser     = "root";
    char          *pszPasswd   = "";

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
                    if (argv[argi][2] == ':')
                        pszDatabase = &argv[argi][3];
                    else
                        fprintf(stderr, "warning: option '-d:' requires database name.\n");
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
            /* processing */
            if (argv[argi] == NULL)
                ulRc = processDir(".", FALSE, &options);
            else
                while (argv[argi] != NULL)
                {
                    ulRc += processDir(argv[argi],
                                       argv[argi][strlen(argv[argi])] == '\\' || argv[argi][strlen(argv[argi])] == '/',
                                       &options);
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

        /* close the logs */
        closeLogs();

        /* close database */
        dbDisconnect();

        /* warn if error during processing. */
        fprintf(stdout,"Number of successfully processed APIs: %4ld\n", (long)(0x0000FFFF & ulRc));
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
        fclose(phSignal);
}


/**
 * Processes a subdirectory and files.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     pszDir    Directory or file, see fFile.
 * @param     fFile     File indicator, TRUE: pszDir is a file, FALSE: pszDir is a dir.
 * @param     pOptions  Pointer to options.
 * @sketch    0. Interpret parameters.
 *            1. Scan current directory for *.cpp and *.c files and process them.
 *            2. If recusion option is enabled:
 *                   Scan current directory for sub-directories, scan them using recursion.
 */
static unsigned long processDir(char *pszDir, BOOL fFile, POPTIONS pOptions)
{
    unsigned long ulRc = 0; /* high word = #signals, low word = #APIs successfully processed */
    char         szFileSpec[CCHMAXPATH];
    APIRET       rc;
    FILEFINDBUF3 ffb;
    ULONG        ul = 1;
    HDIR         hDir = (HDIR)HDIR_CREATE;
    char        *pszFile;

    /* O. */
    if (fFile)
    {
        if ((pszFile = strrchr(pszDir, '\\')) == NULL)
            pszFile = strrchr(pszDir, '/');
        if (pszFile != NULL)
            *pszFile++ = '\0';
        else
        {
            pszFile = pszDir;
            pszDir = ".";
        }
    }
    else
        pszFile = NULL;


    /* 1. */
    if (fFile)
        strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), pszFile);
    else
        strcat(strcpy(&szFileSpec[0], pszDir), "\\*.c*");
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
                ulRc += processDir(&szFileSpec[0], fFile, pOptions);
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
        fprintf(phLog, "Name:      '%s'  (refcode=%ld)\n", FnDesc.pszName, FnDesc.lRefCode);
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
        ulRcTmp = dbUpdateFunction(&FnDesc, &szErrorDesc[0]);
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
    static long lPrevFnDll = -1L; /* fix for duplicate dlls */
    FNFINDBUF FnFindBuf;
    char *pszOS2;
    char *pszP1;
    int   iP1;
    char *pszP2;
    int   iP2;
    char *psz;
    unsigned long ulRc = 0x000000001;
    int   iFn = 0;

    iRet = i+1;

    /* brief algorithm:
     *  1. find function name and start and end parentese. (evt. return type.)
     *  2. copy to szFnDclBuffer.
     *  3. extract name, and do a database lookup on the name.
     *   3b. if more that one match, write a signal. (TODO: a simple fix is done, but there are holes.)
     *  4. analyse parameters. (evt. return type)
     */

    /* 1.*/
    if (!pOptions->fOld)
    {   /* new API naming convention */
        pszP1 = strchr(papszLines[i], '(');
        iP1 = i;
        pszOS2 = pszP1 - 1;
        while (pszOS2 > papszLines[i] && *pszOS2 == ' ')
            pszOS2--;
        pszOS2 = findStartOfWord(pszOS2, papszLines[i]);
    }
    else
    {   /* old API naming convention */
        pszOS2 = strstr(papszLines[i], "OS2");
        iP1 = i;
        pszP1 = findEndOfWord(pszOS2);
        pszP1 = skipInsignificantChars(papszLines, iP1, pszP1);
    }
    iP2 = iP1;
    pszP2 = pszP1 + 1;
    while (*pszP2 != ')')
        if (*pszP2++ == '\0')
            if ((pszP2 = papszLines[++iP2]) == NULL)
                break;

    iRet = iP2 + 1; //assumes: only one function on a single line!

    /* 2. */
    psz = &pFnDesc->szFnDclBuffer[0];
    /* copy name */
    if (pOptions->fOS2 && strncmp(pszOS2, "OS2", 3) == 0)
    {
        if (iP1 == i)
            strncpy(psz, pszOS2+3, pszP1 - (pszOS2+3));
        else
            strcpy(psz, pszOS2+3);
    }
    else if (pOptions->fCOMCTL32 && strncmp(pszOS2, "COMCTL32", 8) == 0)
    {
        if (iP1 == i)
            strncpy(psz, pszOS2+8, pszP1 - (pszOS2+8));
        else
            strcpy(psz, pszOS2+8);
    }
    else if (pOptions->fVERSION && strncmp(pszOS2, "VERSION", 7) == 0)
    {
        if (iP1 == i)
            strncpy(psz, pszOS2+7, pszP1 - (pszOS2+7));
        else
            strcpy(psz, pszOS2+7);
    }
    else
    {
        if (!pOptions->fOld)
        {   /* new API naming convention */
            if (iP1 == i)
                strncpy(psz, pszOS2, pszP1 - pszOS2);
            else
                strcpy(psz, pszOS2);
        }
        else
        {   /* old API naming convention */
            if (iP1 == i)
                strncpy(psz, pszOS2+3, pszP1 - (pszOS2+3));
            else
                strcpy(psz, pszOS2+3);
        }
    }
    trim(psz);
    pFnDesc->pszName = psz;
    psz += strlen(psz) + 1;

    copy(psz, pszP1, iP1, pszP2, iP2, papszLines);

    /* 3.*/
    if (!dbFindFunction(pFnDesc->pszName, &FnFindBuf))
    {
        fprintf(phSignal, "%s, %s: error occured while reading from database, %s\n",
                pszFilename, pFnDesc->pszName, dbGetLastErrorDesc());
        return 0x00010000;
    }

    if (FnFindBuf.cFns == 0)
    {
        fprintf(phLog, "%s was not an API\n", pFnDesc->pszName);
        return 0;
    }
    else if (FnFindBuf.cFns > 1)
    {   /* 3b.*/
        while (iFn < (int)FnFindBuf.cFns && FnFindBuf.alDllRefCode[iFn] != lPrevFnDll)
            iFn++;
        if (lPrevFnDll == -1L && iFn >= (int)FnFindBuf.cFns)
        {
            fprintf(phSignal, "%s, %s: error - more than one function by the name '%s'\n",
                    pszFilename, pFnDesc->pszName, pFnDesc->pszName);
            return 0x00010000;
        }
    }
    pFnDesc->lRefCode = FnFindBuf.alRefCode[iFn];
    lPrevFnDll = FnFindBuf.alDllRefCode[iFn];

    /* 4.*/
    pFnDesc->cParams = 0;
    if (stricmp(psz, "(void)") != 0 && strcmp(psz, "()") != 0 && strcmp(psz, "( )"))
    {
        psz++; /* skip '(' */
        while (*psz != '\0' && *psz != ')')
        {
            char *pszName;
            char *pszEnd = psz;

            while (*pszEnd != '\0' && *pszEnd != ',' && *pszEnd != ')')
                pszEnd++;

            /* fix - if pszEnd == '\0' then there is something wrong */
            if (*pszEnd == '\0')
            {
                fprintf(phLog, "internal error (not fatal): pszEnd = '\\0' when it shouldn't, %d\n", __LINE__);
                break;
            }
            *pszEnd = '\0';

            /* paranoia test */
            if (pszEnd <= psz)
            {
                fprintf(phSignal, "%s: error - pszEnd <= psz\n", pszFilename);
                return 0x00010000;
            }

            pszName = findStartOfWord(pszEnd - 1, psz);
            if (pszName > psz)
            {
                pszName[-1] = '\0'; /* this is required to be space! */
                pFnDesc->apszParamType[pFnDesc->cParams] = trim(psz);
            }
            else
                pFnDesc->apszParamType[pFnDesc->cParams] = ""; /* no parameter type (parameter is usualy a define) */
            pFnDesc->apszParamName[pFnDesc->cParams] = trim(pszName);

            /* next */
            pFnDesc->cParams++;
            psz = pszEnd + 1;
        }
    }

    return ulRc;
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
                    cchFnName--;

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
                            char *pszB = pszP1; /*'{'*/

                            /* 3. */
                            while (*pszB != ')')
                                if (*pszB++ == '\0')
                                    if ((pszB = papszLines[++i]) == NULL)
                                        break;
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
                    char *pszB = pszP1; /*'{'*/

                    /* 3. */
                    while (*pszB != ')')
                        if (*pszB++ == '\0')
                            if ((pszB = papszLines[++i]) == NULL)
                                break;
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
    while (psz > pszStart &&
            (
              (*psz >= 'A' && *psz <= 'Z') || (*psz >= 'a' && *psz <= 'z')
              ||
              (*psz >= '0' && *psz <= '9')
              ||
              *psz == '_'
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
    while (*psz == ' ')
        psz++;
    i = strlen(psz) - 1;
    while (i >= 0 && psz[i] == ' ')
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

