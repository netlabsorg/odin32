/* $Id: kHtmlPC.cpp,v 1.3 2000-02-18 12:42:07 bird Exp $ */
/*
 * kHtmlPC - Special-purpose HTML/SQL preprocessor.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define ErrorDescCase(errorcode)  case errorcode: return formatMessage(#errorcode)
#define TAG_PARAMETER_LIST                        \
    const kTag                        &tag,       \
    kLIFO<kFileEntry>                 &lifoFile,  \
    kLIFO<kFileEntry>                 &lifoUsed,  \
    kLIFO<kVariableEntry>             &lifoVar,   \
    kLIFO2<kSqlEntry, kVariableEntry> &lifoSql,   \
    kFileEntry                      * &pCurFile,  \
    int                               &i,         \
    FILE                              *phLog

#define TAG_PARAMETERS tag, lifoFile, lifoUsed, lifoVar, lifoSql, pCurFile, i, phLog
#define TAG_PARAMETER_LIST_UNREF                       \
        int f = phLog || i || pCurFile || &lifoFile || \
            &lifoUsed || &lifoVar || &lifoSql || &tag; \
        f = f

#ifdef __EMX__
    #define max(a,b) (((a) > (b)) ? (a) : (b))
    #define min(a,b) (((a) < (b)) ? (a) : (b))
    #define _System
#endif

#define TRUE  1
#define FALSE 0
#define CCHMAXPATH 260


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef unsigned long BOOL;
typedef struct _POINTL
{
    long x;
    long y;
} POINTL, *PPOINTL;


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>
#ifdef __EMX__
    #include <math.h>
    #undef _NAN
    static double _NAN = (0.0/0.0);
#else
    #include <float.h>
#endif
#include <assert.h>
/* gifdraw */
extern "C"
{
    #include "gd/gd.h"
    #include "gd/gdfontg.h"
    #include "gd/gdfontl.h"
    #include "gd/gdfonts.h"
    /* Y wrappers; LB = origo at Left Botton corner */
    #define gdImageLineLB(im,x1,y1,x2,y2,cl) gdImageLine(im,x1,gdImageSY(im)-(y1),x2,gdImageSY(im)-(y2),cl)
    #define gdImageStringLB(im,f,x,y,s,cl) gdImageString(im,f,x,gdImageSY(im)-(y),s,cl)
    #define gdImageStringUpLB(im,f,x,y,s,cl) gdImageStringUp(im,f,x,gdImageSY(im)-(y),s,cl)
}

#include "kLIFO.h"
#include "kList.h"
#include "kHtmlPC.h"
#include "db.h"

/*@Struct***********************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _GraphCallBackParameters
{
    kGraph             *pGraph;
    kGraphDataSet      *pDataSet;
    kGraphData         *pData;
    kGraph::enmType     enmTypeCd;
    long                lSeqNbr;
    char               *pszLegend;
    char               *pszColor;
} GRAPHCALLBACKPARAM, *PGRAPHCALLBACKPARAM;



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static FILE  *phLog = NULL;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
extern "C" void      handler(int sig);
static void          syntax(void);
static void          openLog(void);
static void          closeLog(void);
static unsigned long processFile(const char *pszFilenamem, const POPTIONS pOptions);
static unsigned long tagEndkSql(TAG_PARAMETER_LIST);
static unsigned long tagkSql(TAG_PARAMETER_LIST);
static unsigned long tagkTemplate(TAG_PARAMETER_LIST, FILE * &phFile, const POPTIONS pOptions);
static unsigned long tagkInclude(TAG_PARAMETER_LIST);
static unsigned long tagkGraph(TAG_PARAMETER_LIST, const POPTIONS pOptions);
static unsigned long tagkDefine(TAG_PARAMETER_LIST);
static unsigned long tagkUndef(TAG_PARAMETER_LIST);
static unsigned long tagkIf(TAG_PARAMETER_LIST);
static unsigned long tagkElse(TAG_PARAMETER_LIST);
static unsigned long tagkEndif(TAG_PARAMETER_LIST);

static BOOL          rebuildFileLIFO(kLIFO<kFileEntry> &lifoFile, kLIFO<kFileEntry> &lifoUsed, const kFileEntry *pFileEntryTop);
static char         *dupeString(const char *psz);
#if 0
inline char          upcase(char ch);
static char         *stristr(const char *pszStr, const char *pszSubStr);
static char         *trim(char *psz);
static char         *ltrim(char *psz);
static const char   *ltrim(const char *psz);
static char         *ltrimL(char *psz);
#endif
static const char   *ltrimL(const char *psz);
static long _System  dbFetchCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
long _System         dbDataSetCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
long _System         dbGraphCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);


/**
 * Main function.
 * @returns   Number of errors
 * @param     argc  Argument count.
 * @param     argv  Argument array.
 */
int main(int argc, char **argv)
{
    int             argi;
    BOOL            fFatal = FALSE;
    unsigned long   ulRc = 0;
    char           *pszHost     = "localhost";
    char           *pszDatabase = "Odin32";
    char           *pszUser     = "root";
    char           *pszPasswd   = "";
    char            szPathName[CCHMAXPATH];
    OPTIONS         options = {"."};


    /* signal handler */
    if (SIG_ERR == signal(SIGBREAK, handler)
        || SIG_ERR == signal(SIGTERM, handler)
        || SIG_ERR == signal(SIGINT, handler)
        )
        fprintf(stderr, "Error installing signalhandler...");


    /**************************************************************************
    * parse arguments.
    * options:  -h or -?     help
    *           -b:<basepath> Basepath for output files.
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
                case 'b':
                case 'B':
                    printf("%s\n", argv[argi]);
                    if (argv[argi][2] == ':')
                    {
                        if (_fullpath(&szPathName[0], &argv[argi][3], sizeof(szPathName)) != NULL)
                            options.pszBaseDir = &szPathName[0];
                        else
                        {
                            fprintf(stderr, "error: basepath '%s' don't exists\n", &argv[argi][3]);
                            fFatal = TRUE;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "error: option '-b:' requires a directory name.\n");
                        fFatal = TRUE;
                    }
                    break;

                case 'd':
                case 'D':
                    if (argv[argi][2] == ':')
                        pszDatabase = &argv[argi][3];
                    else
                    {
                        fprintf(stderr, "error: option '-d:' requires database name.\n");
                        fFatal = TRUE;
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

                case 'p':
                case 'P':
                    if (argv[argi][2] == ':')
                        pszPasswd = &argv[argi][3];
                    else
                    {
                        fprintf(stderr, "error: option '-p:' requires password.\n");
                        fFatal = TRUE;
                    }
                    break;

                case 'u':
                case 'U':
                    if (argv[argi][2] == ':')
                        pszUser = &argv[argi][3];
                    else
                    {
                        fprintf(stderr, "error: option '-u:' requires userid.\n");
                        fFatal = TRUE;
                    }
                    break;

                    /*
                    options.fSomeOption = argv[argi][2] != '-';
                    break;
                    */

                default:
                    fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                    fFatal = TRUE;
                    break;
            }
        }
        else
        {
            if (phLog == NULL)
                openLog();
            if (dbConnect(pszHost, pszUser, pszPasswd, pszDatabase))
            {
                ulRc += processFile(argv[argi], &options);
                dbDisconnect();
            }
            else
                fprintf(phLog, "Error connecting to database.\n");
        }
        argi++;
    }

    /* close the log */
    closeLog();

    /* warn if error during processing. */
    if (!fFatal)
        fprintf(stderr, "kHTMLPreCompiler compleated with %ld error%s and %ld warning%s.\n",
                ulRc & 0x0000ffffUL, (ulRc & 0x0000ffffUL) != 1 ? "s" : "",
                ulRc >> 16, (ulRc >> 16) != 1 ? "s" : ""
                );
    return (int)(ulRc & 0x0000ffff);
}


/**
 * signal handler....
 * @param     sig
 * @remark    Needs to flush files before termination. (debugging purpose)
 */
void handler(int sig)
{
    fprintf(stderr, "\n\t!signal %d!\n", sig);
    flushall();
    dbDisconnect();
    exit(-1);
}

/**
 * Display syntax.
 */
static void syntax(void)
{
    printf("\n"
           "kHtmlPC v%01d.%02d - General-purpose HTML precompiler.\n"
           "-------------------------------------------------\n"
           "syntax: kHtmlPc.exe  [-h|-?] [options] [file1 [file2 [..]]\n"
           "\n"
           "    -h or -?      Syntax help. (this)\n"
           "    -h:<hostname> Database server hostname.     default: localhost\n"
           "    -u:<username> Username on the server.       default: root\n"
           "    -p:<password> Password.                     default: <empty>\n"
           "    -d:<database> Database to use.              default: Odin32\n"
           "\n"
           "\n"
           "Copyright (c) 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)",
           VER_MAJOR, VER_MINOR
           );
}


/**
 * Opens log file. Currently this we don't use a logfile, stderr is our logfile.
 */
static void openLog(void)
{
    #if 0
    if (phLog == NULL)
    {
        phLog = fopen("APIImport.Log", "w");
        if (phLog == NULL)
        {
            fprintf(stderr,"error occured while opening log file - will use stderr instead.\n");
            phLog = stderr;
        }
    }
    #else
    phLog = stderr;
    #endif
}


/**
 * Closes the log.
 */
static void closeLog(void)
{
    if (phLog != stderr && phLog != NULL)
        fclose(phLog);
}


/**
 * Preprocesses a file.
 * @returns   high word  Number of warnings.
 *            low  word  Number of errors;
 * @param     pszFilename  Pointer to filename.
 * @param     pOption      Pointer to the option struct.
 * @remark    Big function!
 */
