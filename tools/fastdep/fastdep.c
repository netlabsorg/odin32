/* $Id: fastdep.c,v 1.8 2000-03-16 15:27:08 bird Exp $
 *
 * Fast dependents. (Fast = Quick and Dirty!)
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_FILEMGR


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    BOOL            fObjectDir;        /* replace object directory? */
    const char *    pszRsrcExt;
    BOOL            fObjRule;
    BOOL            fNoObjectPath;
    BOOL            fSrcWhenObj;
    BOOL            fAppend;            /* append to the output file, not overwrite it. */
} OPTIONS, *POPTIONS;


/*
 * Language specific analysis functions type.
 */
typedef int ( _FNLANG)  (const char *pszFilename, FILE *phFile,
                         BOOL fHeader, POPTIONS pOptions);
typedef _FNLANG    *PFNLANG;


/**
 * This struct holds the static configuration of the util.
 */
typedef struct _ConfigEntry
{
    const char **papszExts;             /* Pointer to an array of pointer to extentions for this handler. */
                                        /* If NULL this is the last entry. */
    int          iFirstHdr;             /* Index into the papszExts array of the first headerfile/copybook. */
                                        /* Set it to the NULL element of the array if no headers for this extention. */
                                        /* A non-header file may get a object rule. */
    PFNLANG      pfn;                   /* Pointer to handler function. */
} CONFIGENTRY, *PCONFIGENTRY;


/**
 * Dependant Rule
 */
typedef struct _DepRule
{
    char *           pszRule;          /* Pointer to rule name */
    int              cDeps;            /* Entries in the dependant array. */
    char **          papszDep;         /* Pointer to an array of pointers to dependants. */
    struct _DepRule *pNext;            /* Pointer to the next rule */
} DEPRULE, *PDEPRULE;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static int makeDependent(const char *pszFilename, POPTIONS pOptions);

int langC_CPP(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions);
int langAsm(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions);
int langRC(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions);
int langCOBOL(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions);


/* string operations */
static int strnicmpwords(const char *pszS1, const char *pszS2, int cch);

/* file operations */
char *filePath(const char *pszFilename, char *pszBuffer);
char *filePathSlash(const char *pszFilename, char *pszBuffer);
char *fileName(const char *pszFilename, char *pszBuffer);
char *fileNameNoExt(const char *pszFilename, char *pszBuffer);
char *fileExt(const char *pszFilename, char *pszBuffer);

/* pathlist operations */
char *pathlistFindFile(const char *pszPathList, const char *pszFilename, char *pszBuffer);

/* word operations */
static char *findEndOfWord(char *psz);
#if 0 /* not used */
static char *findStartOfWord(char *psz, const char *pszStart);
#endif

/* file helpers */
static signed long fsize(FILE *phFile);

/* text helpers */
static char *trim(char *psz);
static char *trimR(char *psz);

/* textbuffer */
static void *textbufferCreate(const char *pszFilename);
static void  textbufferDestroy(void *pvBuffer);
static char *textbufferNextLine(void *pvBuffer, char *psz);

/* depend workers */
static BOOL  depReadFile(const char *pszFilename);
static BOOL  depWriteFile(const char *pszFilename);
static void  depRemoveAll(void);
static void *depAddRule(const char *pszRulePath, const char *pszName, const char *pszExt);
static BOOL  depAddDepend(void *pvRule, const char *pszDep);

static BOOL  depCleanFile(const char *pszFilename);


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static PDEPRULE pdepList = NULL;

