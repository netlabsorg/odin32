/* $Id: fastdep.c,v 1.46 2002-12-11 15:22:47 bird Exp $
 *
 * Fast dependents. (Fast = Quick and Dirty!)
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_FILEMGR
#define INCL_DOSMISC


/*
 * Size of the \n charater (forget '\r').
 * If you're compiling this under a UNICODE system this may perhaps change,
 * but I doubd that fastdep will work at all under a UNICODE system. ;-)
 */
#if defined(UNICODE) && !defined(__WIN32OS2__)
#define CBNEWLINE       (2)
#else
#define CBNEWLINE       (1)
#endif


/*
 * Time stamp size.
 */
#define TS_SIZE         (48)


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#if defined(OS2FAKE)
#include "os2fake.h"
#else
#include <os2.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <assert.h>

#include "avl.h"

#ifdef __WIN32OS2__
#   define WIN32API
#   include <odinbuild.h>
#else
#   define ODIN32_BUILD_NR -1
#endif

#ifndef INLINE
#   if defined(__IBMC__)
#       define INLINE _Inline
#   elif defined(__IBMCPP__)
#       define INLINE inline
#   elif defined(__WATCOMC__)
#       define INLINE __inline
#   elif defined(__WATCOM_CPLUSPLUS__)
#       define INLINE inline
#   else
#       error message("unknown compiler - inline keyword unknown!")
#   endif
#endif

/*
 * This following section is used while testing fastdep.
 * stdio.h should be included; string.h never included.
 */
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

#if 1
#include <stdio.h>
#else
#include <string.h>
#include <string.h>
#endif

/*
 */ /* */ /*
#include <string.h>
 */
#if 1
#    if 1
        #if 0
# include <string.h>
        #else
#            if 1
                #if 1
                    #if 0
# include <string.h>
                    #else /* */ /*
*/
 # include <stdio.h>
                    #endif
                #endif
            #endif
        #endif
    #endif
#endif

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _Options
{
    const char *    pszInclude;
    const char *    pszExclude;
    BOOL            fExcludeAll;
    const char *    pszObjectExt;
    const char *    pszObjectDir;
    BOOL            fObjectDir;         /* replace object directory? */
    const char *    pszRsrcExt;
    BOOL            fObjRule;
    BOOL            fNoObjectPath;
    BOOL            fSrcWhenObj;
    BOOL            fAppend;            /* append to the output file, not overwrite it. */
    BOOL            fCheckCyclic;       /* allways check for cylic dependency before inserting an dependent. */
    BOOL            fCacheSearchDirs;   /* cache entire search dirs. */
    const char *    pszExcludeFiles;    /* List of excluded files. */
    BOOL            fForceScan;         /* Force scan of all files. */
} OPTIONS, *POPTIONS;


/*
 * Language specific analysis functions type.
 */
typedef int ( _FNLANG)  (const char *pszFilename, const char *pszNormFilename,
                         const char *pszTS, BOOL fHeader, void **ppvRule);
typedef _FNLANG    *PFNLANG;


/**
 * This struct holds the static configuration of the util.
 */
typedef struct _ConfigEntry
{
    char         szId[16];              /* Config ID. */
    const char **papszExts;             /* Pointer to an array of pointer to extentions for this handler. */
                                        /* If NULL this is the last entry. */
    int          iFirstHdr;             /* Index into the papszExts array of the first headerfile/copybook. */
                                        /* Set it to the NULL element of the array if no headers for this extention. */
                                        /* A non-header file may get a object rule. */
    PFNLANG      pfn;                   /* Pointer to handler function. */
    char        *pszzAddDeps;           /* Pointer to an string of string of additional dependencies. */
} CONFIGENTRY, *PCONFIGENTRY;


/**
 * Dependant Rule
 */
typedef struct _DepRule
{
    AVLNODECORE     avlCore;
    char *          pszRule;            /* Pointer to rule name */
    int             cDeps;              /* Entries in the dependant array. */
    char **         papszDep;           /* Pointer to an array of pointers to dependants. */
    BOOL            fUpdated;           /* If we have updated this entry during the run. */
    char            szTS[TS_SIZE];      /* Time stamp. */
} DEPRULE, *PDEPRULE;


/**
 * Filename cache entry.
 */
#define FCACHEENTRY     AVLNODECORE
#define PFCACHEENTRY    PAVLNODECORE


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static int makeDependent(const char *pszFilename, const char *pszTS);

static int langC_CPP(const char *pszFilename, const char *pszNormFilename, const char *pszTS, BOOL fHeader, void **ppvRule);
static int langAsm(  const char *pszFilename, const char *pszNormFilename, const char *pszTS, BOOL fHeader, void **ppvRule);
static int langRC(   const char *pszFilename, const char *pszNormFilename, const char *pszTS, BOOL fHeader, void **ppvRule);
static int langCOBOL(const char *pszFilename, const char *pszNormFilename, const char *pszTS, BOOL fHeader, void **ppvRule);
static int langIPF(  const char *pszFilename, const char *pszNormFilename, const char *pszTS, BOOL fHeader, void **ppvRule);


/* string operations */
static int strnicmpwords(const char *pszS1, const char *pszS2, int cch);

/* file operations */
static char *fileNormalize(char *pszFilename);
static char *fileNormalize2(const char *pszFilename, char *pszBuffer);
       char *filePath(const char *pszFilename, char *pszBuffer);
static char *filePathSlash(const char *pszFilename, char *pszBuffer);
static char *filePathSlash2(const char *pszFilename, char *pszBuffer);
static char *fileName(const char *pszFilename, char *pszBuffer);
static char *fileNameNoExt(const char *pszFilename, char *pszBuffer);
static char *fileExt(const char *pszFilename, char *pszBuffer);

/* filecache operations */
static BOOL filecacheAddFile(const char *pszFilename);
static BOOL filecacheAddDir(const char *pszDir);
INLINE BOOL filecacheFind(const char *pszFilename);
INLINE BOOL filecacheIsDirCached(const char *pszDir);
static char*filecacheFileExist(const char *pszFilename, char *pszBuffer);

/* pathlist operations */
static char *pathlistFindFile(const char *pszPathList, const char *pszFilename, char *pszBuffer);
static BOOL  pathlistFindFile2(const char *pszPathList, const char *pszFilename);

/* word operations */
static char *findEndOfWord(char *psz);
#if 0 /* not used */
static char *findStartOfWord(char *psz, const char *pszStart);
#endif

/* file helpers */
static signed long fsize(FILE *phFile);

/* text helpers */
INLINE char *trim(char *psz);
INLINE char *trimR(char *psz);
INLINE char *trimQuotes(char *psz);

/* preprocessors */
static char *PreProcessLine(char *pszOut, const char *pszIn);

/* textbuffer */
static void *textbufferCreate(const char *pszFilename);
static void  textbufferDestroy(void *pvBuffer);
static char *textbufferNextLine(void *pvBuffer, char *psz);
static char *textbufferGetNextLine(void *pvBuffer, void **ppv, char *pszLineBuffer, int cchLineBuffer);

/* depend workers */
static BOOL  depReadFile(const char *pszFilename, BOOL fAppend);
static BOOL  depWriteFile(const char *pszFilename, BOOL fWriteUpdatedOnly);
static void  depRemoveAll(void);
static void *depAddRule(const char *pszRulePath, const char *pszName, const char *pszExt, const char *pszTS, BOOL fConvertName);
static BOOL  depAddDepend(void *pvRule, const char *pszDep, BOOL fCheckCyclic, BOOL fConvertName);
static int   depNameToReal(char *pszName);
static int   depNameToMake(char *pszName, int cchName, const char *pszSrc);
static void  depMarkNotFound(void *pvRule);
static BOOL  depCheckCyclic(PDEPRULE pdepRule, const char *pszDep);
static BOOL  depValidate(PDEPRULE pdepRule);
INLINE char *depMakeTS(char *pszTS, PFILEFINDBUF3 pfindbuf3);
static void  depAddSrcAddDeps(void *pvRule, const char *pszz);


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * Pointer to the list of dependencies.
 */
static PDEPRULE pdepTree = NULL;


/*
 * Filecache - tree starts here.
 */
static PFCACHEENTRY pfcTree = NULL;
static unsigned     cfcNodes = 0;
static PFCACHEENTRY pfcDirTree = NULL;


/*
 * Current directory stuff
 */
static char     szCurDir[CCHMAXPATH];
static int      aiSlashes[CCHMAXPATH];
static int      cSlashes;


/*
 * Environment variables used.
 * (These has the correct case.)
 */
static char *   pszIncludeEnv;


/*
 * Configuration stuff.
 */
static const char pszDefaultDepFile[] = ".depend";
static const char *apszExtC_CPP[] = {"c", "sqc", "cpp", "h", "hpp", "ih", NULL};
static const char *apszExtAsm[]   = {"asm", "inc", NULL};
static const char *apszExtRC[]    = {"rc",  "dlg", NULL};
static const char *apszExtORC[]   = {"orc", "dlg", NULL};
static const char *apszExtCOBOL[] = {"cbl", "cob", "sqb", "wbl", NULL};
static const char *apszExtIPF[]   = {"ipf", "man", NULL};
static const char *apszExtIPP[]   = {"ipp", NULL};
static CONFIGENTRY aConfig[] =
{
    {
        "CX",
        apszExtC_CPP,
        3,
        langC_CPP,
        NULL,
    },

    {
        "AS",
        apszExtAsm,
        1,
        langAsm,
        NULL,
    },

    {
        "RC",
        apszExtRC,
        1,
        langRC,
        NULL,
    },

    {
        "ORC",
        apszExtORC,
        1,
        langRC,
        NULL,
    },

    {
        "COB",
        apszExtCOBOL,
        -1,
        langCOBOL,
        NULL,
    },

    {
        "IPF",
        apszExtIPF,
        -1,
        langIPF,
        NULL,
    },

    {
        "IPP",
        apszExtIPP,
        -1,
        langC_CPP,
        NULL,
    },

    /* terminating entry */
    {
        "",
        NULL,
        -1,
        NULL,
        NULL
    }
};


static char szObjectDir[CCHMAXPATH];
static char szObjectExt[64] = "obj";
static char szRsrcExt[64]   = "res";
static char szInclude[32768] = ";";
static char szExclude[32768] = ";";
static char szExcludeFiles[65536] = "";

OPTIONS options =
{
    szInclude,       /* pszInclude */
    szExclude,       /* pszExclude */
    FALSE,           /* fExcludeAll */
    szObjectExt,     /* pszObjectExt */
    szObjectDir,     /* pszObjectDir */
    FALSE,           /* fObjectDir */
    szRsrcExt,       /* pszRsrcExt */
    TRUE,            /* fObjRule */
    FALSE,           /* fNoObjectPath */
    TRUE,            /* fSrcWhenObj */
    FALSE,           /* fAppend */
    TRUE,            /* fCheckCyclic */
    TRUE,            /* fCacheSearchDirs */
    szExcludeFiles,  /* pszExcludeFiles */
    FALSE            /* fForceScan */
};


/**
 * Main function.
 * @returns   0 on success.
 *           -n count of failiures.
 * @param
 * @param
 * @equiv
 * @precond
 * @methdesc
 * @result
 * @time
 * @sketch
 * @algo
 * @remark
 */
