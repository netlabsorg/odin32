/* $Id: kFileDef.cpp,v 1.13 2002-08-29 07:59:24 bird Exp $
 *
 * kFileDef - Definition files.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define StringCase(psz, pszMatch) (strnicmp(psz, pszMatch, sizeof(pszMatch)-1) == 0 \
                                   && (   psz[sizeof(pszMatch)-1] == '\0' \
                                       || psz[sizeof(pszMatch)-1] == ' '  \
                                       || psz[sizeof(pszMatch)-1] == '\n' \
                                       || psz[sizeof(pszMatch)-1] == '\r' \
                                       || psz[sizeof(pszMatch)-1] == '\t' \
                                       ) \
                                   )

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern "C" {int     strnicmp(const char *, const char *, size_t);}
#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileInterfaces.h"
#include "kFileDef.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
static kFileDef tst((kFile*)NULL);
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static char *dupeString(const char *psz, KBOOL fSkipFirstWord = FALSE);
static char *trim(char *psz);
static char *ltrim(const char *psz);
static char *removeFnutts(char *pszStr);
inline char  upcase(char ch);
static char *stristr(const char *pszStr, const char *pszSubStr);


/**
 * Duplicates a string.
 * @returns Pointer to stringcopy. Remeber to delete this!
 * @param   psz             Pointer to string to duplicate.
 * @param   fSkipFirstWord  Skips the first word before duplicating the string.
 */
static char *dupeString(const char *psz, KBOOL fSkipFirstWord/* = FALSE*/)
{
    char *pszDupe;
    if (psz == NULL)
        return NULL;

    if (fSkipFirstWord)
    {
        while (*psz != ' ' && *psz != '\t' && *psz != '\n' && *psz != '\r' && *psz != '\0')
            psz++;
        psz = ltrim(psz);
    }

    pszDupe = new char[strlen(psz)+1];
    strcpy(pszDupe, psz);
    if (fSkipFirstWord)
        return removeFnutts(pszDupe);
    return pszDupe;
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
    while (*psz == ' ' || *psz == '\t')
        psz++;
    i = strlen(psz) - 1;
    while (i >= 0 && (psz[i] == ' ' || psz[i] == '\t'))
        i--;
    psz[i+1] = '\0';
    return psz;
}


/**
 * Trims a string, that is removing blank spaces at start and end.
 * @returns   Pointer to first non-blank char.
 * @param     psz  Pointer to string.
 * @result    Blank at end of string is removed. ('\0' is moved to the left.)
 */
static char *ltrim(const char *psz)
{
    if (psz == NULL)
        return NULL;

    while (*psz == ' ' || *psz == '\t')
        psz++;
    return (char *)psz;
}



kFileDef::kFileDef(kFile *pFile) throw (kError) :
    kFileFormatBase(pFile),
    pszType(NULL), pszModName(NULL), pszBase(NULL), pszCode(NULL), pszData(NULL), pszDescription(NULL),
    pszExeType(NULL), pszHeapSize(NULL), pszOld(NULL), pszProtmode(NULL), pszStackSize(NULL),
    pszStub(NULL), pSegments(NULL), pImports(NULL), pExports(NULL),
    fProgram(FALSE), fLibrary(FALSE), fPhysicalDevice(FALSE), fVirtualDevice(FALSE),
    fInitInstance(FALSE), fTermInstance(FALSE), fInitGlobal(FALSE), fTermGlobal(FALSE),
    chAppType(kFileDef::unknown)
{
    pFile->setThrowOnErrors();
    pFile->start();
    read(pFile);
        }


/**
 * Destructor. Frees used memory.
 */