static const char pszDefaultDepFile[] = ".depend";
static const char *apszExtC_CPP[] = {"c", "sqc", "cpp", "h", "hpp", NULL};
static const char *apszExtAsm[]   = {"asm", "inc", NULL};
static const char *apszExtRC[]    = {"rc", "dlg", NULL};
static const char *apszExtCOBOL[] = {"cbl", "cob", "sqb", NULL};
static CONFIGENTRY aConfig[] =
{
    {
        apszExtC_CPP,
        3,
        langC_CPP,
    },

    {
        apszExtAsm,
        1,
        langAsm,
    },

    {
        apszExtRC,
        1,
        langRC,
    },

    {
        apszExtCOBOL,
        3,
        langCOBOL,
    },

    /* terminating entry */
    {
        NULL,
        -1,
        NULL
    }
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
    const char *pszDepFile = pszDefaultDepFile;

    static char szObjectDir[CCHMAXPATH];
    static char szObjectExt[64] = "obj";
    static char szRsrcExt[64]   = "res";
    static char szInclude[32768] = ";";
    static char szExclude[32768] = ";";

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
        FALSE            /* fAppend */
    };

    szObjectDir[0] = '\0';

    if (argc == 1)
    {
        syntax();
        return -87;
    }

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
                        if (argi + 1 < argc)
                            pszDepFile = argv[++argi];
                        else
                        {
                            fprintf(stderr, "invalid parameter -d, filename missing!\n");
                            return -1;
                        }
                    }

                    /* if dependencies are generated we'll flush them to the old filename */
                    if (pdepList != NULL && pszOld != pszDepFile)
                    {
                        if (!depWriteFile(pszOld))
                            fprintf(stderr, "error: failed to write (flush) dependencies.\n");
                        depRemoveAll();
                    }
                    break;
                }

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
                        strcat(szExclude, &argv[argi][2]);
                    else
                    {
                        strcat(szExclude, argv[argi+1]);
                        argi++;
                    }
                    if (szExclude[strlen(szExclude)-1] != ';')
                        strcat(szExclude, ";");
                    break;

                case 'I': /* optional include path. This has precedence over the INCLUDE environment variable. */
                case 'i':
                    if (strlen(argv[argi]) > 2)
                        strcat(szInclude, &argv[argi][2]);
                    else
                    {
                        strcat(szInclude, argv[argi+1]);
                        argi++;
                    }
                    if (szInclude[strlen(szInclude)-1] != ';')
                        strcat(szInclude, ";");
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
                            strcpy(szObjectExt, argv[argi+1]);
                            argi++;
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
                        strcpy(szObjectDir, argv[argi+1]);
                        argi++;
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
                        strcpy(szObjectExt, argv[argi]+2);
                    else
                    {
                        strcpy(szObjectExt, argv[argi+1]);
                        argi++;
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
        else
        {   /* not a parameter! */
            ULONG        ulRc;
            FILEFINDBUF3 filebuf;
            HDIR         hDir = HDIR_CREATE;
            ULONG        ulFound = 1;

            memset(&filebuf, 0, sizeof(filebuf));

            /*
             * Search for the files specified.
             */
            ulRc = DosFindFirst(argv[argi], &hDir,
                                FILE_READONLY |  FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                                &filebuf, sizeof(FILEFINDBUF3), &ulFound, FIL_STANDARD);
            while (ulRc == NO_ERROR)
            {
                char *psz;
                char  szSource[CCHMAXPATH];

                /*
                 * Make full path.
                 */
                if ((psz = strrchr(argv[argi], '\\')) || (psz = strrchr(argv[argi], '/')))
                {
                    strncpy(szSource, argv[argi], psz - argv[argi] + 1);
                    szSource[psz - argv[argi] + 1]  = '\0';
                }
                else
                    szSource[0]  = '\0';
                strcat(szSource, filebuf.achName);

                /*
                 * Analyse the file.
                 */
                rc -= makeDependent(&szSource[0], &options);

                /* next file */
                ulRc = DosFindNext(hDir, &filebuf, sizeof(filebuf), &ulFound);
            }
            DosFindClose(hDir);
        }
        /* next */
        argi++;
    }

    /* Write the depend file! */
    if (!depWriteFile(pszDepFile))
        fprintf(stderr, "error: failed to write dependencies file!\n");

    return rc;
}


