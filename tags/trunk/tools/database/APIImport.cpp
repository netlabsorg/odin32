/* $Id: APIImport.cpp,v 1.2 2000-02-11 18:35:53 bird Exp $ */
/*
 *
 * APIImport - imports a DLL or Dll-.def with functions into the Odin32 database.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "APIImport.h"
#include "kFileFormatBase.h"
#include "kFilePE.h"
#include "kFileDef.h"
#include "db.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static FILE  *phLog = NULL;

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static void openLog(void);
static void closeLog(void);
static long processFile(const char *pszFilename, const POPTIONS pOptions, long &cFunctions);
static void demangle(const char *pszMangledName, char *pszDemangled);


/**
 * Main function.
 * @returns   low word : number of errors
 *            high word: flags.
 * @param     argc  Argument count.
 * @param     argv  Argument array.
 */
int main(int argc, char **argv)
{
    int     argi;
    BOOL    fFatal = FALSE;
    long    lRc = 0;
    OPTIONS options = {0};
    char   *pszHost     = "localhost";
    char   *pszDatabase = "Odin32";
    char   *pszUser     = "root";
    char   *pszPasswd   = "";
    long    cFunctions  = 0;


    /**************************************************************************
    * parse arguments.
    * options:  -h or -?     help
    *           -o[+|-]      ordinals, o- ignore them, o+ import them (def).
    *           -d:<dbname>   Database name
    *           -p:<passwd>   Password
    *           -u:<user>     Userid
    *           -h:<host>     Hostname/IP-address
    **************************************************************************/
    if (argc == 1)
        syntax();
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

                case 'o': /* ordinals*/
                case 'O':
                    options.fIgnoreOrdinals = argv[argi][2] != '-';
                    break;

                case 'p':
                case 'P':
                    if (argv[argi][2] == ':')
                        pszPasswd = &argv[argi][3];
                    else
                        fprintf(stderr, "warning: option '-p:' requires password.\n");
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
        {
            long l;
            if (phLog == NULL)
                openLog();
            /* try connect to db */
            if (dbConnect(pszHost, pszUser, pszPasswd, pszDatabase))
            {
                l = processFile(argv[argi], &options, cFunctions);
                lRc = ((lRc & 0xffff0000UL) | (l & 0xffff0000UL)) | ((lRc & 0x0000ffffUL) + l & 0x0000ffffUL);
                dbDisconnect();
            }
            else
            {
                fprintf(phLog, "Could not connect to database (Odin32). \n\terror description: %s\n", dbGetLastErrorDesc());
                l = 0x00010001;
                lRc = ((lRc & 0xffff0000UL) | (l & 0xffff0000UL)) | ((lRc & 0x0000ffffUL) + l & 0x0000ffffUL);
                break;
            }
        }
        argi++;
    }

    /* write function count */
    if (phLog != NULL)
        fprintf(phLog, "\n %d functions were imported!\n\n", cFunctions);

    /* close the log */
    closeLog();

    /* warn if error during processing. */
    if (!fFatal && lRc > 0)
        fprintf(stderr, "%s did occur during the processing. Please check the log file. lRc=0x%08lx\n",
                (0x0000FFFFL & lRc) > 1 ? "Errors" : "An error",  lRc);
    return (int)lRc;
}



/**
 * Display syntax/help .
 */
static void syntax()
{
    printf("\n"
           "APIImport v%01d.%02d - Odin32 API Database utility\n"
           "----------------------------------------------\n"
           "syntax: APIImport.exe  [-h|-?] [options] <dll/defnames>\n"
           "\n"
           "    -h or -?      Syntax help. (this)\n"
           "    -o[+|-]       Ordinals, '+' imports (default), '-' ignores.\n"
           "    -h:<hostname> Database server hostname.     default: localhost\n"
           "    -u:<username> Username on the server.       default: root\n"
           "    -p:<password> Password.                     default: <empty>\n"
           "    -d:<database> Database to use.              default: Odin32\n"
           "    dll/defnames  List of DLL/Defs to process.\n"
           "\n"
           "Note. Options may be switched on and off between dlls.\n"
           "\n"
           "\n"
           "This utility will insert or update the 'dll' and 'function' tables in the\n"
           "database. It read a PE-DLL or a OS/2 Definition file (.Def). From the file\n"
           "it imports the:\n"
           "\t1) module name\n"
           "\t2) function name\n"
           "\t3) function ordinal (If not option 'o-' is specified.)\n"
           "This utililty is not destructive but additive. That is, it would not remove any\n"
           "rows in the database, only add or update them. On existing functions the\n"
           "ordinal value will be updated when 'o+' is specified.\n"
           "\n"
           "Copyright (c) 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)",
           MAJOR_VER, MINOR_VER
           );
}


/**
 * Opens the log file.
 * @remark    If open fails stderr is used.
 */