int main(int argc, char **argv)
{
    int         rc   = 0;
    int         argi = 1;
    int         i;
    char *      psz;
    char *      psz2;
    const char *pszDepFile = pszDefaultDepFile;
    char        achBuffer[4096];

    szObjectDir[0] = '\0';

    if (argc == 1)
    {
        syntax();
        return -87;
    }

    /*
     * Initiate current directory stuff
     */
    if (_getcwd(szCurDir, sizeof(szCurDir)) == NULL)
    {
        fprintf(stderr, "fatal error: failed to get current directory\n");
        return -88;
    }
    strlwr(szCurDir);
    aiSlashes[0] = 0;
    for (i = 1, cSlashes; szCurDir[i] != '\0'; i++)
    {
        if (szCurDir[i] == '/')
            szCurDir[i] = '\\';
        if (szCurDir[i] == '\\')
            aiSlashes[cSlashes++] = i;
    }
    if (szCurDir[i-1] != '\\')
    {
        aiSlashes[cSlashes] = i;
        szCurDir[i++] = '\\';
        szCurDir[i] = '\0';
    }


    /*
     * Initiate environment variables used: INCLUDE
     */
    psz = getenv("INCLUDE");
    if (psz != NULL)
    {
        pszIncludeEnv = strdup(psz);
        strlwr(pszIncludeEnv);
    }
    else
        pszIncludeEnv = "";


    /*
     * Disable hard errors.
     */
    DosError(FERR_DISABLEHARDERR | FERR_ENABLEEXCEPTION);


    /*
     * parse arguments
     */
    while (argi < argc)
    {
        if (argv[argi][0] == '-' || argv[argi][0] == '/')
        {
            /* parameters */
            switch (argv[argi][1])
            {
                case 'A':
                case 'a': /* Append to the output file */
                    options.fAppend = argv[argi][2] != '-';
                    break;

                case 'D':
                case 'd': /* "-d <filename>" */
                {
                    const char *pszOld = pszDepFile;
                    if (argv[argi][2] != '\0')
                        pszDepFile = &argv[argi][2];
                    else
                    {
                        argi++;
                        if (argi < argc)
                            pszDepFile = argv[argi];
                        else
                        {
                            fprintf(stderr, "invalid parameter -d, filename missing!\n");
                            return -1;
                        }
                    }

                    /* if dependencies are generated we'll flush them to the old filename */
                    if (pdepTree != NULL && pszOld != pszDepFile)
                    {
                        if (!depWriteFile(pszOld, !options.fAppend))
                            fprintf(stderr, "error: failed to write (flush) dependencies.\n");
                        depRemoveAll();
                    }
                    break;
                }

                case 'C': /* forced directory cache  'ca' or cylic check 'cy'*/
                case 'c':
                    if (argv[argi][2] == 'a' || argv[argi][2] == 'A')
                        options.fCacheSearchDirs = TRUE;
                    else if ((argv[argi][2] == 'y' || argv[argi][2] == 'Y'))
                        options.fCheckCyclic = argv[argi][3] != '-';
                    break;

                case 'E': /* list of paths. If a file is found in one of these directories the */
                case 'e': /* filename will be used without the directory path. */
                    /* Eall<[+]|-> ? */
                    if (strlen(&argv[argi][1]) <= 5 && strnicmp(&argv[argi][1], "Eall", 4) == 0)
                    {
                        options.fExcludeAll = argv[argi][5] != '-';
                        break;
                    }
                    /* path or path list */
                    if (strlen(argv[argi]) > 2)
                        psz = &argv[argi][2];
                    else
                    {
                        if (++argi >= argc)
                        {
                            fprintf(stderr, "syntax error! Option -e.\n");
                            return 1;
                        }
                        psz = argv[argi];
                    }
                    /* check if enviroment variable */
                    if (*psz == '%')
                    {
                        psz2 = strdup(psz+1);
                        if (psz2 != NULL && *psz2 != '\0')
                        {
                            if (psz2[strlen(psz2)-1] == '%')
                                psz2[strlen(psz2)-1] = '\0';
                            psz = getenv(psz2);
                            free(psz2);
                            if (psz == NULL)
                                break;
                        }
                        else
                        {
                            fprintf(stderr, "error: -E% is not an valid argument!\n");
                            return -1;
                        }
                    }
                    if (psz != NULL)
                    {
                        strcat(szExclude, psz);
                        strlwr(szExclude);
                        if (szExclude[strlen(szExclude)-1] != ';')
                            strcat(szExclude, ";");
                    }
                    break;

                case 'f':
                case 'F': /* force scan of all files. */
                    options.fForceScan = argv[argi][2] != '-';
                    break;

                case 'I': /* optional include path. This has precedence over the INCLUDE environment variable. */
                case 'i':
                    if (strlen(argv[argi]) > 2)
                        psz = &argv[argi][2];
                    else
                    {
                        if (++argi >= argc)
                        {
                            fprintf(stderr, "syntax error! Option -i.\n");
                            return 1;
                        }
                        psz = argv[argi];
                    }
                    /* check if enviroment variable */
                    if (*psz == '%')
                    {
                        psz2 = strdup(psz+1);
                        if (psz2 != NULL && *psz2 != '\0')
                        {
                            if (psz2[strlen(psz2)-1] == '%')
                                psz2[strlen(psz2)-1] = '\0';
                            psz = getenv(psz2);
                            free(psz2);
                            if (psz == NULL)
                                break;
                        }
                        else
                        {
                            fprintf(stderr, "error: -I% is not an valid argument!\n");
                            return -1;
                        }
                    }
                    if (psz != NULL)
                    {
                        strcat(szInclude, psz);
                        strlwr(szInclude);
                        if (szInclude[strlen(szInclude)-1] != ';')
                            strcat(szInclude, ";");
                    }
                    break;

                case 'n': /* no object path , -N<[+]|-> */
                case 'N':
                    if (strlen(argv[argi]) <= 1+1+1)
                        options.fNoObjectPath = argv[argi][2] != '-';
                    else
                    {
                        fprintf(stderr, "error: invalid parameter!, '%s'\n", argv[argi]);
                        return -1;
                    }
                    break;

                case 'o': /* object base directory, Obj or Obr<[+]|-> */
                case 'O':
                    if (strlen(&argv[argi][1]) <= 4 && strnicmp(&argv[argi][1], "Obr", 3) == 0)
                    {
                        options.fObjRule = argv[argi][4] != '-';
                        break;
                    }

                    if (strlen(&argv[argi][1]) >= 4 && strnicmp(&argv[argi][1], "Obj", 3) == 0)
                    {
                        if (strlen(argv[argi]) > 4)
                            strcpy(szObjectExt, argv[argi]+4);
                        else
                        {
                            if (++argi >= argc)
                            {
                                fprintf(stderr, "syntax error! Option -obj.\n");
                                return 1;
                            }
                            strcpy(szObjectExt, argv[argi]);
                        }
                        break;
                    }

                    /* path: -o or -o- */
                    options.fObjectDir = TRUE;
                    if (strlen(argv[argi]) > 2)
                    {
                        if (argv[argi][2] == '-')  /* no object path */
                            szObjectDir[0] = '\0';
                        else
                            strcpy(szObjectDir, argv[argi]+2);
                    }
                    else
                    {
                        if (++argi >= argc)
                        {
                            fprintf(stderr, "syntax error! Option -o.\n");
                            return 1;
                        }
                        strcpy(szObjectDir, argv[argi]);
                    }
                    if (szObjectDir[0] != '\0'
                        && szObjectDir[strlen(szObjectDir)-1] != '\\'
                        && szObjectDir[strlen(szObjectDir)-1] != '/'
                        )
                        strcat(szObjectDir, "\\");
                    break;

                case 'r':
                case 'R':
                    if (strlen(argv[argi]) > 2)
                        strcpy(szRsrcExt, argv[argi]+2);
                    else
                    {
                        if (++argi >= argc)
                        {
                            fprintf(stderr, "syntax error! Option -r.\n");
                            return 1;
                        }
                        strcpy(szRsrcExt, argv[argi]);
                    }
                    break;

                case 's':
                case 'S':
                    if (!strnicmp(argv[argi]+1, "srcadd", 6))
                    {
                        if (strlen(argv[argi]) > 7)
                            psz = argv[argi]+2;
                        else
                        {
                            if (++argi >= argc)
                            {
                                fprintf(stderr, "syntax error! Option -srcadd.\n");
                                return 1;
                            }
                            psz = argv[argi];
                        }
                        if (!(psz2 = strchr(psz, ':')))
                        {
                            fprintf(stderr, "syntax error! Option -srcadd malformed!\n");
                            return 1;
                        }
                        for (i = 0; aConfig[i].pfn; i++)
                        {
                            if (    !strnicmp(aConfig[i].szId, psz, psz2 - psz)
                                &&  !aConfig[i].szId[psz2 - psz])
                            {
                                int cch, cch2;
                                if (!*++psz2)
                                {
                                    fprintf(stderr, "error: Option -srcadd no additioanl dependancy!\n",
                                            psz2 - psz, psz);
                                    return 1;
                                }
                                cch = 0;
                                psz = aConfig[i].pszzAddDeps;
                                if (psz)
                                {
                                    do
                                    {
                                        cch += (cch2 = strlen(psz)) + 1;
                                        psz += cch2 + 1;
                                    } while (*psz);
                                }
                                cch2 = strlen(psz2);
                                aConfig[i].pszzAddDeps = realloc(aConfig[i].pszzAddDeps, cch + cch2 + 2);
                                if (!aConfig[i].pszzAddDeps)
                                {
                                    fprintf(stderr, "error: Out of memory!\n");
                                    return 1;
                                }
                                strcpy(aConfig[i].pszzAddDeps + cch, psz2);
                                aConfig[i].pszzAddDeps[cch + cch2 + 1] = '\0';
                                psz = NULL;
                                break;
                            }
                        }
                        if (psz)
                        {
                            fprintf(stderr, "error: Option -srcadd, invalid language id '%.*s%'\n",
                                    psz2 - psz, psz);
                            return 1;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "syntax error! Invalid option %s\n", argv[argi]);
                        return 1;
                    }
                    break;

                case 'x':
                case 'X': /* Exclude files */
                    psz = &achBuffer[CCHMAXPATH+8];
                    if (strlen(argv[argi]) > 2)
                        strcpy(psz, &argv[argi][2]);
                    else
                    {
                        if (++argi >= argc)
                        {
                            fprintf(stderr, "syntax error! Option -x.\n");
                            return 1;
                        }
                        strcpy(psz, argv[argi]);
                    }
                    while (psz != NULL && *psz != ';')
                    {
                        char *  pszNext = strchr(psz, ';');
                        int     cch = strlen(szExcludeFiles);
                        if (pszNext)
                            *pszNext++ = '\0';
                        if (DosQueryPathInfo(psz, FIL_QUERYFULLNAME, &szExcludeFiles[cch], CCHMAXPATH))
                        {
                            fprintf(stderr, "error: Invalid exclude name\n");
                            return -1;
                        }
                        strlwr(&szExcludeFiles[cch]);
                        strcat(&szExcludeFiles[cch], ";");
                        psz = pszNext;
                    }
                    break;

                case 'h':
                case 'H':
                case '?':
                    syntax();
                    return 1;

                default:
                    fprintf(stderr, "error: invalid parameter! '%s'\n", argv[argi]);
                    return -1;
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
                        apszArgs = realloc(apszArgs, sizeof(char*) * 512);
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
                continue;
            }
            else
            {
                fprintf(stderr, "error: could not open parameter file\n");
                return -1;
            }
        }
        else
        {   /* not a parameter! */
            ULONG           ulRc;
            PFILEFINDBUF3   pfindbuf3 = (PFILEFINDBUF3)(void*)&achBuffer[0];
            HDIR            hDir = HDIR_CREATE;
            ULONG           cFiles = ~0UL;
            int             i;


            /*
             * If append option is or if the forcescan option isn't is
             * we'll have to read the existing dep file before starting
             * adding new dependencies.
             */
            if (pdepTree == NULL && (options.fAppend || !options.fForceScan))
                depReadFile(pszDepFile, options.fAppend);

            /*
             * Search for the files specified.
             */
            ulRc = DosFindFirst(argv[argi], &hDir,
                                FILE_READONLY |  FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                                pfindbuf3, sizeof(achBuffer), &cFiles, FIL_STANDARD);
            if (!options.fCacheSearchDirs)
                options.fCacheSearchDirs = cFiles > 25;
            while (ulRc == NO_ERROR)
            {
                for (i = 0;
                     i < cFiles;
                     i++, pfindbuf3 = (PFILEFINDBUF3)((int)pfindbuf3 + pfindbuf3->oNextEntryOffset)
                     )
                {
                    const char *    psz;
                    char            szSource[CCHMAXPATH];
                    BOOL            fExcluded;
                    char            szTS[TS_SIZE];

                    /*
                     * Make full path.
                     */
                    if ((psz = strrchr(argv[argi], '\\')) || (psz = strrchr(argv[argi], '/')) || (*(psz = &argv[argi][1]) == ':'))
                    {
                        strncpy(szSource, argv[argi], psz - argv[argi] + 1);
                        szSource[psz - argv[argi] + 1]  = '\0';
                    }
                    else
                        szSource[0]  = '\0';
                    strcat(szSource, pfindbuf3->achName);
                    strlwr(szSource);
                    fileNormalize(szSource);

                    /*
                     * Check if this is an excluded file.
                     */
                    fExcluded = FALSE;
                    psz = options.pszExcludeFiles;
                    while (*psz != '\0' && *psz != ';')
                    {
                        const char * pszNext = strchr(psz, ';');
                        if (strlen(szSource) == pszNext - psz && strncmp(szSource, psz, pszNext - psz) == 0)
                            fExcluded = TRUE;
                        psz = pszNext + 1;
                    }
                    if (fExcluded)
                        continue;

                    /*
                     * Analyse the file.
                     */
                    depMakeTS(szTS, pfindbuf3);
                    rc -= makeDependent(&szSource[0], szTS);
                }

                /* next file */
                cFiles = ~0UL;
                pfindbuf3 = (PFILEFINDBUF3)(void*)&achBuffer[0];
                ulRc = DosFindNext(hDir, pfindbuf3, sizeof(achBuffer), &cFiles);
            }
            DosFindClose(hDir);
        }
        /* next */
        argi++;
    }

    /* Write the depend file! */
    if (!depWriteFile(pszDepFile, !options.fAppend))
        fprintf(stderr, "error: failed to write dependencies file!\n");
    #if 0
    printf("cfcNodes=%d\n", cfcNodes);
    #endif

    return rc;
}