/**
 * Displays the syntax description for this util.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static void syntax(void)
{
    printf(
        "FastDep v0.1\n"
        "Quick and dirty dependant scanner. Creates a makefile readable depend file.\n"
        "\n"
        "Syntax: FastDep [-a<[+]|->] [-d <outputfn>] [-e <excludepath>] [-eall<[+]|->]\n"
        "                [-i <include>] [-n<[+]|->] [-o <objdir>] [-obr<[+]|->] <files>\n"
        "\n"
        "   -a<[+]|->       Append to the output file. Default: Overwrite.\n"
        "   -d <outputfn>   Output filename. Default: %s\n"
        "   -e excludepath  Exclude paths. If a filename is found in any\n"
        "                   of these paths only the filename is used, not\n"
        "                   the path+filename (which is default) (don't work?).\n"
        "   -eall<[+]|->    Include and source filenames, paths or no paths.\n"
        "                   -eall+: No path are added to the filename.\n"
        "                   -eall-: The filename is appended the include path\n"
        "                           was found in.\n"
        "                   Default: eall-\n"
        "   -i <include>    Additional include paths. INCLUDE is searched after this.\n"
        "   -n<[+]|->       No path for object files in the rules.\n"
        "   -o <objdir>     Path were object files are placed. This path replaces the\n"
        "                   entire filename path\n"
        "   -o-             No object path\n"
        "   -obr<[+]|->     -obr+: Object rule.\n"
        "                   -obr-: No object rule, rule for source filename is generated.\n"
        "   -obj[ ]<objext> Object extention.           Default: obj\n"
        "   -r[ ]<rsrcext>  Resource binary extention.  Default: res\n"
        "   <files>         Files to scan. Wildchars are allowed.\n"
        "\n",
        pszDefaultDepFile
        );
}


/**
 * Generates depend info on this file, these are stored internally
 * and written to file later.
 * @returns
 * @param     pszFilename  Pointer to source filename.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static int makeDependent(const char *pszFilename, POPTIONS pOptions)
{
    int    rc = -1;
    FILE  *phFile;

    phFile = fopen(pszFilename, "r");
    if (phFile != NULL)
    {
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
                fHeader = &pCfg->papszExts[pCfg->iFirstHdr] <= ppsz;
                break;
            }
            pCfg++;
        }

        /* Found? */
        if (pCfg->papszExts != NULL)
            rc = (*pCfg->pfn)(pszFilename, phFile, fHeader, pOptions);
        else
        {
            if (*fileName(pszFilename, szExt) != '.') /* these are 'hidden' files, like .cvsignore, let's ignore them. */
                fprintf(stderr, "warning: '%s' has an unknown file type.\n", pszFilename);
            rc = 0;
        }

        fclose(phFile);
    }
    else
        fprintf(stderr, "failed to open '%s'\n", pszFilename);

    return rc;
}


/**
 * Generates depend info on this C or C++ file, these are stored internally
 * and written to file later.
 * @returns   0 on success.
 *            !0 on error.
 * @param     pszFilename  Pointer to source filename.
 * @param     phFile       Pointer to source file handle.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int langC_CPP(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions)
{
    void *  pvRule;                     /* Handle to the current rule. */
    int     iLine;                      /* Linenumber. */
    char    szBuffer[4096];             /* Max line length is 4096... should not be a problem. */
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
    if (pOptions->fObjRule && !fHeader)
    {
        if (pOptions->fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, pOptions->pszObjectExt);
        else
            pvRule = depAddRule(pOptions->fObjectDir ?
                                    pOptions->pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                pOptions->pszObjectExt);

        if (pOptions->fSrcWhenObj && pvRule)
            depAddDepend(pvRule, pOptions->fExcludeAll ? fileName(pszFilename, szBuffer) : pszFilename);
    }
    else
        pvRule = depAddRule(pszFilename, NULL, NULL);

    /* duplicate rule? */
    if (pvRule == NULL)
        return 0;


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
    while (!feof(phFile)) /* line loop */
    {
        if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
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
                        char szFullname[CCHMAXPATH];
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

                        /* find include file! */
                        psz = pathlistFindFile(pOptions->pszInclude, szFullname, szBuffer);
                        if (psz == NULL)
                            psz = pathlistFindFile(getenv("INCLUDE"), szFullname, szBuffer);

                        /* did we find the include? */
                        if (psz != NULL)
                        {
                            char    szBuffer2[CCHMAXPATH];
                            if (pOptions->fExcludeAll ||
                                pathlistFindFile(pOptions->pszExclude, szFullname, szBuffer2) != NULL
                                )
                                depAddDepend(pvRule, szFullname);
                            else
                                depAddDepend(pvRule, szBuffer);
                        }
                        else
                            fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                                    pszFilename, iLine, szFullname);
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
        }
        else
            break;
    } /*while*/

    return 0;
}