static void openLog(void)
{
    if (phLog == NULL)
    {
        phLog = fopen("APIImport.Log", "w");
        if (phLog == NULL)
        {
            fprintf(stderr,"error occured while opening log file - will use stderr instead.\n");
            phLog = stderr;
        }
    }
}


/**
 * Closes log file.
 */
static void closeLog(void)
{
    if (phLog != stderr && phLog != NULL)
        fclose(phLog);
}


/**
 * Processes a file using the given options. Transparently processes .Def's and PE DLLs
 * @returns   low word, number of errors.
 *            high word, flags (currently not flags are defined/used).
 * @param     pszFilename  Pointer to the filename of the file which is to be processed.
 * @param     pOptions     Pointer to the options-struct.
 * @param     cFunctions   Function count which is to be updated when functions are imported.
 */
static long processFile(const char *pszFilename, const POPTIONS pOptions, long &cFunctions)
{
    kFileFormatBase  *pFile;
    FILE             *phFile;
    long              lRc = 1;

    /* try open file */
    phFile = fopen(pszFilename, "rb");
    if (phFile != NULL)
    {
        try
        {
            char    achDataBuffer[0x200];
            char   *pszModuleName = &achDataBuffer[0];
            signed short  sDll;

            /* try create file objects */
            try
            {
                pFile = new kFilePE(phFile);
            }
            catch (int i)
            {
                i = i;
                pFile = new kFileDef(phFile);
            }

            if (pFile->queryModuleName(pszModuleName))
            {
                int cch = strlen(pszModuleName);

                /* remove '.dll' */
                if (cch > 4 && !stricmp(&pszModuleName[cch-4], ".dll"))
                    pszModuleName[cch-4] = '\0';
                fprintf(phLog, "%s: modulename = %s\n", pszFilename, pszModuleName);

                /* find or insert module name */
                if ((sDll = dbCheckInsertDll(pszModuleName)) >= 0)
                {
                    BOOL   fOk;
                    EXPORTENTRY export;

                    lRc = 0;
                    fOk = pFile->findFirstExport(&export);
                    while (fOk)
                    {
                        /* check if name or not */
                        if (!pFile->isDef() || export.ulOrdinal < ORD_START_INTERNAL_FUNCTIONS)
                        {
                            char szIntName[64];
                            char szName[64];

                            demangle(export.achIntName, &szIntName[0]);
                            if (export.achName == '\0')
                                sprintf(&szName[0], "Ordinal%04ld", export.ulOrdinal);
                            else
                                strcpy(&szName[0], &export.achName[0]);

                            fprintf(phLog, "%s: %08ld %-30s %s\n",
                                    pszFilename, export.ulOrdinal, &szName[0], &szIntName[0]);

                            fOk = dbInsertUpdateFunction(sDll,
                                                         &szName[0],
                                                         &szIntName[0],
                                                         export.ulOrdinal,
                                                         pOptions->fIgnoreOrdinals && export.ulOrdinal != 0xffffffffUL);
                            if (fOk)
                                cFunctions++;
                        }

                        if (!fOk)
                        {
                            fprintf(phLog, "%s: error - dbInsertUpdateFunction failed.\n\terror description: %s \n",
                                    pszFilename, dbGetLastErrorDesc());
                            lRc = 1;
                        }

                        /* next */
                        fOk = pFile->findNextExport(&export);
                    }

                }
                else
                    fprintf(phLog, "%s: error - could not find/insert module name (%s).\n", pszFilename, pszModuleName);
            }
            else
                fprintf(phLog, "%s: error - could not get module name.\n", pszFilename);

            delete(pFile);
        }
        catch (int err)
        {
            fprintf(phLog, "%s: error - could not map dll/def into memory, errorno=%d.\n", pszFilename, err);
        }
        fclose(phFile);
    }
    else
        fprintf(phLog, "%s: error - could not open file.\n", pszFilename);

    /* close db */
    dbDisconnect();

    return lRc;
}


/**
 * Demangles stdcall functions.
 * @param       pszMangledName  Mangled name
 * @param       pszDemangled    Pointer to buffer which will hold the demangled name upon return.
 */
static void demangle(const char *pszMangledName, char *pszDemangled)
{
    int iEnd;
    /* check for @ */
    iEnd = strlen(pszMangledName);
    if (iEnd-- > 3 && *pszMangledName == '_')
    {
        while ((pszMangledName[iEnd] >= '0' && pszMangledName[iEnd] <= '9') && iEnd > 0)
            iEnd--;
        if (pszMangledName[iEnd] == '@')
        {
            *pszDemangled = '\0';
            strncat(pszDemangled, pszMangledName+1, iEnd - 1);
            return;
        }
    }

    /* not stdcall */
    strcpy(pszDemangled, pszMangledName);
}