static unsigned long processFile(const char *pszFilename, const POPTIONS pOptions)
{
    unsigned long                      ulRc = 0;
    unsigned long                      ulRc2;
    FILE                              *phFile = NULL;       /* Current output file. */
    kLIFO<kFileEntry>                  lifoFile, lifoUsed;
    kLIFO<kVariableEntry>              lifoVar;             /* tags not implemented yet... */
    kLIFO2<kSqlEntry, kVariableEntry>  lifoSql;
    kFileEntry                        *pCurFile;
    const kVariableEntry              *pVariable = NULL;
    const char                        *psz;
    char                               szVariable[81];

    /* open initial file */
    try
    {
        pCurFile = new kFileEntry(pszFilename);
    }
    catch (int errorcode)
    {
        fprintf(phLog, "error opening initial file. errorcode = %d\n", errorcode);
        return 0x00000001;
    }

    /* loop on pCurFile */
    while (pCurFile != NULL)
    {
        /* loop on curFile->pszCurrent */
        while (pCurFile->pszCurrent != NULL && *pCurFile->pszCurrent != '\0')
        {
            /*********/
            /* !kTag */
            /*********/
            if (pCurFile->pszCurrent[0] == '<' && pCurFile->pszCurrent[1] == '!')
            {   /* find end, replace variables and switch tag type. */
                char  szTag[1024];
                BOOL fQuote = FALSE;
                int  i = 0, j = 0;

                /* copy tag and insert variables */
                while ((fQuote || pCurFile->pszCurrent[i] != '>') && pCurFile->pszCurrent[i] != '\0'
                       && j < (int)sizeof(szTag))
                {
                    fQuote = fQuote ? pCurFile->pszCurrent[i] != '"' : pCurFile->pszCurrent[i] == '"';
                    /* variable? */
                    if (pCurFile->pszCurrent[i] == '$' && pCurFile->pszCurrent[i+1] == '(')
                    {
                        psz = pCurFile->pszCurrent + i + 2;
                        while (*psz != ')' && *psz != '\0' && (psz - &pCurFile->pszCurrent[i]) < (int)(sizeof(szVariable)-1))
                            psz++;
                        if (*psz == ')')
                        {
                            strncpy(&szVariable[0], pCurFile->pszCurrent + i + 2, psz - &pCurFile->pszCurrent[i] - 2);
                            szVariable[psz - &pCurFile->pszCurrent[i] - 2] = '\0';

                            pVariable = lifoSql.findSub(&szVariable[0]);
                            if (pVariable == NULL)
                                pVariable = lifoVar.find(&szVariable[0]);

                            if (pVariable != NULL)
                            {
                                if(j + strlen(pVariable->getValue()) < sizeof(szTag))
                                {   /* copy variable value */
                                    strcpy(&szTag[j], pVariable->getValue());
                                    j += strlen(&szTag[j]) - 1;
                                    i += psz - &pCurFile->pszCurrent[i];
                                }
                                else
                                {   /* warning - complain, skip and continue */
                                    fprintf(phLog, "%s(%ld) : warning: Variable '%s' out of space, increase the size of szTag.\n",
                                           pCurFile->getFilename(), pCurFile->getLineNumber()+1, &szVariable[0]);
                                    szTag[j] = pCurFile->pszCurrent[i];
                                }
                            }
                            else
                            {   /* warning - complain, skip and continue */
                                fprintf(phLog, "%s(%ld) : warning: Variable '%s' not defined.\n",
                                       pCurFile->getFilename(), pCurFile->getLineNumber()+1, &szVariable[0]);
                                szTag[j] = pCurFile->pszCurrent[i];
                            }
                        }
                        else
                        {   /* error - complain, skip and continue. */
                            fprintf(phLog, "%s(%ld) : error: Missing left parenthese on variable expression\n",
                               pCurFile->getFilename(), pCurFile->getLineNumber()+1);
                            szTag[j] = pCurFile->pszCurrent[i];
                        }
                    }
                    else
                        szTag[j] = pCurFile->pszCurrent[i];
                    /* next */
                    i++;
                    j++;
                }


                /* copy ok? */
                if (j < (int)sizeof(szTag) && pCurFile->pszCurrent[i] == '>')
                {
                    szTag[j++] = '>';
                    szTag[j] = '\0';
                    i++;
                    try
                    {
                        /* try create tag object */
                        kTag tag(&szTag[0]);

                        /* check for warning */
                        if (tag.queryWarning() != NULL)
                            fprintf(phLog, "%s(%ld) : warning: tag warning - %s.\n",
                                pCurFile->getFilename(), pCurFile->getLineNumber()+1, tag.queryWarning());


                        /**************/
                        /**************/
                        /* tag switch */
                        /**************/
                        /**************/
                        ulRc2 = 0;
                        if (tag.isTag("!kSql"))                         /* !kSql        */
                            ulRc2 = tagkSql(TAG_PARAMETERS);
                        else if (tag.isTag("!/kSql"))                   /* !/kSql       */
                            ulRc2 = tagEndkSql(TAG_PARAMETERS);
                        else if (tag.isTag("!kTemplate"))               /* !kTemplate   */
                            ulRc2 = tagkTemplate(TAG_PARAMETERS, phFile, pOptions);
                        else if (tag.isTag("!kInclude"))                /* !kInclude    */
                            ulRc2 = tagkInclude(TAG_PARAMETERS);
                        else if (tag.isTag("!kGraph"))                  /* !kGraph      */
                            ulRc2 = tagkGraph(TAG_PARAMETERS, pOptions);
                        else if (tag.isTag("!kDefine"))                 /* !kDefine     */
                            ulRc2 = tagkDefine(TAG_PARAMETERS);
                        else if (tag.isTag("!kUndef"))                  /* !kUndef      */
                            ulRc2 = tagkUndef(TAG_PARAMETERS);
                        else if (tag.isTag("!kIf"))                     /* !kIf         */
                            ulRc2 = tagkIf(TAG_PARAMETERS);
                        else if (tag.isTag("!kElse"))                   /* !kElse       */
                            ulRc2 = tagkElse(TAG_PARAMETERS);
                        else if (tag.isTag("!kEndif"))                  /* !kEndif      */
                            ulRc2 = tagkEndif(TAG_PARAMETERS);
                        else
                        {
                            if (phFile != NULL)
                                fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                            pCurFile->pszCurrent++;
                        }

                        /* if error occurred - skip and continue. */
                        if ((ulRc2 & 0x0000ffff) != 0)
                        {
                            if (phFile != NULL)
                                fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                            pCurFile->pszCurrent++;
                        }
                        ulRc += ulRc2;
                    }
                    catch (kError::enmErrors enmErrorCd)
                    {
                        fprintf(phLog, "%s(%ld) : error: tag error - %s.\n",
                                pCurFile->getFilename(), pCurFile->getLineNumber()+1, kError::queryDescription(enmErrorCd));
                        if (phFile != NULL)
                            fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                        pCurFile->pszCurrent++;
                        ulRc += 0x00000001;
                    }
                }
                else
                {
                    fprintf(phLog, "%s(%ld) : error:  error occurred extracting tag.\n\tPossible reasons: unbalanced quotes, missing '>',...\n",
                            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
                    if (phFile != NULL)
                        fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                    pCurFile->pszCurrent++;
                    ulRc += 0x00000001;
                }

            }
            /*************/
            /*************/
            /*  VARIABLE */
            /*************/
            /*************/
            else if (pCurFile->pszCurrent[0] == '$' && pCurFile->pszCurrent[1] == '(')
            {
                psz = pCurFile->pszCurrent + 2;
                while (*psz != ')' && *psz != '\0' && (psz - pCurFile->pszCurrent) < (int)(sizeof(szVariable)-1))
                    psz++;
                if (*psz == ')')
                {
                    strncpy(&szVariable[0], pCurFile->pszCurrent+2, psz - pCurFile->pszCurrent-2);
                    szVariable[psz - pCurFile->pszCurrent - 2] = '\0';

                    pVariable = lifoSql.findSub(&szVariable[0]);
                    if (pVariable == NULL)
                        pVariable = lifoVar.find(&szVariable[0]);

                    if (pVariable != NULL)
                    {
                        /* write variable */
                        if (phFile != NULL)
                            fwrite(pVariable->getValue(), strlen(pVariable->getValue()), 1, phFile);
                    }
                    else
                    {   /* warning - complain, skip and continue */
                        fprintf(phLog, "%s(%ld) : warning: Variable '%s' not defined.\n",
                               pCurFile->getFilename(), pCurFile->getLineNumber()+1, &szVariable[0]);
                        if (phFile != NULL)
                            fwrite(pCurFile->pszCurrent, psz - pCurFile->pszCurrent + 1, 1, phFile);
                        ulRc += 0x00010000;
                    }
                    pCurFile->pszCurrent = psz + 1;
                }
                else
                {   /* error - complain, skip and continue. */
                    fprintf(phLog, "%s(%ld) : error: Missing left parenthese on variable expression\n",
                           pCurFile->getFilename(), pCurFile->getLineNumber()+1);
                    if (phFile != NULL)
                        fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                    pCurFile->pszCurrent++;
                    ulRc += 0x00000001;
                }
            }
            else
            {   /* next */
                if (phFile != NULL)
                    fputc(*pCurFile->pszCurrent, phFile); /* if this proove to be very slow, we'll have to buffer writes better. */
                pCurFile->pszCurrent++;
            }
        }


        /* resume processing of parent file */
        kFileEntry *pFE = lifoFile.pop();
        if (pFE != NULL)
        {
            assert(pFE == pCurFile->queryParent());
            pFE->pszCurrent = pCurFile->queryParentPointer();
        }
        lifoUsed.push(pCurFile);
        pCurFile = pFE;
    }

    /* close file */
    if (phFile != NULL)
        fclose(phFile);

    return ulRc;
}


/**
 * Tag function - kSql.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkSql(TAG_PARAMETER_LIST)
{
    unsigned long   ulRc = 0;

    try
    {
        lifoSql.push(new kSqlEntry(tag, &pCurFile->pszCurrent[i], pCurFile));
        pCurFile->pszCurrent += i;
    }
    catch (kError::enmErrors enmErrorCd)
    {
        fprintf(phLog, "%s(%ld) : error: kSql tag - %s (errorcode=%d)\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1,
                kError::queryDescription(enmErrorCd), enmErrorCd);
        /* fake entry */
        if (enmErrorCd != kError::error_invalid_tag)
            lifoSql.push(new kSqlEntry());
        TAG_PARAMETER_LIST_UNREF;
    }

    return ulRc;
}


/**
 * Tag function - /kSql.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagEndkSql(TAG_PARAMETER_LIST)
{
    unsigned long  ulRc = 0;
    kSqlEntry      *pCurSql;

    pCurSql = lifoSql.pop();
    if (pCurSql != NULL)
    {
        if (!pCurSql->eof())
        {
            if (pCurSql->fetch())
            {
                if (pCurSql->queryFileEntry() != NULL)
                {
                    /* three cases: current file, lifoFile or lifoUsed */
                    if (pCurSql->queryFileEntry() == pCurFile)
                    {   /* 1. current file */
                        pCurFile->pszCurrent = pCurSql->queryBackTrackPos();
                    }
                    else if (lifoFile.exists(pCurSql->queryFileEntry()))
                    {   /* 2. lifoFile */
                        lifoFile.popPush(pCurSql->queryFileEntry(), lifoUsed);
                        pCurFile = lifoFile.pop();
                        if (pCurFile != NULL)
                            pCurFile->pszCurrent = pCurSql->queryBackTrackPos();
                        else
                        {
                            fprintf(phLog, "fatal internal error(%s, %ld, %s, %d): kSqlEntry - pCurFile == NULL\n",
                                    pCurFile->getFilename(), pCurFile->getLineNumber()+1, __FILE__, __LINE__);
                            return (unsigned long)~0;
                        }
                    }
                    else if (lifoUsed.exists(pCurSql->queryFileEntry()))
                    {   /* 3. lifoUsed */
                        lifoFile.popPush(NULL, lifoUsed);
                        assert(lifoFile.isEmpty());
                        if (rebuildFileLIFO(lifoFile, lifoUsed, pCurSql->queryFileEntry()))
                        {
                            pCurFile = lifoFile.pop(); assert(pCurFile == pCurSql->queryFileEntry());
                            pCurFile->pszCurrent = pCurSql->queryBackTrackPos();
                        }
                        else
                        {
                            fprintf(phLog, "fatal internal error(%s, %ld, %s, %d): rebuildLIFO failed\n",
                                    pCurFile->getFilename(), pCurFile->getLineNumber()+1, __FILE__, __LINE__);
                            return (unsigned long)~0;
                        }
                    }
                    else
                    {
                        fprintf(phLog, "internal error(%s, %ld, %s, %d): pFileEntry == NULL\n",
                                pCurFile->getFilename(), pCurFile->getLineNumber()+1, __FILE__, __LINE__);
                        pCurSql = lifoSql.pop(); /* neutralize the push below */
                    }

                    lifoSql.push(pCurSql);
                    pCurSql = NULL;
                }
                else
                {
                    fprintf(phLog, "internal error(%s, %ld, %s, %d): kSqlEntry - pFileEntry == NULL\n",
                            pCurFile->getFilename(), pCurFile->getLineNumber()+1, __FILE__, __LINE__);
                    pCurFile->pszCurrent += i;
                }
            }
            else
            {
                fprintf(phLog, "%s(%ld) : error: sql error, %s\n",
                        pCurFile->getFilename(), pCurFile->getLineNumber()+1,
                        pCurSql->querySqlLastError());
                pCurFile->pszCurrent += i;
            }
        }

        /* finished processing this tag? then delete it! */
        if (pCurSql != NULL)
        {
            delete pCurSql;
            pCurFile->pszCurrent += i;
        }
    }
    else
    {
        fprintf(phLog, "%s(%ld) : error: unexpected '/kSql' tag.\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1);
        pCurFile->pszCurrent += i;
        TAG_PARAMETER_LIST_UNREF;
    }

    return ulRc;
}