kFileDef::~kFileDef() throw (kError)
{
    if (pszType        != NULL) delete pszType;
    if (pszBase        != NULL) delete pszBase;
    if (pszCode        != NULL) delete pszCode;
    if (pszData        != NULL) delete pszData;
    if (pszDescription != NULL) delete pszDescription;
    if (pszExeType     != NULL) delete pszExeType;
    if (pszHeapSize    != NULL) delete pszHeapSize;
    if (pszOld         != NULL) delete pszOld;
    if (pszProtmode    != NULL) delete pszProtmode;
    if (pszStackSize   != NULL) delete pszStackSize;
    if (pszStub        != NULL) delete pszStub;
    while (pSegments != NULL)
    {
        PDEFSEGMENT p = pSegments;
        pSegments = pSegments->pNext;
        if (p->psz != NULL) delete p->psz;
        delete p;
    }
    while (pImports != NULL)
    {
        PDEFIMPORT p = pImports;
        pImports = pImports->pNext;
        if (p->pszName != NULL)     delete p->pszName;
        if (p->pszDll != NULL)      delete p->pszDll;
        if (p->pszIntName != NULL)  delete p->pszIntName;
        delete p;
    }
    while (pExports != NULL)
    {
        PDEFEXPORT p = pExports;
        pExports = p->pNext;
        if (p->pszName != NULL)     delete p->pszName;
        if (p->pszIntName != NULL)  delete p->pszIntName;
        delete p;
    }
}


/**
 * Read/parse the Definition file.
 * @param   pFile   Pointer to fileobject.
 * @remark  throws errorcode on error (TODO: errorhandling)
 */
