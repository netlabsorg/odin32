/* $Id: fastdep.c,v 1.7 2000-01-22 18:20:54 bird Exp $
 *
 * Fast dependants. (Fast = Quick and Dirty!)
 *
 * Copyright (c) 1999 knut st. osmundsen
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
#include <stdlib.h>
#include <string.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _Options
{
    const char *    pszInclude;
    const char *    pszExclude;
    BOOL            fExcludeAll;
    const char *    pszObjectDir;
    BOOL            fObjRule;
    BOOL            fNoObjectPath;
    BOOL            fSrcWhenObj;
} OPTIONS, *POPTIONS;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static const char pszDefaultDepFile[] = ".depend";

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static int makeDependent(FILE *phDep, const char *pszFilename, POPTIONS pOptions);
static int getFullIncludename(char *pszFilename, const char *pszInclude);

/* file operations */
char *filePath(const char *pszFilename, char *pszBuffer);
char *filePathSlash(const char *pszFilename, char *pszBuffer);
char *fileName(const char *pszFilename, char *pszBuffer);
char *fileNameNoExt(const char *pszFilename, char *pszBuffer);
char *fileExt(const char *pszFilename, char *pszBuffer);

char *pathlistFindFile(const char *pszPathList, const char *pszFilename, char *pszBuffer);


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
    FILE       *phDep;
    int         rc   = 0;
    int         argi = 1;
    const char *pszDepFile = pszDefaultDepFile;

    static char szObjectDir[CCHMAXPATH] = {0};
    static char szInclude[32768] = ";";
    static char szExclude[32768] = ";";

    OPTIONS options =
    {
        szInclude,       /* pszInclude */
        szExclude,       /* pszExclude */
        FALSE,           /* fExcludeAll */
        szObjectDir,     /* pszObjectDir */
        TRUE,            /* fObjRule */
        FALSE,           /* fNoObjectPath */
        TRUE             /* fSrcWhenObj */
    };

    if (argc == 1)
    {
        syntax();
        return -87;
    }

    /* look for depend filename option "-d <filename>" */
    if (argc >= 3 && strcmp(argv[1], "-d") == 0)
    {
        pszDepFile = argv[2];
        argi = 3;
    }

    phDep = fopen(pszDepFile, "w");
    if (phDep != NULL)
    {
        while (argi < argc)
        {
            if (argv[argi][0] == '-' || argv[argi][0] == '/')
            {
                /* parameters */
                switch (argv[argi][1])
                {
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

                    case 'o': /* object base directory or Obr<[+]|-> */
                    case 'O':
                        if (strlen(&argv[argi][1]) <= 4 && strnicmp(&argv[argi][1], "Obr", 3) == 0)
                        {
                            options.fObjRule = argv[argi][4] != '-';
                            break;
                        }

                        /* path */
                        if (strlen(argv[argi]) > 2)
                            strcpy(szObjectDir, argv[argi]+2);
                        else
                        {
                            strcpy(szObjectDir, argv[argi+1]);
                            argi++;
                        }
                        if (szObjectDir[strlen(szObjectDir)-1] != '\\'
                            && szObjectDir[strlen(szObjectDir)-1] != '/'
                            )
                            strcat(szObjectDir, "\\");
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
                FILEFINDBUF3 filebuf = {0};
                HDIR         hDir = HDIR_CREATE;
                ULONG        ulFound = 1;

                ulRc = DosFindFirst(argv[argi], &hDir,
                                    FILE_READONLY |  FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                                    &filebuf, sizeof(FILEFINDBUF3), &ulFound, FIL_STANDARD);
                while (ulRc == NO_ERROR)
                {
                    char *psz;
                    char  szSource[CCHMAXPATH];

                    if ((psz = strrchr(argv[argi], '\\')) || (psz = strrchr(argv[argi], '/')))
                    {
                        strncpy(szSource, argv[argi], psz - argv[argi] + 1);
                        szSource[psz - argv[argi] + 1]  = '\0';
                    }
                    else
                        szSource[0]  = '\0';

                    strcat(szSource, filebuf.achName);
                    rc -= makeDependent(phDep, &szSource[0], &options);
                    ulRc = DosFindNext(hDir, &filebuf, sizeof(filebuf), &ulFound);
                }
                DosFindClose(hDir);
            }
            /* next */
            argi++;
        }
    } else
    {
        fprintf(stderr, "error opening outputfile '%s'.\n", pszDepFile);
        rc = 1;
    }

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
        "Syntax: FastDep [-d <outputfn>] [-e <excludepath>] [-eall<[+]|->]\n"
        "                [-i <include>] [-n<[+]|->] [-o <objdir>] [-obr<[+]|->]\n"
        "\n"
        "   -d <outputfn>   Output filename. Default: %s\n"
        "   -e excludepath  Exclude paths. If a filename is found in any\n"
        "                   of these paths only the filename is used, not\n"
        "                   the path+filename (which is default).\n"
        "   -eall<[+]|->    -eall+: No path are added to the filename.\n"
        "                   -eall-: The filename is appended the include path\n"
        "                           was found in.\n"
        "                   Default: eall-\n"
        "   -i <include>    Additional include paths. INCLUDE is searched after this.\n"
        "   -n<[+]|->       No path for object files in the rules.\n"
        "   -o <objdir>     Path were object files are placed. This path replaces the\n"
        "                   entire filename path\n"
        "   -obr<[+]|->     -obr+: Object rule.\n"
        "                   -obr-: No object rule, rule for source filename is generated.\n"
        "\n",
        pszDefaultDepFile
        );
}