/**
 * Tag function - kTemplate.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @param     phFile    Reference to current output file.
 * @param     pOptions  Pointer to the options struct.
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkTemplate(TAG_PARAMETER_LIST, FILE * &phFile, const POPTIONS pOptions)
{
    unsigned long    ulRc = 0;
    const char      *pszFilename;

    /* verify parameters */
    if (tag.getParameterCount() > 1 && tag.getParameterCount() == 0)
    {
        fprintf(phLog, "%s(%ld) : warning: kTemplate - incorrect number of parameters.\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1);
        ulRc += 0x00010000;
    }

    pszFilename = tag.queryParameter("filename");
    if (pszFilename != NULL)
    {
        char  szFullFileName[CCHMAXPATH];
        if (phFile != NULL)
            fclose(phFile);
        if (strlen(pszFilename) + strlen(pOptions->pszBaseDir) + 1 + 1 < sizeof(szFullFileName))
        {
            sprintf(&szFullFileName[0], "%s\\%s", pOptions->pszBaseDir, pszFilename);
            phFile = fopen(&szFullFileName[0], "wb");
            if (phFile != NULL)
                fprintf(phLog, "%s(%ld) : info: new output file, '%s'.\n",
                        pCurFile->getFilename(), pCurFile->getLineNumber()+1, &szFullFileName[0]);
            else
            {
                fprintf(phLog, "%s(%ld) : error: kTemplate - error opening output file '%s'.\n",
                        pCurFile->getFilename(), pCurFile->getLineNumber()+1, &szFullFileName[0]);
                ulRc += 0x00000001;
            }
        }
        else
        {
            fprintf(phLog, "%s(%ld) : error: kTemplate - filename and base dir is too long! '%s\\%s'.\n",
                    pCurFile->getFilename(), pCurFile->getLineNumber()+1, pOptions->pszBaseDir, pszFilename);
            ulRc += 0x00000001;
        }
    }
    else
    {
        fprintf(phLog, "%s(%ld) : error: kTemplate - filename is missing.\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1);
        ulRc += 0x00000001;
        TAG_PARAMETER_LIST_UNREF;
    }

    pCurFile->pszCurrent += i;

    return ulRc;
}


/**
 * Tag function - kInclude.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkInclude(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    try
    {
        lifoFile.push(pCurFile);
        pCurFile = new kFileEntry(tag, pCurFile->pszCurrent+i, pCurFile);
    }
    catch (kError::enmErrors enmErrorCd)
    {
        fprintf(phLog, "%s(%ld) : error: kInclude - %s\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1,
                kError::queryDescription(enmErrorCd));
        pCurFile = lifoFile.pop();
        ulRc = 0x00000001;
        TAG_PARAMETER_LIST_UNREF;
    }

    return ulRc;
}


/**
 * Tag function - kGraph.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkGraph(TAG_PARAMETER_LIST, const POPTIONS pOptions)
{
    unsigned long ulRc = 0;
    /**
    * Tag description:
    * <!kGraph filename="graph.gif" type=<lines|...> [subtype=<normal|...>]
    *  data="sql x,y,color,legend from...."
    *  data="sql x,y,color,legend from...."
    *  data="sql x,y,color,legend from...."
    *  ...
    *  >
    * [] = optional
    */
    try
    {
        kGraph graph(tag, pOptions->pszBaseDir);
        graph.showWarnings(phLog, pCurFile);
        graph.save();
    }
    catch (kError::enmErrors emnErrorCd)
    {
        fprintf(phLog, "%s(%ld) : error: kGraph - %s.\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1,
                kError::queryDescription(emnErrorCd));
        ulRc = 0x00000001;
        TAG_PARAMETER_LIST_UNREF;
    }
    pCurFile->pszCurrent += i;

    return ulRc;
}


/**
 * Tag function - kDefine.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkDefine(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    fprintf(phLog, "%s(%ld) : error: kDefine - This tag is not implemented yet.\n",
            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
    ulRc = 0x00000001;
    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}


/**
 * Tag function - kUndef.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkUndef(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    fprintf(phLog, "%s(%ld) : error: kUndef - This tag is not implemented yet.\n",
            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
    ulRc = 0x00000001;
    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}


/**
 * Tag function - kIf.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkIf(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    fprintf(phLog, "%s(%ld) : error: kIf - This tag is not implemented yet.\n",
            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
    ulRc = 0x00000001;
    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}


/**
 * Tag function - kElse.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkElse(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    fprintf(phLog, "%s(%ld) : error: kElse - This tag is not implemented yet.\n",
            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
    ulRc = 0x00000001;
    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}


/**
 * Tag function - kEndif.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagkEndif(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    fprintf(phLog, "%s(%ld) : error: kEndif - This tag is not implemented yet.\n",
            pCurFile->getFilename(), pCurFile->getLineNumber()+1);
    ulRc = 0x00000001;
    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}


#if 0
/**
 * Tag function - k.
 * @returns   low  word: number of errors
 *            high word: number of warnings
 * @remark    See TAG_PARAMETER_LIST for parameters.
 *            Use TAG_PARAMETERS when calling this function.
 */
static unsigned long tagk(TAG_PARAMETER_LIST)
{
    unsigned long ulRc = 0;

    TAG_PARAMETER_LIST_UNREF;

    return ulRc;
}
#endif


/**
 * Rebuild a fileentry based on a given file entry.
 * @returns   success indicator. TRUE / FALSE.
 * @param     lifoFile       Target.
 * @param     lifoUsed       Source.
 * @param     pFileEntryTop  New to file entry.
 * @remark    Recusive.
 */
static BOOL rebuildFileLIFO(kLIFO<kFileEntry> &lifoFile, kLIFO<kFileEntry> &lifoUsed, const kFileEntry *pFileEntryTop)
{
    /* base case */
    if (pFileEntryTop == NULL)
        return TRUE;
    /* general case */
    pFileEntryTop = lifoUsed.get(pFileEntryTop);
    if (pFileEntryTop != NULL && rebuildFileLIFO(lifoFile, lifoUsed, pFileEntryTop->queryParent()))
        lifoFile.push((kFileEntry*)pFileEntryTop);
    else
        return FALSE;
    return TRUE;
}


/**
 * Duplicates a string.
 * @returns   Pointer to stringcopy. Remeber to delete this!
 * @param     psz  Pointer to string to duplicate.
 */
static char *dupeString(const char *psz)
{
    char *pszDupe;
    if (psz == NULL)
        return NULL;
    pszDupe = new char[strlen(psz)+1];
    return strcpy(pszDupe, psz);
}


#if 0 // not used
/**
 * Upcases a char.
 * @returns   Upper case of the char given in ch.
 * @param     ch  Char to capitalize.
 */
