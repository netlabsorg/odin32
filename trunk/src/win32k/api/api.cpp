/* $Id: api.cpp,v 1.2 2001-01-20 23:51:06 bird Exp $
 *
 * API Overload Init and Helper Function.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_IO


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"
#include "rmalloc.h"
#include "new.h"

#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "log.h"
#include "OS2Krnl.h"
#include "dev32.h"
#include "api.h"
#include "options.h"




/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _MaskArray
{
    int         cMasks;                 /* Number of elements in papszMasks. */
    PSZ *       papszMasks;             /* Array of module/process masks. */
} MASKARRAY, *PMASKARRAY;

typedef struct _ApiDataEntry
{
    BOOL        fEnabled;               /* Only enabled if data was found in .INI file! */
    MASKARRAY   ProcessInc;             /* Process inclusion rules. */
    MASKARRAY   ProcessExc;             /* Process exclusion rules. */
    MASKARRAY   ModuleInc;              /* Module inclusion rules. */
    MASKARRAY   ModuleExc;              /* Module exclusion rules. */
} APIDATAENTRY, *PAPIDATAENTRY;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
APIDATAENTRY    aApiData[API_MAX];      /* Array of api info. */
PSZ             pszFile;                /* Pointer to entire file mapping. */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
APIRET  apiReadIniFile(PSZ pszIniFile);
APIRET  apiParseIniFile(PSZ pszFile);
PSZ     apiStripIniLine(PSZ pszFile, PSZ * ppszFile);
int     apiInterpretApiNo(PSZ pszSection);
void    apiFreeApiData(PAPIDATAENTRY paNewApiData);




/**
 * This function will read the ini file from a give disklocation.
 * @returns OS/2 return code.
 * @param   pszIniFile  Full path to the inifile.
 * @sketch  Open the file.
 *          Determin file size.
 *          Allocate memory for the file.
 *          Read the entire file.
 *          Parse the file
 *          Close the file.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Must hold the loader semaphore before calling this function!
 */
APIRET  apiReadIniFile(PSZ pszIniFile)
{
    SFN     sfn;
    APIRET  rc;

    rc = IOSftOpen(pszIniFile,
                   OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                   OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY,
                   (PSFN)SSToDS(&sfn),
                   NULL);
    if (rc == NO_ERROR)
    {
        ULONG   cbFile = 0;

        rc = SftFileSize(sfn, &cbFile);
        if (rc == NO_ERROR)
        {
            if (cbFile > 256*1024)      /* Max size if 256 KB! */
            {
                rc = ERROR_NOT_ENOUGH_MEMORY;
                kprintf(("apiReadIniFile: Files %s is too large (%d).\n", pszIniFile, cbFile));
            }
            else
            {
                PSZ pszNewFile = (PSZ)rmalloc((size_t)  cbFile + 1);
                if (pszNewFile)
                {
                    ULONG   cbRead;

                    rc = IOSftReadAt(sfn, &cbRead, pszNewFile, 0UL, 0UL);
                    if (rc == NO_ERROR)
                    {
                        memset(pszNewFile + cbRead, 0, (size_t)(cbFile + 1 - cbRead)); /* terminate the file. */
                        rc = apiParseIniFile(pszNewFile);
                    }
                    else
                        kprintf(("apiReadIniFile: Failed to read %s into memory, rc=%d. (size %d)\n", pszIniFile, rc, cbFile));

                    if (rc != NO_ERROR)
                        rfree(pszNewFile);
                }
                else
                {
                    rc = ERROR_NOT_ENOUGH_MEMORY;
                    kprintf(("apiReadIniFile: Failed to allocate %d of resident memory.\n", cbFile));
                }
            }
        }
        else
            kprintf(("apiReadIniFile: Failed to determin size of %s, rc=%d\n", pszIniFile, rc));

        IOSftClose(sfn);
    }
    else
        kprintf(("apiReadIniFile: Failed to open file %s, rc=%d\n", pszIniFile, rc));

    return rc;
}


