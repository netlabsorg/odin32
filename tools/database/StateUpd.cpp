/* $Id: StateUpd.cpp,v 1.37.2.1 2001-09-07 10:26:39 bird Exp $
 *
 * StateUpd - Scans source files for API functions and imports data on them.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define DEBUGLOG    1                   /* enables debug logging. */
#ifdef DEBUGLOG
#define logprintf(a) fprintf a
#else
#define logprintf(a) ((int)0)
#endif

#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSMISC
#define INCL_DOSPROCESS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "StateUpd.h"
#include "db.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef DEBUGLOG
static FILE  *phLog = NULL;
#endif
static FILE  *phSignal = NULL;

static const char *pszCommonKeywords[] =
{
    "* Author",     "* Status",     "* Remark",     "* Result",
    "* Variable",   "* Parameters", "* Purpose",    NULL
};


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static void openLogs(void);
static void closeLogs(void);
static unsigned long processDir(const char *pszDirOrFile, POPTIONS pOptions);
static unsigned long processFile(const char *pszFilename, POPTIONS pOptions, BOOL fHeader);
static unsigned long processFileHeader(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long processDesignNote(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long processFunction(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnHdr(PFNDESC pFnDesc, char **papszLines, int i, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnDcl(PFNDESC pFnDesc, char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static unsigned long analyseFnDcl2(PFNDESC pFnDesc, char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions);
static void  logFunction(PFNDESC pFnDesc);
static char *ResolvSourceFile(char *pszFileObj, char **papszDirs);
static char *SDSCopyTextUntilNextTag(char *pszTarget, BOOL fHTML, int iStart, int iEnd, char **papszLines, const char *pszStart = NULL);
static char *CommonCopyTextUntilNextTag(char *pszTarget, BOOL fHTML, int iStart, int iEnd, char **papszLines, const char *pszStart = NULL);
static BOOL  isFunction(char **papszLines, int i, POPTIONS pOptions);
static BOOL  isDesignNote(char **papszLines, int i, POPTIONS pOptions);
static BOOL  isClass(char **papszLines, int i, POPTIONS pOptions);
static long _System dbNotUpdatedCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
static char *skipInsignificantChars(char **papszLines, int &i, char *psz);
static char *readFileIntoMemory(const char *pszFilename);
static char **createLineArray(char *pszFile);
static char *findEndOfWord(const char *psz);
static char *findStartOfWord(const char *psz, const char *pszStart);
inline char *trim(char *psz);
inline char *trimR(char *psz);
static char *trimHtml(char *psz);
inline char *skip(const char *psz);
static void  copy(char *psz, char *pszFrom, int iFrom, char *pszTo, int iTo, char **papszLines);
static void  copy(char *psz, int jFrom, int iFrom, int jTo, int iTo, char **papszLines);
static void  copyComment(char *psz, char *pszFrom, int iFrom, char **papszLines, BOOL fStrip, BOOL fHTML);
static void  copyComment(char *psz, int jFrom, int iFrom, char **papszLines, BOOL fStrip, BOOL fHTML);
static char *stristr(const char *pszStr, const char *pszSubStr);
static char *skipBackwards(const char *pszStopAt, const char *pszFrom, int &iLine, char **papszLines);
static int   findStrLine(const char *psz, int iStart, int iEnd, char **papszLines);
static void *textbufferCreate(const char *pszFilename);
static char *textbufferGetNextLine(void *pvBuffer, void **ppv, char *pszLineBuffer, int cchLineBuffer);
static char *textbufferNextLine(void *pvBuffer, register char *psz);
static void  textbufferDestroy(void *pvBuffer);
static signed long fsize(FILE *phFile);
static char *fileNameNoExt(const char *pszFilename, char *pszBuffer);


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
    char           szModName[64];
    char *         apszDirs[2] = {".", NULL};
    OPTIONS        options =
    {   FALSE,                          /* fIntegrityBefore  */
        FALSE,                          /* fIntegrityAfter;  */
        FALSE,                          /* fIntegrityOnly;   */
        FALSE,                          /* fRecursive;       */
        FALSE,                          /* fOld;             */
        FALSE,                          /* fOS2;             */
        FALSE,                          /* fCOMCTL32;        */
        FALSE,                          /* fVERSION;         */
        &szModName[0],                  /* pszModName;       */
        ' ',                            /* chModType;        */
        -1,                             /* lModRefcode;      */
        -1,                             /* lFileRefcode;     */
        &apszDirs[0]                    /* apszDirs;         */
    };
    unsigned long  ulRc2;
    char          *pszErrorDesc = NULL;
    char          *pszHost     = "localhost";
    char          *pszDatabase = "Odin32";
    char          *pszUser     = "root";
    char          *pszPasswd   = "";
    ULONG          ul1, ul2;
    ULONG          cUpdated, cAll, cNotAliased;

    DosError(0x3);
    /*DosSetPriority(PRTYS_PROCESSTREE, PRTYC_REGULAR, 1, 0);*/

    /* get module name from directory */
    ul1 = ul2 = 0;
    DosQueryCurrentDisk(&ul1, &ul2);
    ul2 = sizeof(szModName);
    DosQueryCurrentDir(ul1, &szModName[0], &ul2);
    if (ul2 != 0)
    {
        if (szModName[ul2-1] == '\\' || szModName[ul2-1] == '/')
            szModName[--ul2] = '\0';
        ul1 = ul2;
        while (ul1 != 0 && szModName[ul1-1] != '\\' && szModName[ul1-1] != '/')
            ul1--;
        if (ul1 != 0)
            options.pszModName = &szModName[ul1];
    }
    else
        szModName[0] = '\0';


    /**************************************************************************
    * parse arguments.
    * options:  -h or -?         Syntax help.
    *           -ib<[+]|->       Integrity check at start.
    *           -ie<[+]|->       Integrity check at end.
    *           -io              Integrity check only.
    *           -s               Scan subdirectories.
    *           -OS2<[+]|->      Removes 'OS2'-prefix from function name.
    *           -COMCTL32<[+]|-> Removes 'COMCTL32'-prefix from function name.
    *           -VERSION<[+]|->  Removes 'VERSION'-prefix from function name.
    *           -Mod:<modname>   Name of the module being processed.
    *           -Type:<modtype>  Module type. default API or whatever is in DB.
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
                    if (strnicmp(&argv[argi][1], "dll:", 4) == 0 )
                        options.pszModName = &argv[argi][5];
                    else
                    {
                        if (argv[argi][2] == ':')
                            pszDatabase = &argv[argi][3];
                        else
                            fprintf(stderr, "warning: option '-d:' requires database name.\n");
                    }
                    break;

                case '-':
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

                case 'm':
                case 'M':
                    if (strchr(&argv[argi][1], ':'))
                        options.pszModName = strchr(&argv[argi][1], ':') + 1;
                    else
                    {
                        fFatal = TRUE;
                        fprintf(stderr, "warning: option '-mod:' requires a module name.\n");
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
                    fprintf(stderr, "Warning: -s processes subdirs of source for one module\n");
                    break;

                case 'u':
                case 'U':
                    if (argv[argi][2] == ':')
                        pszUser = &argv[argi][3];
                    else
                        fprintf(stderr, "error: option '-u:' requires userid.\n");
                    break;

                case 't':
                case 'T':
                    if (strchr(&argv[argi][1], ':'))
                    {
                        char ch = *(strchr(&argv[argi][1], ':') + 1);
                        if (strchr("AIST", ch))
                            options.chModType = ch;
                        else
                        {
                            fFatal = TRUE;
                            fprintf(stderr, "warning: option '-type:' requires type char.\n");
                        }
                    }
                    else
                    {
                        fFatal = TRUE;
                        fprintf(stderr, "warning: option '-type:' requires type char.\n");
                    }
                    break;

                default:
                    fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                    fFatal = TRUE;
                    break;
            }
        }
        else if (argv[argi][0] == '@')
        {   /*
             * Parameter file (debugger parameter length restrictions led to this):
             *    Create a textbuffer.
             *    Parse the file and create a new parameter vector.
             *    Set argv to the new parameter vector, argi to 0 and argc to
             *    the parameter count.
             *    Restrictions: Parameters enclosed in "" is not implemented.
             *                  No commandline parameters are processed after the @file
             */
            char *pszBuffer = (char*)textbufferCreate(&argv[argi][1]); /* !ASSUMS! that pvBuffer is the file string! */
            if (pszBuffer != NULL)
            {
                char **apszArgs = NULL;
                char *psz = pszBuffer;
                int  i = 0;

                while (*psz != '\0')
                {
                    /* find end of parameter word */
                    char *pszEnd = psz + 1;
                    char  ch = *pszEnd;
                    while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
                        ch = *++pszEnd;

                    /* allocate more arg array space? */
                    if ((i % 512) == 0)
                    {
                        apszArgs = (char**)realloc(apszArgs, sizeof(char*) * (514 + i));
                        if (apszArgs == NULL)
                        {
                            fprintf(stderr, "error: out of memory. (line=%d)\n", __LINE__);
                            return -8;
                        }
                    }
                    *pszEnd = '\0';
                    apszArgs[i++] = psz;

                    /* next */
                    psz = pszEnd + 1;
                    ch = *psz;
                    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
                        ch = *++psz;
                }

                argc = i;
                argi = 0;
                argv = apszArgs;
                apszArgs[argc] = NULL;
                continue;
            }
            else
            {
                fprintf(stderr, "error: could not open parameter file\n");
                return -1;
            }
            break;
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
        if (options.fIntegrityBefore || options.fIntegrityOnly)
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
            /* add/update module */
            if (options.chModType != ' ' && options.pszModName)
                dbCheckInsertModule(options.pszModName, options.chModType);

            /* find module */
            options.lModRefcode = dbGetModule(options.pszModName);
            logprintf((phLog, "Module: refcode=%d, name=%s\n", options.lModRefcode, options.pszModName));
            if (options.lModRefcode >= 0)
            {
                /* processing */
                if (argv[argi] == NULL || *argv[argi] == '\0')
                    ulRc = processDir(".", &options);
                else
                    while (argi < argc)
                    {
                        char *  pszDirOrFile = argv[argi];
                        int     cchDirOrFile = strlen(pszDirOrFile);

                        if (cchDirOrFile > 4 && !stricmp(pszDirOrFile + cchDirOrFile - 4, ".obj") ) /* check for .obj */
                        {
                            pszDirOrFile = ResolvSourceFile(pszDirOrFile, options.papszDirs);
                            if (!pszDirOrFile)
                            {
                                /*
                                 * Ignore dllentry.
                                 */
                                if (!stristr(argv[argi], "dllentry"))
                                {
                                    fprintf(phSignal, "%s: Failed to resolve source file\n", argv[argi]);
                                    ulRc += 0x00010000;
                                }
                                argi++;
                                break;
                            }
                        }
                        ulRc += processDir(pszDirOrFile, &options);
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
            else
            {   /* failed to find module - concidered nearly fatal. */
                fprintf(phSignal, "-,-: failed to find module (%s)!\n\t%s\n",
                        options.pszModName ? options.pszModName : "<NULL>",
                        dbGetLastErrorDesc());
                ulRc++;
            }
        }

        /* write status to log */
        if (!options.fIntegrityOnly)
        {
            cUpdated    = dbGetNumberOfUpdatedFunction(options.lModRefcode);
            cAll        = dbCountFunctionInModule(options.lModRefcode, FALSE);
            cNotAliased = dbCountFunctionInModule(options.lModRefcode, TRUE);
            if (cNotAliased > cUpdated)
            {
                fprintf(phSignal, "%d functions where not found (found=%d, total=%d).\n",
                        cNotAliased- cUpdated, cUpdated, cNotAliased);
                ulRc += 0x00010000;
            }
            logprintf((phLog, "------------------------------------------------------\n"));
            logprintf((phLog, "-------- Functions which was not updated -------------\n"));
            dbGetNotUpdatedFunction(options.lModRefcode, dbNotUpdatedCallBack);
            logprintf((phLog, "------------------------------------------------------\n"));
            logprintf((phLog, "------------------------------------------------------\n\n"));
            logprintf((phLog,"Number of function in this module:          %4ld (%ld)\n", cAll, cNotAliased));
            logprintf((phLog,"Number of successfully processed functions: %4ld (%ld)\n", (long)(0x0000FFFF & ulRc), cUpdated));
        }
        logprintf((phLog,    "Number of signals:                          %4ld\n", (long)(ulRc >> 16)));

        /* close the logs */
        closeLogs();

        /* close database */
        dbDisconnect();

        /* warn if error during processing. */
        if (!options.fIntegrityOnly)
        {
            fprintf(stdout,"Number of function in this module:          %4ld (%ld)\n", cAll, cNotAliased);
            fprintf(stdout,"Number of successfully processed functions: %4ld (%ld)\n", (long)(0x0000FFFF & ulRc), cUpdated);
        }
        fprintf(stdout,    "Number of signals:                          %4ld\n", (long)(ulRc >> 16));
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
           "    -ib<[+]|->    Integrity check at start.     default: disabled\n"
           "    -ie<[+]|->    Integrity check at end.       default: disabled\n"
           "    -io           Integrity check only.         default: disabled\n"
           "    -s            Scan subdirectories.          default: disabled\n"
           "    -OS2          Ignore 'OS2'-prefix on APIs.  default: disabled\n"
           "    -Mod:<modname> Name of the module.          default: currentdirname\n"
           "    -Type:<type>  Module type. AIST.            default: read database\n"
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
    #ifdef DEBUGLOG
    if (phLog == NULL)
    {
        phLog = fopen("StateUpd.Log", "w");
        if (phLog == NULL)
        {
            fprintf(stderr,"error occured while opening log file - will use stderr instead.\n");
            phLog = stderr;
        }
    }
    #endif

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
    #ifdef DEBUGLOG
    if (phLog != stderr && phLog != NULL)
        fclose(phLog);
    #endif
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
    if (!fFile)
        fFile = strpbrk(pszDirOrFile, "*?") != NULL;

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
            ulRc += processFile(strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), &ffb.achName[0]), pOptions, FALSE);
        else if (psz != NULL && (!stricmp(psz, ".h") || !stricmp(psz, ".hpp")))
            ulRc += processFile(strcat(strcat(strcpy(&szFileSpec[0], pszDir), "\\"), &ffb.achName[0]), pOptions, TRUE);

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
 * @returns high word = number of signals
 *          low  word = number of APIs processed. (1 or 0).
 * @param   pszFilename Filename
 * @param   pOptions    Pointer to options.
 * @param   fHeader     Flags if we're processing a header file or not.
 * @sketch     1. read file into memory.
 *             2. create line array.
 *            (3. simple preprocessing - TODO)
 *             4. process file header.
 *             5. scan thru the line array, looking for APIs and designnotes.
 *                5b. when API is found, process it.
 *                5c. when designnote found, process it.
 */
static unsigned long processFile(const char *pszFilename, POPTIONS pOptions, BOOL fHeader)
{
    unsigned long  cSignals = 0;
    unsigned long  cAPIs = 0;
    char          *pszFile;

    logprintf((phLog, "Processing '%s':\n", pszFilename));

    /*
     * (1) Read file into memory
     */
    pszFile = readFileIntoMemory(pszFilename);
    if (pszFile != NULL)
    {
        char **papszLines;

        /*
         * (2) Create line array.
         */
        papszLines = createLineArray(pszFile);
        if (papszLines != NULL)
        {
            unsigned long ulRc;
            int i = 0;

            /*
             * 3. Simple preprocessing - TODO
             */


            /*
             * (4) Process the file header.
             */
            ulRc = processFileHeader(papszLines, i, i, pszFilename, pOptions);
            cSignals += ulRc >> 16;
            if (ulRc & 0x0000ffff)
            {
                /*
                 * (4b) Remove Design notes.
                 */
                if (!dbRemoveDesignNotes(pOptions->lFileRefcode))
                {
                    fprintf(phSignal, "%s: failed to remove design notes. %s\n",
                            pszFilename, dbGetLastErrorDesc());
                    cSignals++;
                }


                /*
                 * (5) The scan loop.
                 */
                while (papszLines[i] != NULL)
                {
                    /*
                     * (5b) Function.
                     */
                    if (isFunction(papszLines, i, pOptions))
                    {
                        ulRc = processFunction(papszLines, i, i, pszFilename, pOptions);
                        cAPIs += 0x0000ffff & ulRc;
                        cSignals += ulRc >> 16;
                    }
                    /*
                     * (5c) Design note.
                     */
                    else if (isDesignNote(papszLines, i, pOptions))
                    {
                        ulRc = processDesignNote(papszLines, i, i, pszFilename, pOptions);
                        cSignals += ulRc >> 16;
                    }
                    /*
                     * (5d) Class detection (experimental)
                     */
                    #ifdef DEBUGLOG
                    else if (isClass(papszLines, i, pOptions))
                    {
                        logprintf((phLog, "Found class at line %d. %s\n", i, papszLines[i]));
                        i++;
                    }
                    #endif
                    /*
                     * Nothing.
                     */
                    else
                        i++;
                } /* while - scan loop */
            }

            /*
             * Cleanup.
             */
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
    logprintf((phLog, "Processing of '%s' is completed.\n\n", pszFilename));

    fHeader = fHeader;

    return (unsigned long)((cSignals << 16) | cAPIs);
}


/**
 * Processes an file header and other file information.
 * @returns     high word = number of signals.
 *              low  word = Success indicator (TRUE / FALSE).
 * @param       papszLines      Array of lines in the file.
 * @param       i               Index into papszLines.
 * @param       iRet            Index into papszLines. Where to resume search.
 * @param       pszFilename     Filename.
 * @param       pOptions        Pointer to options. lFileRefcode is set on successful return.
 * @sketch      Extract module information if any....
 */
static unsigned long processFileHeader(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions)
{
    char            szDescription[10240];       /* File description buffer. */
    char            szId[128];                  /* CVS Id keyword buffer. */
    char *          psz, *psz2;
    const char *    pszDBFilename;
    char *          pszLastDateTime = NULL;
    char *          pszRevision     = NULL;
    char *          pszAuthor       = NULL;
    signed long     lLastAuthor     = 0;
    unsigned long   ulRc            = 0;

    /*
     * Find the DB filename (skip path!)
     */
    pszDBFilename = strrchr(pszFilename, '\\');
    psz = strrchr(pszFilename, '/');
    if (psz > pszDBFilename)
        pszDBFilename = psz;
    psz = strrchr(pszFilename, ':');
    if (psz > pszDBFilename)
        pszDBFilename = psz;
    if (pszDBFilename == NULL)
        pszDBFilename = pszFilename;
    else
        pszDBFilename++;

    /*
     * The module header is either on single comment or two comments.
     * The first line should be a "$Id" CVS keyword. (by convention).
     * Then the module description follows. So, we'll try find the $Id
     * keyword first.
     */
    iRet = i;
    while (i < iRet + 10 && (psz = papszLines[i]) != NULL && (psz = strstr(psz, "$Id"": ")) == NULL) /* check for end-of-file */
        i++;
    if (psz != NULL)
    {   /* found $Id: */
        psz2 = strchr(psz+3, '$');
        if (psz2 != NULL && psz2 - psz > 39 && psz2 - psz < 256)
        {
            strncpy(&szId[0], psz, psz2 - psz);
            szId[psz2 - psz] = '\0';
            iRet = i;

            /* parse it! */
            psz = strstr(szId+4, ",v ");
            if (psz != NULL)
            {
                pszRevision = trim(psz + 3);
                psz = strchr(pszRevision, ' ');
                *psz++ = '\0';
                trimR(pszRevision);

                pszLastDateTime = trim(psz);
                psz = strchr(strchr(pszLastDateTime, ' ') + 1, ' ');
                *psz++ = '\0';
                pszLastDateTime[4] = pszLastDateTime[7] = '-';
                trimR(pszLastDateTime);

                pszAuthor = trim(psz);
                psz = strchr(pszAuthor, ' ');
                *psz = '\0';
                lLastAuthor = dbFindAuthor(pszAuthor, NULL);
            }
            else
            {
                fprintf(phSignal, "%s, module header: $Id keyword is incorrect (2).\n", pszFilename);
                ulRc += 0x00010000;
            }


            /*
             * Is there more stuff here, in this comment?
             * Skip to end of the current comment and copy the contents to szDescription.
             * if szDescription suddenly contains nothing.
             */
            psz = &szDescription[0];
            copyComment(psz, psz2+1, i, papszLines, TRUE, TRUE);
            if (*psz == '\0')
            {   /*
                 * No description in the first comment. (The one with $Id.)
                 * Is there a comment following the first one?
                 */
                while (papszLines[i] != NULL && strstr(papszLines[i++], "*/") == NULL)
                    i = i;
                while (papszLines[i] != NULL)
                {
                    psz2 = papszLines[i];
                    while (*psz2 == ' ')
                        psz2++;
                    if (*psz2 != '\0')
                        break;
                    i++;
                }
                if (psz2 != NULL && strncmp(psz2, "/*", 2) == 0)
                {
                    psz = &szDescription[0];
                    copyComment(psz, psz2+1, i, papszLines, TRUE, TRUE);
                    while (*psz == '\n' && *psz == ' ')
                        psz++;
                    if (psz == '\0')
                        szDescription[0] = '\0';

                    /* Skip to line after comment end. */
                    while (papszLines[i] != NULL && strstr(papszLines[i++], "*/") == NULL)
                        i = i;
                }
            }
            else
            {
                /* Skip to line after comment end. */
                while (papszLines[i] != NULL && strstr(papszLines[i++], "*/") == NULL)
                    i = i;
            }
        }
        else
        {
            fprintf(phSignal, "%s, module header: $Id keyword is incorrect (1).\n", pszFilename);
            ulRc += 0x00010000;
        }
        iRet = i;


        /*
         * Information is collected.
         * Insert or update the database.
         */
        if (dbInsertUpdateFile((unsigned short)pOptions->lModRefcode, pszDBFilename,
                               &szDescription[0], pszLastDateTime, lLastAuthor, pszRevision))
        {
            /*
             * Get file refcode.
             */
            pOptions->lFileRefcode = dbFindFile(pOptions->lModRefcode, pszDBFilename);
            if (pOptions->lFileRefcode < 0)
            {
                fprintf(phSignal, "%s, module header: failed to find file in DB. %s\n",
                        pszDBFilename, dbGetLastErrorDesc());
                return 0x00010000;
            }
        }
        else
        {
            fprintf(phSignal, "%s, module header: failed to insert/update file. %s\n",
                    pszDBFilename, dbGetLastErrorDesc());
            return 0x00010000;
        }
    }
    else
    {
        fprintf(phSignal, "%s, module header: $Id keyword is missing.\n", pszFilename);
        return 0x00010000;
    }

    return TRUE;
}


/**
 * Processes an API function.
 * @returns   high word = number of signals
 *            low  word = Success indicator (TRUE/FALSE).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     iRet         Index into papszLines. Where to resume search.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions  Pointer to options.
 */
static unsigned long processDesignNote(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions)
{
    unsigned long   ulRc = 0;
    char            szBuffer[0x10000];
    char *          pszTitle;

    /*
     *  Find and parse the @design tag/keyword.
     *      syntax:     @design [seqnbr] <title>
     *                  <text>
     *
     */
    pszTitle = stristr(papszLines[i], "@design");
    if (pszTitle != NULL && (pszTitle[7] == '\0' || pszTitle[7] == ' '))
    {
        char *          psz;
        signed long     lSeqNbr;
        signed long     lSeqNbrNote;
        signed long     lRefCode;
        long            alSeqNbrs[1024];
        long            lLevel;

        memset(alSeqNbrs, 0, sizeof(alSeqNbrs));

        /*
         * Get title and seqnbr. Then copy the entire stuff to the buffer.
         */
        pszTitle = findEndOfWord(pszTitle+1)+1;
        lSeqNbr = atol(pszTitle);
        if (lSeqNbr != 0)
            pszTitle = findEndOfWord(pszTitle);
        pszTitle = trim(pszTitle);
        if (pszTitle != NULL && strstr(pszTitle, "*/") != NULL)
        {
            szBuffer[0] = '\0';
            *strstr(pszTitle, "*/") = '\0';
        }
        else
            copyComment(&szBuffer[0], 0, i+1, papszLines, TRUE, TRUE);
        pszTitle = trim(pszTitle);

        /*
         * Add design note section by section.
         */
        psz = &szBuffer[0];
        lLevel = 0;
        lSeqNbrNote = 0;
        do
        {
            char *  pszEnd;
            long    lNextLevel = -1;

            /*
             * Parse out title and section/squence number if not lLevel 0.
             * (psz = "@sub...")
             */
            if (lLevel > 0)
            {
                pszTitle = findEndOfWord(psz+1);
                lSeqNbr = atol(pszTitle);
                if (lSeqNbr != 0)
                {
                    pszTitle = findEndOfWord(pszTitle);
                    alSeqNbrs[lLevel] = lSeqNbr;
                }
                else
                    lSeqNbr = ++alSeqNbrs[lLevel];

                pszTitle = trim(pszTitle);
                if (pszTitle != NULL && (psz = strstr(pszTitle, "\n")) != NULL)
                    *psz++ = '\0';
                else
                    psz = "";
            }


            /*
             * Find end of this section.
             * (pszEnd will point at @sub or '\0'.)
             */
            pszEnd = psz;
            do
            {
                while (*pszEnd == '\n' || *pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r')
                    pszEnd++;
                if (!strnicmp(pszEnd, "@sub", 4) && !strnicmp(findEndOfWord(pszEnd + 1) - 7, "section", 7))
                {
                    lNextLevel = 1;
                    while (!strnicmp(pszEnd + 1 + lNextLevel * 3, "sub", 3))
                        lNextLevel++;
                    break;
                }
            } while ((pszEnd = strchr(pszEnd, '\n')) != NULL);
            if (!pszEnd)
                pszEnd = psz + strlen(psz);
            else if (psz != pszEnd)
                pszEnd[-1] = '\0';
            else
                psz = "";

            /*
             * Strip end and start of section.
             */
            psz = trimHtml(psz);
            pszTitle = trimHtml(pszTitle);

            /*
             * Add the note.
             */
            if (!dbAddDesignNote(pOptions->lModRefcode, pOptions->lFileRefcode,
                                 pszTitle, psz,
                                 lLevel, lSeqNbr, lSeqNbrNote++, i + 1, lLevel > 0, &lRefCode))
            {
                ulRc += 0x00010000;
                fprintf(phSignal, "%s(%d): Failed to add designnote. %s\n", pszFilename, i, dbGetLastErrorDesc());
            }

            /*
             * Next.
             */
            psz = pszEnd;
            if (lLevel < lNextLevel)
                memset(&alSeqNbrs[lLevel+1], 0, (lNextLevel - lLevel) * sizeof(alSeqNbrs[0]));
            lLevel = lNextLevel;

        } while (*psz);


        /* Skip to line after comment end. */
        while (papszLines[i] != NULL && strstr(papszLines[i++], "*/") == NULL)
            i = i;
        iRet = i;
    }
    else
    {
        fprintf(phSignal, "%s(%d): internal error @design\n", pszFilename, i);
        ulRc = 0x00010000;
    }

    return ulRc;
}




/**
 * Processes an function.
 * @returns   high word = number of signals
 *            low  word = number of APIs processed. (1 or 0).
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     iRet         Index into papszLines. Where to resume search.
 * @param     pszFilename  Filename used in the signal log.
 * @param     pOptions  Pointer to options.
 */
static unsigned long processFunction(char **papszLines, int i, int &iRet, const char *pszFilename, POPTIONS pOptions)
{
    unsigned long ulRc;
    int           j;
    FNDESC        FnDesc;
    memset(&FnDesc, 0, sizeof(FnDesc));

    /* default value and file number. */
    FnDesc.lStatus = 99;
    FnDesc.lFile = pOptions->lFileRefcode;
    FnDesc.lLine = i + 1;

    /* precondition: isFunction is true.
     * brief algorithm:
     *  1. Analyse function declaration.
     *  2. Analyse function header.
     *  3. Log data (for debug purpose).
     *  4. Update database
     */

    /*
     * (1) Analyse function declaration.
     */
    ulRc = analyseFnDcl(&FnDesc, papszLines, i, iRet, pszFilename, pOptions);
    if (0x0000ffff & ulRc) /* if low word is 0 the fatal */
    {
        unsigned long   ulRcTmp;
        char  *         pszErrorDesc = (char*)malloc(20480);

        /*
         * (2) Analyse function header.
         */
        ulRcTmp = analyseFnHdr(&FnDesc, papszLines, i, pszFilename, pOptions);
        if (ulRcTmp == ~0UL) /* check for fatal error */
        {
            free(pszErrorDesc);
            return (0xffff0000UL & ulRc) + 0x00010000UL;
        }
        ulRc += 0xffff0000UL & ulRcTmp;

        /*
         * (3) Log data (for debug purpose).
         */
        logFunction(&FnDesc);

        /*
         * (4) Update database.
         */
        ulRcTmp = dbUpdateFunction(&FnDesc, pOptions->lModRefcode, pszErrorDesc);
        if (ulRcTmp != 0)
        {
            fprintf(phSignal, "%s,%s: %s\n", pszFilename, FnDesc.pszName, pszErrorDesc);
            ulRc += ulRcTmp << 16;
        }
        free(pszErrorDesc);
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
    unsigned long   ulRc;
    FNFINDBUF       FnFindBuf;
    long            lFn = 0;

    /* brief algorithm:
     * 1. Read function declaration using analyseFnDcl2.
     * 2. Apply name rules.
     * 3. Do a database lookup on the name.
     *  3b. If more that one match, write a signal. (TODO: a simple fix is done, but there are holes.)
     * 4. if not found then add the function as other (type=O). Only do this if we know which module we're in.
     *  4b. do 3.
     */

    /*
     * (1) Read function declaration using analyseFnDcl2.
     */
    ulRc = analyseFnDcl2(pFnDesc, papszLines, i, iRet, pszFilename, pOptions);
    if (ulRc != 1)
        return ulRc;

    /*
     * (2) Apply name rules (if api only?).
     */
    if (pOptions->fOS2 && strncmp(pFnDesc->pszName, "OS2", 3) == 0)
        pFnDesc->pszName += 3;
    else if (pOptions->fCOMCTL32 && strncmp(pFnDesc->pszName, "COMCTL32", 8) == 0)
        pFnDesc->pszName += 8;
    else if (pOptions->fVERSION && strncmp(pFnDesc->pszName, "VERSION", 7) == 0)
        pFnDesc->pszName += 7;
    else if (pOptions->fOld)
        pFnDesc->pszName += 3;

    /*
     * (3) Do a database lookup on the name.
     */
    if (!dbFindFunction(pFnDesc->pszName, &FnFindBuf, pOptions->lModRefcode))
    {
        fprintf(phSignal, "%s, %s: error occured while reading from database, %s\n",
                pszFilename, pFnDesc->pszName, dbGetLastErrorDesc());
        return 0x00010000;
    }

    pFnDesc->cRefCodes = 0;
    if (FnFindBuf.cFns != 0)
    {
        if (pOptions->lModRefcode < 0)
        {
            if (FnFindBuf.cFns > 1)
            {
                fprintf(phSignal, "%s: unknown module and more than two occurences of this function!\n", pszFilename);
                return 0x00010000;
            }
            pOptions->lModRefcode = FnFindBuf.alModRefCode[0];
            logprintf((phLog, "ModRef = %d\n", pOptions->lModRefcode));
        }

        for (lFn = 0; lFn < FnFindBuf.cFns; lFn++)
            pFnDesc->alRefCode[pFnDesc->cRefCodes++] = FnFindBuf.alRefCode[lFn];

        if (pFnDesc->cRefCodes == 0)
            logprintf((phLog, "%s was not an API in this module(%d)!\n", pFnDesc->pszName, pOptions->lModRefcode));
    }
    else
        logprintf((phLog, "%s was not an API\n", pFnDesc->pszName));

    /*
     * (4) If not found then add the function as other (type=O).
     *     Only do this if we know which module we're in.
     */
    if (FnFindBuf.cFns == 0 || pFnDesc->cRefCodes == 0 && pOptions->lModRefcode >= 0)
    {
        if (!dbInsertUpdateFunction(pOptions->lModRefcode,
                                    pFnDesc->pszName, pFnDesc->pszName,
                                    -1, TRUE, FUNCTION_OTHER))
        {
            fprintf(phSignal, "%s, %s: error occured inserting new function, %s\n",
                    pszFilename, pFnDesc->pszName, dbGetLastErrorDesc());
            return 0x00010000;
        }

        if (!dbFindFunction(pFnDesc->pszName, &FnFindBuf, pOptions->lModRefcode))
        {
            fprintf(phSignal, "%s, %s: error occured while reading from database, %s\n",
                    pszFilename, pFnDesc->pszName, dbGetLastErrorDesc());
            return 0x00010000;
        }

        for (lFn = 0; lFn < FnFindBuf.cFns; lFn++)
            pFnDesc->alRefCode[pFnDesc->cRefCodes++] = FnFindBuf.alRefCode[lFn];

        if (pFnDesc->cRefCodes == 0)
        {
            fprintf(phSignal, "%s, %s: Function was not found even though it was just added.\n",
                    pszFilename, pFnDesc->pszName);
            return 0x00010000;
        }
    }

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
     * 1. Find the '('
     * 2. Find the word ahead of the '(', this is the function name.
     * 2a. Class test.
     * 3. Find the closing ')'
     * 4. Copy the parameters, which is between the two '()'
     * 5. Format the parameters
     * 6. Return type, function name and arguments.
     *    Check for the ODINFUNCTION macro.
     */

    int     iFn, iP1, iP2, j, c;
    char *  pszFn, *pszFnEnd, *pszP1, *pszP2;
    char *  psz, *pszEnd;
    int     cArgs;
    char *  apszArgs[30];
    int     iClass;
    char *  pszClass, *pszClassEnd;

    /* 1.*/
    iP1 = i;
    while (papszLines[iP1] != NULL
           && (pszP1 = strchr(papszLines[iP1], '(')) == NULL)
        iP1++;
    if (papszLines[iP1] == NULL)
    {
        fprintf(phSignal, "%d: oops! didn't find end of function!, %d\n", pszFilename, __LINE__);
        iRet = iP1+1;
        return 0x00010000;
    }

    /*
     * (2) find the word ahead of the '(', this is the function name.
     */
    iFn = iP1;
    if (papszLines[iFn] != pszP1)
        pszFn = pszP1 - 1;
    else
    {
        pszFn = papszLines[--iFn];
        pszFn += strlen(pszFn) - (*pszFn != '\0');
    }
    while (iFn >= 0 && *pszFn == ' ')
    {
        if (pszFn != papszLines[iFn])
            pszFn--;
        else
        {
            pszFn = papszLines[--iFn];
            pszFn += strlen(pszFn) - (*pszFn != '\0');
        }
    }
    if (iFn < 0 || *pszFn == ' ' || *pszFn == '\0')
    {
        fprintf(phSignal, "%s: internal error!, %d\n", pszFilename, __LINE__);
        iRet = iP1+1;
        return 0x00010000;
    }
    pszFnEnd = pszFn;
    pszFn = findStartOfWord(pszFn, papszLines[iFn]);

    /*
     * (2a) class test.
     */
    /* operators are not supported (BOOL kTime::operator > (const kTime &time) const) */
    if (pszFn > papszLines[iFn])
    {
        pszClassEnd = pszFn - 1;
        iClass = iFn;
    }
    else
    {
        pszClassEnd = pszFn - 1;
        iClass = iFn - 1;
    }
    c = 2;
    while (iClass >= 0 && c >= 0)
    {
        if (*pszClassEnd == ':')
            c--;
        else if (*pszClassEnd != ' ')
            break;
        pszClassEnd--;
    }
    if (*pszClassEnd != ' ' && c == 0)
        pszClass = findStartOfWord(pszClassEnd, papszLines[iClass]);
    else
        pszClass = pszClassEnd = NULL;

    /*
     * (3) find the closing ')'
     */
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

    /*
     * (4) Copy the parameters, which is between the two '()'
     */
    psz = pFnDesc->szFnDclBuffer;
    copy(pFnDesc->szFnDclBuffer, pszP1, iP1, pszP2, iP2, papszLines);
    pszEnd = psz + strlen(psz) + 1;

    /*
     * (5) Format the parameters.
     */
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

    /*
     * (6) Return type, function name and arguments.
     *     Check for the ODINFUNCTION macro.
     */
    if (strnicmp(pszFn, "ODINFUNCTION", 12) == 0 || strnicmp(pszFn, "ODINPROCEDURE", 13) == 0)
    {
        BOOL fProc = strnicmp(pszFn, "ODINPROCEDURE", 13) == 0;
        j = 0;
        if (cArgs < (fProc ? 1 : 2))
        {
            fprintf(phSignal, "%s: Invalid ODINFUNCTION function too few parameters!\n", pszFilename);
            return 0x00010000;
        }

        /* return type */
        if (fProc)
            pFnDesc->pszReturnType = "void";
        else
            pFnDesc->pszReturnType = apszArgs[j++];

        /* function name */
        pFnDesc->pszName = apszArgs[j++];

        /* arguments */
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
        /* return type */
        int     iReturn = pszClass != NULL ? iClass : iFn;
        char *  pszReturn = pszClass != NULL ? pszClass : pszFn;

        if (pszReturn != papszLines[iReturn])
            pszReturn--;
        else
        {
            pszReturn = papszLines[--iReturn];
            pszReturn += strlen(pszReturn) - (*pszReturn != '\0');
        }
        pszReturn = skipBackwards("{};-+#:\"\'", pszReturn, iReturn, papszLines);
        *pszEnd = '\0';
        copy(pszEnd, pszReturn, iReturn, pszFn-1, iFn, papszLines);
        if (strlen(pszEnd) > 128)
        {
            /* FIXME LATER! Some constructors calling baseclass constructors "breaks" this rule. Win32MDIChildWindow in /src/user32/win32wmdichild.cpp for example. */
            fprintf(phSignal,"Fatal error? return statement is too larget. len=%d\n", strlen(pszEnd));
            logprintf((phLog,   "Fatal error? return statement is too larget. len=%d\n", strlen(pszEnd)));
            if (strlen(pszEnd) > 512)
                fprintf(stderr,  "Fatal error? return statement is too larget. len=%d\n", strlen(pszEnd));
            #ifdef DEBUGLOG
            fflush(phLog);
            #endif
            fflush(phSignal);
            fflush(stderr);
        }
        pszEnd = trim(pszEnd);
        pFnDesc->pszReturnType = *pszEnd == '\0' ? NULL : pszEnd;
        pszEnd = strlen(pszEnd) + pszEnd + 1;
        *pszEnd = '\0';

        /* !BugFix! some function occur more than once, usually as inline functions */
        if (pFnDesc->pszReturnType != NULL
            && (pFnDesc->fchType == FUNCTION_ODIN32_API || pFnDesc->fchType == FUNCTION_INTERNAL_ODIN32_API)
            && stristr(pFnDesc->pszReturnType, "inline ") != NULL)
        {
            logprintf((phLog, "Not an API. Inlined functions can't be exported!\n"));
            return 0;
        }

        /* function name */
        if (pFnDesc->pszReturnType != NULL
            && (stristr(pFnDesc->pszReturnType, "cdecl") != NULL
                || strstr(pFnDesc->pszReturnType, "VFWAPIV") != NULL
                || strstr(pFnDesc->pszReturnType, "WINAPIV") != NULL
                )
            )
        {   /* cdecl function is prefixed with an '_' */
            strcpy(pszEnd, "_"); /* BUGBUG */
        }
        if (pszClass != NULL)
        {
            strncat(pszEnd,pszClass, pszClassEnd - pszClass + 1);
            strcat(pszEnd, "::");
        }
        strncat(pszEnd, pszFn, pszFnEnd - pszFn + 1);
        pFnDesc->pszName = pszEnd;
        pszEnd = strlen(pszEnd) + pszEnd + 1;
        *pszEnd = '\0';

        /* class name and type */
        if (pszClass != NULL)
        {
            pFnDesc->pszClass = pszEnd;
            strncat(pszEnd,pszClass, pszClassEnd - pszClass + 1);
            if (!pFnDesc->pszReturnType || !*pFnDesc->pszReturnType)
                pFnDesc->fchType =  *pszFn != '~' ? FUNCTION_CONSTRUCTOR : FUNCTION_DESTRUCTOR;
            else
                pFnDesc->fchType = FUNCTION_METHOD; /* BUGBUG operator. */
            pszEnd = strlen(pszEnd) + pszEnd + 1;
            *pszEnd = '\0';
        }

        /* arguments */
        pFnDesc->cParams = cArgs;
        for (j = 0; j < cArgs; j++)
        {
            int cch = strlen(apszArgs[j]);
            if ((psz = strchr(apszArgs[j], ')')) == NULL)
            {   /* Common argument */
                if (apszArgs[j][cch-1] != '*' || (apszArgs[j][cch - 1] == ']' && cch < 5))
                {   /* nearly Normal case, Type [moretype] Name.*/
                    if (apszArgs[j][cch - 1] != ']')
                    {
                        if (strcmp(apszArgs[j], "...") != 0)
                        {   /* Normal case! */
                            pFnDesc->apszParamName[j] = findStartOfWord(apszArgs[j] + cch - 1,
                                                                        apszArgs[j]);
                            if (strcmp(pFnDesc->apszParamName[j], "OPTIONAL") == 0)
                            {
                                pFnDesc->apszParamName[j]--;
                                while (*pFnDesc->apszParamName[j] == ' ')
                                    pFnDesc->apszParamName[j]--;
                                pFnDesc->apszParamName[j] = findStartOfWord(pFnDesc->apszParamName[j],
                                                                            apszArgs[j]);
                            }
                            else if (pFnDesc->cParams > j + 1 &&
                                     strncmp(apszArgs[j + 1], "OPTIONAL ", 9) == 0) //fix for ?bad? code (OPTIONAL is after the colon).
                            {
                                /* hack! !!!ASSUMES A LOT!!! */
                                apszArgs[j + 1] += 9;
                                strcat(pFnDesc->apszParamName[j], " OPTIONAL");
                            }
                            pFnDesc->apszParamName[j][-1] = '\0';
                            pFnDesc->apszParamType[j] = trim(apszArgs[j]);
                        }
                        else
                        {   /* eliptic */
                            pFnDesc->apszParamName[j] = "...";
                            pFnDesc->apszParamType[j] = "";
                        }
                    }
                    else
                    {   /* arg yet another special case! 'fn(int argc, char *argv[])' */
                        char *pszP2 = NULL;
                        cch = strlen(apszArgs[j]);
                        psz = &apszArgs[j][cch-2];
                        while (psz > apszArgs[j] && ((*psz >= '0' && *psz <= '9') || *psz == ' '))
                            psz--;

                        if (psz > apszArgs[j] && *psz == '[')
                        {
                            pszP2 = psz--;
                            while (psz >= apszArgs[j] && *psz == ' ')
                                psz--;
                        }

                        if (psz <= apszArgs[j])
                        {   /* funny case - no name? */
                            sprintf(pszEnd, "arg%i", j);
                            pFnDesc->apszParamName[j] = pszEnd;
                            pszEnd = strlen(pszEnd) + pszEnd + 1;
                            *pszEnd = '\0';
                        }
                        else
                        {   /* *pszP2 = '[' and psz = end of name */
                            psz = findStartOfWord(psz, apszArgs[j]);
                            strncat(pszEnd, psz, pszP2 - psz);
                            pFnDesc->apszParamName[j] = pszEnd;
                            pszEnd = strlen(pszEnd) + pszEnd + 1;
                            *pszEnd = '\0';
                            if (pszP2 > psz) //FIXME here is a bug. (opengl\mesa\span.c(gl_write_multitexture_span))
                                memset(psz, ' ', pszP2 - psz);
                            else
                                logprintf((phLog, "assert: line %d\n", __LINE__));
                        }
                        pFnDesc->apszParamType[j] = trim(apszArgs[j]);
                    }
                }
                else
                {   /* No argument name only type - make a dummy one: 'arg[1..n]' */
                    sprintf(pszEnd, "arg%i", j);
                    pFnDesc->apszParamName[j] = pszEnd;
                    pszEnd = strlen(pszEnd) + pszEnd + 1;
                    *pszEnd = '\0';
                    pFnDesc->apszParamType[j] = trim(apszArgs[j]);
                }
            }
            else
            {   /* Function pointer argument... */
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
 * @sketch    0. initiate pFnDesc struct
 *            1. Search backwards (start at i-1) for a comment or code.
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
    char *pszAuthors = NULL;
    unsigned long ulRc = 0x00000001;

    pOptions = pOptions;

    if (i < 0) /* parameter validation */
        return 0;

    /*
     * 0. initiate pFnDesc struct
     */
    for (j = 0; j < pFnDesc->cParams; j++)
        pFnDesc->apszParamDesc[j] = NULL;
    pFnDesc->cAuthors = 0;
    pFnDesc->pszDescription = pFnDesc->pszRemark = pFnDesc->pszReturnDesc = NULL;
    pFnDesc->pszSketch = pFnDesc->pszEquiv = pFnDesc->pszTime = pFnDesc->pszStatus = NULL;
    pFnDesc->lStatus = 99; /* unknown */

    /*
     * 1. + 2a.
     */
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
    while (iStart >= 0
           && (j < 0
               || !(fFound = (papszLines[iStart][j] == '/' && papszLines[iStart][j+1] == '*'))
               )
           )
        if (j-- <= 0)
            if (iStart-- > 0)
            {
                j = strlen(papszLines[iStart]);
                j -= j > 1 ? 2 : j;
            }

    if (!fFound) /* fail if not found */
        return 0;
    jStart = j;

    /*
     * 2b.
     */
    if (strncmp(&papszLines[iStart][jStart], "/**", 3) != 0) /* checks that there are more than one star at start of comment */
        return 0;

    /* Odin32 common? */
    iName   = findStrLine("* Name", iStart, iEnd, papszLines);
    iStatus = findStrLine("* Status", iStart, iEnd, papszLines);
    iAuthor = findStrLine("* Author", iStart, iEnd, papszLines);

    if (!(iName <= iEnd || iStatus <= iEnd || iAuthor <= iEnd))
    {
        /* SDS ? */
        iStatus = findStrLine("@status", iStart, iEnd, papszLines);
        iAuthor = findStrLine("@author", iStart, iEnd, papszLines);
        iName   = iEnd + 1;

        if (!(iStatus <= iEnd || iAuthor <= iEnd))
            return 0;
        fSDS = TRUE;
    }
    else
    {
        /* 2c.*/
        if (iName <= iEnd && strstr(papszLines[iName], pFnDesc->pszName) == NULL)
            logprintf((phLog, "Warning: a matching function name is not found in the name Field\n"));
    }

    /* 2d.*/
    pszB = &pFnDesc->szFnHdrBuffer[0];
    if (!fSDS)
    {
        /*
         * Odin32 common styled header
         */

        /* Author(s) */
        pszAuthors              = CommonCopyTextUntilNextTag(pszB, FALSE, iAuthor, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* Status */
        pFnDesc->pszStatus      = CommonCopyTextUntilNextTag(pszB, FALSE, iStatus, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* Remark */
        i = findStrLine("* Remark", iStart, iEnd, papszLines);
        pFnDesc->pszRemark      = CommonCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* Description */
        i = findStrLine("* Purpose", iStart, iEnd, papszLines);
        pFnDesc->pszDescription = CommonCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* Return(result) description */
        i = findStrLine("* Result", iStart, iEnd, papszLines);
        pFnDesc->pszReturnDesc  = CommonCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* FIXME parameters */

    }
    else
    {
        /*
         * SDS styled header
         */

        /* author */
        pszAuthors              = SDSCopyTextUntilNextTag(pszB, FALSE, iAuthor, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* status */
        pFnDesc->pszStatus      = SDSCopyTextUntilNextTag(pszB, FALSE, iStatus, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* remark */
        i = findStrLine("@remark", iStart, iEnd, papszLines);
        pFnDesc->pszRemark      = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* description */
        i = findStrLine("@desc",   iStart, iEnd, papszLines);
        pFnDesc->pszDescription = SDSCopyTextUntilNextTag(pszB, TRUE, i > iEnd ? iStart : i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* sketch */
        i = findStrLine("@sketch", iStart, iEnd, papszLines);
        pFnDesc->pszSketch      = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* return description */
        i = findStrLine("@return", iStart, iEnd, papszLines);
        pFnDesc->pszReturnDesc  = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* time */
        i = findStrLine("@time", iStart, iEnd, papszLines);
        pFnDesc->pszTime        = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* equiv */
        i = findStrLine("@equiv", iStart, iEnd, papszLines);
        pFnDesc->pszEquiv       = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
        pszB += strlen(pszB) + 1;

        /* Set parameter descriptions to NULL */
        for (i = 0; i < pFnDesc->cParams; i++)
            pFnDesc->apszParamDesc[i] = NULL;

        /*
         * parameters, new @param for each parameter!
         */
        i = iStart - 1;
        do
        {
            char *pszParam;

            /* find first */
            i = findStrLine("@param", i + 1, iEnd, papszLines);
            if (i >= iEnd)
                break;

            /* Get parameter name - first word */
            pszParam = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines);
            if (pszParam != NULL)
            {
                /* first word in psz is the parameter name! */
                psz = findEndOfWord(pszParam);
                *psz++ = '\0';

                /* find parameter */
                for (j = 0; j < pFnDesc->cParams; j++)
                    if (strcmp(pFnDesc->apszParamName[j], pszParam) == 0)
                        break;
                if (j < pFnDesc->cParams)
                {
                    /* find end of parameter name */
                    psz = findEndOfWord(strstr(papszLines[i], pszParam));
                    /* copy from end of parameter name */
                    pFnDesc->apszParamDesc[j] = SDSCopyTextUntilNextTag(pszB, TRUE, i, iEnd, papszLines, psz);
                    pszB += strlen(pszB) + 1;
                }
                else
                {   /* signal that parameter name wasn't found! */
                    fprintf(phSignal, "%s, %s: '%s' is not a valid parameter.\n",
                            pszFilename, pFnDesc->pszName, pszParam);
                    ulRc += 0x00010000;
                }
            }
        } while (i < iEnd);
    }

    /*
     * Status - refcodes are hardcoded here!
     */
    if (pFnDesc->pszStatus != NULL && *pFnDesc->pszStatus != '\0')
    {
        if (stristr(pFnDesc->pszStatus, "STUB") != NULL || *pFnDesc->pszStatus == '1')
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

    /*
     * Author
     */
    if (pszAuthors != NULL)
    {   /* author1, author2, author3 */
        char *pszBr1;
        char *pszBr2;

        /* remove '[text]' text - this is usualy used for time/date */
        psz = trim(pszAuthors);
        pszBr1 = strchr(psz, '[');
        pszBr2 = strchr(psz, ']');
        while (pszBr1 != NULL && pszBr2 != NULL && pszBr1 < pszBr2)
        {
            memset(pszBr1, ' ', pszBr2 - pszBr1 +1);
            pszBr1 = strchr(psz, '[');
            pszBr2 = strchr(psz, ']');
        }

        j = 0;
        psz = trim(pszAuthors);
        pFnDesc->cAuthors = 0;
        while (*psz != '\0' && pFnDesc->cAuthors < (int)(sizeof(pFnDesc->apszAuthor) / sizeof(pFnDesc->apszAuthor[0])))
        {
            char *pszEmail = NULL;
            char *pszNext;

            /* terminate string */
            pszNext = strchr(psz, '\n');
            if (pszNext == NULL)
                pszNext = strchr(psz, ',');
            if (pszNext != NULL)
                *pszNext = '\0';

            /* check for (email) test */
            pszBr1 = strchr(psz, '(');
            pszBr2 = strchr(psz, ')');
            if (pszBr1 != NULL || pszBr2 != NULL)
            {
                if (pszBr1 != NULL)
                    *pszBr1++ = '\0';
                if (pszBr2 != NULL)
                    *pszBr2++ = '\0';

                if (pszBr1 != NULL && pszBr1 < pszBr2)
                    pszEmail = trim(pszBr1 + 1);
            }

            pFnDesc->apszAuthor[pFnDesc->cAuthors]      = trim(psz);
            pFnDesc->alAuthorRefCode[pFnDesc->cAuthors] =
                dbFindAuthor(pFnDesc->apszAuthor[pFnDesc->cAuthors], pszEmail);

            if (pFnDesc->alAuthorRefCode[pFnDesc->cAuthors] == -1)
            {
                fprintf(phSignal, "%s, %s: author '%s' is not recognized.\n", pszFilename, pFnDesc->pszName,
                        pFnDesc->apszAuthor[pFnDesc->cAuthors]);
                ulRc += 0x00010000;
            }

            /* next */
            pFnDesc->cAuthors++;
            psz = pszNext ? skip(pszNext + 1) : "";
        }
    }

    return ulRc;
}


/**
 * Writes debug log data on a function.
 * @param   pFnDesc     Pointer to function description block.
 */
void logFunction(PFNDESC pFnDesc)
{
    #ifdef DEBUGLOG
    int j;

    fprintf(phLog, "Name:      '%s'  (refcodes=", pFnDesc->pszName);
    for (j = 0; j < pFnDesc->cRefCodes; j++)
        fprintf(phLog, j > 0 ? ", %ld" : "%ld", pFnDesc->alRefCode[j]);
    fprintf(phLog, ")\n");
    fprintf(phLog, "  Returns: '%s'\n", pFnDesc->pszReturnType != NULL ? pFnDesc->pszReturnType : "<missing>");
    fprintf(phLog, "  cParams: %2d\n", pFnDesc->cParams);
    for (j = 0; j < pFnDesc->cParams; j++)
        fprintf(phLog, "  Param %2d: type '%s' %*s name '%s' description: %s\n", j, pFnDesc->apszParamType[j],
                max((int)(15 - strlen(pFnDesc->apszParamType[j])), 0), "", pFnDesc->apszParamName[j],
                pFnDesc->apszParamDesc[j] != NULL ?  pFnDesc->apszParamDesc[j] : "(null)");
    fprintf(phLog, "  Status:   %ld - '%s'\n", pFnDesc->lStatus, pFnDesc->pszStatus != NULL ? pFnDesc->pszStatus : "<missing>");
    fprintf(phLog, "  cAuthors: %2d\n", pFnDesc->cAuthors);
    for (j = 0; j < pFnDesc->cAuthors; j++)
        fprintf(phLog, "  Author %d: '%s'  (refcode=%ld)\n", j, pFnDesc->apszAuthor[j], pFnDesc->alAuthorRefCode[j]);

    fprintf(phLog, "  Description: %s\n", pFnDesc->pszDescription != NULL ? pFnDesc->pszDescription : "(null)");
    fprintf(phLog, "  Remark:      %s\n", pFnDesc->pszRemark != NULL ? pFnDesc->pszRemark : "(null)");
    fprintf(phLog, "  Return Desc: %s\n", pFnDesc->pszReturnDesc != NULL ? pFnDesc->pszReturnDesc : "(null)");
    fprintf(phLog, "  Sketch:      %s\n", pFnDesc->pszSketch != NULL ? pFnDesc->pszSketch : "(null)");
    fprintf(phLog, "  Equiv:       %s\n", pFnDesc->pszEquiv != NULL ? pFnDesc->pszEquiv : "(null)");
    fprintf(phLog, "  Time:        %s\n", pFnDesc->pszTime != NULL ? pFnDesc->pszTime : "(null)");
    fprintf(phLog, "------------\n");
    #else
    pFnDesc = pFnDesc;
    #endif
}


/**
 * Resolves the source filename of an .obj file.
 * @returns Pointer to static filename buffer.
 * @param   pszFileObj  Name of object file.
 * @param   papszDirs   Array of directories to search. NULL terminated.
 * @remark  The string returned is in _static_ memory.
 */
char *ResolvSourceFile(char *pszFileObj, char **papszDirs)
{
    static char     szFile[CCHMAXPATH];
    static char *   aszSuffixes[] = { ".cpp", ".c", ".orc", NULL };
    char *  aszDirs[] = { ".;", NULL };
    char *  pszDir;
    int     i;
    char    szFileTmp[CCHMAXPATH];

    /*
     * Extract file name.
     */
    fileNameNoExt(pszFileObj, szFileTmp);


    /*
     * Loop dirs.
     */
    for (i = 0; papszDirs[i]; i++)
    {
        int j;

        for (j = 0; aszSuffixes[j]; j++)
        {
            APIRET       rc;
            FILESTATUS3  fs;

            strcat(strcat(strcat(strcpy(szFile, papszDirs[i]), "\\"), szFileTmp), aszSuffixes[j]);
            rc = DosQueryPathInfo(szFile, FIL_STANDARD, &fs , sizeof(fs));
            if (rc == NO_ERROR && (fs.attrFile & FILE_DIRECTORY) != FILE_DIRECTORY)
            {
                logprintf((phLog, "ResolveSourceFile(%s,..) -> %s\n", pszFileObj, szFile));
                return szFile;
            }
        }
    }

    logprintf((phLog, "ResolveSourceFile(%s,..) -> NULL\n", pszFileObj));
    return NULL;
}


/**
 * Copies a piece of tag/keyword text into an buffer. SDS.
 * @returns   Pointer to buffer. If iStart > iEnd NULL is returned.
 * @param     pszTarget  Pointer to buffer.
 * @param     fHTML      Add HTML tags like <br> or not
 * @param     iStart     Index of start line.
 * @param     iEnd       Index of last line.
 * @param     apszLines  Array lines.
 * @param     pszStart   Pointer to char to start at (into papszLines[iStart] of course!). Defaults to start of line iStart.
 * @status    completely impelmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Addes some HTML tags.
 */
static char *SDSCopyTextUntilNextTag(char *pszTarget, BOOL fHTML, int iStart, int iEnd, char **papszLines, const char *pszStart)
{
    char *pszRet = pszTarget;

    if (iStart <= iEnd)
    {
        int         iStartColumn;
        const char *psz = pszStart != NULL ? pszStart : papszLines[iStart];

        /*
         * skip dummy chars.
         */
        while (iStart <= iEnd && (*psz == ' ' || *psz == '/' || *psz == '*' || *psz == '\0' ))
        {
            if (*psz == '\0')
                psz = papszLines[++iStart];
            else
                psz++;
        }

        /* Anything left of the area to copy? */
        if (iStart <= iEnd && (!pszStart || *psz != '@')) /* last test is fix for parameters without description. */
        {
            /*
             * if we stand at a tag, skip over the tag
             */
            if (*psz == '@')
                psz = skip(findEndOfWord(psz+1));

            /*
             * save start columen
             */
            iStartColumn = psz - papszLines[iStart];

            /*
             * Copy loop.
             */
            int  cBlanks = 0;
            do
            {
                int i;

                /*
                 * new paragraph?.
                 */
                if (fHTML && cBlanks == 1)
                {
                    strcpy(pszTarget, "<p>\n");
                    pszTarget += 4;
                }

                /*
                 * indent...?
                 */
                for (i = psz - papszLines[iStart]; i < iStartColumn; i++)
                    *pszTarget++ = ' '; /* FIXME HTML and indenting... */

                strcpy(pszTarget, psz);
                trimR(pszTarget);
                if (*pszTarget == '\0')
                    cBlanks++;
                else
                    cBlanks = 0;
                pszTarget += strlen(pszTarget);
                *pszTarget++ = '\n';
                *pszTarget = '\0';

                /* Next */
                psz = skip(papszLines[++iStart]);
                if (iStart <= iEnd)
                {
                    while (psz != NULL && *psz == '*')
                        ++psz;
                    psz = skip(psz);
                    /* end test comment */
                    if (psz > papszLines[iStart] && psz[-1] == '*' && *psz == '/')
                        break;
                }
            } while (iStart <= iEnd && *psz != '@');

            /*
             * remove empty lines at end.
             */
            if (fHTML)
            {
                pszTarget--;
                while ((pszTarget >= pszRet && (*pszTarget == '\n' || *pszTarget == ' '))
                       || (pszTarget - 3 >= pszRet && strnicmp(pszTarget - 3, "<p>", 3) == 0)
                       || (pszTarget - 4 >= pszRet && strnicmp(pszTarget - 4, "<br>",4) == 0)
                       )
                {
                    if (*pszTarget != '\n' && *pszTarget != ' ')
                        pszTarget -= pszTarget[3] == '<' ? 3 : 4;
                    *pszTarget-- = '\0';
                }
            }
            else
            {
                pszTarget--;
                while (pszTarget >= pszRet && (*pszTarget == '\n' || *pszTarget == ' '))
                    *pszTarget-- = '\0';
            }
        }
        else
            pszTarget = '\0';
    }
    else
        pszRet = NULL;

    return pszRet != NULL && *pszRet == '\0' ?  NULL : pszRet;
}



/**
 * Copies a piece of tag/keyword text into an buffer. SDS.
 * @returns   Pointer to buffer. If iStart > iEnd NULL is returned.
 * @param     pszTarget  Pointer to buffer.
 * @param     fHTML      Add HTML tags like <br> or not
 * @param     iStart     Index of start line.
 * @param     iEnd       Index of last line.
 * @param     apszLines  Array lines.
 * @param     pszStart   Pointer to char to start at (into papszLines[iStart] of course!)
 * @status    completely impelmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Addes some HTML tags.
 */
static char *CommonCopyTextUntilNextTag(char *pszTarget, BOOL fHTML, int iStart, int iEnd, char **papszLines, const char *pszStart)
{
    char *pszRet = pszTarget;

    if (iStart <= iEnd)
    {
        /* known keywords */
        int         iStartColumn;
        int         i;
        const char *psz = pszStart != NULL ? pszStart : papszLines[iStart];

        /*
         * Skip evt. keyword
         */
        psz = skip(psz);
        for (i = 0; pszCommonKeywords[i] != NULL; i++)
            if (strnicmp(pszCommonKeywords[i], psz, strlen(pszCommonKeywords[i])) == 0)
            {
                psz = skip(psz + strlen(pszCommonKeywords[i]));
                psz = skip(*psz == ':' ? psz + 1 : psz);
                break;
            }

        /*
         * save start columen
         */
        iStartColumn = psz - papszLines[iStart];

        /*
         * copy loop
         */
        int cBlanks = 0;
        do
        {
            /*
             * Skip '*'s at start of the line.
             */
            while (*psz == '*')
                psz++;

            /* end comment check */
            if (psz > papszLines[iStart] && psz[-1] == '*' && *psz == '/')
                break;
            psz = skip(psz);

            /*
             * new paragraph?.
             */
            if (fHTML && cBlanks == 1)
            {
                strcpy(pszTarget, "<p>\n");
                pszTarget += 4;
            }

            /*
             * Indent up to iStartColumn
             */
            for (i = psz - papszLines[iStart]; i < iStartColumn; i++)
                *pszTarget++ = ' '; /* FIXME HTML and indenting... */

            /*
             * Copy the rest of the line and add HTML break.
             */
            strcpy(pszTarget, psz);
            trimR(pszTarget);
            if (*pszTarget == '\0')
                cBlanks++;
            else
                cBlanks = 0;
            pszTarget += strlen(pszTarget);
            *pszTarget++ = '\n';
            *pszTarget = '\0';

            /*
             * Next
             */
            psz = skip(papszLines[++iStart]);

            /*
             * Check for keyword
             */
            if (iStart <= iEnd)
            {
                psz = skip(psz);
                for (i = 0; pszCommonKeywords[i] != NULL; i++)
                    if (strnicmp(pszCommonKeywords[i], psz, strlen(pszCommonKeywords[i])) == 0)
                        break;
                if (pszCommonKeywords[i] != NULL)
                    break;
            }
        } while (iStart < iEnd);

        /*
         * remove empty lines at end.
         */
        if (fHTML)
        {
            pszTarget--;
            while ((pszTarget >= pszRet && (*pszTarget == '\n' || *pszTarget == ' '))
                   || (pszTarget - 3 >= pszRet && strnicmp(pszTarget - 3, "<p>", 3) == 0)
                   || (pszTarget - 4 >= pszRet && strnicmp(pszTarget - 4, "<br>",4) == 0)
                   )
            {
                if (*pszTarget != '\n' && *pszTarget != ' ')
                    pszTarget -= pszTarget[3] == '<' ? 3 : 4;
                *pszTarget-- = '\0';
            }
        }
        else
        {
            pszTarget--;
            while (pszTarget >= pszRet && (*pszTarget == '\n' || *pszTarget == ' '))
                *pszTarget-- = '\0';
        }
    }
    else
        pszRet = NULL;

    return pszRet != NULL && *pszRet == '\0' ?  NULL : pszRet;
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
            pszP1++;
        if (*pszP1 != '\0' && *pszP1 != '/' && pszP1[1] != '/'
            && *pszP1 != '{' && *pszP1 != '}')
        {
            pszP1 = strchr(papszLines[i], '(');
            if (pszP1 != NULL && pszP1 >= papszLines[i])
            {
                int  cchFnName  = 0;
                char *pszFnName = pszP1 - 1;

                while (pszFnName - cchFnName > papszLines[0] && pszFnName[cchFnName] == ' ')
                    cchFnName--, pszFnName--;

                pszFnName = findStartOfWord(pszFnName, papszLines[0]);
                cchFnName += pszP1 - pszFnName;
                if (cchFnName >= 0)
                {
                    /* 1. */
                    if (
                        strncmp(pszFnName, "for", cchFnName) != 0   &&
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
                                    logprintf((phLog, "Function found: %.*s\n", cchFnName, pszFnName));
                                    return TRUE;
                                }
                                /* FIXME: constructors with ':' afterwards are not supported !TODO! */
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
                            logprintf((phLog, "Possible API: %.*s\n", cchFnName, pszOS2));
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
 * Checks if there may be a design note starting at the current line.
 * @returns   TRUE if design note found, else FALSE.
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     pOptions     Pointer to options.
 */
static BOOL isDesignNote(char **papszLines, int i, POPTIONS pOptions)
{
    char *psz = papszLines[i];

    if (psz == NULL)
        return FALSE;

    // look for /**@design
    while (*psz == ' ')
        psz++;
    if (strncmp(psz, "/**", 3) == 0)
    {
        psz++;
        while (*psz == '*' || *psz == ' ')
            psz++;
        return strnicmp(psz, "@design", 7) == 0 && (psz[7] == '\0' || psz[7] == ' ');
    }
    pOptions = pOptions;
    return FALSE;
}


/**
 * Checks if there is a class declaration starting at the current line.
 * @returns   TRUE if design note found, else FALSE.
 * @param     papszLines   Array of lines in the file.
 * @param     i            Index into papszLines.
 * @param     pOptions     Pointer to options.
 */
BOOL isClass(char **papszLines, int i, POPTIONS pOptions)
{
    char *psz = papszLines[i];

    if (psz == NULL)
        return FALSE;

    // look for class
    while (*psz == ' ')
        psz++;

    pOptions = pOptions;
    return !strncmp(psz, "class", 5)
           && (psz[5] == '\t' || psz[5] == ' ' || psz[5] == '\n' || psz[5] == '\r' || psz[5] == '\0');
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
            logprintf((phLog, "%s", pszValue));
            break;
        case 1:
            logprintf((phLog, "(%s)", pszValue));
            break;
        case 2: /* updated */
            logprintf((phLog, " %s=%s", pszFieldName, pszValue));
            break;
        case 3: /* aliasfn */
            logprintf((phLog, " %s=%s", pszFieldName, pszValue));
            break;
        case 4:
            if (pszValue != NULL)
                logprintf((phLog, " --> %s.", pszValue));
            break;
        case 5:
            if (pszValue != NULL)
                logprintf((phLog, "%s", pszValue));
            break;
        case 6:
            if (pszValue != NULL)
                logprintf((phLog, "(%s)", pszValue));
            break;

        default:
            i = 0;
            logprintf((phLog, "\n"));
    }

    if (stricmp(pszFieldName, "last") == 0)
    {
        i = 0;
        logprintf((phLog, "\n"));
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
 * It also replaces '\t' with ' '.
 * @returns   Pointer to the array of lines.
 * @param     pszFile  Pointer to "memory" file.
 */
static char **createLineArray(char *pszFile)
{
    char *psz = pszFile;
    char **papszLines = NULL;
    int  cLines = 1;

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
    logprintf((phLog, "%d lines\n", cLines));

    papszLines = (char**)calloc(cLines + 1, sizeof(char *));
    if (papszLines != NULL)
    {
        cLines = 1;
        papszLines[0] = psz = pszFile;
        while (*psz != '\0')
        {
            if (*psz == '\t')
                *psz = ' ';
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
static char *findEndOfWord(const char *psz)
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
    return (char *)psz;
}


/** starting char of word */
static char *findStartOfWord(const char *psz, const char *pszStart)
{
    const char *pszR = psz;
    while (psz >= pszStart &&
            (
                 (*psz >= 'A' && *psz <= 'Z')
              || (*psz >= 'a' && *psz <= 'z')
              || (*psz >= '0' && *psz <= '9')
              || *psz == '_'
             )
          )
        pszR = psz--;
    return (char*)pszR;
}


/**
 * Trims a string, ie. removing spaces (and tabs) from both ends of the string.
 * @returns   Pointer to first not space or tab char in the string.
 * @param     psz   Pointer to the string which is to be trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
inline char *trim(char *psz)
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


/**
 * Right trims a string, ie. removing spaces (and tabs) from the end of the string.
 * @returns   Pointer to the string passed in.
 * @param     psz   Pointer to the string which is to be right trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
inline char *trimR(char *psz)
{
    int i;
    if (psz == NULL)
        return NULL;
    i = strlen(psz) - 1;
    while (i >= 0 && (psz[i] == ' ' || *psz == '\t'))
        i--;
    psz[i+1] = '\0';
    return psz;
}

/**
 * Trims string. <BR>, <P>, '@', '\t', '\n' and '\t' is trimmed from both ends of the string.
 * @returns Pointer to string.
 * @param   psz     String to trim.
 */
char *trimHtml(char *psz)
{
    if (!psz)
        return NULL;

    char *pszEnd = psz + strlen(psz) - 1;

    while (pszEnd > psz)
    {
        if (*pszEnd == '@' || *pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\n' || *pszEnd == '\r')
            pszEnd--;
        else if (!strnicmp(pszEnd - 3, "<BR>", 4))
            pszEnd -= 4;
        else if (!strnicmp(pszEnd - 2, "<P>", 3))
            pszEnd -= 3;
        else
            break;
        pszEnd[1] = '\0';
    }

    while (psz < pszEnd)
    {
        if (*psz == '@' || *psz == ' ' || *psz == '\t' || *psz == '\n' || *psz == '\r')
            psz++;
        else if (!strnicmp(psz, "<BR>", 4))
            psz += 4;
        else if (!strnicmp(psz, "<P>", 3))
            psz += 3;
        else
            break;
    }

    return psz;
}


/**
 * skips blanks until first non-blank.
 * @returns   Pointer to first not space or tab char in the string.
 * @param     psz   Pointer to the string.
 * @status    completely implmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
inline char *skip(const char *psz)
{
    if (psz == NULL)
        return NULL;

    while (*psz == ' ' || *psz == '\t')
        psz++;
    return (char*)psz;
}


/* copy: remove remarks, and unneeded spaces, ensuring no space after '(',
 *       ensuring space after '*', ensuring no space before ',' and ')'.
 */
static void copy(char *psz, char *pszFrom, int iFrom, char *pszTo, int iTo, char **papszLines)
{
    copy(psz, pszFrom - papszLines[iFrom], iFrom, pszTo - papszLines[iTo], iTo, papszLines);
}

#if 0
static void copy(char *psz, int jFrom, int iFrom, int jTo, int iTo, char **papszLines)
{
    char chPrev = '\n';
    int i, j;
    int fComment = 0;

    i = iFrom;
    j = jFrom;
    while (i < iTo || (i == iTo && j <= jTo))
    {
        if (papszLines[i][j] != '\0'
            && !(papszLines[i][j] == '/' && papszLines[i][j+1] == '/')  /* '//' coments */
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

#else
/**
 * Copies a set of lines to a buffer (psz) removing precompiler lines and all comments.
 * @param     psz
 * @param     jFrom       Starting position, index into line iFrom.
 * @param     iFrom       Starting position, index into papszLines.
 * @param     jTo         Ending position, index into line iFrom.
 * @param     iTo         Ending position, index into papszLines.
 * @param     papszLines  Array of lines.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static void copy(char *psz, int jFrom, int iFrom, int jTo, int iTo, char **papszLines)
{
    char chPrev = '\n';
    int i, j;
    int fComment = FALSE;
    int fFirst = TRUE;

    i = iFrom;
    j = jFrom;
    while (i < iTo || (i == iTo && j <= jTo))
    {
        if (papszLines[i][j] != '\0'
            && !(papszLines[i][j] == '/' && papszLines[i][j+1] == '/')  /* '//' coments */
            && !(!fComment && fFirst && papszLines[i][j] == '#')
            )
        {
            fFirst = papszLines[i][j] == ' ';

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
                if ((fComment = papszLines[i][j] != '*' || papszLines[i][j+1] != '/') == FALSE)
                    j++;
            j++;
        }
        else
        {
            /* next */
            j = 0;
            fFirst = TRUE;
            i++;
            if (chPrev != ' ' && chPrev != '(')
                chPrev = *psz++ = ' ';
        }
    }
    *psz = '\0';
}
#endif


/* copyComment: Wrapper for the other copyComment function.
 *
 */
static void copyComment(char *psz, char *pszFrom, int iFrom, char **papszLines, BOOL fStrip, BOOL fHTML)
{
    copyComment(psz, pszFrom - papszLines[iFrom], iFrom, papszLines, fStrip, fHTML);
}




/**
 * Copies a set of lines to a buffer (psz) stopping at the first end comment.
 * (If a start comment occurs it is concidered an error.)
 * Stars begining lines are removed.
 * @param       psz         Target buffer.
 * @param       jFrom       Starting position, index into line iFrom.
 * @param       iFrom       Starting position, index into papszLines.
 * @param       papszLines  Array of lines.
 * @param       fStrip      Strip blank lines at start and end + LICENCE notice (at end).
 * @param       fHTML       Convert to HTML while copying.
 * @status      completely implemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
static void copyComment(char *psz, int jFrom, int iFrom, char **papszLines, BOOL fStrip, BOOL fHTML)
{
    char *  pszStartBuffer = psz;       /* Start of the target buffer. */
    char *  pszStart = psz;             /* Start of current line. */
    int     fFirst = TRUE;              /* True while we're still processing the start of the line. */
    int     i, j;                       /* Indexes into papszLines. */

    i = iFrom;
    j = jFrom;
    while (papszLines[i] != NULL && !(papszLines[i][j] == '*' && papszLines[i][j+1] == '/'))
    {
        if (papszLines[i][j] != '\0')
        {
            /* Skip or copy the char ? */
            if (fFirst && papszLines[i][j] == ' ')
                j++;
            else if (fFirst && papszLines[i][j] == '*')
                j += papszLines[i][j+1] == ' ' ? (fFirst = FALSE) + 2 : 1; /* bad habbit...*/
            else
            {   /* Copy it */
                *psz++ = papszLines[i][j++];
                fFirst = FALSE;
            }
        }
        else
        {   /* End of line:
             *  Check if empty line. If so truncate it.
             *  Add new line char if not empty first line...
             */
            j = 0; /* use this as an index from line start on the copied line. */
            while (pszStart + j < psz && pszStart[j] == ' ')
                j++;
            if (pszStart + j == psz)
                psz = pszStart;
            if (psz > pszStartBuffer || !fStrip)
            {
                if (fHTML)
                {
                    *psz++ = '<';
                    *psz++ = 'B';
                    *psz++ = 'R';
                    *psz++ = '>';
                }
                *psz++ = '\n';
            }

            /* next */
            i++;
            j = 0;
            fFirst = TRUE;
            pszStart = psz;
        }
    }
    *psz = '\0';

    /*
     * Strip end + license.
     */
    if (fStrip)
    {
        if (fHTML)
        {
            while (psz >= pszStartBuffer)
            {
                if (*psz == ' ' || *psz == '\n' || *psz == '\0')
                    *psz-- = '\0';
                else if (psz - 4 >= pszStartBuffer && strncmp(psz - 4, "<BR>", 4) == 0)
                    *(psz -= 4) = '\0';
                else
                    break;
            }
        }
        while (psz >= pszStartBuffer && (*psz == ' ' || *psz == '\n' || *psz == '\0'))
            *psz-- = '\0';


        if (psz - 20 > pszStartBuffer && strstr(psz - 20, "LICENSE.TXT") != NULL)
        {
            while (psz >= pszStartBuffer && *psz != '\n')
                *psz-- = '\0';
        }

        if (fHTML)
        {
            while (psz >= pszStartBuffer)
            {
                if (*psz == ' ' || *psz == '\n' || *psz == '\0')
                    *psz-- = '\0';
                else if (psz - 4 >= pszStartBuffer && strncmp(psz - 4, "<BR>", 4) == 0)
                    *(psz -= 4) = '\0';
                else
                    break;
            }
        }
        while (psz >= pszStartBuffer && (*psz == ' ' || *psz == '\n' || *psz == '\0'))
            *psz-- = '\0';
    }
}


/**
 * Upcases a char.
 * @returns   Upper case of the char given in ch.
 * @param     ch  Char to capitalize.
 */
#if 0
inline char upcase(char ch)
{
    return ch >= 'a' && ch <= 'z' ?  (char)(ch - ('a' - 'A')) : ch;
}
#else
#define  upcase(ch) ((ch) >= 'a' && (ch) <= 'z' ?  (char)((ch) - ('a' - 'A')) : (ch))
#endif


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



/**
 * Skips backwards until one of the chars in pszStopAt or star of file is reached.
 * @returns   Pointer to end.
 * @param     pszStopAt   Array of chars which we should stop at.
 * @param     pszFrom     Start pointer.
 * @param     iLine       Reference to index to array of lines. input: start line, output: result line.
 * @param     papszLines  Array lines.
 * @sketch
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Comments are skipped.
 *            No tests for strings ("...asdf").
 *            TODO: Multiline preprocessor directives....
 */
static char *skipBackwards(const char *pszStopAt, const char *pszFrom, int &iLine, char **papszLines)
{
    BOOL        fComment = FALSE;
    int         i = iLine;
    const char *psz = pszFrom;

    while (i >= 0)
    {
        /* check for stop char */
        const char *psz1 = pszStopAt;
        while (*psz1 != '\0')
            if (*psz1++ == *psz)
                break;
        if (*psz1 != '\0')
            break;

        /* comment check */
        if (!fComment && psz > papszLines[i] && *psz == '/' && psz[-1] == '*')
            fComment = TRUE;
        else if (fComment && *psz == '/' && psz[1] == '*')
            fComment = FALSE;

        /* ok position to return? */
        if (!fComment)
        {
            iLine = i;
            pszFrom = psz;
        }

        /* prev */
        if (psz > papszLines[i])
            psz--;
        else
        {   /* try find line comment */
            do
            {
                char *pszStart = papszLines[--i];
                while (*pszStart == ' ')
                    pszStart++;

                /* stop at preprocessor stuff */
                if (*pszStart == '#')
                    return (char*)pszFrom;

                if (*pszStart != '\0' && !(*pszStart == '/' && pszStart[1] == '/'))
                {   /* find '//' */
                    pszStart = strstr(pszStart, "//");
                    if (pszStart != NULL)
                        psz = pszStart-1;
                    else
                        psz = papszLines[i] + strlen(papszLines[i]) - 1;
                    break;
                }
            } while (i > 0);
        }
    }

    return (char*)pszFrom;
}


/**
 * Finds a string in a range of pages.
 * @returns   Index of the line (into ppaszLines).
 * @param     psz         String to search for.
 * @param     iStart      Start line.
 * @param     iEnd        Line to stop at
 * @param     papszLines  Array of lines to search within.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static int findStrLine(const char *psz, int iStart, int iEnd, char **papszLines)
{
    while (iStart <= iEnd &&
           stristr(papszLines[iStart], psz) == NULL)
        iStart++;
    return iStart;
}


/**
 * Creates a memory buffer for a text file.
 * @returns   Pointer to file memoryblock. NULL on error.
 * @param     pszFilename  Pointer to filename string.
 * @remark    This function is the one using most of the execution
 *            time (DosRead + DosOpen) - about 70% of the execution time!
 */
void *textbufferCreate(const char *pszFilename)
{
    void *pvFile = NULL;
    FILE *phFile;

    phFile = fopen(pszFilename, "rb");
    if (phFile != NULL)
    {
        signed long cbFile = fsize(phFile);
        if (cbFile >= 0)
        {
            pvFile = malloc(cbFile + 1);
            if (pvFile != NULL)
            {
                memset(pvFile, 0, cbFile + 1);
                if (cbFile > 0 && fread(pvFile, 1, cbFile, phFile) == 0)
                {   /* failed! */
                    free(pvFile);
                    pvFile = NULL;
                }
            }
            else
                fprintf(stderr, "warning/error: failed to open file %s\n", pszFilename);
        }
        fclose(phFile);
    }
    return pvFile;
}


/**
 * Destroys a text textbuffer.
 * @param     pvBuffer   Buffer handle.
 */
void textbufferDestroy(void *pvBuffer)
{
    free(pvBuffer);
}


/**
 * Gets the next line from an textbuffer.
 * @returns   Pointer to the next line.
 * @param     pvBuffer  Buffer handle.
 * @param     psz       Pointer to current line.
 *                      NULL is passed in to get the first line.
 */
char *textbufferNextLine(void *pvBuffer, register char *psz)
{
    register char ch;

    /* if first line psz is NULL. */
    if (psz == NULL)
        return (char*)pvBuffer;

    /* skip till end of file or end of line. */
    ch = *psz;
    while (ch != '\0' && ch != '\n' && ch != '\r')
        ch = *++psz;

    /* skip line end */
    if (ch == '\r')
        ch = *++psz;
    if (ch == '\n')
        psz++;

    return psz;
}


/**
 * Gets the next line from an textbuffer.
 * (fgets for textbuffer)
 * @returns   Pointer to pszOutBuffer. NULL when end of file.
 * @param     pvBuffer  Buffer handle.
 * @param     ppv       Pointer to a buffer index pointer. (holds the current buffer index)
 *                      Pointer to a null pointer is passed in to get the first line.
 * @param     pszLineBuffer  Output line buffer. (!= NULL)
 * @param     cchLineBuffer  Size of the output line buffer. (> 0)
 * @remark    '\n' and '\r' are removed!
 */
char *textbufferGetNextLine(void *pvBuffer, void **ppv, char *pszLineBuffer, int cchLineBuffer)
{
    char *          pszLine = pszLineBuffer;
    char *          psz = *(char**)(void*)ppv;
    register char   ch;

    /* first line? */
    if (psz == NULL)
        psz = (char*)pvBuffer;

    /* Copy to end of the line or end of the linebuffer. */
    ch = *psz;
    cchLineBuffer--; /* reserve space for '\0' */
    while (cchLineBuffer > 0 && ch != '\0' && ch != '\n' && ch != '\r')
    {
        *pszLine++ = ch;
        ch = *++psz;
    }
    *pszLine = '\0';

    /* skip line end */
    if (ch == '\r')
        ch = *++psz;
    if (ch == '\n')
        psz++;

    /* check if position has changed - if unchanged it's the end of file! */
    if (*ppv == (void*)psz)
        pszLineBuffer = NULL;

    /* store current position */
    *ppv = (void*)psz;

    return pszLineBuffer;
}


/**
 * Find the size of a file.
 * @returns   Size of file. -1 on error.
 * @param     phFile  File handle.
 */
signed long fsize(FILE *phFile)
{
    int ipos;
    signed long cb;

    if ((ipos = ftell(phFile)) < 0
        ||
        fseek(phFile, 0, SEEK_END) != 0
        ||
        (cb = ftell(phFile)) < 0
        ||
        fseek(phFile, ipos, SEEK_SET) != 0
        )
        cb = -1;
    return cb;
}


/**
 * Copies the name part with out extention into pszBuffer and returns
 * a pointer to the buffer.
 * If no name is found "" is returned.
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *fileNameNoExt(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '\\');
    if (psz == NULL)
        psz = strrchr(pszFilename, '/');

    strcpy(pszBuffer, psz == NULL ? pszFilename : psz + 1);

    psz = strrchr(pszBuffer, '.');
    if (psz > pszBuffer) /* an extetion on it's own (.depend) is a filename not an extetion! */
        *psz = '\0';

    return pszBuffer;
}