inline char upcase(char ch)
{
    return ch >= 'a' && ch <= 'z' ? (char)(ch - ('a' - 'A')) : ch;
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


/**
 * Trims a string, that is removing blank spaces at start and end.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @result    Blank at end of string is removed. ('\0' is moved to the left.)
 */
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


/**
 * Trims left side of a string; that is removing blank spaces at start.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @remark    non-const edtion.
 */
static char *ltrim(char *psz)
{
    while (*psz == ' ')
        psz++;
    return psz;
}


/**
 * Trims left side of a string; that is removing blank spaces.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @remark    const edtion.
 */
static const char *ltrim(const char *psz)
{
    while (*psz == ' ')
        psz++;
    return psz;
}


/**
 * Trims left side of a string; that is removing blank spaces, new lines and tabs.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @remark    non-const edtion.
 */
static char *ltrimL(char *psz)
{
    while (*psz == ' ' || *psz == '\t' || *psz == '\r' || *psz == '\n')
        psz++;
    return psz;
}
#endif

/**
 * Trims left side of a string; that is removing blank spaces, new lines and tabs.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @remark    const edtion.
 */
static const char *ltrimL(const char *psz)
{
    while (*psz == ' ' || *psz == '\t' || *psz == '\r' || *psz == '\n')
        psz++;
    return psz;
}


/**
 * Formats a raw message to a readable message.
 * @returns   Pointer to a static data field containing the formatted message.
 * @param     pszMsg  message to format.
 */
const char *kError::formatMessage(const char *pszMsg)
{
    static char szData[256];
    int i = 0;

    /* skip first word */
    while (*pszMsg != '_' && *pszMsg != '\0')
        pszMsg++;
    if (pszMsg != '\0')
    {
        pszMsg++;
        do
        {
            if (*pszMsg == '_')
                szData[i++] = ' ';
            else
                szData[i++] = *pszMsg;
        } while (*pszMsg++ != '\0');

        strcat(&szData[0], ".");
    }
    else
        szData[0] = '\0';

    return &szData[0];
}


/**
 * Give a description of an error code.
 * @returns   "Read only"/const string. Never NULL.
 * @param     emnError  Error code.
 */
const char *kError::queryDescription(kError::enmErrors enmError)
{
    switch (enmError)
    {
        case error_sql_failed:
            return kSqlEntry::querySqlLastError();

        ErrorDescCase(no_error                            );
        ErrorDescCase(error_unexpected_end_of_string                 );
        ErrorDescCase(error_unexpected_eot                           );
        ErrorDescCase(error_invalid_tag_start_char                   );
        ErrorDescCase(error_invalid_tagname                          );
        ErrorDescCase(error_invalid_tag                              );
        ErrorDescCase(error_no_filename                              );
        ErrorDescCase(error_unexpected_eof                           );
        ErrorDescCase(error_unbalanced_quotes                        );
        ErrorDescCase(error_eot_not_found                            );
        ErrorDescCase(error_opening_file                             );
        ErrorDescCase(error_determing_file_size                      );
        ErrorDescCase(error_new_failed                               );
        ErrorDescCase(error_reading_file                             );
        ErrorDescCase(error_incorrect_number_of_parameters           );
        ErrorDescCase(error_missing_sql                              );
        ErrorDescCase(error_invalid_sql_tag                          );
        ErrorDescCase(error_opening_output_file                      );
        ErrorDescCase(error_graph_to_small                           );
        ErrorDescCase(error_graph_type_must_be_specified_before_data );
        ErrorDescCase(error_data_param_is_missing_value              );
        ErrorDescCase(error_type_param_is_missing_value              );
        ErrorDescCase(error_invalid_type                             );
        ErrorDescCase(error_filename_param_is_missing_value          );
        ErrorDescCase(error_width_cx_param_is_missing_value          );
        ErrorDescCase(error_height_cy_param_is_missing_value         );
        ErrorDescCase(error_graph_type_is_missing                    );
        ErrorDescCase(error_graph_subtype_is_invalid                 );
        ErrorDescCase(error_filename_is_missing                      );
        ErrorDescCase(error_no_data                                  );
        ErrorDescCase(error_invalid_dimentions                       );
        ErrorDescCase(error_data_param_is_missing_sql_statement      );
        ErrorDescCase(error_xstart_param_is_missing_value            );
        ErrorDescCase(error_xend_param_is_missing_value              );
        ErrorDescCase(error_ystart_param_is_missing_value            );
        ErrorDescCase(error_yend_param_is_missing_value              );

        ErrorDescCase(warning_illegal_string_char                    );
        ErrorDescCase(warning_too_many_parameters                    );
        ErrorDescCase(warning_unexpected_end_of_string               );
        ErrorDescCase(warning_unexpected_eot                         );
        ErrorDescCase(warning_failed_to_open_background_image        );
        ErrorDescCase(warning_failed_to_load_background_image        );
        ErrorDescCase(warning_invalid_sub_type                       );
        ErrorDescCase(warning_title_param_is_missing_value           );
        ErrorDescCase(warning_titlex_param_is_missing_value          );
        ErrorDescCase(warning_titley_param_is_missing_value          );
        ErrorDescCase(warning_invalid_parameter                      );
        ErrorDescCase(warning_legend_is_not_implemented_yet          );
        ErrorDescCase(warning_failed_to_convert_date                 );
        ErrorDescCase(warning_invalid_color_value                    );
        ErrorDescCase(warning_to_many_fields_in_data_sql             );
        ErrorDescCase(warning_pie_not_implemented_yet                );
        ErrorDescCase(warning_negative_values_are_not_supported_yet  );
        ErrorDescCase(warning_background_param_is_missing_value      );
        ErrorDescCase(warning_backgroundcolor_is_invalid             );
        ErrorDescCase(warning_backgroundcolor_param_is_missing_value );
        ErrorDescCase(warning_foregroundcolor_is_invalid             );
        ErrorDescCase(warning_foregroundcolor_param_is_missing_value );
        ErrorDescCase(warning_axiscolor_is_invalid                   );
        ErrorDescCase(warning_axiscolor_param_is_missing_value       );
    }
    return "unknown error";
}


/**
 * Query for a warning.
 * @returns   Const pointer to warning description. NULL if no warning.
 */
const char *kTag::queryWarning(void)
{
    return enmWarning != kError::no_error ? kError::queryDescription(enmWarning) : NULL;
}


/**
 * Anayses an tag: get tagname and parameters (if any).
 * @param     pszTag  Pointer to tag buffer.
 * @sketch
 * @remark    Throws kError::emnErrors on error.
 */
void kTag::analyseTag(const char *pszTag) throw(kError::enmErrors)
{
    char *psz = &szTag[0];

    /* skip blanks at start */
    while (*pszTag == ' ')
        pszTag++;

    /* tagname */
    if (*pszTag++ != '<')
        throw(kError::error_invalid_tag_start_char);
    pszTag = copyTag(psz, pszTag);
    pszTagname = psz;
    psz += strlen(psz) + 1;

    /* parameters */
    if (strncmp(pszTagname, "!--", 3) != 0) /* ignore comments */
    {
        try
        {
            pszTag = ltrimL(pszTag);
            while (*pszTag != '>' && *pszTag != '\0' && cParameters < TAG_MAX_PARAMETERS)
            {
                /* parametername */
                pszTag = copyParameterName(psz, pszTag);
                apszParameters[cParameters] = psz;
                psz += strlen(psz) + 1;

                /* parametervalue */
                apszValues[cParameters] = NULL;
                pszTag = ltrimL(pszTag);
                if (*pszTag == '=')
                {
                    pszTag = ltrimL(pszTag + 1);
                    if (*pszTag != '\"')
                        pszTag = copyParameterValue1(psz, pszTag);
                    else
                        pszTag = copyParameterValue2(psz, pszTag);
                    apszValues[cParameters] = psz;
                    psz += strlen(psz) + 1;
                }

                /* parameter finished - next */
                cParameters++;
                pszTag = ltrimL(pszTag);
            }

            /* check for warnings */
            if (*pszTag != '>' && cParameters >= TAG_MAX_PARAMETERS)
                throw(kError::warning_too_many_parameters);
            if (*pszTag != '>')
                throw(kError::warning_unexpected_eot);
        }
        catch (kError::enmErrors enmErrorCd)
        {
            /* no fatal error --> warning */
            enmWarning = enmErrorCd;
        }
    }
}


/**
 * Copy a tagname.
 * @returns   Pointer to first char after the tag. (pszFrom)
 * @param     pszTo    Pointer to target.
 * @param     pszFrom  Pointer to value string.
 * @remark    Throws kError::emnErrors on error, fatal for this object.
 */
const char *kTag::copyTag(char *pszTo, const char *pszFrom) throw(kError::enmErrors)
{
    const char *psz = pszTo;

    /* checks for errors */
    if (*pszFrom == '>')
        throw(kError::error_unexpected_eot);

    /* skip to start of tagname */
    while (*pszFrom == ' ')
        pszFrom++;

    /* copy string */
    while (
           (*pszFrom >= 'a' && *pszFrom <= 'z') ||
           (*pszFrom >= 'A' && *pszFrom <= 'Z') ||
           (*pszFrom >= '0' && *pszFrom <= '9') ||
           *pszFrom  == '/' ||
           *pszFrom  == '!' || *pszFrom == '-' //<!--> and <!k...>
           )
        *pszTo++ = *pszFrom++;
    *pszTo = '\0';

    /* checks for errors */
    if (*pszFrom == '\0')
        throw(kError::error_unexpected_eot);
    if (*pszFrom != '>' && *pszFrom != ' ' && strncmp("!--", psz, 3) != 0) //<!--aasdfv-->
        throw(kError::error_invalid_tagname);

    return pszFrom;
}


/**
 * Copy a
 * @returns   Pointer to first char after the tag. (pszFrom)
 * @param     pszTo    Pointer to target.
 * @param     pszFrom  Pointer to value string.
 * @remark    Throws kError::emnErrors on error, fatal for this object.
 */
const char *kTag::copyParameterName(char *pszTo, const char *pszFrom) throw(kError::enmErrors)
{
    /* checks for errors */
    if (*pszFrom == '>')
        throw(kError::warning_unexpected_eot);

    /* skip to start of parameter name */
    while (*pszFrom == ' ')
        pszFrom++;

    /* copy string */
    while (
           (*pszFrom >= 'a' && *pszFrom <= 'z') ||
           (*pszFrom >= 'A' && *pszFrom <= 'Z') ||
           (*pszFrom >= '0' && *pszFrom <= '9')
           )
        *pszTo++ = *pszFrom++;
    *pszTo = '\0';

    /* checks for errors */
    if (*pszFrom == '\0')
        throw(kError::warning_unexpected_eot);

    return pszFrom;
}


/**
 * Copy a simple parameter value. A simple parameter value is not enclosed in "".
 * @returns   Pointer to first char after the value. (pszFrom)
 * @param     pszTo    Pointer to target.
 * @param     pszFrom  Pointer to value string.
 * @remark    Throws kError::emnErrors on warning, do not continue analysis.
 */
const char *kTag::copyParameterValue1(char *pszTo, const char *pszFrom) throw(kError::enmErrors)
{
    /* skip to start of parameter value */
    while (*pszFrom == ' ')
        pszFrom++;

    /* copy string */
    while (
           (*pszFrom >= 'a' && *pszFrom <= 'z') ||
           (*pszFrom >= 'A' && *pszFrom <= 'Z') ||
           (*pszFrom >= '0' && *pszFrom <= '9') ||
           *pszFrom  == '.' || *pszFrom == ','
           )
        *pszTo++ = *pszFrom++;
    *pszTo = '\0';

    /* check for warnings */
    if (*pszFrom == '\0')
        throw(kError::warning_unexpected_eot);
    if (*pszFrom !=  ' ' && *pszFrom != '>' && *pszFrom != '\n' && *pszFrom != '\r')
        throw(kError::warning_illegal_string_char);

    return pszFrom;
}


/**
 * Copy an enclosed parameter value. Enclosed in "".
 * @returns   Pointer to first char after the value. (pszFrom)
 * @param     pszTo    Pointer to target.
 * @param     pszFrom  Pointer to value string.
 * @remark    Throws kError::emnErrors on warning, do not continue analysis.
 */
const char *kTag::copyParameterValue2(char *pszTo, const char *pszFrom) throw(kError::enmErrors)
{
    /* skip to start " */
    while (*pszFrom != '\"' && *pszFrom != '>' && *pszFrom != '\0')
        pszFrom++;
    if (*pszFrom != '\"')
        throw(kError::error_unexpected_end_of_string);
    pszFrom++;

    /* copy string */
    while (*pszFrom != '\"' /*&& *pszFrom != '>'*/ && *pszFrom != '\0')
        *pszTo++ = *pszFrom++;
    *pszTo = '\0';

    /* check for warnings */
    if (*pszFrom != '\"')
        throw(kError::warning_unexpected_end_of_string);

    return pszFrom + 1;
}


/**
 * Is this the correct tag?
 * @returns   TRUE pszTagname matches this->pszTagname, else FALSE.
 * @param     pszTagname  Name to match.
 */
BOOL kTag::isTag(const char *pszTagname) const
{
    if (pszTagname == NULL)
        return FALSE;
    return stricmp(pszTagname, this->pszTagname) == 0;
}


/**
 * Constructor.
 * @param     pszTag  Pointer to tag to analyse.
 * @remark    Throws kError::emnErrors on warning, do not continue analysis.
 */
kTag::kTag(const char *pszTag) throw(kError::enmErrors)
    : pszTagname(NULL), cParameters(0), enmWarning(kError::no_error)
{
    analyseTag(pszTag);
}


/**
 * Destructor.
 */
kTag::~kTag(void)
{
}


/**
 * Gets the tag name.
 * @returns   Const pointer to tag name. Not NULL.
 */
const char *kTag::getTagname(void) const
{
    return pszTagname;
}


/**
 * Gets the number of parameters found for this tag.
 * @returns   Number of parameters. -1 on error (should never occur!).
 */
unsigned long kTag::getParameterCount(void) const
{
    return cParameters;
}


/**
 * Seek for the value of a given parameter.
 * @returns   "Readonly"/const pointer to value. NULL if no value or parameter not found.
 * @param     pszParameter  Pointer to parameter name.
 * @remark    Parameters are case insensitive!
 */
const char *kTag::queryParameter(const char *pszParameter) const
{
    unsigned int i = 0;
    while (i < cParameters && stricmp(apszParameters[i], pszParameter) != 0)
        i++;
    return i < cParameters ? apszValues[i]: NULL;
}


/**
 * Checks if a parameter exists.
 * @returns   TRUE: exists, FALSE: does not exist.
 * @param     pszParameter  Pointer to parameter name.
 * @remark    Parameters are case insensitive!
 */
BOOL kTag::queryExists(const char *pszParameter) const
{
    unsigned int i = 0;
    while (i < cParameters && stricmp(apszParameters[i], pszParameter) != 0)
        i++;
    return i < cParameters;
}


/**
 * Gets the parameter name for the given parameter number.
 * @returns   Pointer to "readonly"/const parametername. NULL if outof range.
 * @param     ulOrdinal  Parameter number. 0-based.
 */
const char *kTag::queryParameterName(unsigned long ulOrdinal) const
{
    return ulOrdinal < cParameters ? apszParameters[ulOrdinal] : NULL;
}


/**
 * Gets the value for the given parameter number.
 * @returns   Pointer to "readonly"/const parameter value. NULL if outof range or no value.
 * @param     ulOrdinal  Parameter number. 0-based.
 * @remark    Return value NULL doesn't have to be an error, the parameter may not have a value!
 */
const char *kTag::queryParameterValue(unsigned long ulOrdinal) const
{
    return ulOrdinal < cParameters ? apszValues[ulOrdinal] : NULL;
}


/**
 * Opens and reads a file into memory.
 * @param     pszFilename  Pointer to input filename.
 * @remark    throws error code (enum).
 */
void kFileEntry::openAndReadFile(const char *pszFilename) throw (kError::enmErrors)
{
    FILE *phFile;

    phFile = fopen(pszFilename, "rb");
    if (phFile == NULL)
        throw(kError::error_opening_file);
    if (fseek(phFile, 0, SEEK_END) == 0 &&
        (cbFile = ftell(phFile)) != 0 &&
        fseek(phFile, 0, SEEK_SET) == 0
        )
    {
        pszFile = new char[cbFile+1];
        if (pszFile != NULL)
        {
            memset(pszFile, 0, (size_t)(cbFile+1));
            if (fread(pszFile, cbFile, 1, phFile) != 1)
            {
                fclose(phFile);
                throw(kError::error_reading_file);
            }
            fclose(phFile);
        }
        else
        {
            fclose(phFile);
            throw(kError::error_new_failed);
        }
    }
    else
    {
        fclose(phFile);
        throw(kError::error_determing_file_size);
    }
}


/**
 * Creates a file entry object.
 * @param     pszFilename  Pointer input filename.
 * @sketch    Open file.
 *            Find file length.
 *            Read file into memory.
 * @remark    throws error code enum.
 */
kFileEntry::kFileEntry(const char *pszFilename) throw (kError::enmErrors) : pParent(NULL), pszParent(NULL)
{
    openAndReadFile(pszFilename);
    strcpy(&szFilename[0], pszFilename);
    pszCurrent = pszFile;
}


/**
 * Create a file entry object with parent from a include statement.
 * @parma     tag          Reference to tag object.
 * @param     pszParent    Current pointer in parent file.
 * @param     pParent      Pointer to current file entry.
 * @sketch    Open and read file into memory.
 *            Copy filename.
 *            Set current pointer.
 * @remark    throws error code enum.
 *            TODO - use not fully implemented.
 */
kFileEntry::kFileEntry(const kTag &tag, const char *pszParent, const kFileEntry *pParent) throw (kError::enmErrors)
    : pParent(pParent), pszParent(pszParent)
{
    const char *pszFilename;

    /* verify tag */
    if (stricmp(tag.getTagname(), "!kInclude"))
        throw(kError::error_invalid_tag);

    /* verify parameters */
    if (tag.getParameterCount() != 1)
        throw(kError::error_incorrect_number_of_parameters);

    /* get filename */
    pszFilename = tag.queryParameter("filename");
    if (pszFilename == NULL)
        throw(kError::error_no_filename);

    /* copy filename */
    strcpy(&szFilename[0], pszFilename);

    /* open and read file */
    openAndReadFile(&szFilename[0]);
    pszCurrent = pszFile;
}


/**
 * Destroys the file entry object.
 */
kFileEntry::~kFileEntry()
{
    delete pszFile;
    pszCurrent = NULL;

}


/**
 * Get linenumber.
 * @returns   linenumber, -1 on error
 * @remark    0 based.
 */
long kFileEntry::getLineNumber(void) const
{
    long cLines = 0;
    char *psz = pszFile;

    while (psz < pszCurrent && *psz != '\0')
    {
        if (*psz == '\n')
            cLines++;
        psz++;
    }

    return cLines;
}


/**
 * Creates a variable entry object.
 */
kVariableEntry::kVariableEntry(const char *pszName, const char *pszValue)
{
    this->pszName = dupeString(pszName);
    this->pszValue = dupeString(pszValue);
}


/**
 * Destroys the variable entry object.
 */
kVariableEntry::~kVariableEntry()
{
    if (pszName != NULL)
        delete pszName;
    if (pszValue != NULL)
        delete pszValue;
}


/**
 * Executes the sql query present in pszSql.
 * @remark    Throws error code.
 */
void kSqlEntry::dbExecuteQuery(void) throw (kError::enmErrors)
{
    pres = ::dbExecuteQuery(pszSql);
    if (pres != NULL)
    {
        cRows = dbQueryResultRows(pres);
        dbFetch();
    }
    else
        throw(kError::error_sql_failed);
}


/**
 * Fetch next row of results.
 * @returns   success indicator. TRUE / FASLE.
 * @result    lifoVariables are destroyed.
 */
BOOL kSqlEntry::dbFetch(void)
{
    BOOL fRc = FALSE;

    lifoVariables.destroy();

    if (cRows != 0)
    {
        cRows--;
        fRc = ::dbFetch(pres, dbFetchCallBack, (void*)&lifoVariables);
    }

    return fRc;
}


/**
 * Callback function used when receiving data from a dbFetch call.
 * This function creates a new kVariableEntry from the input, and pushes it
 * onto the variable lifo.
 * @returns   0  success, -1 error.
 * @param     pszValue      Field value. Used as variable value.
 * @param     pszFieldName  Field name. Used as variable name.
 * @param     pvUser        Pointer to user defined data, here a variable lifo.
 */
static long _System dbFetchCallBack(const char *pszValue, const char *pszFieldName, void *pvUser)
{
    kLIFO<kVariableEntry> *plifoVar = (kLIFO<kVariableEntry> *) pvUser;
    kVariableEntry  *pVarEntry;

    if (pszFieldName == NULL)
        return -1;
    if (pszValue == NULL)
        pszValue = "-NULL-";
    pVarEntry = new kVariableEntry(pszFieldName, pszValue);
    if (pVarEntry != NULL)
        plifoVar->push(pVarEntry);
    else
        return -1;
    return 0;
}


/**
 * Frees the database result.
 * @remark    Called during destruction of object.
 */
void kSqlEntry::dbFreeResult(void)
{
    if (pres != NULL)
    {
        ::dbFreeResult(pres);
        pres = NULL;
    }
}


/**
 * Creates a dummy sql entry.
 */
kSqlEntry::kSqlEntry() :
    pszSql(NULL), pres(NULL), cRows(0), pFileEntry(pFileEntry), pszCurrent(pszCurrent)
{
}


/**
 * Interpret the <!kSql ...> tag and creates an sql entry object from it.
 * @param     pszCurrent  Current pointer into the file entry object.
 * @param     pFileEntry  Pointer to current file entry object.
 * @remark    throws error code.
 */
kSqlEntry::kSqlEntry(const kTag &tag, const char *pszCurrent, const kFileEntry *pFileEntry) throw (kError::enmErrors)
    : pszSql(NULL), pres(NULL), cRows(0), pFileEntry(pFileEntry), pszCurrent(pszCurrent)
{
    const char *pszSql;

    /* verify tag */
    if (!tag.isTag("!kSql"))
        throw(kError::error_invalid_tag);
    if (!tag.queryExists("sql"))
        throw(kError::error_missing_sql);
    if (tag.getParameterCount() > 1)
        throw(kError::error_incorrect_number_of_parameters);

    /* get sql */
    pszSql = tag.queryParameter("sql");
    if (pszSql == NULL || strlen(pszSql) == 0)
        throw(kError::error_invalid_sql_tag);

    this->pszSql = dupeString(pszSql);
    if (this->pszSql == NULL)
        throw(kError::error_new_failed);

    /* execute sql */
    dbExecuteQuery();
}


/**
 * Destructor.
 */
kSqlEntry::~kSqlEntry()
{
    if (pszSql != NULL)
        delete pszSql;
    dbFreeResult();
}


/**
 * Equal operator. Comapres this object with a key string.
 * @returns   TRUE if equal, FALSE if not equal.
 * @param     psz  Pointer to key string.
 */
BOOL kSqlEntry::operator ==(const char *psz) const
{
    return lifoVariables.find(psz) != NULL;
}


/**
 * Fetches the next set of data.
 * @returns   Success indicator. TRUE / FALSE.
 */
BOOL kSqlEntry::fetch(void)
{
    return dbFetch();
}


/**
 * Finds a kVariableEntry matching a given key.
 * @returns   Const pointer to variable node or NULL (if not found).
 * @param     pszKey  Key to match.
 */
const kVariableEntry *kSqlEntry::find(const char *pszKey) const
{
    return lifoVariables.find(pszKey);
}


/**
 * Checks if a node exists.
 * @returns   TRUE - node exists, FALSE - node does not exist.
 * @param     pVariableEntry  Pointer to node/entry.
 */
BOOL kSqlEntry::exists(const kVariableEntry *pVariableEntry) const
{
    return lifoVariables.exists(pVariableEntry);
}


/**
 * Gets last sql error.
 * @returns   Pointer to readonly string describing the error.
 */
const char *kSqlEntry::querySqlLastError(void)
{
    return dbGetLastErrorDesc();
}


/**
 * Finds an item in the sublist of the nodes.. Matching a given key.
 * @returns   Pointer to subnode if found.
 * @param     pszKey  Key to match.
 * @remark    uses the const EntryEntry *find(const char *pszKey) const; function.
 */
template <class kEntry, class kEntryEntry>
const kEntryEntry *kLIFO2<kEntry, kEntryEntry>::findSub(const char *pszKey) const
{
    const kEntry *pE = pTop;
    const kEntryEntry *pEE = NULL;

    while (pE != NULL && (pEE = pE->find(pszKey)) == NULL)
        pE = (const kEntry*)pE->getNext();

    return pEE;
}


/**
 * Checks if an item exists.
 * @returns   TRUE - exists, FALSE - doesn't exists
 * @param     pEntryEntry  Pointer to sub not which is to exists.
 * @remark    uses the BOOL exist(const char *pszKey) const; function.
 */
template <class kEntry, class kEntryEntry>
BOOL  kLIFO2<kEntry, kEntryEntry>::existsSub(const kEntryEntry *pEntryEntry) const
{
    kEntry *pE = pTop;
    BOOL    f= FALSE;

    while (pE != NULL && !(f = pE->exists(pEntryEntry)))
        pE = (kEntry *)pE->getNext();

    return f;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator==(const kGraphData &entry) const
{
    return rdX == entry.rdX;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator!=(const kGraphData &entry) const
{
    return rdX != entry.rdX;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator< (const kGraphData &entry) const
{
    return rdX < entry.rdX;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator<=(const kGraphData &entry) const
{
    return rdX <= entry.rdX;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator> (const kGraphData &entry) const
{
    return rdX > entry.rdX;
}


/**
 * BOOL operator.
 */
BOOL kGraphData::operator>=(const kGraphData &entry) const
{
    return rdX >= entry.rdX;
}


/**
 * Adds a warning to the list of warnings.
 * @param     enmErrorCd  Warning code.
 */
void kGraphDataSet::addWarning(kError::enmErrors enmErrorCd)
{
    if (plistWarnings != NULL)
        plistWarnings->insert(new kWarningEntry(enmErrorCd));
    else
        assert(!"internal error");
}


/**
 * Constructs an empty dataset.
 * @remark    Throws kError::enmErrors
 */
kGraphDataSet::kGraphDataSet() throw(kError::enmErrors)
    : rdCount(_NAN), ulColor(0xffffffffUL), clrColor(-1), pszLegend(NULL),
    fXDate(FALSE), plistWarnings(NULL)
{
}


/**
 * Constructs a dataset from a sql statement and a given graph type.
 * @param     pszSql         Sql-statement which gives us the data.
 * @param     enmTypeCd      This is the integer value of the graph type.
 * @param     plistWarnings  Pointer to warning list.
 * @remark    Throws kError::enmErrors
 */
kGraphDataSet::kGraphDataSet(const char *pszSql, int enmTypeCd,
    kSortedList<kWarningEntry> *plistWarnings) throw(kError::enmErrors)
    : rdCount(_NAN), ulColor(0xffffffffUL), clrColor(-1), pszLegend(NULL),
      fXDate(FALSE), plistWarnings(plistWarnings)
{
    void *pres;
    if (pszSql == NULL || strlen(pszSql) == 0)  throw(kError::error_data_param_is_missing_sql_statement);
    if (enmTypeCd == kGraph::unknown)           throw(kError::error_graph_type_must_be_specified_before_data);

    switch (enmTypeCd)
    {
        case kGraph::lines:
        {
            pres = ::dbExecuteQuery(pszSql);
            if (pres != NULL)
            {
                GRAPHCALLBACKPARAM Param = {NULL, this, NULL, (kGraph::enmType)enmTypeCd, 0};
                while (::dbFetch(pres, dbDataSetCallBack, (void*)&Param))
                    Param.lSeqNbr = 0;
            }
            else
                throw(kError::error_sql_failed);
            break;
        }

        case kGraph::pie:
            addWarning(kError::warning_pie_not_implemented_yet);

        default:
            break;
    }
}


/**
 * Callback function used when receiving data.
 * @returns   0  success, -1 error.
 * @param     pszValue      Field value. Used as variable value.
 * @param     pszFieldName  Field name. Used as variable name.
 * @param     pvUser        Pointer to user defined data, here a variable lifo.
 */
long _System dbDataSetCallBack(const char *pszValue, const char *pszFieldName, void *pvUser)
{
    PGRAPHCALLBACKPARAM pParam = (PGRAPHCALLBACKPARAM)pvUser;

    assert(pParam != NULL);

    switch (pParam->enmTypeCd)
    {
        case kGraph::lines:
        {
            switch (pParam->lSeqNbr)
            {
                case 0:
                    pParam->pData = new kGraphData;
                    /* date test */
                    if (strlen(pszValue) == (4+1+2+1+2) && pszValue[4] == '-' && pszValue[7] == '-')
                    {
                        strcpy(&pParam->pData->szXDate[0], pszValue);
                        pParam->pData->rdX = kGraph::dateToDbl(&pParam->pData->szXDate[0]);
                        pParam->pDataSet->fXDate = TRUE;
                    }
                    else
                    {
                        pParam->pData->szXDate[0] = '\0';
                        pParam->pData->rdX = atol(pszValue);
                    }
                    pParam->pDataSet->listData.insert(pParam->pData);
                    break;

                case 1:
                    pParam->pData->rdY = atol(pszValue);
                    break;

                case 2:
                    if (pszFieldName == NULL || stricmp(pszFieldName, "legend") != 0)
                        pParam->pDataSet->setColor(pszValue);
                    else
                        pParam->pDataSet->setLegend(pszValue);
                    break;

                case 3:
                    if (pszFieldName == NULL || stricmp(pszFieldName, "color") != 0)
                        pParam->pDataSet->setLegend(pszValue);
                    else
                        pParam->pDataSet->setColor(pszValue);
                    break;

                default:
                    pParam->pDataSet->addWarning(kError::warning_to_many_fields_in_data_sql);
            }
            break;
        }

        case kGraph::pie:
        default:
            break;
    }

    pParam->lSeqNbr++;
    return 0;
}


/**
 * Destructor.
 */
kGraphDataSet::~kGraphDataSet(void)
{
    if (pszLegend != NULL) delete pszLegend;
}


/**
 * Find max X value in set.
 * @returns   max X value. 0.0 if empty set.
 */
double kGraphDataSet::maxX(void) const
{
    double rdRet = _NAN;
    kGraphData *p = listData.getFirst();

    while (p != NULL)
    {
        if (rdRet < p->rdX || rdRet != rdRet)
            rdRet = p->rdX;
        p = (kGraphData*)p->getNext();
    }

    return rdRet == rdRet ? rdRet : 0.0;
}


/**
 * Find min X value in set.
 * @returns   min X value. 0.0 if empty set.
 */
double kGraphDataSet::minX(void) const
{
    double rdRet = _NAN;
    kGraphData *p = listData.getFirst();

    while (p != NULL)
    {
        if (rdRet > p->rdX || rdRet != rdRet)
            rdRet = p->rdX;
        p = (kGraphData*)p->getNext();
    }

    return rdRet == rdRet ? rdRet : 0.0;
}


/**
 * Find max Y value in set.
 * @returns   max Y value. 0.0 if empty set.
 */
double kGraphDataSet::maxY(void) const
{
    double rdRet = _NAN;
    kGraphData *p = listData.getFirst();

    while (p != NULL)
    {
        if (rdRet < p->rdY || rdRet != rdRet)
            rdRet = p->rdY;
        p = (kGraphData*)p->getNext();
    }

    return rdRet == rdRet ? rdRet : 0.0;
}


/**
 * Find min Y value in set.
 * @returns   min Y value. 0.0 if empty set.
 */
double kGraphDataSet::minY(void) const
{
    double rdRet = _NAN;
    kGraphData *p = listData.getFirst();

    while (p != NULL)
    {
        if (rdRet > p->rdY || rdRet != rdRet)
            rdRet = p->rdY;
        p = (kGraphData*)p->getNext();
    }

    return rdRet == rdRet ? rdRet : 0.0;
}


/**
 * Sets the ulColor data member. This
 * @param     pszColor  Pointer to color string. That is "#RRGGBB".
 */
void kGraphDataSet::setColor(const char *pszColor)
{
    if (ulColor != 0xffffffffUL)
        return;

    if (*pszColor == '#')
    {
        ulColor = 0;
        for (int i = 1; i < 7; i++)
        {
            ulColor = ulColor << 4;
            if (pszColor[i] >= '0' && pszColor[i] <= '9')
                ulColor |= pszColor[i] - '0';
            else if (pszColor[i] >= 'A' && pszColor[i] <= 'F')
                ulColor |= pszColor[i] - 'A' + 0xa;
            else if (pszColor[i] >= 'a' && pszColor[i] <= 'f')
                ulColor |= pszColor[i] - 'a' + 0xa;
            else
                addWarning(kError::warning_invalid_color_value);
        }
    }
    else
        addWarning(kError::warning_invalid_color_value);
}


/**
 * Allocates color and set the clrColor datamember.
 * @param     pGraph  Pointer to gifdraw image structure.
 */
void kGraphDataSet::setColor(gdImagePtr pGraph)
{
    if (clrColor == -1)
        clrColor = gdImageColorAllocate(pGraph,
                                        (int)(0xFF & (ulColor >> 16)),
                                        (int)(0xFF & (ulColor >> 8)),
                                        (int)(0xFF & ulColor));
}


/**
 * Sets the legend string.
 * @param     pszLegend  Legend.
 */
void kGraphDataSet::setLegend(const char *pszLegend)
{
    if (this->pszLegend == NULL)
        this->pszLegend = dupeString(pszLegend);
}


/**
 * Analyses the tag and executes SQL statements.
 * @param     tag         Tag.
 * @param     pszBaseDir  Base output directory.
 * @remark    Throws kError::enmErrors
 */
void  kGraph::analyseTag(const kTag &tag, const char *pszBaseDir) throw(kError::enmErrors)
{
    int cArgs,    i;
    const char    *pszName;
    const char    *pszValue;
    /*const char    *psz;
    long           l;
    unsigned long  ul;*/

    /* tagname */
    if (stricmp(tag.getTagname(), "!kGraph") != 0)
        throw(kError::error_invalid_tagname);

    /* parameters */
    i = 0;
    cArgs = (int)tag.getParameterCount();
    while (i < cArgs)
    {
        pszName  = tag.queryParameterName(i);
        pszValue = tag.queryParameterValue(i);

        if (stricmp(pszName, "data") == 0)
        {
            if ((int)enmTypeCd == unknown) throw(kError::error_graph_type_must_be_specified_before_data);
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_data_param_is_missing_value);
            #if 0
            listDataSets.insert(new kGraphDataSet(pszValue, enmTypeCd, &listWarnings));
            #else
            fetchData(pszValue);
            #endif
        }
        else if (stricmp(pszName, "type") == 0)
        {
            if (pszValue == NULL) throw(kError::error_type_param_is_missing_value);
            if (stricmp(pszValue, "lines") == 0)
                enmTypeCd = lines;
            else if (stricmp(pszValue, "pie") == 0)
                enmTypeCd = pie;
            else
                throw(kError::error_invalid_type);

        }
        else if (stricmp(pszName, "subtype") == 0)
        {
            if (pszValue != NULL && stricmp(pszValue, "normal") == 0)
                enmSubTypeCd = normal;
            else
                addWarning(kError::warning_invalid_sub_type);
        }
        else if (stricmp(pszName, "filename") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_filename_param_is_missing_value);
            pszFilename = new char[strlen(pszValue) + 2 + strlen(pszBaseDir)];
            if (pszFilename == NULL) throw(kError::error_filename_param_is_missing_value);
            sprintf(pszFilename, "%s\\%s", pszBaseDir, pszValue);
        }
        else if (stricmp(pszName, "title") == 0)
        {
            if (pszValue != NULL)
                pszTitle = dupeString(pszValue);
            else
                addWarning(kError::warning_title_param_is_missing_value);
        }
        else if (stricmp(pszName, "titlex") == 0)
        {
            if (pszValue != NULL)
                pszTitleX = dupeString(pszValue);
            else
                addWarning(kError::warning_titlex_param_is_missing_value);
        }
        else if (stricmp(pszName, "titley") == 0)
        {
            if (pszValue != NULL)
                pszTitleY = dupeString(pszValue);
            else
                addWarning(kError::warning_titley_param_is_missing_value);
        }
        else if (stricmp(pszName, "legend") == 0)
        {
            fLegend = pszValue == NULL || stricmp(pszValue, "yes") == 0;
        }
        else if (stricmp(pszName, "width") == 0 || stricmp(pszName, "cx") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_width_cx_param_is_missing_value);
            cX = atol(pszValue);
            if (cX < 100)
                throw(kError::error_graph_to_small);
        }
        else if (stricmp(pszName, "height") == 0 || stricmp(pszName, "cy") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_height_cy_param_is_missing_value);
            cY = atol(pszValue);
            if (cY < 100)
                throw(kError::error_graph_to_small);
        }
        else if (stricmp(pszName, "startx") == 0 || stricmp(pszName, "minx") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_xstart_param_is_missing_value);
            if (isDate(pszValue))
                rdStartX = dateToDbl(pszValue);
            else
                rdStartX = (double)atol(pszValue); /* TODO: real value not supported */
        }
        else if (stricmp(pszName, "endx") == 0 || stricmp(pszName, "maxx") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_xend_param_is_missing_value);
            if (isDate(pszValue))
                rdEndX = dateToDbl(pszValue);
            else
                rdEndX = (double)atol(pszValue); /* TODO: real value not supported */
        }
        else if (stricmp(pszName, "starty") == 0 || stricmp(pszName, "miny") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_ystart_param_is_missing_value);
            rdStartY = (double)atol(pszValue); /* TODO: real value not supported */
        }
        else if (stricmp(pszName, "endy") == 0 || stricmp(pszName, "maxy") == 0)
        {
            if (pszValue == NULL || strlen(pszValue) == 0) throw(kError::error_yend_param_is_missing_value);
            rdEndY = (double)atol(pszValue); /* TODO: real value not supported */
        }
        else if (stricmp(pszName, "background") == 0)
        {
            if (pszValue != NULL)
            {
                if (pszValue[0] == '#')
                    ulBGColor = readColor(pszValue);
                else
                    pszBackground = dupeString(pszValue);
            }
            else
                addWarning(kError::warning_background_param_is_missing_value);
        }
        else if (stricmp(pszName, "backgroundcolor") == 0)
        {
            if (pszValue != NULL)
            {
                if (pszValue[0] == '#')
                    ulBGColor = readColor(pszValue);
                else
                    addWarning(kError::warning_backgroundcolor_is_invalid);
            }
            else
                addWarning(kError::warning_backgroundcolor_param_is_missing_value);
        }
        else if (stricmp(pszName, "foregroundcolor") == 0)
        {
            if (pszValue != NULL)
            {
                if (pszValue[0] == '#')
                    ulFGColor = readColor(pszValue);
                else
                    addWarning(kError::warning_foregroundcolor_is_invalid);
            }
            else
                addWarning(kError::warning_foregroundcolor_param_is_missing_value);
        }
        else if (stricmp(pszName, "axiscolor") == 0)
        {
            if (pszValue != NULL)
            {
                if (pszValue[0] == '#')
                    ulAxisColor = readColor(pszValue);
                else
                    addWarning(kError::warning_axiscolor_is_invalid);
            }
            else
                addWarning(kError::warning_axiscolor_param_is_missing_value);
        }
        else
            addWarning(kError::warning_invalid_parameter);

        /* next */
        i++;
    }

    /* check that required parameters are present */
    if ((int)enmTypeCd == unknown)       throw(kError::error_graph_type_is_missing);
    if ((int)enmSubTypeCd != normal)     throw(kError::error_graph_subtype_is_invalid);
    if (pszFilename == NULL)             throw(kError::error_filename_is_missing);
    if (listDataSets.getFirst() == NULL) throw(kError::error_no_data);
    if (cX < 50 || cY < 40)              throw(kError::error_invalid_dimentions);

    #if 0
    /* Check for date X values */
    fXDate = listDataSets.getFirst()->getXDate();
    #endif
}