/**
 * Parse the inifile.
 * @returns OS/2 return code.
 * @param   pszFile     Pointer to file mapping.
 * @sketch
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET  apiParseIniFile(PSZ pszFile)
{
    PAPIDATAENTRY   paNewApiData;
    PSZ *           ppszFile = (PSZ*)SSToDS(&pszFile);
    PSZ             pszLine;
    APIRET          rc;

    /*
     * Allocate and zero temporary api data structure.
     */
    paNewApiData = (PAPIDATAENTRY)rmalloc(sizeof(aApiData));
    if (paNewApiData == NULL)
    {
        kprintf(("apiParseIniFile: failed to allocate temporary struct.\n"));
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    memset(paNewApiData, 0, sizeof(aApiData));

    /*
     * We'll loop until end of file.
     * This is a double loop. Outer loop on section level. Inner loop
     * on Type level.
     */
    rc = NO_ERROR;
    pszLine = apiStripIniLine(pszFile, ppszFile);
    while (pszLine != NULL && rc == NO_ERROR)
    {
        char ch;

        if ((ch = *pszLine) != '\0')
        {
            if (ch == '[')
            {
                int iApi = apiInterpretApiNo(pszLine);
                if (iApi >= 0 && iApi < API_MAX)
                {
                    PMASKARRAY pMaskArray = &paNewApiData[iApi].ModuleExc;

                    /*
                     * Enable api data entry.
                     * Get a line.
                     * Check for end-of-file and new section.
                     * Skip empty lines.
                     * Uppercase the line.
                     * If "Type=" line Then Change type entry.
                     * Else Add line to current type entry (default to module exclude).
                     */
                    paNewApiData[iApi].fEnabled = TRUE;
                    while ((pszLine = apiStripIniLine(pszFile, ppszFile)) != NULL
                           && *pszLine != '[')
                    {
                        if (*pszLine == '\0')
                            continue;
                        strupr(pszLine);
                        if (strcmp("TYPE=", pszLine) == 0)
                        {
                            pszLine += 5;
                            if (strstr(pszLine, "PROC"))
                                pMaskArray = strstr(pszLine, "INC")
                                    ? &paNewApiData[iApi].ProcessInc
                                    : &paNewApiData[iApi].ProcessExc; /* default */
                            else
                                pMaskArray = strstr(pszLine, "INC")
                                    ? &paNewApiData[iApi].ModuleInc
                                    : &paNewApiData[iApi].ModuleExc; /* default */
                        }
                        else
                        {
                            if (pMaskArray->cMasks % 8 == 0)
                            {
                                void *pv = rrealloc(pMaskArray->papszMasks, 8 + pMaskArray->cMasks);
                                if (pv == NULL)
                                {
                                    rc = ERROR_NOT_ENOUGH_MEMORY;
                                    kprintf(("apiParseIniFile: Failed to allocate more mask array memory. %c masks\n", pMaskArray->cMasks));
                                    break;
                                }
                                pMaskArray->papszMasks = (PSZ*)pv;
                            }
                            pMaskArray->papszMasks[pMaskArray->cMasks++] = pszLine;
                        }
                    } /* inner loop */
                }
                else
                {
                    kprintf(("apiParseIniFile: bogus api no.%d (%s)\n", iApi, pszLine));
                    pszLine = apiStripIniLine(pszFile, ppszFile);
                }
            }
            else
            {
                kprintf(("apiParseIniFile: bogus line encountered: %s\n", pszLine));
                pszLine = apiStripIniLine(pszFile, ppszFile);
            }
        }
        else
            pszLine = apiStripIniLine(pszFile, ppszFile);
    } /* outer loop */


    /*
     * If we were successfull we'll replace the existing api data with
     * the data we've just read.
     *   (TODO: Not sure (ie. quite sure) if we need some kind of serialization here...)
     * If not we'll free any used memory before returning failure.
     */
    if (rc == NO_ERROR)
    {
        /* add spin lock */
        apiFreeApiData(&aApiData[0]);
        memcpy(&aApiData[0], paNewApiData, sizeof(aApiData));
        /* remove spin lock */
    }
    else
        apiFreeApiData(paNewApiData);
    rfree(paNewApiData);

    return rc;
}



