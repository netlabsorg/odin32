/*
 * kFileDef - Definition files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define StringCase(psz, pszMatch) (strnicmp(psz, pszMatch, sizeof(pszMatch)-1) == 0)

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kFileFormatBase.h"
#include "kFileDef.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static char *dupeString(const char *psz);
static char *trim(char *psz);
static char *removeFnutts(char *pszStr);

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


kFileDef::kFileDef(FILE *phFile) throw(int)
    :pszType(NULL), pszBase(NULL), pszCode(NULL), pszData(NULL), pszDescription(NULL),
    pszExeType(NULL), pszHeapSize(NULL), pszOld(NULL), pszProtmode(NULL), pszStackSize(NULL),
    pszStub(NULL), pSegments(NULL), pImports(NULL), pExports(NULL)
{
    /* determin file size */
    if (!fseek(phFile, 0, SEEK_SET))
    {
        this->read(phFile);
    }
    else
        throw (0x001);
}


/**
 * Destructor. Frees used memory.
 */
kFileDef::~kFileDef()
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
 * @param     phFile  Handle to file.
 * @remark    throws errorcode on error (TODO: errorhandling)
 */
void kFileDef::read(FILE *phFile) throw (int)
{
    char *pszTmp;
    char *psz;
    char  szBuffer[256];

    /* readloop */
    psz = readln(phFile, &szBuffer[0], sizeof(szBuffer));
    while (psz != NULL)
    {
        BOOL fNext = TRUE;

        /* if-switch */
        if (StringCase(psz, "LIBRARY"))
        {
            if (pszType != NULL) throw (0x101);
            pszType = dupeString(psz);
        }
        else if (StringCase(psz, "NAME"))
        {
            if (pszType != NULL) throw (0x101);
            pszType = dupeString(psz);
        }
        else if (StringCase(psz, "PHYSICAL DEVICE")) //gap is fixed to one space, this may be fixed in readln.
        {
            if (pszType != NULL) throw (0x101);
            pszType = dupeString(psz);
        }
        else if (StringCase(psz, "VIRTUAL DEVICE")) //gap is fixed to one space, this may be fixed in readln.
        {
            if (pszType != NULL) throw (0x101);
            pszType = dupeString(psz);
        }
        else if (StringCase(psz, "BASE"))
            pszBase = dupeString(psz);
        else if (StringCase(psz, "CODE"))
            pszCode = dupeString(psz);
        else if (StringCase(psz, "DATA"))
            pszData = dupeString(psz);
        else if (StringCase(psz, "DESCRIPTION"))
            pszDescription = dupeString(psz);
        else if (StringCase(psz, "EXETYPE"))
            pszExeType = dupeString(psz);
        else if (StringCase(psz, "HEAPSIZE"))
            pszHeapSize = dupeString(psz);
        else if (StringCase(psz, "OLD"))
            pszOld = dupeString(psz);
        else if (StringCase(psz, "PROTMODE"))
            pszProtmode = dupeString(psz);
        else if (StringCase(psz, "STACKSIZE"))
            pszStackSize = dupeString(psz);
        else if (StringCase(psz, "STUB"))
            pszStub = dupeString(psz);
        else if (StringCase(psz, "SEGMENTS"))
        {
            PDEFSEGMENT *pps = &pSegments;
            while (!isKeyword(psz = readln(phFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
            {
                *pps = new DEFSEGMENT; memset(*pps, 0, sizeof(**pps));
                (**pps).psz = dupeString(psz);
                pps = &(**pps).pNext;
            }
            fNext = FALSE;
        }
        else if (StringCase(psz, "IMPORTS"))
        {
            PDEFIMPORT *ppi = &pImports;
            while (!isKeyword(psz = readln(phFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
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
                    if (psz ==pszTmp) throw(0x102);
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
            while (!isKeyword(psz = readln(phFile, &szBuffer[0], sizeof(szBuffer))) && psz != NULL)
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
                        if (pszTmp == psz) throw (0x103);
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
                        if (pszTmp == psz) throw (0x104);
                    }
                }

                removeFnutts((**ppe).pszIntName);
                removeFnutts((**ppe).pszName);
                ppe = &(**ppe).pNext;
            }
            fNext = FALSE;
        }
        else
            throw(0x105);

        /* next ? */
        if (fNext)
            psz = readln(phFile, &szBuffer[0], sizeof(szBuffer));
    }

    /* sanity check */
    if (pszType == NULL)
        throw (0x106);
}


/**
 * Reads first meaning full line from a file into a buffer.
 * @returns   Pointer to buffer on success;  NULL on error.
 * @param     phFile     Filea handle.
 * @param     pszBuffer  Pointer to buffer.
 * @param     cbBuffer   Size of buffer.
 * @remark    tabs are expanded. string is trimmed. comments removed.
 */
char *kFileDef::readln(FILE *phFile, char *pszBuffer, int cbBuffer) throw (int)
{
    int i;
    int cch;

    do
    {
        /* read line */
        if (!fgets(pszBuffer, cbBuffer, phFile))
        {
            if (feof(phFile))
                return FALSE;
            else
                throw (0x201);
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
    } while ((*pszBuffer == ';' || cch == 0) && !feof(phFile));

    return !(*pszBuffer == ';' || cch == 0) ? pszBuffer : NULL;
}


/**
 * Checks for keyword.
 * @returns   TRUE - keyword; FALSE - not keyword;
 * @param     psz  Pointer to word/string to check.
 * @remark    TODO - we are going to check WORDS.
 */
BOOL kFileDef::isKeyword(const char *psz)
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
 * Queries the module name.
 * @returns   Success indicator.
 * @param     pszBuffer  Pointer to string buffer which is to hold the module name upon return.
 * @remark    Assumes that pszBuffer is large enough.
 */
BOOL  kFileDef::queryModuleName(char *pszBuffer)
{
    char *psz;

    if (pszType != NULL && StringCase(pszType, "LIBRARY"))
    {
        psz = pszType + sizeof("LIBRARY") - 1;
        while (*psz == ' ')
            psz++;
        while (*psz != '\0' && *psz != ' ')
            *pszBuffer++ = *psz++;
        *pszBuffer = '\0';
    }
    else
        return FALSE;
    return TRUE;
}


/**
 * Finds the first exports.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 * @remark
 */
BOOL  kFileDef::findFirstExport(PEXPORTENTRY pExport)
{
    if (pExports != NULL && pExport != NULL)
    {
        pExport->ulOrdinal = pExports->ulOrdinal;
        if (pExports->pszName != NULL)
            strcpy(&pExport->achName[0], pExports->pszName);
        else
            pExport->achName[0] = '\0';
        if (pExports->pszIntName)
            strcpy(&pExport->achIntName[0], pExports->pszIntName);
        else
            pExport->achIntName[0] = '\0';
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
 * @remark
 */
BOOL  kFileDef::findNextExport(PEXPORTENTRY pExport)
{
    if (pExport != NULL && pExport->pv != NULL)
    {
        PDEFEXPORT pExp = (PDEFEXPORT)pExport->pv;

        pExport->ulOrdinal = pExp->ulOrdinal;
        if (pExp->pszName != NULL)
            strcpy(&pExport->achName[0], pExp->pszName);
        else
            pExport->achName[0] = '\0';
        if (pExp->pszIntName)
            strcpy(&pExport->achIntName[0], pExp->pszIntName);
        else
            pExport->achIntName[0] = '\0';
        pExport->pv = (void*)pExp->pNext;
    }
    else
        return FALSE;
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