/**
 * Fetches data for multiple data sets.
 * @param     pszSql  Pointer to sql statement.
 */
void    kGraph::fetchData(const char *pszSql) throw(kError::enmErrors)
{
    void *pres;

    switch (enmTypeCd)
    {
        case kGraph::lines:
        {
            pres = ::dbExecuteQuery(pszSql);
            if (pres != NULL)
            {
                kGraphDataSet *pDataSet = NULL;
                GRAPHCALLBACKPARAM Param = {this, NULL, NULL, enmTypeCd, 0, NULL, NULL};
                while (::dbFetch(pres, dbGraphCallBack, (void*)&Param))
                {
                    /* find correct dataset: legend present - allways; else first time only.*/
                    if (Param.pszLegend != NULL || pDataSet == NULL)
                        pDataSet = findOrCreateDataSet(Param.pszLegend);
                    assert(pDataSet != NULL);

                    pDataSet->setLegend(Param.pszLegend);
                    pDataSet->setColor(Param.pszColor);
                    pDataSet->listData.insert(Param.pData);

                    /* next */
                    Param.lSeqNbr = 0;
                    Param.pData = NULL;
                    if (Param.pszColor != NULL)
                    {
                        delete Param.pszColor;
                        Param.pszColor = NULL;
                    }
                    if (Param.pszLegend != NULL)
                    {
                        delete Param.pszLegend;
                        Param.pszLegend = NULL;
                    }
                }
                if (Param.pData != NULL) delete Param.pData;
                if (Param.pszColor != NULL) delete Param.pszColor;
                if (Param.pszLegend != NULL) delete Param.pszLegend;
            }
            else
                throw(kError::error_sql_failed);
            break;
        }

        case kGraph::pie:
            addWarning(kError::warning_pie_not_implemented_yet);

        default:
            break;
    }
}