/**
 * Displays the syntax description for this util.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
void syntax(void)
{
    printf(
        "FastDep v0.48 (build %d)\n"
        "Dependency scanner. Creates a makefile readable depend file.\n"
        " - was quick and dirty, now it's just quick -\n"
        "\n"
        "Syntax: FastDep [options] <files> [more options [more files [...]]]\n"
        "    or\n"
        "        FastDep [options] @<parameterfile>\n"
        "\n"
        "Options:\n"
        "   -a<[+]|->       Append to the output file.            Default: Overwrite.\n"
        "   -ca             Force search directory caching.\n"
        "                   Default: cache if more that 25 files are to be searched.\n"
        "                            (more than 25 in the first file expression.)\n"
        "   -cy<[+]|->      Check for cylic dependencies.         Default: -cy-\n"
        "   -d <outputfn>   Output filename.                      Default: %s\n"
        "   -e excludepath  Exclude paths. If a filename is found in any\n"
        "                   of these paths only the filename is used, not\n"
        "                   the path+filename (which is default).\n"
        "   -eall<[+]|->    Include and source filenames, paths or no paths.\n"
        "                   -eall+: No path are added to the filename.\n"
        "                   -eall-: The filename is appended the include path\n"
        "                           was found in.\n"
        "                   Default: eall-\n"
        "   -f<[+]|->       Force scanning of all files. If disabled we'll only scan\n"
        "                   files which are younger or up to one month older than the\n"
        "                   dependancy file (if it exists).       Default: disabled\n"
        "   -i <include>    Additional include paths. INCLUDE is searched after this.\n"
        "   -n<[+]|->       No path for object files in the rules.\n"
        "   -o <objdir>     Path were object files are placed. This path replaces the\n"
        "                   entire filename path\n"
        "   -o-             No object path\n"
        "   -obr<[+]|->     -obr+: Object rule.\n"
        "                   -obr-: No object rule, rule for source filename is generated.\n"
        "   -obj[ ]<objext> Object extention.                     Default: obj\n"
        "   -srcadd[ ]<langid>:<dep>\n"
        "                   Additional dependants for source file of the given language\n"
        "                   type. langid: AS,CX,RC,ORC,COB,IPF\n"
        "                   This is very usfull for compiler configuration files.\n"
        "   -r[ ]<rsrcext>  Resource binary extention.            Default: res\n"
        "   -x[ ]<f1[;f2]>  Files to exclude. Only exact filenames.\n"
        "   <files>         Files to scan. Wildchars are allowed.\n"
        "\n"
        "Options and files could be mixed.\n"
        " copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)\n",
        ODIN32_BUILD_NR,
        pszDefaultDepFile
        );
}


/**
 * Generates depend info on this file, these are stored internally
 * and written to file later.
 * @returns
 * @param   pszFilename     Pointer to source filename. Correct case is assumed!
 * @param   pszTS           File time stamp.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
int makeDependent(const char *pszFilename, const char *pszTS)
{
    int    rc = -1;

    char            szExt[CCHMAXPATH];
    PCONFIGENTRY    pCfg = &aConfig[0];
    BOOL            fHeader;

    /*
     * Find which filetype this is...
     */
    fileExt(pszFilename, szExt);
    while (pCfg->papszExts != NULL)
    {
        const char **ppsz = pCfg->papszExts;
        while (*ppsz != NULL && stricmp(*ppsz, szExt) != 0)
            ppsz++;
        if (*ppsz != NULL)
        {
            fHeader = pCfg->iFirstHdr > 0 && &pCfg->papszExts[pCfg->iFirstHdr] <= ppsz;
            break;
        }
        pCfg++;
    }

    /* Found? */
    if (pCfg->papszExts != NULL)
    {
        void *  pvRule = NULL;
        char    szNormFile[CCHMAXPATH];
        fileNormalize2(pszFilename, szNormFile);
        rc = (*pCfg->pfn)(pszFilename, &szNormFile[0], pszTS, fHeader, &pvRule);
        if (!rc && pvRule)
        {
            if (!fHeader && pCfg->pszzAddDeps)
                depAddSrcAddDeps(pvRule, pCfg->pszzAddDeps);
        }
    }
    else
    {
        if (*fileName(pszFilename, szExt) != '.') /* these are 'hidden' files, like .cvsignore, let's ignore them. */
            fprintf(stderr, "warning: '%s' has an unknown file type.\n", pszFilename);
        rc = 0;
    }


    return rc;
}


/**
 * Generates depend info on this C or C++ file, these are stored internally
 * and written to file later.
 * @returns 0 on success.
 *          !0 on error.
 * @param   pszFilename         Pointer to source filename. Correct case is assumed!
 * @param   pszNormFilename     Pointer to normalized source filename.
 * @param   pszTS               File time stamp.
 * @parma   fHeader             True if header file is being scanned.
 * @param   ppvRule             Variabel to return any new rule handle.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
int langC_CPP(const char *pszFilename, const char *pszNormFilename,
              const char *pszTS, BOOL fHeader, void **ppvRule)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Max line length is 4096... should not be a problem. */
    int     iLine;                      /* Linenumber. */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */
    BOOL    fComment;                   /* TRUE when within a multiline comment. */
                                        /* FALSE when not within a multiline comment. */
    int     iIfStack;                   /* StackPointer. */
    struct  IfStackEntry
    {
        int fIncluded : 1;              /* TRUE:  include this code;
                                         * FALSE: excluded */
        int fIf : 1;                    /* TRUE:  #if part of the expression.
                                         * FALSE: #else part of the expression. */
        int fSupported : 1;             /* TRUE:  supported if/else statement
                                         * FALSE: unsupported all else[<something>] are ignored
                                         *        All code is included.
                                         */
    } achIfStack[256];
    char    szSrcDir[CCHMAXPATH];
    filePath(pszNormFilename, szSrcDir);/* determin source code directory. */

    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszObjectExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszObjectExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename,
                         options.fCheckCyclic,
                         FALSE);
    }
    else
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    /* Initiate the IF-stack, comment state and line number. */
    iIfStack = 0;
    achIfStack[iIfStack].fIf = TRUE;
    achIfStack[iIfStack].fIncluded = TRUE;
    achIfStack[iIfStack].fSupported = TRUE;
    fComment = FALSE;
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        /* search for #include */
        register char *pszC;
        int cbLen;
        int i = 0;
        iLine++;

        /* skip blank chars */
        cbLen = strlen(szBuffer);
        while (i + 2 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
            i++;

        /* preprocessor statement? */
        if (!fComment && szBuffer[i] == '#')
        {
            /*
             * Preprocessor checks
             * We known that we have a preprocessor statment (starting with an '#' * at szBuffer[i]).
             * Depending on the word afterwards we'll take some different actions.
             * So we'll start of by extracting that word and make a string swich on it.
             * Note that there might be some blanks between the hash and the word.
             */
            int     cchWord;
            char *  pszEndWord;
            char *  pszArgument;
            i++;                /* skip hash ('#') */
            while (szBuffer[i] == '\t' || szBuffer[i] == ' ') /* skip blanks */
                i++;
            pszArgument = pszEndWord = findEndOfWord(&szBuffer[i]);
            cchWord = pszEndWord - &szBuffer[i];

            /*
             * Find the argument by skipping the blanks.
             */
            while (*pszArgument == '\t' || *pszArgument == ' ') /* skip blanks */
                pszArgument++;

            /*
             * string switch.
             */
            if (strncmp(&szBuffer[i], "include", cchWord) == 0)
            {
                /*
                 * #include
                 *
                 * Are we in a state where this file is to be included?
                 */
                if (achIfStack[iIfStack].fIncluded)
                {
                    char    szFullname[CCHMAXPATH];
                    char *  psz;
                    BOOL    f = FALSE;
                    int     j;
                    BOOL    fQuote;

                    /* extract info between "" or <> */
                    while (i < cbLen && !(f = (szBuffer[i] == '"' || szBuffer[i] == '<')))
                        i++;
                    fQuote = szBuffer[i] == '"';
                    i++; /* skip '"' or '<' */

                    /* if invalid statement then continue with the next line! */
                    if (!f) continue;

                    /* find end */
                    j = f = 0;
                    while (i + j < cbLen &&  j < CCHMAXPATH &&
                           !(f = (szBuffer[i+j] == '"' || szBuffer[i+j] == '>')))
                        j++;

                    /* if invalid statement then continue with the next line! */
                    if (!f) continue;

                    /* copy filename */
                    strncpy(szFullname, &szBuffer[i], j);
                    szFullname[j] = '\0'; /* ensure terminatition. */
                    strlwr(szFullname);

                    /* find include file! */
                    psz = fQuote ? pathlistFindFile(szSrcDir, szFullname, szBuffer) : NULL;
                    if (psz == NULL)
                        psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
                    if (psz == NULL)
                        psz = pathlistFindFile(pszIncludeEnv, szFullname, szBuffer);

                    /* did we find the include? */
                    if (psz != NULL)
                    {
                        if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                        {   /* #include <sys/stats.h> makes trouble, check for '/' and '\'. */
                            if (!strchr(szFullname, '/') && !strchr(szFullname, '\\'))
                                depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                            else
                                fprintf(stderr, "%s(%d): warning include '%s' is ignored.\n",
                                        pszFilename, iLine, szFullname);
                        }
                        else
                            depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
                    }
                    else
                    {
                        fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                                pszFilename, iLine, szFullname);
                        depMarkNotFound(pvRule);
                    }
                }
            }
            else
                /*
                 * #if
                 */
                if (strncmp(&szBuffer[i], "if", cchWord) == 0)
            {   /* #if 0 and #if <1-9> are supported */
                pszEndWord = findEndOfWord(pszArgument);
                iIfStack++;
                if ((pszEndWord - pszArgument) == 1
                    && *pszArgument >= '0' && *pszArgument <= '9')
                {
                    if (*pszArgument != '0')
                        achIfStack[iIfStack].fIncluded =  TRUE;
                    else
                        achIfStack[iIfStack].fIncluded =  FALSE;
                }
                else
                    achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
                achIfStack[iIfStack].fIf = TRUE;
            }
            else
                /*
                 * #else
                 */
                if (strncmp(&szBuffer[i], "else", cchWord) == 0)
            {
                if (achIfStack[iIfStack].fSupported)
                {
                    if (achIfStack[iIfStack].fIncluded) /* ARG!! this'll prevent warning */
                        achIfStack[iIfStack].fIncluded = FALSE;
                    else
                        achIfStack[iIfStack].fIncluded = TRUE;
                }
                achIfStack[iIfStack].fIf = FALSE;
            }
            else
                /*
                 * #endif
                 */
                if (strncmp(&szBuffer[i], "endif", cchWord) == 0)
            {   /* Pop the if-stack. */
                if (iIfStack > 0)
                    iIfStack--;
                else
                    fprintf(stderr, "%s(%d): If-Stack underflow!\n", pszFilename, iLine);
            }
            /*
             * general if<something> and elseif<something> implementations
             */
            else
                if (strncmp(&szBuffer[i], "elseif", 6) == 0)
            {
                achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
            }
            else
                if (strncmp(&szBuffer[i], "if", 2) == 0)
            {
                iIfStack++;
                achIfStack[iIfStack].fIf = TRUE;
                achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
            }
            /* The rest of them aren't implemented yet.
            else if (strncmp(&szBuffer[i], "if") == 0)
            {
            }
            */
        }

        /*
         * Comment checks.
         *  -Start at first non-blank.
         *  -Loop thru the line since we might have more than one
         *   comment statement on a single line.
         */
        pszC = &szBuffer[i];
        while (pszC != NULL && *pszC != '\0')
        {
            if (fComment)
                pszC = strstr(pszC, "*/");  /* look for end comment mark. */
            else
            {
                char *pszLC;
                pszLC= strstr(pszC, "//");  /* look for single line comment mark. */
                pszC = strstr(pszC, "/*");  /* look for start comment mark */
                if (pszLC && pszLC < pszC)  /* if there is an single line comment mark before the */
                    break;                  /* muliline comment mark we'll ignore the multiline mark. */
            }

            /* Comment mark found? */
            if (pszC != NULL)
            {
                fComment = !fComment;
                pszC += 2;          /* skip comment mark */

                /* debug */
                /*
                if (fComment)
                    fprintf(stderr, "starts at line %d\n", iLine);
                else
                    fprintf(stderr, "ends   at line %d\n", iLine);
                    */
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);

    return 0;
}