/**
 * Generates depend info on this file, these are stored internally
 * and written to file later.
 * @returns   0 on success.
 *            !0 on error.
 * @param     pszFilename  Pointer to source filename.
 * @param     phFile       Pointer to source file handle.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int langAsm(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions)
{
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (pOptions->fObjRule && !fHeader)
    {
        if (pOptions->fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, pOptions->pszObjectExt);
        else
            pvRule = depAddRule(pOptions->fObjectDir ?
                                    pOptions->pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                pOptions->pszObjectExt);

        if (pOptions->fSrcWhenObj && pvRule)
            depAddDepend(pvRule, pOptions->fExcludeAll ? fileName(pszFilename, szBuffer) : pszFilename);
    }
    else
        pvRule = depAddRule(pszFilename, NULL, NULL);

    /* duplicate rule? */
    if (pvRule == NULL)
        return 0;


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (!feof(phFile)) /* line loop */
    {
        if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
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

                /* find include file! */
                psz = pathlistFindFile(pOptions->pszInclude, szFullname, szBuffer);
                if (psz == NULL)
                    psz = pathlistFindFile(getenv("INCLUDE"), szFullname, szBuffer);

                /* Did we find the include? */
                if (psz != NULL)
                {
                    char szBuffer2[CCHMAXPATH];
                    if (pOptions->fExcludeAll ||
                        pathlistFindFile(pOptions->pszExclude, szFullname, szBuffer2) != NULL
                        )
                        depAddDepend(pvRule, szFullname);
                    else
                        depAddDepend(pvRule, szBuffer);
                }
                else
                    fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                            pszFilename, iLine, szFullname);
            }
        }
        else
            break;
    } /*while*/

    return 0;
}


/**
 * Generates depend info on this Resource file, these are stored internally
 * and written to file later.
 * @returns   0 on success.
 *            !0 on error.
 * @param     pszFilename  Pointer to source filename.
 * @param     phFile       Pointer to source file handle.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int langRC(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions)
{
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (pOptions->fObjRule && !fHeader)
    {
        if (pOptions->fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, pOptions->pszRsrcExt);
        else
            pvRule = depAddRule(pOptions->fObjectDir ?
                                    pOptions->pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                pOptions->pszRsrcExt);

        if (pOptions->fSrcWhenObj && pvRule)
            depAddDepend(pvRule, pOptions->fExcludeAll ? fileName(pszFilename, szBuffer) : pszFilename);
    }
    else
        pvRule = depAddRule(pszFilename, NULL, NULL);

    /* duplicate rule? */
    if (pvRule == NULL)
        return 0;


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (!feof(phFile)) /* line loop */
    {
        if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
        {
            /* search for #include */
            int cbLen;
            int i = 0;
            iLine++;

            /* skip blank chars */
            cbLen = strlen(szBuffer);
            while (i + 9 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
                i++;

            /* is this an include? */
            if (   strncmp(&szBuffer[i], "#include", 8) == 0
                || strncmp(&szBuffer[i], "RCINCLUDE", 9) == 0
                || strncmp(&szBuffer[i], "DLGINCLUDE", 10) == 0
                )
            {
                char szFullname[CCHMAXPATH];
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

                /* find include file! */
                psz = pathlistFindFile(pOptions->pszInclude, szFullname, szBuffer);
                if (psz == NULL)
                    psz = pathlistFindFile(getenv("INCLUDE"), szFullname, szBuffer);

                /* did we find the include? */
                if (psz != NULL)
                {
                    char szBuffer2[CCHMAXPATH];
                    if (pOptions->fExcludeAll ||
                        pathlistFindFile(pOptions->pszExclude, szFullname, szBuffer2) != NULL
                        )
                        depAddDepend(pvRule, szFullname);
                    else
                        depAddDepend(pvRule, szBuffer);
                }
                else
                    fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                            pszFilename, iLine, szFullname);
            }
        }
        else
            break;
    } /*while*/

    return 0;
}