/**
 * Callback function used when receiving data.
 * @returns   0  success, -1 error.
 * @param     pszValue      Field value. Used as variable value.
 * @param     pszFieldName  Field name. Used as variable name.
 * @param     pvUser        Pointer to user defined data, here a variable lifo.
 */
long _System dbGraphCallBack(const char *pszValue, const char *pszFieldName, void *pvUser)
{
    PGRAPHCALLBACKPARAM pParam = (PGRAPHCALLBACKPARAM)pvUser;

    assert(pParam != NULL);

    switch (pParam->enmTypeCd)
    {
        case kGraph::lines:
        {
            switch (pParam->lSeqNbr)
            {
                case 0:
                    pParam->pData = new kGraphData;
                    /* date test */
                    if (strlen(pszValue) == (4+1+2+1+2) && pszValue[4] == '-' && pszValue[7] == '-')
                    {
                        strcpy(&pParam->pData->szXDate[0], pszValue);
                        pParam->pData->rdX = kGraph::dateToDbl(&pParam->pData->szXDate[0]);
                        pParam->pGraph->fXDate = TRUE;
                    }
                    else
                    {
                        pParam->pData->szXDate[0] = '\0';
                        pParam->pData->rdX = atol(pszValue);
                    }
                    break;

                case 1:
                    pParam->pData->rdY = atol(pszValue);
                    break;

                case 2:
                    if (pszFieldName == NULL || stricmp(pszFieldName, "legend") != 0)
                        pParam->pszColor = dupeString(pszValue);
                    else
                        pParam->pszLegend = dupeString(pszValue);
                    break;

                case 3:
                    if (pszFieldName == NULL || stricmp(pszFieldName, "color") != 0)
                        pParam->pszLegend = dupeString(pszValue);
                    else
                        pParam->pszColor = dupeString(pszValue);
                    break;

                default:
                    pParam->pGraph->addWarning(kError::warning_to_many_fields_in_data_sql);
            }
            break;
        }

        case kGraph::pie:
        default:
            break;
    }

    pParam->lSeqNbr++;
    return 0;
}