/**
 * Generates depend info on this file, and fwrites it to phDep.
 * @returns
 * @param     phDep        Pointer to file struct for outfile.
 * @param     pszFilename  Pointer to source filename.
 * @param     pOptions     Pointer to options struct.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static int makeDependent(FILE *phDep, const char *pszFilename, POPTIONS pOptions)
{
    FILE  *phFile;

    phFile = fopen(pszFilename, "r");
    if (phFile != NULL)
    {
        char szBuffer[4096]; /* max line lenght */
        int  k = strlen(pszFilename) - 1;
        int  l;
        int  iLine;

        /**********************************/
        /* print file name to depend file */
        /**********************************/
        if (pOptions->fObjRule)
        {
            char szExt[CCHMAXPATH];
            char szObj[CCHMAXPATH];

            if (pOptions->fNoObjectPath)
                fileNameNoExt(pszFilename, szObj);
            else if (*pOptions->pszObjectDir != '\0')
            {
                fileNameNoExt(pszFilename, szExt);
                strcpy(szObj, pOptions->pszObjectDir);
                strcat(szObj, szExt);
            }
            else
            {
                filePathSlash(pszFilename, szObj);
                fileNameNoExt(pszFilename, szObj + strlen(szObj));
            }

            fileExt(pszFilename, szExt);
            if (!stricmp(szExt, "c") || !stricmp(szExt, "sqc")
                || !stricmp(szExt, "cpp") || !stricmp(szExt, "asm")
                || !stricmp(szExt, "rc"))
            {
                if (!stricmp(szExt, "rc"))
                    strcat(szObj, ".res");
                else
                    strcat(szObj, ".obj");
                fprintf(phDep, "%s:", szObj);

                if (pOptions->fSrcWhenObj)
                    fprintf(phDep, " \\\n%6s %s", "", pszFilename);
            }
            else
                fprintf(phDep, "%s:", pszFilename);
        }
        else
            fprintf(phDep, "%s:", pszFilename);

        /*******************/
        /* find dependants */
        /*******************/
        iLine = 0;
        while (!feof(phFile)) /* line loop */
        {
            if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
            {
                /* search for #include or RCINCLUDE */
                int cbLen;
                int i = 0;
                iLine++;

                cbLen = strlen(szBuffer);
                while (i + 9 < cbLen && (szBuffer[i] == ' ' || szBuffer[i] == '\t'))
                    i++;

                /* Found include! */
                if (strncmp(&szBuffer[i], "#include", 8) == 0 || strncmp(&szBuffer[i], "RCINCLUDE", 9) == 0)
                {
                    int f = 0;

                    /* extract info between "" or <> */
                    while (i < cbLen && !(f = (szBuffer[i] == '"' || szBuffer[i] == '<')))
                        i++;
                    i++; /* skip '"' or '<' */
                    if (f)
                    {
                        int j;
                        /* find end */
                        j = f = 0;
                        while (i + j < cbLen &&  j < CCHMAXPATH &&
                               !(f = (szBuffer[i+j] == '"' || szBuffer[i+j] == '>')))
                            j++;

                        if (f)
                        {
                            char szFullname[CCHMAXPATH];
                            char *psz;

                            /* copy filename */
                            strncpy(szFullname, &szBuffer[i], j);
                            szFullname[j] = '\0'; /* ensure terminatition. */

                            /* find include file! */
                            psz = pathlistFindFile(pOptions->pszInclude, szFullname, szBuffer);
                            if (psz == NULL)
                                psz = pathlistFindFile(getenv("INCLUDE"), szFullname, szBuffer);

                            if (psz != NULL)
                            {
                                char szBuffer2[CCHMAXPATH];
                                if (pOptions->fExcludeAll ||
                                    pathlistFindFile(pOptions->pszExclude, szFullname, szBuffer2) != NULL
                                    )
                                    strcpy(szBuffer, szFullname);
                                fprintf(phDep, " \\\n%6.s %s", "", szBuffer);
                            }
                            else
                                fprintf(stderr, "%s(%d): warning include file '%s' not found!\n",
                                        pszFilename, iLine, szFullname);
                        }
                    }
                }
            }
            /*
            else
               break;
            */
        } /*while*/
        fputs("\n", phDep);
        fclose(phFile);
    }
    else
    {
        fprintf(stderr, "failed to open '%s'\n", pszFilename);
        return -1;
    }

    return 0;
}