/**
 * Generates depend info on this file, these are stored internally
 * and written to file later.
 * @returns 0 on success.
 *          !0 on error.
 * @param   pszFilename         Pointer to source filename. Correct case is assumed!
 * @param   pszNormFilename     Pointer to normalized source filename.
 * @param   pszTS               File time stamp.
 * @parma   fHeader             True if header file is being scanned.
 * @param   ppvRule             Variabel to return any new rule handle.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
int langAsm(const char *pszFilename, const char *pszNormFilename,
            const char *pszTS, BOOL fHeader, void **ppvRule)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszObjectExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszObjectExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename,
                         options.fCheckCyclic, FALSE);
    }
    else
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        /* search for include */
        int cbLen;
        int i = 0;
        iLine++;

        /* skip blank chars */
        cbLen = strlen(szBuffer);
        while (i + 9 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
            i++;

        /* is this an include? */
        if (strnicmp(&szBuffer[i], "include", 7) == 0
            && (szBuffer[i + 7] == '\t' || szBuffer[i + 7] == ' ')
            )
        {
            char szFullname[CCHMAXPATH];
            char *psz;
            int  j;

            /* skip to first no blank char  */
            i += 7;
            while (i < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
                i++;

            /* comment check - if comment found, no filename was given. continue. */
            if (szBuffer[i] == ';') continue;

            /* find end */
            j = 0;
            while (i + j < cbLen
                   &&  j < CCHMAXPATH
                   && szBuffer[i+j] != ' '  && szBuffer[i+j] != '\t' && szBuffer[i+j] != '\n'
                   && szBuffer[i+j] != '\0' && szBuffer[i+j] != ';'  && szBuffer[i+j] != '\r'
                   )
                j++;

            /* copy filename */
            strncpy(szFullname, &szBuffer[i], j);
            szFullname[j] = '\0'; /* ensure terminatition. */
            strlwr(szFullname);

            /* find include file! */
            psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
            if (psz == NULL)
                psz = pathlistFindFile(pszIncludeEnv, szFullname, szBuffer);

            /* Did we find the include? */
            if (psz != NULL)
            {
                if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                {   /* include sys/stats.inc makes trouble, check for '/' and '\'. */
                    if (!strchr(szFullname, '/') && !strchr(szFullname, '\\'))
                        depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                    else
                        fprintf(stderr, "%s(%d): warning include '%s' is ignored.\n",
                                pszFilename, iLine, szFullname);
                }
                else
                    depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
            }
            else
            {
                fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                        pszFilename, iLine, szFullname);
                depMarkNotFound(pvRule);
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);

    return 0;
}


/**
 * Generates depend info on this Resource file, these are stored internally
 * and written to file later.
 * @returns 0 on success.
 *          !0 on error.
 * @param   pszFilename         Pointer to source filename. Correct case is assumed!
 * @param   pszNormFilename     Pointer to normalized source filename.
 * @param   pszTS               File time stamp.
 * @parma   fHeader             True if header file is being scanned.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
#if 0
int langRC(const char *pszFilename, const char *pszNormFilename, void *pvFile, BOOL fHeader)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszRsrcExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszRsrcExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : fileNormalize2(pszFilename, szBuffer),
                         options.fCheckCyclic,
                         FALSE);
    }
    else
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        /* search for #include */
        int cbLen;
        int i = 0;
        int i1;
        iLine++;

        /* skip blank chars */
        cbLen = strlen(szBuffer);
        while (i + 9 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
            i++;

        /* is this an include? */
        i1 = 1;
        if (   strncmp(&szBuffer[i], "#include", 8) == 0
            || (i1 = strnicmp(&szBuffer[i], "RCINCLUDE", 9)) == 0
            || strnicmp(&szBuffer[i], "DLGINCLUDE", 10) == 0
            )
        {
            char szFullname[CCHMAXPATH];
            char *psz;
            BOOL f = FALSE;
            int  j;

            if (i1 != 0)
            {   /*
                 * #include <file.h>,  #include "file.h" or DLGINCLUDE 1 "file.h"
                 *
                 * extract info between "" or <>
                 */
                while (i < cbLen && !(f = (szBuffer[i] == '"' || szBuffer[i] == '<')))
                    i++;
                i++; /* skip '"' or '<' */

                /* if invalid statement then continue with the next line! */
                if (!f) continue;

                /* find end */
                j = f = 0;
                while (i + j < cbLen &&  j < CCHMAXPATH &&
                       !(f = (szBuffer[i+j] == '"' || szBuffer[i+j] == '>')))
                    j++;

                /* if invalid statement then continue with the next line! */
                if (!f) continue;
            }
            else
            {   /*
                 * RCINCLUDE ["]filename.dlg["]
                 * Extract filename.
                 */

                /* skip to filename.dlg start - if eol will continue to loop. */
                i += 9;
                while (szBuffer[i] == ' ' || szBuffer[i] == '\t' || szBuffer[i] == '"')
                    i++;
                if (szBuffer[i] == '\0')
                    continue;

                /* search to end of filename. */
                j = i+1;
                while (   szBuffer[i+j] != ' ' && szBuffer[i+j] != '\t'
                       && szBuffer[i+j] != '"' && szBuffer[i+j] != '\0')
                    j++;
            }

            /* copy filename */
            strncpy(szFullname, &szBuffer[i], j);
            szFullname[j] = '\0'; /* ensure terminatition. */
            strlwr(szFullname);

            /* find include file! */
            psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
            if (psz == NULL)
                psz = pathlistFindFile(pszIncludeEnv, szFullname, szBuffer);

            /* did we find the include? */
            if (psz != NULL)
            {
                if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                {   /* #include <sys/stats.h> makes trouble, check for '/' and '\'. */
                    if (!strchr(szFullname, '/') && !strchr(szFullname, '\\'))
                        depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                    else
                        fprintf(stderr, "%s(%d): warning include '%s' is ignored.\n",
                                pszFilename, iLine, szFullname);
                }
                else
                    depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
            }
            else
            {
                fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                        pszFilename, iLine, szFullname);
                depMarkNotFound(pvRule);
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);
    return 0;
}
#else
int langRC(const char *pszFilename, const char *pszNormFilename,
           const char *pszTS, BOOL fHeader, void **ppvRule)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Max line length is 4096... should not be a problem. */
    int     iLine;                      /* Linenumber. */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */
    BOOL    fComment;                   /* TRUE when within a multiline comment. */
                                        /* FALSE when not within a multiline comment. */
    int     iIfStack;                   /* StackPointer. */
    struct  IfStackEntry
    {
        int fIncluded : 1;              /* TRUE:  include this code;
                                         * FALSE: excluded */
        int fIf : 1;                    /* TRUE:  #if part of the expression.
                                         * FALSE: #else part of the expression. */
        int fSupported : 1;             /* TRUE:  supported if/else statement
                                         * FALSE: unsupported all else[<something>] are ignored
                                         *        All code is included.
                                         */
    } achIfStack[256];


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszRsrcExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszRsrcExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename,
                         options.fCheckCyclic,
                         FALSE);
    }
    else
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    /* Initiate the IF-stack, comment state and line number. */
    iIfStack = 0;
    achIfStack[iIfStack].fIf = TRUE;
    achIfStack[iIfStack].fIncluded = TRUE;
    achIfStack[iIfStack].fSupported = TRUE;
    fComment = FALSE;
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        register char * pszC;
        char            szFullname[CCHMAXPATH];
        int             cbLen;
        int             i1 = 1;
        int             i = 0;
        iLine++;

        /* skip blank chars */
        cbLen = strlen(szBuffer);
        while (i + 2 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
            i++;

        /* preprocessor statement? */
        if (!fComment && szBuffer[i] == '#')
        {
            /*
             * Preprocessor checks
             * We known that we have a preprocessor statment (starting with an '#' * at szBuffer[i]).
             * Depending on the word afterwards we'll take some different actions.
             * So we'll start of by extracting that word and make a string swich on it.
             * Note that there might be some blanks between the hash and the word.
             */
            int     cchWord;
            char *  pszEndWord;
            char *  pszArgument;
            i++;                /* skip hash ('#') */
            while (szBuffer[i] == '\t' || szBuffer[i] == ' ') /* skip blanks */
                i++;
            pszArgument = pszEndWord = findEndOfWord(&szBuffer[i]);
            cchWord = pszEndWord - &szBuffer[i];

            /*
             * Find the argument by skipping the blanks.
             */
            while (*pszArgument == '\t' || *pszArgument == ' ') /* skip blanks */
                pszArgument++;

            /*
             * string switch.
             */
            if (strncmp(&szBuffer[i], "include", cchWord) == 0)
            {
                /*
                 * #include
                 *
                 * Are we in a state where this file is to be included?
                 */
                if (achIfStack[iIfStack].fIncluded)
                {
                    char *psz;
                    BOOL f = FALSE;
                    int  j;

                    /* extract info between "" or <> */
                    while (i < cbLen && !(f = (szBuffer[i] == '"' || szBuffer[i] == '<')))
                        i++;
                    i++; /* skip '"' or '<' */

                    /* if invalid statement then continue with the next line! */
                    if (!f) continue;

                    /* find end */
                    j = f = 0;
                    while (i + j < cbLen &&  j < CCHMAXPATH &&
                           !(f = (szBuffer[i+j] == '"' || szBuffer[i+j] == '>')))
                        j++;

                    /* if invalid statement then continue with the next line! */
                    if (!f) continue;

                    /* copy filename */
                    strncpy(szFullname, &szBuffer[i], j);
                    szFullname[j] = '\0'; /* ensure terminatition. */
                    strlwr(szFullname);

                    /* find include file! */
                    psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
                    if (psz == NULL)
                        psz = pathlistFindFile(pszIncludeEnv, szFullname, szBuffer);

                    /* did we find the include? */
                    if (psz != NULL)
                    {
                        if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                        {   /* #include <sys/stats.h> makes trouble, check for '/' and '\'. */
                            if (!strchr(szFullname, '/') && !strchr(szFullname, '\\'))
                                depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                            else
                                fprintf(stderr, "%s(%d): warning include '%s' is ignored.\n",
                                        pszFilename, iLine, szFullname);
                        }
                        else
                            depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
                    }
                    else
                    {
                        fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                                pszFilename, iLine, szFullname);
                        depMarkNotFound(pvRule);
                    }
                }
            }
            else
                /*
                 * #if
                 */
                if (strncmp(&szBuffer[i], "if", cchWord) == 0)
            {   /* #if 0 and #if <1-9> are supported */
                pszEndWord = findEndOfWord(pszArgument);
                iIfStack++;
                if ((pszEndWord - pszArgument) == 1
                    && *pszArgument >= '0' && *pszArgument <= '9')
                {
                    if (*pszArgument != '0')
                        achIfStack[iIfStack].fIncluded =  TRUE;
                    else
                        achIfStack[iIfStack].fIncluded =  FALSE;
                }
                else
                    achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
                achIfStack[iIfStack].fIf = TRUE;
            }
            else
                /*
                 * #else
                 */
                if (strncmp(&szBuffer[i], "else", cchWord) == 0)
            {
                if (achIfStack[iIfStack].fSupported)
                {
                    if (achIfStack[iIfStack].fIncluded) /* ARG!! this'll prevent warning */
                        achIfStack[iIfStack].fIncluded = FALSE;
                    else
                        achIfStack[iIfStack].fIncluded = TRUE;
                }
                achIfStack[iIfStack].fIf = FALSE;
            }
            else
                /*
                 * #endif
                 */
                if (strncmp(&szBuffer[i], "endif", cchWord) == 0)
            {   /* Pop the if-stack. */
                if (iIfStack > 0)
                    iIfStack--;
                else
                    fprintf(stderr, "%s(%d): If-Stack underflow!\n", pszFilename, iLine);
            }
            /*
             * general if<something> and elseif<something> implementations
             */
            else
                if (strncmp(&szBuffer[i], "elseif", 6) == 0)
            {
                achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
            }
            else
                if (strncmp(&szBuffer[i], "if", 2) == 0)
            {
                iIfStack++;
                achIfStack[iIfStack].fIf = TRUE;
                achIfStack[iIfStack].fSupported = FALSE;
                achIfStack[iIfStack].fIncluded = TRUE;
            }
            /* The rest of them aren't implemented yet.
            else if (strncmp(&szBuffer[i], "if") == 0)
            {
            }
            */
        } else
            /*
             * Check for resource compiler directives.
             */
            if (    !fComment
                &&  !strchr(&szBuffer[i], ',')
                &&  (   !strnicmp(&szBuffer[i], "ICON", 4)
                     || !strnicmp(&szBuffer[i], "FONT", 4)
                     || !strnicmp(&szBuffer[i], "BITMAP", 6)
                     || !strnicmp(&szBuffer[i], "POINTER", 7)
                     || !strnicmp(&szBuffer[i], "RESOURCE", 8)
                     || !(i1 = strnicmp(&szBuffer[i], "RCINCLUDE", 9))
                   /*|| !strnicmp(&szBuffer[i], "DLGINCLUDE", 10) - only used by the dlgeditor */
                     || !strnicmp(&szBuffer[i], "DEFAULTICON", 11)
                     )
                )
        {
            /*
             * RESOURCE 123 1 ["]filename.ext["]
             */
            char    szLine[1024];
            char *  pszFile;
            char    chQuote = ' ';

            PreProcessLine(szLine, &szBuffer[i]);

            pszFile = &szLine[strlen(szLine)-1];
            if (*pszFile == '\"' || *pszFile == '\'')
            {
                chQuote = *pszFile;
                *pszFile-- = '\0';
            }
            while (*pszFile != chQuote)
                pszFile--;
            *pszFile++ = '\0'; /* We now have extracted the filename - pszFile. */
            strlwr(pszFile);

            /* Add filename to the dependencies. */
            if (i1)
                depAddDepend(pvRule, pszFile, options.fCheckCyclic, FALSE);
            else
            {
                char *psz;
                /* find include file! */
                psz = pathlistFindFile(options.pszInclude, pszFile, szFullname);
                if (psz == NULL)
                    psz = pathlistFindFile(pszIncludeEnv, pszFile, szFullname);

                /* did we find the include? */
                if (psz != NULL)
                {
                    if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szFullname))
                    {   /* #include <sys/stats.h> makes trouble, check for '/' and '\'. */
                        if (!strchr(pszFile, '/') && !strchr(pszFile, '\\'))
                            depAddDepend(pvRule, pszFile, options.fCheckCyclic, FALSE);
                        else
                            fprintf(stderr, "%s(%d): warning include '%s' is ignored.\n",
                                    pszFilename, iLine, pszFile);
                    }
                    else
                        depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                }
                else
                {
                    fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                            pszFilename, iLine, pszFile);
                    depMarkNotFound(pvRule);
                }
            }
        }


        /*
         * Comment checks.
         *  -Start at first non-blank.
         *  -Loop thru the line since we might have more than one
         *   comment statement on a single line.
         */
        pszC = &szBuffer[i];
        while (pszC != NULL && *pszC != '\0')
        {
            if (fComment)
                pszC = strstr(pszC, "*/");  /* look for end comment mark. */
            else
            {
                char *pszLC;
                pszLC= strstr(pszC, "//");  /* look for single line comment mark. */
                pszC = strstr(pszC, "/*");  /* look for start comment mark */
                if (pszLC && pszLC < pszC)  /* if there is an single line comment mark before the */
                    break;                  /* muliline comment mark we'll ignore the multiline mark. */
            }

            /* Comment mark found? */
            if (pszC != NULL)
            {
                fComment = !fComment;
                pszC += 2;          /* skip comment mark */

                /* debug */
                /*
                if (fComment)
                    fprintf(stderr, "starts at line %d\n", iLine);
                else
                    fprintf(stderr, "ends   at line %d\n", iLine);
                    */
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);

    return 0;
}
#endif