/**
 * Finds or creates an dataset for the given legend (id).
 * @returns   Pointer to DataSet. (NULL only when new failes.)
 * @param     pszLegend   DataSet identifier. NULL is allowed.
 */
kGraphDataSet *kGraph::findOrCreateDataSet(const char *pszLegend) throw(kError::enmErrors)
{
    kGraphDataSet *p;

    if (pszLegend != NULL)
    {
        p = listDataSets.getFirst();
        while (p != NULL && *p != pszLegend)
            p = (kGraphDataSet *)p->getNext();
    }
    else
        p = NULL;


    if (p == NULL)
        listDataSets.insert(p = new kGraphDataSet());

    return p;
}


/**
 * Draw the base structure of the graph.
 */
void    kGraph::createBaseGraph(void)       throw(kError::enmErrors)
{
    POINTL ptl;
    char   szMax[32];

    pGraph = gdImageCreate(cX, cY);

    /* background? */
    if (pszBackground != NULL)
    {
        gdImagePtr  pBackground;
        FILE       *phFile;
        phFile = fopen(pszBackground, "rb");
        if (phFile)
        {
            pBackground = gdImageCreateFromGif(phFile);
            if (pBackground != NULL)
            {
                gdImageCopyResized(pGraph, pBackground,
                                   0, 0,    //destination low left corner
                                   0, 0,    //source      low left corner
                                   cX, cY,  //destination width and height
                                   gdImageSX(pBackground), //source width
                                   gdImageSY(pBackground)  //source height
                                   );
                gdImageDestroy(pBackground);
            }
            else
                addWarning(kError::warning_failed_to_load_background_image);
            fclose(phFile);
        }
        else
            addWarning(kError::warning_failed_to_open_background_image);
    }

    /* calc base coordinates */
    ptlOrigo.x = max(2 + (fXDate ? 5 : 0)*6, (long)(cX*0.05));
    _itoa((int)maxY(), &szMax[0], 10);
    ptlOrigo.y = max((long)(2 + strlen(&szMax[0])*6), (long)(cY*0.05));
    if (pszTitleX) ptlOrigo.y += 16;
    if (pszTitleY) ptlOrigo.x += 16;
    ptlYEnd.x  = ptlOrigo.x;
    ptlYEnd.y  = cY - ptlOrigo.y - (pszTitle ? max(20, (long)(cY*0.10)) : 0);
    ptlXEnd.x  = cX - ptlOrigo.x;
    ptlXEnd.y  = ptlOrigo.y;

    if (fLegend)
    {
        addWarning(kError::warning_legend_is_not_implemented_yet);
        ptlXEnd.x -= (long)(cX * 0.20);
    }

    /* allocate default colors */
    setColors();

    /* draw axsis */
    if (pszTitleX)
        gdImageStringLB(pGraph, gdFontGiant,
                        (int)(ptlOrigo.x + (ptlXEnd.x - ptlOrigo.x)/2.0 - strlen(pszTitleX)*9/2.0),
                        max(2, (int)(cY*0.01))+ 16,
                        pszTitleX, clrForeground);
    if (pszTitleY)
        gdImageStringUpLB(pGraph, gdFontGiant,
                          max(2, (int)(cX*0.01)),
                          (int)(ptlOrigo.y + (ptlYEnd.y - ptlOrigo.y)/2.0 - strlen(pszTitleY)*9/2.0),
                          pszTitleY, clrForeground);
    if (pszTitle)
    {
        ptl.x = (long)(cX/2.0 - strlen(pszTitle)*9/2.0);
        ptl.y = cY - max(5, (int)(cY*0.02));
        gdImageStringLB(pGraph, gdFontGiant, (int)ptl.x, (int)ptl.y, pszTitle, clrForeground);
        gdImageLineLB(pGraph, (int)ptl.x - 3, (int)ptl.y - 18,
                      (int)ptl.x + strlen(pszTitle)*9 + 5, (int)ptl.y - 18, clrForeground);
    }


    gdImageLineLB(pGraph, (int)ptlOrigo.x,  (int)ptlOrigo.y-2, (int)ptlYEnd.x, (int)ptlYEnd.y+3, clrAxis);
    gdImageLineLB(pGraph, (int)ptlYEnd.x+2, (int)ptlYEnd.y,    (int)ptlYEnd.x, (int)ptlYEnd.y+3, clrAxis);
    gdImageLineLB(pGraph, (int)ptlYEnd.x-2, (int)ptlYEnd.y,    (int)ptlYEnd.x, (int)ptlYEnd.y+3, clrAxis);

    gdImageLineLB(pGraph, (int)ptlOrigo.x-2, (int)ptlOrigo.y,  (int)ptlXEnd.x+3, (int)ptlXEnd.y, clrAxis);
    gdImageLineLB(pGraph, (int)ptlXEnd.x,    (int)ptlXEnd.y+2, (int)ptlXEnd.x+3, (int)ptlXEnd.y, clrAxis);
    gdImageLineLB(pGraph, (int)ptlXEnd.x,    (int)ptlXEnd.y-2, (int)ptlXEnd.x+3, (int)ptlXEnd.y, clrAxis);


    /* scale - x */
    char    szData[20];
    double  rdMin, rdMax, rd, rdD;
    int     cTags;

    rdMax = maxX();
    rdMin = minX();

    if (fXDate)
    {
        if (!kGraph::dblToDate(rdMin, &szData[0]))
            addWarning(kError::warning_failed_to_convert_date);
    }
    else
        sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rdMin);
    gdImageStringLB(pGraph, gdFontSmall,
                    (int)(ptlOrigo.x - strlen(&szData[0])*6/2.0),
                    (int)(ptlOrigo.y - 3),
                    &szData[0], clrForeground);
    if (fXDate)
    {
        if (!kGraph::dblToDate(rdMax, &szData[0]))
            addWarning(kError::warning_failed_to_convert_date);
    }
    else
        sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rdMax);
    gdImageStringLB(pGraph, gdFontSmall,
                    (int)(ptlXEnd.x - strlen(&szData[0])*6/2.0),
                    (int)(ptlOrigo.y - 3),
                    &szData[0], clrForeground);

    rd    = strlen(&szData[0])*6 + 20;             //length of on scale tag
    cTags = (int)((ptlXEnd.x - ptlOrigo.x) / rd);  //max number of tags
    if (cTags > 0)
    {
        rdD = rdMax - rdMin;
        if (rd > 10)
            rdD = (int)(rdD/cTags);
        else
            rdD = ((int)(rdD/cTags*10))/10;
        rd = rdMin + rdD;
        while (rdD > 0.0 && (rd + rdD) <= rdMax)
        {
            int x = (int)(ptlOrigo.x + (ptlXEnd.x - ptlOrigo.x)/(rdMax-rdMin)*(rd - rdMin));
            if (fXDate)
            {
                if (!kGraph::dblToDate(rd, &szData[0]))
                    addWarning(kError::warning_failed_to_convert_date);
            }
            else
                sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rd);
            gdImageStringLB(pGraph, gdFontSmall, x - (int)(strlen(&szData[0])*6/2.0), (int)(ptlOrigo.y - 3),
                            &szData[0], clrForeground);
            gdImageLineLB(pGraph, x, (int)(ptlOrigo.y - 1), x, (int)(ptlOrigo.y + 1), clrAxis);

            /* next */
            rd += rdD;
        }
    }

    /* scale - y */
    rdMax = maxY();
    rdMin = minY();

    sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rdMin);
    gdImageStringLB(pGraph, gdFontSmall,
                    (int)(ptlOrigo.x - 3 - strlen(&szData[0])*6),
                    (int)(ptlOrigo.y + 8/2 + 2),
                    &szData[0], clrForeground);
    sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rdMax);
    gdImageStringLB(pGraph, gdFontSmall,
                    (int)(ptlOrigo.x - 3 - strlen(&szData[0])*6),
                    (int)(ptlYEnd.y + 8/2),
                    &szData[0], clrForeground);

    rd    = 14 + 10;                               //length of on scale tag
    cTags = (int)((ptlYEnd.y - ptlOrigo.y) / rd);  //max number of tags
    if (cTags > 0)
    {
        rdD = rdMax - rdMin;
        if (rd > 10)
            rdD = (int)(rdD/cTags);
        else
            rdD = ((int)(rdD/cTags*10))/10;
        rd = rdMin + rdD;
        while (rdD > 0.0 && (rd + rdD) <= rdMax)
        {
            int y = (int)(ptlOrigo.y + (ptlYEnd.y - ptlOrigo.y)/(rdMax-rdMin)*(rd - rdMin));
            sprintf(&szData[0], "%1.*f", rdMax > 10 ? 0 : 1, rd);
            gdImageStringLB(pGraph, gdFontSmall,
                            (int)(ptlOrigo.x - 3 - strlen(&szData[0])*6),
                            y + 8/2,
                            &szData[0], clrForeground);
            gdImageLineLB(pGraph, (int)(ptlOrigo.x - 1), y, (int)(ptlOrigo.x + 1), y, clrAxis);

            /* next */
            rd += rdD;
        }
    }
}


/**
 * Draw all lines.
 */