/**
 * Gets the fullpath include-filename.
 * @returns   0  on success,  -1 on error.
 * @param     pszFilename  Input: Pointer to filename to be found, and buffer for output.
 *                         Ouput: Buffer now contains fullpath include-filename.
 * @param     pszInclude   Additional includepath.
 */
static int getFullIncludename(char *pszFilename, const char *pszInclude)
{
    const char *pszEnvInclude;
    const char *psz;

    pszEnvInclude = getenv("INCLUDE");
    if ((pszEnvInclude == NULL && (pszInclude == NULL || strlen(pszInclude) == 0)) || strlen(pszFilename) == 0)
        return -1;

    psz = "";
    while (psz != NULL && psz != '\0')
    {
        const char    *pszNext;
        int            cbLen;
        char           szFileTmpIn[260];
        FILEFINDBUF3   filebuf;
        ULONG          ulRc;
        HDIR           hDir = HDIR_CREATE;
        ULONG          ulFound = 1;

        /* get addr of next ';' or '\0' */
        pszNext = strchr(psz, ';');
        if (pszNext == NULL)
            pszNext = psz + strlen(psz);

        /* add a '\\' and the pszFilename string to the include path part. */
        cbLen = (int)pszNext - (int)psz;
        if (cbLen > 0)
        {
            strncpy(szFileTmpIn, psz, (int)pszNext - (int)psz);
            if (szFileTmpIn[cbLen - 1] != '\\' && szFileTmpIn[cbLen - 1] != '/')
                szFileTmpIn[cbLen++] = '\\';
        }
        strcpy(&szFileTmpIn[cbLen], pszFilename);


        /**************************/
        /* check if file is found */
        /**************************/
        ulRc = DosFindFirst(&szFileTmpIn[0], &hDir,
                            FILE_READONLY |  FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                            &filebuf, sizeof(FILEFINDBUF3), &ulFound, FIL_STANDARD);
        if (ulRc == NO_ERROR)
        {
            strcpy(pszFilename, szFileTmpIn);
            DosFindClose(hDir);
            return 0;
        }

        /* next */
        if (*pszNext == ';' && pszNext[1] != '\0')
            psz = pszNext + 1;
        else
        {
            psz = pszInclude;
            pszInclude = NULL;

            if (psz == NULL)
            {
                psz = pszEnvInclude;
                pszEnvInclude = NULL;
            }
        }
    }

    return -1;
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
 * Copies the path name (with extention) into pszBuffer and returns
 * a pointer to the buffer.
 * If no path is found "" is returned.
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