/**
 * Strips and extract a line advancing the *ppszFile pointer to
 * the next line. Comments are also stripped.
 * @returns Pointer to line. NULL if end of file.
 * @param   pszFile     Pointer to line.
 * @param   ppszFile    Pointer to pointer variable.
 * @sketch
 * @status  completly implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
PSZ     apiStripIniLine(PSZ pszFile, PSZ * ppszFile)
{
    PSZ     pszComment;
    PSZ     pszLine;
    char    ch;

    /*
     * If end of file Then return NULL.
     */
    if (*pszFile)
        return NULL;

    /*
     * Strip start of line.
     */
    while ((ch = *pszFile) == ' ' || ch == '\t')
        pszFile++;
    pszLine = pszFile;

    /*
     * Look for the line end and any evt. comment (starts with a ';').
     */
    pszComment = NULL;
    while ((ch = *pszFile) != '\r' && ch != '\n' && ch != '\0')
    {
        if (ch == ';')
            pszComment = pszFile;
        pszFile++;
    }

    /*
     * Update pszComment with the line end if not already set (we'll
     *   use this pointer to right strip the line).
     * If not last line, then terminate the line.
     * Skip return and newline characters to position the *ppszFile pointer
     *   at the next line.
     * Update the next line pointer.
     */
    if (pszComment == NULL)
        pszComment = pszFile;
    if (ch != '\0')
    {
        *pszFile = '\0';
        while ((ch = *pszFile) == '\r' || ch == '\n')
            pszFile++;
    }
    *ppszFile = pszFile;

    /*
     * Right strip the line (starts with pszComment).
     */
    pszComment--;
    while ((ch = *pszComment) == ' ' || ch == '\t')
        pszComment--;
    pszComment[1] = '\0';

    return pszLine;
}


/**
 * Reads the section header '[<ApiNbr]' and translates it into an
 * api index (into aApiData). Range is not checked.
 * @returns API index. -1 on error. Check range!
 * @param   pszSection  Pointer to the section header string.
 *                      Assumes that it's pointing to the '['.
 * @sketch  Skip '['.
 *          Skip blanks.
 *          Convert decimal number string and return it.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Layz, we only read '[n' no checking for the ending ']'.
 */
int     apiInterpretApiNo(PSZ pszSection)
{
    int iApi = 0;

    pszSection++;                          /* skip '[' */
    if (*pszSection < '0' || *pszSection > '9')
        return -1;

    while (*pszSection == ' ' || *pszSection == '\t')
        pszSection++;

    while (*pszSection >= '0' || *pszSection <= '9')
    {
        iApi = (iApi * 10) + *pszSection - '0';
        pszSection++;
    }

    return iApi;
}


/**
 * Frees internal data in a api data structure.
 * @param   paNewApiData    Pointer to api data table.
 * @sketch  Loop thru all api entries and free mask array pointers.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Any serialization is not my problem.
 */
void    apiFreeApiData(PAPIDATAENTRY paNewApiData)
{
    int i;

    for (i = 0; i < API_MAX; i++)
    {
        if (paNewApiData[i].ProcessInc.cMasks)
            rfree(paNewApiData[i].ProcessInc.papszMasks);
        if (paNewApiData[i].ProcessExc.cMasks)
            rfree(paNewApiData[i].ProcessExc.papszMasks);
        if (paNewApiData[i].ModuleInc.cMasks)
            rfree(paNewApiData[i].ModuleInc.papszMasks);
        if (paNewApiData[i].ModuleExc.cMasks)
            rfree(paNewApiData[i].ModuleExc.papszMasks);
    }
}



BOOL _Optlink   APIQueryEnabled(int iApi, USHORT usCS, LONG ulEIP)
{
    PAPIDATAENTRY   pEntry;

    /*
     * Check if these enhancements are switched off globally.
     */
    if (isApiEnhDisabled())
        return FALSE;

    /*
     * Aquire read lock - TODO.
     */


    /*
     * Get data entry pointer.
     * Check if entry is enabled.
     */
    pEntry = &aApiInfo[iApi];
    if (pEntry->fEnabled)
    {
        CHAR    szName[CCHMAXPATH];

    }
    else
        fRet = FALSE;


    /*
     * Release read lock - TODO.
     */


    return fRet;
}



/**
 * Init The Api Overloading SubSystem.
 * @returns OS/2 return code.
 * @sketch  Find Ini file location.
 *          Read the inifile and there by initiate the aApiData strcut.
 * @status  completly implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET _Optlink APIInit(void)
{
    APIRET  rc;

    rc = apiReadIniFile(&szWin32kIni[0]);

    return rc;
}