void    kGraph::drawLines(void)            throw(kError::enmErrors)
{
    kGraphDataSet *pDataSet;

    assert((int)enmTypeCd == (int)lines);

    pDataSet = listDataSets.getFirst();
    while (pDataSet != NULL)
    {
        pDataSet->setColor(pGraph);
        drawLine(pDataSet);

        /* next */
        pDataSet = (kGraphDataSet *)pDataSet->getNext();
    }
}


/**
 * Draws a line for the given data set.
 * @param     pDataSet  Pointer to the dataset for the line.
 * @remark    not implemented.
 */
void    kGraph::drawLine(const kGraphDataSet *pDataSet) throw(kError::enmErrors)
{
    BOOL   fFirstTime = TRUE;
    int    xPrev = 0; /* don't have to initialized, but it removes gcc warning (release) */
    int    yPrev = 0; /* don't have to initialized, but it removes gcc warning (release) */
    double rdMinX = minX();
    double rdMinY = minY();
    double rdDX = maxX() - rdMinX;
    double rdDY = maxY() - rdMinY;

    kGraphData *p = pDataSet->listData.getFirst();

    if (rdDX != 0.0 && rdDY != 0.0)
    {
        while (p != NULL)
        {
            int x, y;

            x = (int)(ptlOrigo.x + ((ptlXEnd.x - ptlOrigo.x)/rdDX * (p->rdX - rdMinX)));
            y = (int)(ptlOrigo.y + ((ptlYEnd.y - ptlOrigo.y)/rdDY * (p->rdY - rdMinY)));
            if (fFirstTime)
            {
                xPrev = x;
                yPrev = y;
                fFirstTime = FALSE;
            }
            gdImageLineLB(pGraph, xPrev, yPrev, x, y, pDataSet->getColor());

            /* next */
            p = (kGraphData*)p->getNext();
            xPrev = x;
            yPrev = y;
        }
    }
}


/**
 * Draws the legend.
 * @remark    not implemented.
 */
void kGraph::drawLegend(void)  throw(kError::enmErrors)
{
    if (fLegend == FALSE)
        return;

    /* TODO */
}


/**
 * Converts an ISO date to days.
 * @returns   Returns days after Christ, year 0.
 * @param     pszDate  Pointer to ISO date string.
 * @remark    pszDate should be an ISO date, if not an exception may occur.
 *            Use isDate to check it is an ISO date.
 */
double kGraph::dateToDbl(const char *pszDate)
{
    double rdRet = dbDateToDaysAfterChrist(pszDate);
    return rdRet != -1 ? rdRet : _NAN;
}


/**
 * Converts days to an ISO date.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     rdDate   Date in double, (really days from year 0).
 * @param     pszDate  Pointer to result buffer. Will hold ISO date string on successful return.
 */
BOOL kGraph::dblToDate(double rdDate, char *pszDate)
{
    if (rdDate != rdDate)
        return FALSE;
    return dbDaysAfterChristToDate((signed long)rdDate, pszDate);
}


/**
 * Checks if the given text string is an ISO date.
 * @returns   TRUE - if date, else FALSE.
 * @param     pszDate  Possible ISO date string.
 */
BOOL kGraph::isDate(const char *pszDate)
{
    return strlen(pszDate) == 10 && pszDate[4] == '-' && pszDate[7] == '-';
}


/**
 * frees all storage.
 * @remark    Called only at destruction. (That is destructor or when contruction failes.)
 */
void kGraph::destroy(void)
{
    if (pGraph != NULL)         gdImageDestroy(pGraph);
    if (pszFilename != NULL)    delete pszFilename;
    if (pszTitle != NULL)       delete pszTitle;
    if (pszTitleX != NULL)      delete pszTitleX;
    if (pszTitleY != NULL)      delete pszTitleY;
    if (pszBackground != NULL)  delete pszBackground;
}


/**
 * Adds a warning to the list of warnings.
 * @param     enmErrorCd  Warning code.
 */
void   kGraph::addWarning(kError::enmErrors enmErrorCd)
{
    listWarnings.insert(new kWarningEntry(enmErrorCd));
}


/**
 * Finds the max X value in all data sets.
 * @returns   max X value. 0.0 if no sets
 */
double kGraph::maxX(void)
{
    if (rdMaxX != rdMaxX)
    {
        /* first time */
        if (rdEndX != rdEndX)
        {   /* not spesified by user */
            kGraphDataSet *p = listDataSets.getFirst();

            while (p != NULL)
            {
                double rd = p->maxX();
                if (rdMaxX < rd || rdMaxX != rdMaxX)
                    rdMaxX = rd;
                p = (kGraphDataSet*)p->getNext();
            }

            rdMaxX = rdMaxX == rdMaxX ? rdMaxX : 0.0;
        }
        else /* spesified by user */
            rdMaxX = rdEndX;
    }

    return rdMaxX;
}


/**
 * Finds the min X value in all data sets.
 * @returns   min X value. 0.0 if no sets
 * @remark    Addjusts value if close to 0.
 */
double kGraph::minX(void)
{
    if (rdMinX != rdMinX)
    {
        /* first time */
        if (rdStartX != rdStartX)
        {   /* not spesified by user */
            kGraphDataSet *p = listDataSets.getFirst();

            while (p != NULL)
            {
                double rd = p->minX();
                if (rdMinX > rd || rdMinX != rdMinX)
                    rdMinX = rd;
                p = (kGraphDataSet*)p->getNext();
            }

            /* addjustment */
            if (rdMinX != rdMinX)
                rdMinX = fXDate ? maxX() - 1.0 : 0.0;
            else if (rdMinX >= 0.0)
                rdMinX = fXDate ? rdMinX : 0.0;
            else
            {
                /* TODO negative values are not supported yet. */
                addWarning(kError::warning_negative_values_are_not_supported_yet);
                rdMinX = 0.0;
            }
        }
        else /* spesified by user */
            rdMinX = rdStartX;
    }

    return rdMinX;
}


/**
 * Finds the max Y value in all data sets.
 * @returns   max Y value. 0.0 if no sets
 */
double kGraph::maxY(void)
{
    if (rdMaxY != rdMaxY)
    {
        /* first time */
        if (rdEndY != rdEndY)
        {   /* not spesified by user */
            kGraphDataSet *p = listDataSets.getFirst();

            while (p != NULL)
            {
                double rd = p->maxY();
                if (rdMaxY < rd || rdMaxY != rdMaxY)
                    rdMaxY = rd;
                p = (kGraphDataSet*)p->getNext();
            }

            rdMaxY = rdMaxY == rdMaxY ? rdMaxY : 0.0;
        }
        else /* spesified by user */
            rdMaxY = rdEndY;
    }

    return rdMaxY;
}


/**
 * Finds the min Y value in all data sets.
 * @returns   min Y value. 0.0 if no sets
 * @remark    Addjusts value if close to 0.
 */
double kGraph::minY(void)
{
    if (rdMinY != rdMinY)
    {
        /* first time */
        if (rdStartY != rdStartY)
        {   /* not spesified by user */
            kGraphDataSet *p = listDataSets.getFirst();

            while (p != NULL)
            {
                double rd = p->minY();
                if (rdMinY > rd || rdMinY != rdMinY)
                    rdMinY = rd;
                p = (kGraphDataSet*)p->getNext();
            }

            /* addjustment */
            if (rdMinY != rdMinY)
                rdMinY = 0.0;
            else if (rdMinY >= 0.0)
                rdMinY = 0.0;
            else
            {   /* TODO negative values are not supported yet. */
                addWarning(kError::warning_negative_values_are_not_supported_yet);
                rdMinY = 0.0;
            }
        }
        else /* spesified by user */
            rdMinY = rdStartY;
    }

    return rdMinY;
}


/**
 * Reads a color parameter.
 * @returns   24Bit RGB color value.
 * @param     pszColor  Pointer to color string. That is "#RRGGBB".
 */
unsigned long   kGraph::readColor(const char *pszColor)
{
    unsigned long ulColor = 0xffffff;

    if (*pszColor == '#')
    {
        ulColor = 0;
        for (int i = 1; i < 7; i++)
        {
            ulColor = ulColor << 4;
            if (pszColor[i] >= '0' && pszColor[i] <= '9')
                ulColor |= pszColor[i] - '0';
            else if (pszColor[i] >= 'A' && pszColor[i] <= 'F')
                ulColor |= pszColor[i] - 'A' + 0xa;
            else if (pszColor[i] >= 'a' && pszColor[i] <= 'f')
                ulColor |= pszColor[i] - 'a' + 0xa;
            else
                addWarning(kError::warning_invalid_color_value);
        }
    }
    else
        addWarning(kError::warning_invalid_color_value);

    return ulColor;
}


/**
 * Allocates a color in the image.
 * @returns   GifDraw Color.
 * @param     ulColor  24Bit RGB color value.
 */
int kGraph::setColor(unsigned long ulColor)
{
    return gdImageColorAllocate(pGraph,
                                (int)(0xFF & (ulColor >> 16)),
                                (int)(0xFF & (ulColor >> 8)),
                                (int)(0xFF & ulColor));
}


/**
 * Allocates/sets default colors.
 */
void kGraph::setColors(void)
{
    clrBackground = setColor(ulBGColor != ~0UL ? ulBGColor : 0x00000000UL); /* vitally important that this is done first! */
    clrForeground = setColor(ulFGColor != ~0UL ? ulFGColor : 0x0000FF00UL);
    clrAxis      = setColor(ulAxisColor != ~0UL ? ulAxisColor : 0x00808080UL);
}


/**
 * Constructor.
 * @remark    Throws kError::enmError on error.
 */
kGraph::kGraph(const kTag &tag, const char *pszBaseDir) throw(kError::enmErrors)
    : pGraph(NULL), enmTypeCd(unknown), enmSubTypeCd(normal),
    pszFilename(NULL), pszTitle(NULL), pszTitleX(NULL),
    pszTitleY(NULL), pszBackground(NULL), fLegend(FALSE),
    cX(~0L), cY(~0L),
    rdStartX(_NAN), rdEndX(_NAN), rdStartY(_NAN), rdEndY(_NAN),
    fXDate(FALSE),
    ulBGColor(~0UL), ulFGColor(~0UL), ulAxisColor(~0UL),
    rdMaxX(_NAN), rdMinX(_NAN), rdMaxY(_NAN), rdMinY(_NAN)
{
    try
    {
        analyseTag(tag, pszBaseDir);
        createBaseGraph();
        if (fLegend)
            drawLegend();
        drawLines();
    }
    catch (kError::enmErrors enmErrorCd)
    {
        destroy();
        throw(enmErrorCd);
    }
}


/**
 * Destructor.
 * @remark    calls destroy().
 */
kGraph::~kGraph(void)
{
    destroy();
}


/**
 * Saves the graph to the filename specified in the tag.
 * 2
 * @returns   Errorcode.
 */
kError::enmErrors kGraph::save(void)
{
    FILE *phFileOut;

    assert(pGraph != NULL);
    assert(pszFilename != NULL);

    phFileOut = fopen(pszFilename, "wb");
    if (phFileOut != NULL)
    {
        gdImageGif(pGraph, phFileOut);
        fclose(phFileOut);
    }
    else
        return kError::error_opening_output_file;
    return kError::no_error;
}


/**
 * Prints warnings.
 * @returns   Number of warnings.
 * @param     phLog  Pointer to log file handle.
 */
unsigned long kGraph::showWarnings(FILE *phLog, const kFileEntry *pCurFile)
{
    unsigned long cWarnings = 0;
    kWarningEntry *p = listWarnings.getFirst();

    while (p != NULL)
    {
        cWarnings++;
        fprintf(phLog, "%s(%ld) : warning: kGraph - %s\n",
                pCurFile->getFilename(), pCurFile->getLineNumber()+1,
                kError::queryDescription(p->getWarningCode()));
        p = (kWarningEntry*)p->getNext();
    }

    return cWarnings;
}


/* include template code */
#include "kLIFO.cpp"
#include "kList.cpp"

#ifdef __EMX__
template class kLIFO<kSqlEntry>;
#endif