/**
 * Generates depend info on this COBOL file, these are stored internally
 * and written to file later.
 * @returns 0 on success.
 *          !0 on error.
 * @param   pszFilename         Pointer to source filename. Correct case is assumed!
 * @param   pszNormFilename     Pointer to normalized source filename.
 * @param   pszTS               File time stamp.
 * @parma   fHeader             True if header file is being scanned.
 * @param   ppvRule             Variabel to return any new rule handle.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
int langCOBOL(const char *pszFilename, const char *pszNormFilename,
              const char *pszTS, BOOL fHeader, void **ppvRule)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszObjectExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszObjectExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename)
                            ? fileName(pszFilename, szBuffer) : fileNormalize2(pszFilename, szBuffer),
                         options.fCheckCyclic,
                         FALSE);
    }
    else
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        /* search for #include */
        int cbLen;
        int i = 0;
        int i1, i2;
        iLine++;

        /* check for comment mark (column 7) */
        if (szBuffer[6] == '*')
            continue;

        /* skip blank chars */
        cbLen = strlen(szBuffer);
        while (i + 9 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
            i++;

        /* is this an include? */
        if (   (i1 = strnicmp(&szBuffer[i], "COPY", 4)) == 0
            || (i2 = strnicmpwords(&szBuffer[i], "EXEC SQL INCLUDE", 16)) == 0
            )
        {
            char szFullname[CCHMAXPATH];
            char *psz;
            int  j;

            /* skip statement */
            i += 4;
            if (i1 != 0)
            {
                int y = 2; /* skip two words */
                do
                {
                    /* skip blanks */
                    while (szBuffer[i] == ' ' || szBuffer[i] == '\t')
                        i++;
                    /* skip word */
                    while (szBuffer[i] != ' ' && szBuffer[i] != '\t'
                           && szBuffer[i] != '\0' && szBuffer[i] != '\n')
                        i++;
                    y--;
                } while (y > 0);
            }

            /* check for blank */
            if (szBuffer[i] != ' ' && szBuffer[i] != '\t') /* no copybook specified... */
                continue;

            /* skip blanks */
            while (szBuffer[i] == ' ' || szBuffer[i] == '\t')
                i++;

            /* if invalid statement then continue with the next line! */
            if (szBuffer[i] == '\0' || szBuffer[i] == '\n')
                continue;

            /* find end */
            j = 0;
            while (i + j < cbLen && j < CCHMAXPATH
                   && szBuffer[i+j] != '.'
                   && szBuffer[i+j] != ' '  && szBuffer[i+j] != '\t'
                   && szBuffer[i+j] != '\0' && szBuffer[i+j] != '\n'
                   )
                j++;

            /* if invalid statement then continue with the next line! */
            if (szBuffer[i+j] != '.' && szBuffer[i+j] != ' ' && szBuffer[i] != '\t')
                continue;

            /* copy filename */
            strncpy(szFullname, &szBuffer[i], j);
            szFullname[j] = '\0'; /* ensure terminatition. */
            strlwr(szFullname);

            /* add extention .cpy - hardcoded for the moment. */
            strcpy(&szFullname[j], ".cbl");

            /* find include file! */
            psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
            if (!psz)
            {
                strcpy(&szFullname[j], ".cpy");
                psz = pathlistFindFile(options.pszInclude, szFullname, szBuffer);
            }

            /* did we find the include? */
            if (psz != NULL)
            {
                if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                    depAddDepend(pvRule, szFullname, options.fCheckCyclic, FALSE);
                else
                    depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
            }
            else
            {
                szFullname[j] = '\0';
                fprintf(stderr, "%s(%d): warning copybook '%s' was not found!\n",
                        pszFilename, iLine, szFullname);
                depMarkNotFound(pvRule);
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);

    return 0;
}


/**
 * Generates depend info on this IPF file, these are stored internally
 * and written to file later.
 * @returns 0 on success.
 *          !0 on error.
 * @param   pszFilename         Pointer to source filename. Correct case is assumed!
 * @param   pszNormFilename     Pointer to normalized source filename.
 * @param   pszTS               File time stamp.
 * @param   fHeader             True if header file is being scanned.
 * @param   ppvRule             Variabel to return any new rule handle.
 * @status  completely implemented.
 * @author  knut st. osmundsen
 */
int langIPF(  const char *pszFilename, const char *pszNormFilename,
              const char *pszTS, BOOL fHeader, void **ppvRule)
{
    void *  pvFile;                     /* Text buffer pointer. */
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */
    void *  pv = NULL;                  /* An index used by textbufferGetNextLine. */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    /*if (options.fObjRule && !fHeader)
    {
        if (options.fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, options.pszObjectExt, pszTS, FALSE);
        else
            pvRule = depAddRule(options.fObjectDir ?
                                    options.pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                options.pszObjectExt, pszTS, FALSE);

        if (options.fSrcWhenObj && pvRule)
            depAddDepend(pvRule,
                         options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename)
                            ? fileName(pszFilename, szBuffer) : fileNormalize2(pszFilename, szBuffer),
                         options.fCheckCyclic,
                         FALSE);
    }
    else */
        pvRule = depAddRule(options.fExcludeAll || pathlistFindFile2(options.pszExclude, pszNormFilename) ?
                            fileName(pszFilename, szBuffer) : pszNormFilename, NULL, NULL, pszTS, FALSE);

    /* duplicate rule? */
    *ppvRule = pvRule;
    if (pvRule == NULL)
        return 0;


    /********************/
    /* Make file buffer */
    /********************/
    pvFile = textbufferCreate(pszFilename);
    if (!pvFile)
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (textbufferGetNextLine(pvFile, &pv, szBuffer, sizeof(szBuffer)) != NULL) /* line loop */
    {
        iLine++;

        /* is this an imbed statement? */
        if (!strncmp(&szBuffer[0], ".im", 3))
        {
            char    szFullname[CCHMAXPATH];
            char *  psz;
            int     i;
            int     j;
            char    chQuote = 0;

            /* skip statement and blanks */
            i = 4;
            while (szBuffer[i] == ' ' || szBuffer[i] == '\t')
                i++;

            /* check for quotes */
            if (szBuffer[i] == '\'' || szBuffer[i] == '\"')
                chQuote = szBuffer[i++];

            /* find end */
            j = 0;
            if (chQuote != 0)
            {
                while (szBuffer[i+j] != chQuote && szBuffer[i+j] != '\n' && szBuffer[i+j] != '\r' && szBuffer[i+j] != '\0')
                    j++;
            }
            else
            {
                while (szBuffer[i+j] != '\n' && szBuffer[i+j] != '\r' && szBuffer[i+j] != '\0')
                    j++;
            }

            /* find end */
            if (j >= CCHMAXPATH)
            {
                fprintf(stderr, "%s(%d) warning: Filename too long ignored.\n", pszFilename, iLine);
                continue;
            }

            /* copy filename */
            strncpy(szFullname, &szBuffer[i], j);
            szFullname[j] = '\0'; /* ensure terminatition. */
            strlwr(szFullname);

            /* find include file! */
            psz = filecacheFileExist(szFullname, szBuffer);

            /* did we find the include? */
            if (psz != NULL)
            {
                if (options.fExcludeAll || pathlistFindFile2(options.pszExclude, szBuffer))
                    depAddDepend(pvRule, fileName(szFullname, szBuffer), options.fCheckCyclic, FALSE);
                else
                    depAddDepend(pvRule, szBuffer, options.fCheckCyclic, FALSE);
            }
            else
            {
                fprintf(stderr, "%s(%d): warning imbeded file '%s' was not found!\n",
                        pszFilename, iLine, szFullname);
                depMarkNotFound(pvRule);
            }
        }
    } /*while*/

    textbufferDestroy(pvFile);
    fHeader = fHeader;

    return 0;
}


#define upcase(ch)   \
     (ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch)

/**
 * Compares words. Multiple spaces are treates as on single blank i both string when comparing them.
 * @returns   0 equal. (same as strnicmp)
 * @param     pszS1  String 1
 * @param     pszS2  String 2
 * @param     cch    Length to compare (relative to string 1)
 * @author    knut st. osmundsen (bird@anduin.net)
 */
int strnicmpwords(const char *pszS1, const char *pszS2, int cch)
{
    do
    {
        while (cch > 0 && upcase(*pszS1) == upcase(*pszS2) && *pszS1 != ' ')
            pszS1++, pszS2++, cch--;

        /* blank test and skipping */
        if (cch > 0 && *pszS1 == ' ' && *pszS2 == ' ')
        {
            while (cch > 0 && *pszS1 == ' ')
                pszS1++, cch--;

            while (*pszS2 == ' ')
                pszS2++;
        }
        else
            break;
    } while (cch > 0);

    return cch == 0 ? 0 : *pszS1 - *pszS2;
}


/**
 * Normalizes the path slashes for the filename. It will partially expand paths too.
 * @returns   pszFilename
 * @param     pszFilename  Pointer to filename string. Not empty string!
 *                         Much space to play with.
 */
char *fileNormalize(char *pszFilename)
{
    char *psz = pszFilename;

    /* correct slashes */
    while ((pszFilename = strchr(pszFilename, '//')) != NULL)
        *pszFilename++ = '\\';

    /* expand path? */
    pszFilename = psz;
    if (pszFilename[1] != ':')
    {   /* relative path */
        int     iSlash;
        char    szFile[CCHMAXPATH];
        char *  psz = szFile;

        strcpy(szFile, pszFilename);
        iSlash = *psz == '\\' ? 1 : cSlashes;
        while (*psz != '\0')
        {
            if (*psz == '.' && psz[1] == '.'  && psz[2] == '\\')
            {   /* up one directory */
                if (iSlash > 0)
                    iSlash--;
                psz += 3;
            }
            else if (*psz == '.' && psz[1] == '\\')
            {   /* no change */
                psz += 2;
            }
            else
            {   /* completed expantion! */
                strncpy(pszFilename, szCurDir, aiSlashes[iSlash]+1);
                strcpy(pszFilename + aiSlashes[iSlash]+1, psz);
                break;
            }
        }
    }
    /* else: assume full path */

    return psz;
}


/**
 * Normalizes the path slashes for the filename. It will partially expand paths too.
 * Makes name all lower case too.
 * @returns   pszFilename
 * @param     pszFilename  Pointer to filename string. Not empty string!
 *                         Much space to play with.
 * @param     pszBuffer    Pointer to output buffer.
 */
char *fileNormalize2(const char *pszFilename, char *pszBuffer)
{
    char *  psz = pszBuffer;
    int     iSlash;

    if (pszFilename[1] != ':')
    {
        /* iSlash */
        if (*pszFilename == '\\' || *pszFilename == '/')
            iSlash = 1;
        else
            iSlash = cSlashes;

        /* interpret . and .. */
        while (*pszFilename != '\0')
        {
            if (*pszFilename == '.' && pszFilename[1] == '.'  && (pszFilename[2] == '\\' || pszFilename[1] == '/'))
            {   /* up one directory */
                if (iSlash > 0)
                    iSlash--;
                pszFilename += 3;
            }
            else if (*pszFilename == '.' && (pszFilename[1] == '\\' || pszFilename[1] == '/'))
            {   /* no change */
                pszFilename += 2;
            }
            else
            {   /* completed expantion! - TODO ..\ or .\ may appare within the remaining path too... */
                strncpy(pszBuffer, szCurDir, aiSlashes[iSlash]+1);
                strcpy(pszBuffer + aiSlashes[iSlash]+1, pszFilename);
                break;
            }
        }
    }
    else
    {   /* have drive letter specified - assume ok (TODO)*/
        strcpy(pszBuffer, pszFilename);
    }

    /* correct slashes */
    while ((pszBuffer = strchr(pszBuffer, '//')) != NULL)
        *pszBuffer++ = '\\';

    /* lower case it */
    /*strlwr(psz);*/

    return psz;
}


/**
 * Copies the path part (excluding the slash) into pszBuffer and returns
 * a pointer to the buffer.
 * If no path is found "" is returned.
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *filePath(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '\\');
    if (psz == NULL)
        psz = strrchr(pszFilename, '/');

    if (psz == NULL)
        *pszBuffer = '\0';
    else
    {
        strncpy(pszBuffer, pszFilename, psz - pszFilename);
        pszBuffer[psz - pszFilename] = '\0';
    }

    return pszBuffer;
}


/**
 * Copies the path part including the slash into pszBuffer and returns
 * a pointer to the buffer.
 * If no path is found "" is returned.
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *filePathSlash(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '\\');
    if (psz == NULL)
        psz = strrchr(pszFilename, '/');

    if (psz == NULL)
        *pszBuffer = '\0';
    else
    {
        strncpy(pszBuffer, pszFilename, psz - pszFilename + 1);
        pszBuffer[psz - pszFilename + 1] = '\0';
    }

    return pszBuffer;
}


/**
 * Copies the path part including the slash into pszBuffer and returns
 * a pointer to the buffer. If no path is found "" is returned.
 * The path is normalized to only use '\\'.
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *filePathSlash2(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '\\');
    if (psz == NULL)
        psz = strrchr(pszFilename, '/');

    if (psz == NULL)
        *pszBuffer = '\0';
    else
    {
        strncpy(pszBuffer, pszFilename, psz - pszFilename + 1);
        pszBuffer[psz - pszFilename + 1] = '\0';

        /* normalize all '/' to '\\' */
        psz = pszBuffer;
        while ((psz = strchr(psz, '/')) != NULL)
               *psz++ = '\\';
    }

    return pszBuffer;
}