/**
 * Generates depend info on this COBOL file, these are stored internally
 * and written to file later.
 * @returns   0 on success.
 *            !0 on error.
 * @param     pszFilename  Pointer to source filename.
 * @param     phFile       Pointer to source file handle.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int langCOBOL(const char *pszFilename, FILE *phFile, BOOL fHeader, POPTIONS pOptions)
{
    void *  pvRule;                     /* Handle to the current rule. */
    char    szBuffer[4096];             /* Temporary buffer (max line lenght size...) */
    int     iLine;                      /* current line number */


    /**********************************/
    /* Add the depend rule            */
    /**********************************/
    if (pOptions->fObjRule && !fHeader)
    {
        if (pOptions->fNoObjectPath)
            pvRule = depAddRule(fileNameNoExt(pszFilename, szBuffer), NULL, pOptions->pszObjectExt);
        else
            pvRule = depAddRule(pOptions->fObjectDir ?
                                    pOptions->pszObjectDir :
                                    filePathSlash(pszFilename, szBuffer),
                                fileNameNoExt(pszFilename, szBuffer + CCHMAXPATH),
                                pOptions->pszObjectExt);

        if (pOptions->fSrcWhenObj && pvRule)
            depAddDepend(pvRule, pOptions->fExcludeAll ? fileName(pszFilename, szBuffer) : pszFilename);
    }
    else
        pvRule = depAddRule(pszFilename, NULL, NULL);

    /* duplicate rule? */
    if (pvRule == NULL)
        return 0;


    /*******************/
    /* find dependants */
    /*******************/
    iLine = 0;
    while (!feof(phFile)) /* line loop */
    {
        if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
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

                /* add extention .cpy - hardcoded for the moment. */
                strcat(szFullname, ".cpy");

                /* find include file! */
                psz = pathlistFindFile(pOptions->pszInclude, szFullname, szBuffer);

                /* did we find the include? */
                if (psz != NULL)
                {
                    char szBuffer2[CCHMAXPATH];
                    if (pOptions->fExcludeAll ||
                        pathlistFindFile(pOptions->pszExclude, szFullname, szBuffer2) != NULL
                        )
                        depAddDepend(pvRule, szFullname);
                    else
                        depAddDepend(pvRule, szBuffer);
                }
                else
                    fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                            pszFilename, iLine, szFullname);
            }
        }
        else
            break;
    } /*while*/

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
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static int strnicmpwords(const char *pszS1, const char *pszS2, int cch)
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
        strncpy(pszBuffer, pszFilename, psz - pszFilename - 1);
        pszBuffer[psz - pszFilename - 1] = '\0';
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
            HDIR            hDir = HDIR_CREATE;
            ULONG           cFiles = 1UL;
            FILEFINDBUF3    FileFindBuf;
            APIRET          rc;
            char            szFile[CCHMAXPATH];

            /* make search statment */
            strncpy(szFile, psz, pszNext - psz);
            szFile[pszNext - psz] = '\0';
            if (szFile[pszNext - psz - 1] != '\\' && szFile[pszNext - psz - 1] != '/')
                strcpy(&szFile[pszNext - psz], "\\");
            strcat(szFile, pszFilename);

            /* search for file */
            rc = DosFindFirst(szFile, &hDir, FILE_NORMAL, &FileFindBuf, sizeof(FileFindBuf),
                              &cFiles, FIL_STANDARD);
            DosFindClose(hDir);
            if (rc == NO_ERROR)
            {
                strncpy(pszBuffer, psz, pszNext - psz);
                pszBuffer[pszNext - psz] = '\0';
                if (pszBuffer[pszNext - psz - 1] != '\\' && pszBuffer[pszNext - psz - 1] != '/')
                    strcpy(&pszBuffer[pszNext - psz], "\\");
                strcat(pszBuffer, pszFilename);
                break;
            }
        }

        /* next */
        if (*pszNext != ';')
            break;
        psz = pszNext + 1;
    }

    return *pszBuffer == '\0' ? NULL : pszBuffer;
}