void kFileDef::read(kFile *pFile) throw (kError)
{
    char *pszTmp;
    char *psz;
    char  szBuffer[256];
    char  ch;

    /* readloop */
    psz = readln(pFile, &szBuffer[0], sizeof(szBuffer));
    while (psz != NULL)
    {
        KBOOL   fNext = TRUE;

        /* if-switch */
        if (StringCase(psz, "LIBRARY"))
        {
            if (pszType != NULL) throw (kError(kError::DEF_MULIPLE_MODULE_STATEMENT));
            pszType = dupeString(psz);
            fLibrary = TRUE;
            if (!setModuleName())
                throw (kError(kError::DEF_BAD_LIBRARY_STATEMENT));
            fInitInstance = stristr(pszType, "INITINSTANCE") != NULL;
            fInitGlobal   = stristr(pszType, "INITGLOBAL")   != NULL;
            fTermInstance = stristr(pszType, "TERMINSTANCE") != NULL;
            fTermGlobal   = stristr(pszType, "TERMGLOBAL")   != NULL || !fTermInstance;
        }
        else if (StringCase(psz, "NAME"))
        {
            if (pszType != NULL) throw (kError(kError::DEF_MULIPLE_MODULE_STATEMENT));
            pszType = dupeString(psz);
            fProgram = TRUE;
            setModuleName();
            if (stristr(pszType, "WINDOWAPI"))
                chAppType = kFileDef::pm;
            else if (stristr(pszType, "NOTWINDOWCOMPAT"))
                chAppType = kFileDef::fullscreen;
            else if (stristr(pszType, "WINDOWCOMPAT"))
                chAppType = kFileDef::pmvio;
            else
                chAppType = kFileDef::unknown;
        }
        else if (StringCase(psz, "PHYSICAL DEVICE")) //gap is fixed to one space, this may be fixed in readln.
        {
            if (pszType != NULL) throw (kError(kError::DEF_MULIPLE_MODULE_STATEMENT));
            pszType = dupeString(psz);
            fPhysicalDevice = TRUE;
            setModuleName();
        }
        else if (StringCase(psz, "VIRTUAL DEVICE")) //gap is fixed to one space, this may be fixed in readln.
        {
            if (pszType != NULL) throw (kError(kError::DEF_MULIPLE_MODULE_STATEMENT));
            pszType = dupeString(psz);
            fVirtualDevice = TRUE;
            setModuleName();
        }
        else if (StringCase(psz, "BASE"))
            pszBase = dupeString(psz, TRUE);
        else if (StringCase(psz, "CODE"))
            pszCode = dupeString(psz, TRUE);
        else if (StringCase(psz, "DATA"))
            pszData = dupeString(psz, TRUE);
        else if (StringCase(psz, "DESCRIPTION"))
            pszDescription = dupeString(psz, TRUE);
        else if (StringCase(psz, "EXETYPE"))
            pszExeType = dupeString(psz, TRUE);
        else if (StringCase(psz, "HEAPSIZE"))
            pszHeapSize = dupeString(psz, TRUE);
        else if (StringCase(psz, "OLD"))
            pszOld = dupeString(psz, TRUE);
        else if (StringCase(psz, "PROTMODE"))
            pszProtmode = dupeString(psz, TRUE);
        else if (StringCase(psz, "STACKSIZE"))
            pszStackSize = dupeString(psz, TRUE);
        else if (StringCase(psz, "STUB"))
            pszStub = dupeString(psz, TRUE);
        else if (StringCase(psz, "SEGMENTS"))
        {
            PDEFSEGMENT *pps = &pSegments;
            while (!isKeyword(psz = readln(pFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
            {
                *pps = new DEFSEGMENT; memset(*pps, 0, sizeof(**pps));
                (**pps).psz = dupeString(psz);

                printf("debug: parsing: %s\n", psz);
                /* seg name */
                (**pps).pszName = ltrim((**pps).psz);
                ch = *(**pps).pszName;
                if (ch == '\'' || ch == '\"')
                    pszTmp = strchr(++(**pps).pszName, ch);
                else
                    pszTmp = strpbrk((**pps).pszName, " \t");
                if (pszTmp)
                {
                    *pszTmp = '\0';

                    /* class ? */
                    pszTmp = ltrim(pszTmp + 1);
                    if (!strnicmp(pszTmp, "CLASS", 5))
                    {
                        (**pps).pszClass = ltrim(pszTmp + 5);
                        ch = *(**pps).pszClass;
                        if (ch == '\'' || ch == '\"')
                            pszTmp = strchr(++(**pps).pszClass, ch);
                        else
                            pszTmp = strpbrk((**pps).pszClass, " \t");
                        if (pszTmp)
                        {
                            *pszTmp++ = '\0';
                        }
                    }
                    if (pszTmp)
                    {
                        /* attribs */
                        (**pps).pszAttr = trim(pszTmp);
                        if (!*(**pps).pszAttr)
                            (**pps).pszAttr = NULL;
                    }
                }

                printf("debug: pszName=%s\n", (**pps).pszName);
                printf("debug: pszClass=%s\n", (**pps).pszClass);
                printf("debug: pszAttr=%s\n", (**pps).pszAttr);
                /* next */
                pps = &(**pps).pNext;
            }
            fNext = FALSE;
        }
        else if (StringCase(psz, "IMPORTS"))
        {
            PDEFIMPORT *ppi = &pImports;
            while (!isKeyword(psz = readln(pFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
            {
                //DOSCALL1.154 or DosQueryHeaderInfo = DOSCALL1.154
                *ppi = new DEFIMPORT; memset(*ppi, 0, sizeof(**ppi));
                (**ppi).ulOrdinal = 0xffffffffUL;

                if ((pszTmp = strchr(psz, '=')) != NULL)
                {
                   *pszTmp = '\0';
                   (**ppi).pszIntName = dupeString(trim(psz));
                   psz = pszTmp + 1;
                }
                if ((pszTmp = strchr(psz, '.')) != NULL)
                {
                    *pszTmp = '\0';
                    (**ppi).pszDll = dupeString(trim(psz));
                    psz = pszTmp + 1;
                }
                psz = trim(psz);
                if (*psz >= '0' && *psz <= '9')
                {
                    (**ppi).ulOrdinal = strtol(psz, &pszTmp, 0);
                    if (psz ==pszTmp) throw (kError(kError::DEF_BAD_IMPORT));
                }
                else
                    (**ppi).pszName = dupeString(psz);
                ppi = &(**ppi).pNext;
            }
            fNext = FALSE;
        }
        else if (StringCase(psz, "EXPORTS"))
        {
            PDEFEXPORT *ppe = &pExports;
            while (!isKeyword(psz = readln(pFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
            {
                /* CloseHandle = CloseHandle@4 @1234 RESIDENTNAME 2 */
                *ppe = new DEFEXPORT; memset(*ppe, 0, sizeof(**ppe));
                (**ppe).ulOrdinal = 0xffffffffUL;
                (**ppe).cParam    = 0xffffffffUL;

                /* look for '=' */
                pszTmp = strchr(psz, '=');
                if (pszTmp != NULL)
                {   /* CloseHandle = CloseHandle@4 */
                    *pszTmp ='\0';
                    (**ppe).pszName = dupeString(trim(psz));
                    psz = trim(pszTmp + 1);

                    pszTmp = strchr(psz, ' ');
                    if (pszTmp != NULL)
                        *pszTmp = '\0';
                    (**ppe).pszIntName = dupeString(trim(psz));
                    if (pszTmp != NULL)
                        psz = pszTmp + 1;
                    else
                        psz = NULL;
                }
                else
                {   /* CloseHandle (no '= CloseHandle@4')*/
                    pszTmp = strchr(psz, ' ');
                    if (pszTmp != NULL)
                        *pszTmp = '\0';
                    (**ppe).pszName = dupeString(trim(psz));
                    if (pszTmp != NULL)
                        psz = pszTmp + 1;
                    else
                        psz = NULL;
                }

                if (psz != NULL)
                {   /* @1234 RESIDENTNAME 2 */
                    pszTmp = strchr(psz, '@');
                    if (pszTmp)
                    {   /* @1234 RESIDENTNAME 2 */
                        psz = pszTmp + 1;
                        (**ppe).ulOrdinal = strtol(psz, &pszTmp, 0);
                        if (pszTmp == psz) throw (kError(kError::DEF_BAD_EXPORT));
                        psz = trim(pszTmp);

                        if (*psz != '\0')
                        {   /* RESIDENTNAME 2 */
                            if (StringCase(psz, "RESIDENTNAME"))
                            {
                                (**ppe).fResident = TRUE;
                                psz = trim(psz + sizeof("RESIDENTNAME") - 1);
                            }
                            else if (StringCase(psz, "NONAME"))
                            {
                                (**ppe).fResident = FALSE;
                                psz = trim(psz + sizeof("NONAME") - 1);
                            }
                        }
                        else
                        {
                            (**ppe).fResident = (**ppe).ulOrdinal == 0xffffffffUL;
                        }
                    }

                    if (*psz != '\0')
                    {   /* 2 */
                        (**ppe).cParam = strtol(psz, &pszTmp, 0);
                        if (pszTmp == psz) throw (kError(kError::DEF_BAD_EXPORT));
                    }
                }

                removeFnutts((**ppe).pszIntName);
                removeFnutts((**ppe).pszName);
                ppe = &(**ppe).pNext;
            }
            fNext = FALSE;
        }
        else
            throw (kError(kError::DEF_UNKOWN_VERB));

        /* next ? */
        if (fNext)
            psz = readln(pFile, &szBuffer[0], sizeof(szBuffer));
    }

    /* sanity check */
    if (pszType == NULL)
        throw (kError(kError::DEF_NO_MODULE_STATEMENT));
}


/**
 * Reads first meaning full line from a file into a buffer.
 * @returns   Pointer to buffer on success;  NULL on error.
 * @param     pFile      Pointer to fileobject to read line from.
 * @param     pszBuffer  Pointer to buffer.
 * @param     cbBuffer   Size of buffer.
 * @remark    tabs are expanded. string is trimmed. comments removed.
 */
char *kFileDef::readln(kFile *pFile, char *pszBuffer, int cbBuffer) throw (kError)
{
    int i;
    int cch;

    do
    {
        /* read line */
        if (pFile->readln(pszBuffer, cbBuffer))
        {
            if (!pFile->isEOF())
                throw (kError(pFile->getLastError()));
            return NULL;
        }

        /* check for and remove comments, and get string length. */
        cch = 0;
        while (pszBuffer[cch] != '\0' &&  pszBuffer[cch] != ';')
            cch++;
        pszBuffer[cch] = '\0';

        if (cch > 0)
        {
            /* remove '\t' - tab is trouble some! */
            for (i = 0; i < cch; i++)
                if (pszBuffer[i] == '\t')
                    pszBuffer[i] = ' ';

            /* trim line - right */
            i = cch - 1;
            while (i >= 0 &&
                   (
                   pszBuffer[i] == '\n' ||
                   pszBuffer[i] == '\r' ||
                   pszBuffer[i] == ' '
                   ))
                i--;
            pszBuffer[++i] = '\0';
            cch = i;

            /* trim line - left */
            i = 0;
            while (i < cch && pszBuffer[i] == ' ')
                i++;
            cch -= i;
            if (i > 0)
                memmove(pszBuffer, &pszBuffer[i], cch + 1);
        }
    } while ((*pszBuffer == ';' || cch == 0) && !pFile->isEOF());

    return !(*pszBuffer == ';' || cch == 0) ? pszBuffer : NULL;
}


/**
 * Checks for keyword.
 * @returns   TRUE - keyword; FALSE - not keyword;
 * @param     psz  Pointer to word/string to check.
 * @remark    TODO - we are going to check WORDS.
 */
KBOOL kFileDef::isKeyword(const char *psz)
{
    return  psz != NULL
        &&
        (
        StringCase(psz, "LIBRARY") ||
        StringCase(psz, "NAME") ||
        StringCase(psz, "PHYSICAL DEVICE") || //gap is fixed to one space, this may be fixed in readln.
        StringCase(psz, "VIRTUAL DEVICE") || //gap is fixed to one space, this may be fixed in readln.
        StringCase(psz, "BASE") ||
        StringCase(psz, "CODE") ||
        StringCase(psz, "DATA") ||
        StringCase(psz, "DESCRIPTION") ||
        StringCase(psz, "EXETYPE") ||
        StringCase(psz, "HEAPSIZE") ||
        StringCase(psz, "OLD") ||
        StringCase(psz, "STACKSIZE") ||
        StringCase(psz, "STUB") ||
        StringCase(psz, "SEGMENTS") ||
        StringCase(psz, "IMPORTS") ||
        StringCase(psz, "EXPORTS")
        );
}


/**
 * Extracts the module name from the pszType string.
 * @returns   Success indicator.
 * @param     pszBuffer  Pointer to string buffer which is to hold the module name upon return.
 * @remark    Assumes that pszBuffer is large enough.
 */
KBOOL  kFileDef::setModuleName(void)
{
    char *pszEnd;
    char *pszStart;

    kASSERT(pszType);

    /* skip the first word */
    pszStart = strpbrk(pszType, " \t");
    if (pszStart != NULL)
    {
        /* if phys/virt device skip second word too */
        if (fVirtualDevice || fPhysicalDevice)
        {
            pszStart = strpbrk(ltrim(pszStart), " \t");
            if (pszStart == NULL)
                return TRUE;
        }

        pszStart = ltrim(pszStart);
        pszEnd = strpbrk(pszStart, " \t");
        if (pszEnd == NULL)
            pszEnd = pszStart + strlen(pszStart);
        pszModName = new char[pszEnd - pszStart + 1];
        memcpy(pszModName, pszStart, pszEnd - pszStart);
        pszModName[pszEnd - pszStart] = '\0';
    }
    else
        return !StringCase(pszType, "LIBRARY");
    return TRUE;
}


/**
 * Query for the module name.
 * @returns Success indicator. TRUE / FALSE.
 * @param   pszBuffer   Pointer to buffer which to put the name into.
 * @param   cchBuffer   Size of the buffer (defaults to 260 chars).
 */
KBOOL  kFileDef::moduleGetName(char *pszBuffer, int cchSize/* = 260*/)
{
    int cch;
    if (pszModName == NULL)
        return FALSE;

    cch = strlen(pszModName) + 1;
    if (cch > cchSize)
        return FALSE;
    memcpy(pszBuffer, pszModName, cch);

    return TRUE;
}


/**
 * Finds the first exports.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 */
KBOOL kFileDef::exportFindFirst(kExportEntry *pExport)
{
    if (pExports != NULL && pExport != NULL)
    {
        pExport->ulOrdinal = pExports->ulOrdinal;
        pExport->achName[0] = '\0';
        if (pExports->pszName != NULL)
            strcpy(&pExport->achName[0], pExports->pszName);

        pExport->achIntName[0] = '\0';
        if (pExports->pszIntName)
            strcpy(&pExport->achIntName[0], pExports->pszIntName);

        pExport->ulAddress = pExport->iObject = pExport->ulOffset = ~0UL;
        pExport->pv = (void*)pExports->pNext;
    }
    else
        return FALSE;
    return TRUE;
}


/**
 * Finds the next export.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 */
KBOOL kFileDef::exportFindNext(kExportEntry *pExport)
{
    if (pExport != NULL && pExport->pv != NULL)
    {
        PDEFEXPORT pExp = (PDEFEXPORT)pExport->pv;

        pExport->ulOrdinal = pExp->ulOrdinal;
        pExport->achName[0] = '\0';
        if (pExp->pszName != NULL)
            strcpy(&pExport->achName[0], pExp->pszName);
        pExport->achIntName[0] = '\0';
        if (pExp->pszIntName)
            strcpy(&pExport->achIntName[0], pExp->pszIntName);
        pExport->ulAddress = pExport->iObject = pExport->ulOffset = ~0UL;
        pExport->pv = (void*)pExp->pNext;
    }
    else
        return FALSE;
    return TRUE;
}


/**
 * Frees resources associated with the communicatin area.
 * It's not necessary to call this when exportFindNext has return FALSE.
 * (We don't allocate anything so it's not a problem ;-)
 * @param   pExport     Communication area which has been successfully
 *                      processed by findFirstExport.
 */
void kFileDef::exportFindClose(kExportEntry *pExport)
{
    pExport = pExport;
    return;
}


/**
 * Lookup information on a spesific export given by ordinal number.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
KBOOL kFileDef::exportLookup(unsigned long ulOrdinal, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
    ulOrdinal = ulOrdinal;
    pExport = pExport;
    return FALSE;
}

/**
 * Lookup information on a spesific export given by name.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
KBOOL kFileDef::exportLookup(const char *  pszName, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
    pszName = pszName;
    pExport = pExport;
    return FALSE;
}


/**
 * Make a Watcom Linker parameter file addtition of this definition file.
 * @returns Success indicator.
 * @param   pFile   File which we're to write to (append).
 *                  Appends at current posistion.
 * @param   enmOS   The target OS of the link operation.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
KBOOL kFileDef::makeWatcomLinkFileAddtion(kFile *pOut, int enmOS) throw(kError)
{
    PDEFSEGMENT pSeg;
    PDEFIMPORT  pImp;
    PDEFEXPORT  pExp;
    pOut->setThrowOnErrors();

    /*
     * Write a little remark first to tell that converted stuff starts here.
     */
    pOut->printf("#\n# Directives generated from .DEF-file.\n#\n");

    /* Format - Module type */
    switch (enmOS)
    {
        case kFileDef::os2:
            pOut->printf("FORMAT OS2 LX %s %s %s\n",
                          fLibrary                               ? "DLL" :
                            (fProgram ? (chAppType == pm         ? "PM"
                                      : (chAppType == fullscreen ? "FULLSCREEN"
                                                                 : "PMCOMPATIBLE"))
                            : (fVirtualDevice                    ? "VIRTDEVICE"
                                                                 : "PHYSDEVICE" )),
                          fLibrary ? (fInitGlobal || (!fInitInstance && !fTermInstance)
                                                                 ? "INITGLOBAL"
                                                                 : "INITINSTANCE")
                                                                 : "",
                          fLibrary ? (fTermGlobal || (!fTermInstance && !fInitInstance)
                                                                 ? "TERMGLOBAL"
                                                                 : "TERMINSTANCE")
                                                                 : "");
            break;

        case kFileDef::os2v1:
            pOut->printf("FORMAT OS2 %s %s %s\n",
                          fLibrary                               ? "DLL" :
                            (fProgram ? (chAppType == pm         ? "PM"
                                      : (chAppType == fullscreen ? "FULLSCREEN"
                                                                 : "PMCOMPATIBLE"))
                            : (fVirtualDevice                    ? "VIRTDEVICE"
                                                                 : "PHYSDEVICE" )),
                          fLibrary ? (fInitGlobal || fTermGlobal ? "INITGLOBAL" /* Not sure how correct these things are! */
                                                                 : "")
                                                                 : "",
                          /*fLibrary ? (fTermGlobal || fInitGlobal ? "TERMGLOBAL"
                                                                 : "")
                                                                 :*/ "");
            break;

        case kFileDef::win32:
            if (fLibrary)
                pOut->printf("FORMAT Window NT DLL %s %s\n"
                             "Runtime Windows\n",
                             fLibrary ? (fInitGlobal ? "INITGLOBAL" : "INITINSTANCE") : "",
                             fLibrary ? (fTermGlobal ? "TERMGLOBAL" : "TERMINSTANCE") : "");
            else
                pOut->printf("FORMAT Window NT\n"
                             "Runtime %s\n",
                             fProgram ? chAppType == pm  ? "Windows" : "Console" : "Native");
            break;
        default:
            return FALSE;
    }


    /* Module name */
    if (pszModName)
        pOut->printf("OPTION MODNAME=%s\n", pszModName);

    /* Description */
    if (pszDescription)
        pOut->printf("OPTION DESCRIPTION '%s'\n", pszDescription);

    /* Base */
    if (pszBase)
        pOut->printf("OPTION OFFSET=%s\n", pszBase);

    /* Stub */
    if (pszStub)
        pOut->printf("OPTION STUB='%s'\n", pszStub);

    /* Old */
    if (pszOld)
        pOut->printf("OPTION OLDLIBRARY=%s\n", pszOld);

    /* Protected mode */
    if (pszProtmode && (enmOS == kFileDef::os2))
        pOut->printf("OPTION PROTMODE\n", pszProtmode);

    /* Stacksize */
    if (pszStackSize)
        pOut->printf("OPTION STACK=%s\n", pszStackSize);

    /* HeapSize */
    if (pszHeapSize)
        pOut->printf("OPTION HEAPSIZE=%s\n", pszHeapSize);

    /* Code  -  not supported */

    /* Data  -  not supported */

    /*
     * Segments.
     */
    pSeg = pSegments;
    while (pSeg != NULL)
    {
        if (pSeg->pszName)
            pOut->printf("SEGMENT '%s' %s\n", pSeg->pszName, pSeg->pszAttr ? pSeg->pszAttr : "");
        if (pSeg->pszClass)
            pOut->printf("SEGMENT CLASS '%s' %s\n", pSeg->pszClass, pSeg->pszAttr ? pSeg->pszAttr : "");
        pSeg = pSeg->pNext;
    }

    /*
     * Imports.
     */
    pImp = pImports;
    while (pImp != NULL)
    {
        if (pImp->pszName == NULL)
            pOut->printf("IMPORT '%s' '%s'.%d\n", pImp->pszIntName, pImp->pszDll, pImp->ulOrdinal);
        else
            pOut->printf("IMPORT '%s' '%s'.'%s'\n", pImp->pszIntName, pImp->pszDll, pImp->pszName);
        pImp = pImp->pNext;
    }

    /*
     * Exports.
     */
    pExp = pExports;
    while (pExp != NULL)
    {
        pOut->printf("EXPORT '%s'", pExp->pszName);
        if (pExp->ulOrdinal != ~0UL)
            pOut->printf(".%d", pExp->ulOrdinal);
        if (pExp->pszIntName)
            pOut->printf("='%s'", pExp->pszIntName);
        if (pExp->fResident && (enmOS == kFileDef::os2 || enmOS == kFileDef::win16))
            pOut->printf(" RESIDENT");
        if (pExp->cParam != ~0UL)
            pOut->printf(" %d", pExp->cParam * 2); /* .DEFs this is number of words. Watcom should have bytes. */
        pOut->printf("\n");
        pExp = pExp->pNext;
    }

    return TRUE;
}




/**
 * Removes '"' and ''' at start and end of the string.
 * @returns     pszStr!
 * @param       pszStr  String that is to have "s and 's removed.
 */
static char *removeFnutts(char *pszStr)
{
    if (pszStr != NULL)
    {
        int cch = strlen(pszStr);
        if (cch > 0 && (pszStr[cch-1] == '"' || pszStr[cch-1] == '\''))
            pszStr[cch-1] = '\0';

        if (*pszStr == '"' || *pszStr == '\'')
            memmove(pszStr, pszStr+1, cch-1);
    }
    return pszStr;
}


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