/**
 * Copies the filename (with extention) into pszBuffer and returns
 * a pointer to the buffer.
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *fileName(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '\\');
    if (psz == NULL)
        psz = strrchr(pszFilename, '/');

    strcpy(pszBuffer, psz == NULL ? pszFilename : psz + 1);

    return pszBuffer;
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


/**
 * Copies the extention part into pszBuffer and returns
 * a pointer to the buffer.
 * If no extention is found "" is returned.
 * The dot ('.') is not included!
 * @returns   Pointer to pszBuffer with path.
 * @param     pszFilename  Pointer to readonly filename.
 * @param     pszBuffer    Pointer to output Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *fileExt(const char *pszFilename, char *pszBuffer)
{
    char *psz = strrchr(pszFilename, '.');
    if (psz != NULL)
    {
        if (strchr(psz, '\\') != NULL || strchr(psz, '/') != NULL)
            *pszBuffer = '\0';
        else
            strcpy(pszBuffer, psz + 1);
    }
    else
        *pszBuffer = '\0';

    return pszBuffer;
}


/**
 * Adds a file to the cache.
 * @returns   Success indicator.
 * @param     pszFilename   Name of the file which is to be added. (with path!)
 */
BOOL filecacheAddFile(const char *pszFilename)
{
    PFCACHEENTRY pfcNew;

    /* allocate new block and fill in data */
    pfcNew = malloc(sizeof(FCACHEENTRY) + strlen(pszFilename) + 1);
    if (pfcNew == NULL)
    {
        fprintf(stderr, "error: out of memory! (line=%d)\n", __LINE__);
        return FALSE;
    }
    pfcNew->Key = (char*)(void*)pfcNew + sizeof(FCACHEENTRY);
    strcpy((char*)(unsigned)pfcNew->Key, pszFilename);
    if (!AVLInsert(&pfcTree, pfcNew))
    {
        free(pfcNew);
        return TRUE;
    }
    cfcNodes++;

    return TRUE;
}


/**
 * Adds a file to the cache.
 * @returns   Success indicator.
 * @param     pszDir   Name of the path which is to be added. (with slash!)
 */
BOOL filecacheAddDir(const char *pszDir)
{
    PFCACHEENTRY    pfcNew;
    APIRET          rc;
    char            szDir[CCHMAXPATH];
    int             cchDir;
    char            achBuffer[32768];
    PFILEFINDBUF3   pfindbuf3 = (PFILEFINDBUF3)(void*)&achBuffer[0];
    HDIR            hDir = HDIR_CREATE;
    ULONG           cFiles = 0xFFFFFFF;
    int             i;

    /* Make path */
    filePathSlash2(pszDir, szDir);
    //strlwr(szDir); /* Convert name to lower case to allow faster searchs! */
    cchDir = strlen(szDir);


    /* Add directory to pfcDirTree. */
    pfcNew = malloc(sizeof(FCACHEENTRY) + cchDir + 1);
    if (pfcNew == NULL)
    {
        fprintf(stderr, "error: out of memory! (line=%d)\n", __LINE__);
        DosFindClose(hDir);
        return FALSE;
    }
    pfcNew->Key = (char*)(void*)pfcNew + sizeof(FCACHEENTRY);
    strcpy((char*)(unsigned)pfcNew->Key, szDir);
    AVLInsert(&pfcDirTree, pfcNew);


    /* Start to search directory - all files */
    strcat(szDir + cchDir, "*");
    rc = DosFindFirst(szDir, &hDir, FILE_NORMAL,
                      pfindbuf3, sizeof(achBuffer),
                      &cFiles, FIL_STANDARD);
    while (rc == NO_ERROR)
    {
        for (i = 0;
             i < cFiles;
             i++, pfindbuf3 = (PFILEFINDBUF3)((int)pfindbuf3 + pfindbuf3->oNextEntryOffset)
             )
        {
            pfcNew = malloc(sizeof(FCACHEENTRY) + cchDir + pfindbuf3->cchName + 1);
            if (pfcNew == NULL)
            {
                fprintf(stderr, "error: out of memory! (line=%d)\n", __LINE__);
                DosFindClose(hDir);
                return FALSE;
            }
            pfcNew->Key = (char*)(void*)pfcNew + sizeof(FCACHEENTRY);
            strcpy((char*)(unsigned)pfcNew->Key, szDir);
            strcpy((char*)(unsigned)pfcNew->Key + cchDir, pfindbuf3->achName);
            strlwr((char*)(unsigned)pfcNew->Key + cchDir); /* Convert name to lower case to allow faster searchs! */
            if (!AVLInsert(&pfcTree, pfcNew))
                free(pfcNew);
            else
                cfcNodes++;
        }

        /* next */
        cFiles = 0xFFFFFFF;
        pfindbuf3 = (PFILEFINDBUF3)(void*)&achBuffer[0];
        rc = DosFindNext(hDir, pfindbuf3, sizeof(achBuffer), &cFiles);
    }

    DosFindClose(hDir);

    return TRUE;
}


/**
 * Checks if pszFilename is exists in the cache.
 * @return    TRUE if found. FALSE if not found.
 * @param     pszFilename   Name of the file to be found. (with path!)
 *                          This is in lower case!
 */
INLINE BOOL filecacheFind(const char *pszFilename)
{
    return AVLGet(&pfcTree, (AVLKEY)pszFilename) != NULL;
}


/**
 * Checks if pszFilename is exists in the cache.
 * @return    TRUE if found. FALSE if not found.
 * @param     pszFilename   Name of the file to be found. (with path!)
 *                          This is in lower case!
 */
INLINE BOOL filecacheIsDirCached(const char *pszDir)
{
    return AVLGet(&pfcDirTree, (AVLKEY)pszDir) != NULL;
}


/**
 * Checks if a file exist, uses file cache if possible.
 * @returns   Pointer to a filename consiting of the path part + the given filename.
 *            (pointer into pszBuffer)
 *            NULL if file is not found. ("" in buffer)
 * @parma     pszFilename  Filename to find.
 * @parma     pszBuffer    Ouput Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *filecacheFileExist(const char *pszFilename, char *pszBuffer)
{
    APIRET          rc;

    *pszBuffer = '\0';

    fileNormalize2(pszFilename, pszBuffer);

    /*
     * Search for the file in this directory.
     *   Search cache first
     */
    if (!filecacheFind(pszBuffer))
    {
        char szDir[CCHMAXPATH];

        filePathSlash(pszBuffer, szDir);
        if (!filecacheIsDirCached(szDir))
        {
            /*
             * If caching of entire dirs are enabled, we'll
             * add the directory to the cache and search it.
             */
            if (options.fCacheSearchDirs && filecacheAddDir(szDir))
            {
                if (filecacheFind(pszBuffer))
                    return pszBuffer;
            }
            else
            {
                FILESTATUS3 fsts3;

                /* ask the OS */
                rc = DosQueryPathInfo(pszBuffer, FIL_STANDARD, &fsts3, sizeof(fsts3));
                if (rc == NO_ERROR)
                {   /* add file to cache. */
                    filecacheAddFile(pszBuffer);
                    return pszBuffer;
                }
            }
        }
    }
    else
        return pszBuffer;

    return NULL;
}


/**
 * Finds a filename in a specified pathlist.
 * @returns   Pointer to a filename consiting of the path part + the given filename.
 *            (pointer into pszBuffer)
 *            NULL if file is not found. ("" in buffer)
 * @param     pszPathList  Path list to search for filename.
 * @parma     pszFilename  Filename to find.
 * @parma     pszBuffer    Ouput Buffer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
char *pathlistFindFile(const char *pszPathList, const char *pszFilename, char *pszBuffer)
{
    const char *psz = pszPathList;
    const char *pszNext = NULL;

    *pszBuffer = '\0';

    if (pszPathList == NULL)
        return NULL;

    while (*psz != '\0')
    {
        /* find end of this path */
        pszNext = strchr(psz, ';');
        if (pszNext == NULL)
            pszNext = psz + strlen(psz);

        if (pszNext - psz > 0)
        {
            APIRET          rc;

            /* make search statment */
            strncpy(pszBuffer, psz, pszNext - psz);
            pszBuffer[pszNext - psz] = '\0';
            if (pszBuffer[pszNext - psz - 1] != '\\' && pszBuffer[pszNext - psz - 1] != '/')
                strcpy(&pszBuffer[pszNext - psz], "\\");
            strcat(pszBuffer, pszFilename);
            fileNormalize(pszBuffer);

            /*
             * Search for the file in this directory.
             *   Search cache first
             */
            if (!filecacheFind(pszBuffer))
            {
                char szDir[CCHMAXPATH];

                filePathSlash(pszBuffer, szDir);
                if (!filecacheIsDirCached(szDir))
                {
                    /*
                     * If caching of entire dirs are enabled, we'll
                     * add the directory to the cache and search it.
                     */
                    if (options.fCacheSearchDirs && filecacheAddDir(szDir))
                    {
                        if (filecacheFind(pszBuffer))
                            return pszBuffer;
                    }
                    else
                    {
                        FILESTATUS3 fsts3;

                        /* ask the OS */
                        rc = DosQueryPathInfo(pszBuffer, FIL_STANDARD, &fsts3, sizeof(fsts3));
                        if (rc == NO_ERROR)
                        {   /* add file to cache. */
                            filecacheAddFile(pszBuffer);
                            return pszBuffer;
                        }
                    }
                }
            }
            else
                return pszBuffer;
        }

        /* next */
        if (*pszNext != ';')
            break;
        psz = pszNext + 1;
    }

    return NULL;
}


/**
 * Checks if the given filename may exist within any of the given paths.
 * This check only matches the filename path agianst the paths in the pathlist.
 * @returns   TRUE: if exists.
 *            FALSE: don't exist.
 * @param     pszPathList  Path list to search for filename.
 * @parma     pszFilename  Filename to find. The filename should be normalized!
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL pathlistFindFile2(const char *pszPathList, const char *pszFilename)
{
    const char *psz = pszPathList;
    const char *pszNext = NULL;
    char        szBuffer[CCHMAXPATH];
    char        szBuffer2[CCHMAXPATH];
    char       *pszPathToFind = &szBuffer2[0];

    /*
     * Input checking
     */
    if (pszPathList == NULL)
        return FALSE;

    /*
     * Normalize the filename and get it's path.
     */
    filePath(pszFilename, pszPathToFind);


    /*
     * Loop thru the path list.
     */
    while (*psz != '\0')
    {
        /* find end of this path */
        pszNext = strchr(psz, ';');
        if (pszNext == NULL)
            pszNext = psz + strlen(psz);

        if (pszNext - psz > 0)
        {
            char *  pszPath = &szBuffer[0];

            /*
             * Extract and normalize the path
             */
            strncpy(pszPath, psz, pszNext - psz);
            pszPath[pszNext - psz] = '\0';
            if (pszPath[pszNext - psz - 1] == '\\' && pszPath[pszNext - psz - 1] == '/')
                pszPath[pszNext - psz - 1] = '\0';
            fileNormalize(pszPath);

            /*
             * Check if it matches the path of the filename
             */
            if (strcmp(pszPath, pszPathToFind) == 0)
                return TRUE;
        }

        /*
         * Next part of the path list.
         */
        if (*pszNext != ';')
            break;
        psz = pszNext + 1;
    }

    return FALSE;
}