/**
 * Finds the first char after word.
 * @returns   Pointer to the first char after word.
 * @param     psz  Where to start.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
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
    return (char *)psz;
}

#if 0 /* not used */
/**
 * Find the starting char of a word
 * @returns   Pointer to first char in word.
 * @param     psz       Where to start.
 * @param     pszStart  Where to stop.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
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
#endif

/**
 * Find the size of a file.
 * @returns   Size of file. -1 on error.
 * @param     phFile  File handle.
 */
static signed long fsize(FILE *phFile)
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
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
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


/**
 * Right trims a string, ie. removing spaces (and tabs) from the end of the stri
 * @returns   Pointer to the string passed in.
 * @param     psz   Pointer to the string which is to be right trimmed.
 * @status    completely implmented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static char *trimR(char *psz)
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
 * Creates a memory buffer for a text file.
 * @returns   Pointer to file memoryblock. NULL on error.
 * @param     pszFilename  Pointer to filename string.
 */
static void *textbufferCreate(const char *pszFilename)
{
    void *pvFile = NULL;
    FILE *phFile;

    phFile = fopen(pszFilename, "r");
    if (phFile != NULL)
    {
        signed long cbFile = fsize(phFile);
        if (cbFile != -1)
        {
            pvFile = malloc(cbFile + 1);
            if (pvFile != NULL)
            {
                memset(pvFile, 0, cbFile + 1);
                if (fread(pvFile, 1, cbFile, phFile) == 0)
                {   /* failed! */
                    free(pvFile);
                    pvFile = NULL;
                }
            }
        }
        fclose(phFile);
    }
    return pvFile;
}


/**
 * Destroys a text textbuffer.
 * @param     pvBuffer   Buffer handle.
 */
static void textbufferDestroy(void *pvBuffer)
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
static char *textbufferNextLine(void *pvBuffer, register char *psz)
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
    if (ch == '\n')
        psz++;
    if (*psz == '\r')
        psz++;

    return psz;
}


/**
 * Appends a depend file to the internal file.
 */
