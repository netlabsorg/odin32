/*
 * Fast dependands.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#define INCL_DOSERRORS
#define INCL_FILEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*@Global***********************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static const char pszDefaultDepFile[] = ".depend";


/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int makeDependent(FILE *phDep, const char *pszSource, const char *pszInclude, const char *pszObjectDir);
static int getFullIncludename(char *pszFilename, const char *pszInclude);


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
    char        szObjectDir[CCHMAXPATH] = {0};
    static char szInclude[32768] = ";";

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
            ULONG        ulRc;
            FILEFINDBUF3 filebuf;
            HDIR         hDir = HDIR_CREATE;
            ULONG        ulFound = 1;

            if (argv[argi][0] == '-' || argv[argi][0] == '/')
            {
                /* parameters */
                switch (argv[argi][1])
                {
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

                    case 'o': /* object base directory */
                    case 'O':
                        strcpy(szObjectDir, argv[argi]+2);
                        break;

                    default:
                        fprintf(stderr, "error: invalid parameter! '%s'\n", argv[argi]);
                        return -1;
                }

            }
            else
            {
                /* not a parameter! */
                ulRc = DosFindFirst(argv[argi], &hDir,
                                    FILE_READONLY |  FILE_HIDDEN | FILE_SYSTEM | FILE_ARCHIVED,
                                    &filebuf, sizeof(FILEFINDBUF3), &ulFound, FIL_STANDARD);
                while (rc == NO_ERROR && ulFound == 1)
                {
                    char *psz;
                    char  szSource[CCHMAXPATH];
                    char  szObjectDirTmp[CCHMAXPATH];

                    if ((psz = strrchr(argv[argi], '\\')) || (psz = strrchr(argv[argi], '/')))
                    {
                        strncpy(szSource, argv[argi], psz - argv[argi] + 1);
                        szSource[psz - argv[argi] + 1]  = '\0';
                        if (szObjectDir[0] == '\0')
                        {
                            strncpy(szObjectDirTmp, argv[argi], psz - argv[argi] + 1);
                            szObjectDirTmp[psz - argv[argi] + 1]  = '\0';
                        }
                    }
                    else
                        szSource[0]  = '\0';
                    if (szObjectDir[0] != '\0')
                    {
                        int i;
                        strcpy(szObjectDirTmp, szObjectDir);
                        i = strlen(szObjectDirTmp);
                        if (szObjectDirTmp[i - 1] == '\\' || szObjectDirTmp[i - 1] == '/')
                            szObjectDirTmp[i - 1] = '\0';
                    }

                    strcat(szSource, filebuf.achName);
                    rc -= makeDependent(phDep, &szSource[0], &szInclude[0], &szObjectDirTmp[0]);
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
 * Generates depend info on this file, and fwrites it to phDep.
 * @returns
 * @param     phDep         Pointer to file struct for outfile.
 * @param     pszSource     Name of source file.
 * @param     pszIncldue    Pointer to additional include path.
 * @param     pszObjectDir  Pointer to object directory.
 */
static int makeDependent(FILE *phDep, const char *pszSource, const char *pszInclude, const char *pszObjectDir)
{
    FILE  *phFile;

    phFile = fopen(pszSource, "r");
    if (phFile != NULL)
    {
        char szBuffer[4096]; /* max line lenght */
        int  k = strlen(pszSource) - 1;
        int  l;
        /**********************************/
        /* print file name to depend file */
        /**********************************/
        while (pszSource[k] != '.' && k >= 0) k--;
        l = k;
        while (pszSource[l] != '\\' && pszSource[l] != '/' && l >= 0)
            l--;
        if (stricmp(&pszSource[k], ".c") == 0
            || stricmp(&pszSource[k], ".cpp") == 0
            || stricmp(&pszSource[k], ".asm") == 0
           )
            fprintf(phDep, "%s%.*s.obj:%*s %s",
                    pszObjectDir,
                    k - l, &pszSource[l],
                    (k - l + strlen(pszObjectDir)) + 4 > 20 ? 0 : 20 - (k - l + strlen(pszObjectDir)) - 4, "",
                    pszSource);
        else if (stricmp(&pszSource[k], ".rc") == 0)
            fprintf(phDep, "%s%.*s.res:%*s %s",
                    pszObjectDir,
                    k - l, &pszSource[l],
                    (k - l + strlen(pszObjectDir)) + 4 > 20 ? 0 : 20 - (k - l + strlen(pszObjectDir)) - 4, "",
                    pszSource);
        else
            fprintf(phDep, "%s:%-*s", pszSource, strlen(pszSource) > 20 ? 0 : 20 - strlen(pszSource), "");


        /*******************/
        /* find dependants */
        /*******************/
        while (!feof(phFile))
        {
            if (fgets(szBuffer, sizeof(szBuffer), phFile) != NULL)
            {
                /* search for #include */
                int cbLen;
                int i = 0;
                int f = 0;

                cbLen = strlen(szBuffer);
                while (i + 9 < cbLen
                       && !(f = (strncmp(&szBuffer[i], "#include", 8) == 0
                                 || strncmp(&szBuffer[i], "RCINCLUDE", 9) == 0)
                           )
                       /*&& (szBuffer[i] == ' ' || szBuffer[i] == '\t') */
                      )
                    i++;

                /* found */
                if (f)
                {
                    /* extract info between "" or <> */
                    f = 0;
                    while (i < cbLen && !(f = (szBuffer[i] == '"' || szBuffer[i] == '<')))
                        i++;
                    i++; /* skipp '"' or '<' */
                    if (f)
                    {
                        int j;
                        /* find end */
                        j = f = 0;
                        while (i + j < cbLen &&  j < 260 && !(f = (szBuffer[i+j] == '"' || szBuffer[i+j] == '>')))
                            j++;

                        if (f)
                        {
                            char szFullname[261];

                            /* find include file path */
                            strncpy(&szFullname[1], &szBuffer[i], j);
                            szFullname[j+1] = '\0'; /* ensure terminatition. */
                            if (getFullIncludename(&szFullname[1], pszInclude) == 0)
                            {
                                szFullname[0] = ' '; /* blank char at begining */
                                if (fwrite(&szFullname[0], strlen(szFullname), 1, phDep) != 1)
                                    fprintf(stderr, "fwrite failed - loc 2\n");
                            }
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
    } else
        return -1;

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
        if (ulRc == NO_ERROR && ulFound == 1)
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