/**
 * Finds the first char after word.
 * @returns   Pointer to the first char after word.
 * @param     psz  Where to start.
 * @author    knut st. osmundsen (bird@anduin.net)
 */
char *findEndOfWord(char *psz)
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

#if 0 /* not used */
/**
 * Find the starting char of a word
 * @returns   Pointer to first char in word.
 * @param     psz       Where to start.
 * @param     pszStart  Where to stop.
 * @author    knut st. osmundsen (bird@anduin.net)
 */
char *findStartOfWord(const char *psz, const char *pszStart)
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
#endif

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
 * Trims a string, ie. removing spaces (and tabs) from both ends of the string.
 * @returns   Pointer to first not space or tab char in the string.
 * @param     psz   Pointer to the string which is to be trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (bird@anduin.net)
 */
INLINE char *trim(char *psz)
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
 * Right trims a string, ie. removing spaces (and tabs) from the end of the stri
 * @returns   Pointer to the string passed in.
 * @param     psz   Pointer to the string which is to be right trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (bird@anduin.net)
 */
INLINE char *trimR(char *psz)
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
 * Trims any quotes of a possibly quoted string.
 * @returns   Pointer to the string passed in.
 * @param     psz   Pointer to the string which is to be quote-trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (bird@anduin.net)
 */
INLINE char *trimQuotes(char *psz)
{
    int i;
    if (psz == NULL)
        return NULL;

    if (*psz == '\"' || *psz == '\'')
        psz++;
    i = strlen(psz) - 1;
    if (psz[i] == '\"' || psz[i] == '\'')
        psz[i] = '\0';

    return psz;
}


/**
 * C/C++ preprocess a single line. Assumes that we're not starting
 * with at comment.
 * @returns Pointer to output buffer.
 * @param   pszOut  Ouput (preprocessed) string.
 * @param   pszIn   Input string.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
char *PreProcessLine(char *pszOut, const char *pszIn)
{
    char *  psz = pszOut;
    BOOL    fComment = FALSE;
    BOOL    fQuote = FALSE;

    /*
     * Loop thru the string.
     */
    while (*pszIn != '\0')
    {
        if (fQuote)
        {
            *psz++ = *pszIn;
            if (*pszIn == '\\')
            {
                *psz++ = *++pszIn;
                pszIn++;
            }
            else if (*pszIn++ == '"')
                fQuote = FALSE;
        }
        else if (fComment)
        {
            if (*pszIn == '*' && pszIn[1] == '/')
            {
                fComment = FALSE;
                pszIn += 2;
            }
            else
                pszIn++;
        }
        else
        {
            if (   (*pszIn == '/' && pszIn[1] == '/')
                ||  *pszIn == '\0')
            {   /* End of line. */
                break;
            }

            if (*pszIn == '/' && pszIn[1] == '*')
            {   /* Start comment */
                fComment = TRUE;
                pszIn += 2;
            }
            else
                *psz++ = *pszIn++;
        }
    }

    /*
     * Trim right.
     */
    psz--;
    while (psz >= pszOut && (*psz == ' ' || *psz == '\t'))
        psz--;
    psz[1] = '\0';

    return pszOut;
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
        psz = pvBuffer;

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
 * Appends a depend file to the internal file.
 * This will update the date in the option struct.
 */
BOOL  depReadFile(const char *pszFilename, BOOL fAppend)
{
    void *      pvFile;
    char *      pszNext;
    char *      pszPrev;                /* Previous line, only valid when finding new rule. */
    BOOL        fMoreDeps = FALSE;
    void *      pvRule = NULL;


    /* read depend file */
    pvFile = textbufferCreate(pszFilename);
    if (pvFile == NULL)
        return FALSE;

    /* parse the original depend file */
    pszPrev = NULL;
    pszNext = pvFile;
    while (*pszNext != '\0')
    {
        int   i;
        int   cch;
        char *psz;

        /* get the next line. */
        psz = pszNext;
        pszNext = textbufferNextLine(pvFile, pszNext);

        /*
         * Process the current line:
         *   Start off by terminating the line.
         *   Trim the line,
         *   Skip empty lines.
         *   If not looking for more deps Then
         *     Check if new rule starts here.
         *   Endif
         *
         *   If more deps to last rule Then
         *     Get dependant name.
         *   Endif
         */
        i = -1;
        while (psz <= &pszNext[i] && pszNext[i] == '\n' || pszNext[i] == '\r')
            pszNext[i--] = '\0';
        trimR(psz);
        cch = strlen(psz);
        if (cch == 0)
        {
            fMoreDeps = FALSE;
            continue;
        }

        if (*psz == '#')
        {
            pszPrev = psz;
            continue;
        }

        /* new rule? */
        if (!fMoreDeps)
        {
            if (*psz != ' ' && *psz != '\t' && *psz != '\0')
            {
                i = 0;
                while (psz[i] != '\0')
                {
                    if (psz[i] == ':'
                        && (psz[i+1] == ' '
                            || psz[i+1] == '\t'
                            || psz[i+1] == '\0'
                            || (psz[i+1] == '\\' && psz[i+2] == '\0')
                            )
                        )
                    {
                        char    szTS[TS_SIZE];
                        char *  pszCont = strchr(&psz[i], '\\');
                        fMoreDeps = pszCont != NULL && pszCont[1] == '\0';

                        /* read evt. timestamp. */
                        szTS[0] = '\0';
                        if (pszPrev && strlen(pszPrev) > 25 && *pszPrev == '#')
                            strcpy(szTS, pszPrev + 2);

                        psz[i] = '\0';
                        pvRule = depAddRule(trimQuotes(trimR(psz)), NULL, NULL, szTS, TRUE);
                        if (pvRule)
                            ((PDEPRULE)pvRule)->fUpdated = fAppend;
                        psz += i + 1;
                        cch -= i + 1;
                        break;
                    }
                    i++;
                }
            }
            pszPrev = NULL;
        }


        /* more dependants */
        if (fMoreDeps)
        {
            if (cch > 0 && psz[cch-1] == '\\')
            {
                fMoreDeps = TRUE;
                psz[cch-1] = '\0';
            }
            else
                fMoreDeps = FALSE;

            /* if not duplicate rule */
            if (pvRule != NULL)
            {
                psz = trimQuotes(trim(psz));
                if (*psz != '\0')
                    depAddDepend(pvRule, psz, options.fCheckCyclic, TRUE);
            }
        }
    } /* while */


    /* return succesfully */
    textbufferDestroy(pvFile);
    return TRUE;
}

/**
 *
 * @returns   Success indicator.
 * @param     pszFilename           Pointer to name of the output file.
 * @param     fWriteUpdatedOnly     If set we'll only write updated rules.
 */
BOOL  depWriteFile(const char *pszFilename, BOOL fWriteUpdatedOnly)
{
    FILE *phFile;
    phFile = fopen(pszFilename, "w");
    if (phFile != NULL)
    {
        AVLENUMDATA EnumData;
        PDEPRULE    pdep;
        static char szBuffer[0x10000];
        int         iBuffer = 0;
        int         cch;

        /*
         * Write warning on top of file.
         */
        fputs("#\n"
              "# This file was automatically generated by FastDep.\n"
              "# FastDep was written by knut st. osmundsen, and it's GPL software.\n"
              "#\n"
              "# THIS FILE SHOULD   N O T   BE EDITED MANUALLY!!!\n"
              "#\n"
              "# (As this may possibly make it unreadable for fastdep\n"
              "#  and ruin the caching methods of FastDep.)\n"
              "#\n"
              "\n",
              phFile);

        /* normal dependency output */
        pdep = (PDEPRULE)(void*)AVLBeginEnumTree((PPAVLNODECORE)(void*)&pdepTree, &EnumData, TRUE);
        while (pdep != NULL)
        {
            if (!fWriteUpdatedOnly || pdep->fUpdated)
            {
                int cchTS = strlen(pdep->szTS);
                int fQuoted = strpbrk(pdep->pszRule, " \t") != NULL; /* TODO/BUGBUG/FIXME: are there more special chars to look out for?? */

                /* Write rule. Flush the buffer first if necessary. */
                cch = strlen(pdep->pszRule);
                if (iBuffer + cch*3 + fQuoted * 2 + cchTS + 9 >= sizeof(szBuffer))
                {
                    fwrite(szBuffer, iBuffer, 1, phFile);
                    iBuffer = 0;
                }

                memcpy(szBuffer + iBuffer, "# ", 2);
                memcpy(szBuffer + iBuffer + 2, pdep->szTS, cchTS);
                iBuffer += cchTS + 2;
                szBuffer[iBuffer++] = '\n';

                if (fQuoted) szBuffer[iBuffer++] = '"';
                iBuffer += depNameToMake(szBuffer + iBuffer, sizeof(szBuffer) - iBuffer, pdep->pszRule);
                if (fQuoted) szBuffer[iBuffer++] = '"';
                strcpy(szBuffer + iBuffer++, ":");

                /* write rule dependants. */
                if (pdep->papszDep != NULL)
                {
                    char **ppsz = pdep->papszDep;
                    while (*ppsz != NULL)
                    {
                        /* flush buffer? */
                        fQuoted = strpbrk(*ppsz, " \t") != NULL; /* TODO/BUGBUG/FIXME: are there more special chars to look out for?? */
                        cch = strlen(*ppsz);
                        if (iBuffer + cch*3 + fQuoted * 2 + 20 >= sizeof(szBuffer))
                        {
                            fwrite(szBuffer, iBuffer, 1, phFile);
                            iBuffer = 0;
                        }
                        strcpy(szBuffer + iBuffer, " \\\n    ");
                        iBuffer += 7;
                        if (fQuoted) szBuffer[iBuffer++] = '"';
                        iBuffer += depNameToMake(szBuffer + iBuffer, sizeof(szBuffer) - iBuffer, *ppsz);
                        if (fQuoted) szBuffer[iBuffer++] = '"';

                        /* next dependant */
                        ppsz++;
                    }
                }

                /* Add two new lines. Flush buffer first if necessary. */
                if (iBuffer + CBNEWLINE*2 >= sizeof(szBuffer))
                {
                    fwrite(szBuffer, iBuffer, 1, phFile);
                    iBuffer = 0;
                }

                /* add 2 linefeeds */
                strcpy(szBuffer + iBuffer, "\n\n");
                iBuffer += CBNEWLINE*2;
            }

            /* next rule */
            pdep = (PDEPRULE)(void*)AVLGetNextNode(&EnumData);
        }


        /* flush buffer. */
        fwrite(szBuffer, iBuffer, 1, phFile);

        fclose(phFile);
        return TRUE;
    }

    return FALSE;
}


/**
 * Removes all nodes in the tree of dependencies. (pdepTree)
 */
void  depRemoveAll(void)
{
    AVLENUMDATA EnumData;
    PDEPRULE    pdep;

    pdep = (PDEPRULE)(void*)AVLBeginEnumTree((PPAVLNODECORE)(void*)&pdepTree, &EnumData, TRUE);
    while (pdep != NULL)
    {
        /* free this */
        if (pdep->papszDep != NULL)
        {
            char ** ppsz = pdep->papszDep;
            while (*ppsz != NULL)
                free(*ppsz++);
            free(pdep->papszDep);
        }
        free(pdep);

        /* next */
        pdep = (PDEPRULE)(void*)AVLGetNextNode(&EnumData);
    }
    pdepTree = NULL;
}


/**
 * Adds a rule to the list of dependant rules.
 * @returns   Rule handle. NULL if rule exists/error.
 * @param     pszRulePath   Pointer to rule text. Empty strings are banned!
 *                          This string might only contain the path of the rule. (with '\\')
 * @param     pszName       Name of the rule.
 *                          NULL if pszRulePath contains the entire rule.
 * @param     pszExt        Extention (without '.')
 *                          NULL if pszRulePath or pszRulePath and pszName contains the entire rule.
 * @param     fConvertName  If set we'll convert from makefile name to realname.
 */