static BOOL  depReadFile(const char *pszFilename)
{
    void *pvFile;
    char *pszNext;
    BOOL  fMoreDeps = FALSE;
    void *pvRule = NULL;

    /* read depend file */
    pvFile = textbufferCreate(pszFilename);
    if (pvFile == NULL)
        return FALSE;

    /* parse the original depend file */
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
            continue;

        /* new rule? */
        if (!fMoreDeps && *psz != ' ' && *psz != '\t' && *psz != '\0')
        {
            i = 0;
            while (psz[i] != '\0')
            {
                if (psz[i] == ':'
                    && (psz[i+1] == ' '
                        || psz[i+1] == '\t'
                        || psz[i+1] == '\0'
                        || psz[i+1] == '\\'
                        )
                    )
                    break;
                i++;
            }

            if (psz[i] == ':')
            {   /* new rule! */
                psz[i] = '\0';
                pvRule = depAddRule(trimR(psz), NULL, NULL);
                psz += i + 1;
                cch -= i + 1;
                fMoreDeps = TRUE;
            }
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
                psz = trim(psz);
                if (*psz != '\0')
                    depAddDepend(pvRule, psz);
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
 * @params    pszFilename  Pointer to name of the output file.
 */
static BOOL  depWriteFile(const char *pszFilename)
{
    FILE *phFile;
    phFile = fopen(pszFilename, "w");
    if (phFile != NULL)
    {
        char     szBuffer[4096];
        int      iBuffer = 0;
        PDEPRULE pdep = pdepList;

        while (pdep != NULL)
        {
            /* Write rule. Flush the buffer first if necessary. */
            if (iBuffer + 2 >= sizeof(szBuffer))
            {
                fwrite(szBuffer, iBuffer, 1, phFile);
                iBuffer = 0;
            }
            iBuffer += sprintf(szBuffer + iBuffer, "%s:", pdep->pszRule);

            /* write rule dependants. */
            if (pdep->papszDep != NULL)
            {
                char **ppsz = pdep->papszDep;
                while (*ppsz != NULL)
                {
                    /* flush buffer? */
                    if (iBuffer + strlen(*ppsz) + 20 >= sizeof(szBuffer))
                    {
                        fwrite(szBuffer, iBuffer, 1, phFile);
                        iBuffer = 0;
                    }
                    iBuffer += sprintf(szBuffer + iBuffer, " \\\n    %s", *ppsz);

                    /* next dependant */
                    ppsz++;
                }
            }

            /* Add two new lines. Flush buffer first if necessary. */
            if (iBuffer + 2 >= sizeof(szBuffer))
            {
                fwrite(szBuffer, iBuffer, 1, phFile);
                iBuffer = 0;
            }
            strcpy(szBuffer + iBuffer, "\n\n");
            iBuffer += 2;

            /* next rule */
            pdep = pdep->pNext;
        }

        /* flush buffer. */
        fwrite(szBuffer, iBuffer, 1, phFile);

        fclose(phFile);
        return TRUE;
    }

    return FALSE;
}


/**
 * Removes all entries in the list of dependencies. (pdepList)
 */
static void  depRemoveAll(void)
{
    while (pdepList != NULL)
    {
        register PDEPRULE pdepToBeFree = pdepList;
        /* next */
        pdepList = pdepToBeFree->pNext;

        /* free this */
        if (pdepToBeFree->papszDep != NULL)
        {
            char ** ppsz = pdepToBeFree->papszDep;
            while (*ppsz != NULL)
                free(*ppsz++);
            free(pdepToBeFree->papszDep);
        }
        free(pdepToBeFree);
    }
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
 */
static void *depAddRule(const char *pszRulePath, const char *pszName, const char *pszExt)
{
    char     szRule[CCHMAXPATH*2];
    PDEPRULE pdepPrev = NULL;
    PDEPRULE pdep = pdepList;
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

    /* find location */
    while (pdep != NULL && stricmp(pdep->pszRule, szRule) < 0)
    {
        pdepPrev = pdep;
        pdep = pdep->pNext;
    }

    /* check if matching rule name */
    if (pdep != NULL && stricmp(pdep->pszRule, szRule) == 0)
        return NULL;

    /*
     * Allocate a new rule structure and fill in data
     * Note. One block for both the DEPRULE and the pszRule string.
     */
    pNew = malloc(sizeof(DEPRULE) + cch + 1);
    if (pNew == NULL)
        return NULL;
    pNew->pszRule = (char*)(void*)(pNew + 1);
    strcpy(pNew->pszRule, szRule);
    pNew->cDeps = 0;
    pNew->papszDep = NULL;

    /* link in module (before pdep) */
    pNew->pNext = pdep;
    if (pdepPrev == NULL)
        pdepList = pNew;
    else
        pdepPrev->pNext = pNew;

    return pNew;
}



/**
 * Adds a dependant to a rule.
 * @returns   Successindicator.
 * @param     pvRule   Rule handle.
 * @param     pszDep   Pointer to dependant name
 */
static BOOL  depAddDepend(void *pvRule, const char *pszDep)
{
    PDEPRULE pdep = (PDEPRULE)pvRule;

    /* allocate more array space */
    if (pdep->cDeps == 0 || ((pdep->cDeps + 2) % 32) == 0)
    {
        pdep->papszDep = realloc(pdep->papszDep, sizeof(char*) * (pdep->cDeps + 32));
        if (pdep->papszDep == NULL)
        {
            pdep->cDeps = 0;
            return FALSE;
        }
    }

    /* allocate string space and copy pszDep */
    if ((pdep->papszDep[pdep->cDeps] = malloc(strlen(pszDep) + 1)) == NULL)
        return FALSE;
    strcpy(pdep->papszDep[pdep->cDeps], pszDep);

    /* terminate array and increment dep count */
    pdep->papszDep[++pdep->cDeps] = NULL;

    /* successful! */
    return TRUE;
}



/**
 * Removes double dependencies.
 * @returns   Success indicator.
 * @param     pszFilename  Depend filename.
 */
static BOOL depCleanFile(const char *pszFilename)
{
    if (depReadFile(pszFilename))
        return depWriteFile(pszFilename);
    return FALSE;
}


/*
 * Testing purpose.
 */
#include <os2.h>