void *depAddRule(const char *pszRulePath, const char *pszName, const char *pszExt, const char *pszTS, BOOL fConvertName)
{
    char     szRule[CCHMAXPATH*2];
    PDEPRULE pNew;
    int      cch;

    /* make rulename */
    strcpy(szRule, pszRulePath);
    cch = strlen(szRule);
    if (pszName != NULL)
    {
        strcpy(szRule + cch, pszName);
        cch += strlen(szRule + cch);
    }
    if (pszExt != NULL)
    {
        strcat(szRule + cch++, ".");
        strcat(szRule + cch, pszExt);
        cch += strlen(szRule + cch);
    }
    if (fConvertName)
        cch = depNameToReal(szRule);

    /*
     * Allocate a new rule structure and fill in data
     * Note. One block for both the DEPRULE and the pszRule string.
     */
    pNew = malloc(sizeof(DEPRULE) + cch + 1);
    if (pNew == NULL)
    {
        fprintf(stderr, "error: out of memory. (line=%d)\n", __LINE__);
        return NULL;
    }
    pNew->pszRule = (char*)(void*)(pNew + 1);
    strcpy(pNew->pszRule, szRule);
    pNew->cDeps = 0;
    pNew->papszDep = NULL;
    pNew->fUpdated = TRUE;
    pNew->avlCore.Key = pNew->pszRule;
    strcpy(pNew->szTS, pszTS);

    /* Insert the rule */
    if (!AVLInsert((PPAVLNODECORE)(void*)&pdepTree, &pNew->avlCore))
    {   /*
         * The rule existed.
         * If it's allready touched (updated) during this session
         *   there is nothing to be done.
         * If not force scan and it's newer than depfile-1month then
         *   we'll use the information we've got.
         * Reuse the node in the tree.
         */
        PDEPRULE    pOld = (PDEPRULE)(void*)AVLGet((PPAVLNODECORE)(void*)&pdepTree, pNew->avlCore.Key);
        assert(pOld);
        free(pNew);
        if (pOld->fUpdated)
            return NULL;

        pOld->fUpdated = TRUE;
        if (!options.fForceScan && !strcmp(pOld->szTS, pszTS) && depValidate(pOld))
            return NULL;
        strcpy(pOld->szTS, pszTS);

        if (pOld->papszDep)
        {
            free(pOld->papszDep);
            pOld->papszDep = NULL;
        }
        pOld->cDeps = 0;

        return pOld;
    }

    return pNew;
}


/**
 * Adds a dependant to a rule.
 * @returns   Successindicator. TRUE = success.
 *            FALSE = cyclic or out of memory.
 * @param     pvRule        Rule handle.
 * @param     pszDep        Pointer to dependant name
 * @param     fCheckCyclic  When set we'll check that we're not creating an cyclic dependency.
 * @param     fConvertName  If set we'll convert from makefile name to realname.
 */
BOOL  depAddDepend(void *pvRule, const char *pszDep, BOOL fCheckCyclic, BOOL fConvertName)
{
    PDEPRULE    pdep = (PDEPRULE)pvRule;
    int         cchDep;

    if (pszDep[0] == '\0')
    {
        fprintf(stderr, "warning-internal: empty dependancy filename to '%s'. Ignored.\n",
                pdep->pszRule);
        /* __interrupt(3); */
        return FALSE;
    }

    if (fCheckCyclic && depCheckCyclic(pdep, pszDep))
    {
        fprintf(stderr, "warning: Cylic dependancy caused us to ignore '%s' in rule '%s'.\n",
                pszDep, pdep->pszRule);
        return FALSE;
    }

    /* allocate more array space */
    if (((pdep->cDeps) % 48) == 0)
    {
        pdep->papszDep = realloc(pdep->papszDep, sizeof(char*) * (pdep->cDeps + 50));
        if (pdep->papszDep == NULL)
        {
            pdep->cDeps = 0;
            fprintf(stderr, "error: out of memory, (line=%d)\n", __LINE__);
            return FALSE;
        }
    }

    /* allocate string space and copy pszDep */
    cchDep = strlen(pszDep) + 1;
    if ((pdep->papszDep[pdep->cDeps] = malloc(cchDep)) == NULL)
    {
        fprintf(stderr, "error: out of memory, (line=%d)\n", __LINE__);
        return FALSE;
    }
    strcpy(pdep->papszDep[pdep->cDeps], pszDep);

    /* convert ^# and other stuff */
    if (fConvertName)
        depNameToReal(pdep->papszDep[pdep->cDeps]);

    /* terminate array and increment dep count */
    pdep->papszDep[++pdep->cDeps] = NULL;

    /* successful! */
    return TRUE;
}


/**
 * Converts from makefile filename to real filename.
 * @returns New name length.
 * @param   pszName     Pointer to the string to make real.
 */
int depNameToReal(char *pszName)
{
    int cchNewName = strlen(pszName);
    int iDisplacement = 0;

    /*
     * Look for '^' and '$$'.
     */
    while (*pszName)
    {
        if (    *pszName == '^'
            ||  (*pszName == '$' && pszName[1] == '$'))
        {
            iDisplacement--;
            pszName++;
            cchNewName--;
        }
        if (iDisplacement)
            pszName[iDisplacement] = *pszName;
        pszName++;
    }
    pszName[iDisplacement] = '\0';

    return cchNewName;
}


/**
 * Converts from real filename to makefile filename.
 * @returns New name length.
 * @param   pszName     Output name buffer.
 * @param   cchName     Size of name buffer.
 * @param   pszSrc      Input name.
 */
int   depNameToMake(char *pszName, int cchName, const char *pszSrc)
{
    char *pszNameOrg = pszName;

    /*
     * Convert real name to makefile name.
     */
    while (*pszSrc)
    {
        if (    *pszSrc == '#'
            ||  *pszSrc == '!'
            ||  (*pszSrc == '$' && pszSrc[1] != '(')
            ||  *pszSrc == '@'
            ||  *pszSrc == '-'
            ||  *pszSrc == '^'
           /* ||  *pszSrc == '('
            ||  *pszSrc == ')'
            ||  *pszSrc == '{'
            ||  *pszSrc == '}'*/)
        {
            if (!cchName--)
            {
                fprintf(stderr, "error: buffer too small, (line=%d)\n", __LINE__);
                return pszName - pszNameOrg + strlen(pszName);
            }
            *pszName++ = '^';
        }
        if (!cchName--)
        {
            fprintf(stderr, "error: buffer too small, (line=%d)\n", __LINE__);
            return pszName - pszNameOrg + strlen(pszName);
        }
        *pszName++ = *pszSrc++;
    }
    *pszName = '\0';

    return pszName - pszNameOrg;
}



/**
 * Marks the file as one which is to be rescanned next time
 * since not all dependencies was found...
 * @param   pvRule  Rule handle...
 */
void  depMarkNotFound(void *pvRule)
{
    ((PDEPRULE)pvRule)->szTS[0] = '\0';
}


/**
 * Checks if adding this dependent will create a cyclic dependency.
 * @returns   TRUE: Cyclic.
 *            FALSE: Non-cylic.
 * @param     pdepRule  Rule pszDep is to be inserted in.
 * @param     pszDep    Depend name.
 */
BOOL depCheckCyclic(PDEPRULE pdepRule, const char *pszDep)
{
#define DEPTH_FIRST 1
#ifdef DEPTH_FIRST
    #define DEPTH 32
#else
    #define DEPTH 128
#endif
    #define HISTORY 256
    char *  pszRule = pdepRule->pszRule;
    char ** appsz[DEPTH];
#if HISTORY
    char *  apszHistory[HISTORY];
    int     iHistory;
    int     j;
    int     iStart;
    int     iEnd;
    int     iCmp;
#endif
    PDEPRULE pdep;
    int     i;

    /* self check */
    if (strcmp(pdepRule->pszRule, pszDep) == 0)
        return TRUE;

    /* find rule for the dep. */
    if ((pdep = (PDEPRULE)(void*)AVLGet((PPAVLNODECORE)(void*)&pdepTree, pszDep)) == NULL
        || pdep->papszDep == NULL)
        return FALSE; /* no rule, or no dependents, not cyclic */

    i = 1;
    appsz[0] = pdep->papszDep;
#ifdef HISTORY
    iHistory = 1;
    apszHistory[0] = pdep->pszRule;
#endif
    while (i > 0)
    {
        /* pop off element */
        register char **  ppsz = appsz[--i];

        while (*ppsz != NULL)
        {
            /* check if equal to the main rule */
            if (strcmp(pszRule, *ppsz) == 0)
                return TRUE;

            /* push onto stack (ppsz is incremented in this test!) */
            if ((pdep = (PDEPRULE)(void*)AVLGet((PPAVLNODECORE)(void*)&pdepTree, *ppsz++)) != NULL
                && pdep->papszDep != NULL)
            {
                if (i >= DEPTH)
                {
                    fprintf(stderr, "error: too deep chain (%d). pszRule=%s  pszDep=%s\n",
                            i, pszRule, pszDep);
                    return FALSE;
                }
#ifdef HISTORY
                /*
                 * Check if in history, if so we'll skip it.
                 */
                #if 0
                for (j = 0;  j < iHistory; j++)
                    if (!strcmp(apszHistory[j], pdep->pszRule))
                        break;
                if (j != iHistory)
                    continue;           /* found */

                /*
                 * Push into history - might concider make this binary sorted one day.
                 */
                if (iHistory < HISTORY)
                    apszHistory[iHistory++] = pdep->pszRule;

                #else

                /*
                 * Check if in history, if so we'll skip it.
                 *  (Binary search)
                 * ASSUMES: Always something in the history!
                 */
                iEnd = iHistory - 1;
                iStart = 0;
                j = iHistory / 2;
                while (    (iCmp = strcmp(pdep->pszRule, apszHistory[j])) != 0
                       &&   iEnd != iStart)
                {
                    if (iCmp < 0)
                        iEnd = j - 1;
                    else
                        iStart = j + 1;
                    if (iStart > iEnd)
                        break;
                    j = (iStart + iEnd) / 2;
                }

                if (!iCmp)
                    continue;           /* found */

                /*
                 * Push into history - might concider make this binary sorted one day.
                 */
                if (iHistory < HISTORY)
                {
                    int k;
                    if (iCmp > 0)       /* Insert after. */
                        j++;
                    for (k = iHistory; k > j; k--)
                        apszHistory[k] = apszHistory[k - 1];
                    apszHistory[j] = pdep->pszRule;
                    iHistory++;
                }

                #endif

#endif
                /*
                 * Push on to the stack.
                 */
                #ifdef DEPTH_FIRST
                /* dept first */
                appsz[i++] = ppsz;      /* save current posistion */
                ppsz = pdep->papszDep;  /* process new node */
                #else
                /* complete current node first. */
                appsz[i++] = pdep->papszDep;
                #endif
            }
        }
    }

    return FALSE;
}


/**
 * Validates that the dependencies for the file exists
 * in the given locations. Dependants without path is ignored.
 * @returns TRUE if all ok.
 *          FALSE if one (or possibly more) dependants are non-existing.
 * @param   pdepRule    Pointer to rule we're to validate.
 */
BOOL depValidate(PDEPRULE pdepRule)
{
    int i;

    for (i = 0; i < pdepRule->cDeps; i++)
    {
        char *psz = pdepRule->papszDep[i];
        if (    !strchr(psz, '$')
            &&
            (   psz[1] == ':'
            ||  strchr(psz, '\\')
            ||  strchr(psz, '/')
                 )
            )
        {
            /*
             * Check existance of the file.
             *   Search cache first
             */
            if (!filecacheFind(psz))
            {
                char szDir[CCHMAXPATH];

                filePathSlash(psz, szDir);
                if (!filecacheIsDirCached(szDir))
                {
                    /*
                     * If caching of entire dirs are enabled, we'll
                     * add the directory to the cache and search it.
                     */
                    if (options.fCacheSearchDirs && filecacheAddDir(szDir))
                    {
                        if (!filecacheFind(psz))
                            return FALSE;
                    }
                    else
                    {
                        FILESTATUS3 fsts3;

                        /* ask the OS */
                        if (DosQueryPathInfo(psz, FIL_STANDARD, &fsts3, sizeof(fsts3)))
                            return FALSE;
                        /* add file to cache. */
                        filecacheAddFile(psz);
                    }
                }
                /*
                 * Dir was cached, hence the file doesn't exist
                 * and the we should rescan the source file.
                 */
                else
                    return FALSE;
            }
        }
    }

    return TRUE;
}


/**
 * Make a timestamp from the file data provided thru the
 * search API.
 * @returns Pointer to pszTS
 * @param   pszTS       Pointer to timestamp (output).
 * @param   pfindbuf3   Pointer to search result.
 */
INLINE char *depMakeTS(char *pszTS, PFILEFINDBUF3 pfindbuf3)
{
    sprintf(pszTS, "%04d-%02d-%02d-%02d.%02d.%02d 0x%04x%04x %d",
            pfindbuf3->fdateLastWrite.year + 1980,
            pfindbuf3->fdateLastWrite.month,
            pfindbuf3->fdateLastWrite.day,
            pfindbuf3->ftimeLastWrite.hours,
            pfindbuf3->ftimeLastWrite.minutes,
            pfindbuf3->ftimeLastWrite.twosecs * 2,
            (ULONG)*(PUSHORT)(void*)&pfindbuf3->fdateCreation,
            (ULONG)*(PUSHORT)(void*)&pfindbuf3->ftimeCreation,
            pfindbuf3->cbFile);
    return pszTS;
}


/**
 * Adds the src additioanl dependenies to a rule.
 * @param   pvRule  Rule to add them to.
 * @param   pszz    Pointer to the string of strings of extra dependencies.
 */
void depAddSrcAddDeps(void *pvRule, const char *pszz)
{
    while (*pszz)
    {
        depAddDepend(pvRule, pszz, FALSE, FALSE);
        pszz += strlen(pszz) + 1;
    }
}





/*
 * Testing purpose.
 */

#if !defined(OS2FAKE)
#include <os2.h>
#endif
#ifdef OLEMANN
#include "olemann.h"
#endif
