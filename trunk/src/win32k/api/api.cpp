/* $Id: api.cpp,v 1.3 2001-01-21 07:52:46 bird Exp $
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
#define INCL_OS2KRNL_ALL


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
#include "ldrCalls.h"
#include "dev32.h"
#include "api.h"
#include "options.h"
#include "locks.h"




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
APIDATAENTRY    aApiData[API_CENTRIES]; /* Array of api info. */
PSZ             pszFile;                /* Pointer to entire file mapping. */
RWLOCK          ApiInfoRWLock;          /* Read/Write lock for api data. */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
APIRET  apiReadIniFile(PSZ pszIniFile);
APIRET  apiParseIniFile(PSZ pszFile);
PSZ     apiStripIniLine(PSZ pszFile, PSZ * ppszFile);
int     apiInterpretApiNo(PSZ pszSection);
void    apiFreeApiData(PAPIDATAENTRY paNewApiData);
void    apiSortApiData(PAPIDATAENTRY paApiData);
void    apiSortMaskArray(PMASKARRAY pMasks);
BOOL    apiFindNameInMaskArray(PSZ pszName, PMASKARRAY pMasks);
APIRET  apiGetProccessName(PSZ pszName);
APIRET  apiGetModuleName(PSZ pszName, USHORT usCS, ULONG ulEIP);


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
                if (iApi >= 0 && iApi < API_CENTRIES)
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
        apiSortApiData(paNewApiData);
        RWLockAcquireWrite(&ApiInfoRWLock);
        apiFreeApiData(&aApiData[0]);
        memcpy(&aApiData[0], paNewApiData, sizeof(aApiData));
        RWLockReleaseWrite(&ApiInfoRWLock);
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
 * @param   paApiData   Pointer to api data table.
 * @sketch  Loop thru all api entries and free mask array pointers.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Any serialization is not my problem.
 */
void    apiFreeApiData(PAPIDATAENTRY paApiData)
{
    int i;

    for (i = 0; i < API_CENTRIES; i++)
    {
        if (paApiData[i].ProcessInc.cMasks)
            rfree(paApiData[i].ProcessInc.papszMasks);
        if (paApiData[i].ProcessExc.cMasks)
            rfree(paApiData[i].ProcessExc.papszMasks);
        if (paApiData[i].ModuleInc.cMasks)
            rfree(paApiData[i].ModuleInc.papszMasks);
        if (paApiData[i].ModuleExc.cMasks)
            rfree(paApiData[i].ModuleExc.papszMasks);
    }
}


/**
 * Sort the entire api data structure.
 * @param   paApiData   Pointer to api data to sort.
 * @sketch  Loop thru all entries and sort all four mask arrays.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  See apiSortMaskArray.
 */
void    apiSortApiData(PAPIDATAENTRY paApiData)
{
    int i;

    for (i = 0; i < API_CENTRIES; i++)
    {
        apiSortMaskArray(&paApiData[i].ProcessInc);
        apiSortMaskArray(&paApiData[i].ProcessExc);
        apiSortMaskArray(&paApiData[i].ModuleInc);
        apiSortMaskArray(&paApiData[i].ModuleExc);
    }
}


/**
 * Sorts the content of an mask array.
 * Duplicates are removed.
 * @param   pMasks  Pointer to a mask array structure.
 * @sketch  Use bouble sort.
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Duplicate submasks aren't tested for.
 *          example: "DOSCALL1.DLL" is equal to "DOS*"
 */
void    apiSortMaskArray(PMASKARRAY pMasks)
{
    int i;
    PSZ pszTmp;

    do
    {
        for (i = 1, pszTmp = NULL; i < pMasks->cMasks; i++)
        {
            int iDiff = strcmp(pMasks->papszMasks[i], pMasks->papszMasks[i-1]);
            if (iDiff == 0)
            {   /* remove entry */
                memmove(&pMasks->papszMasks[i], &pMasks->papszMasks[i+1],
                        (pMasks->cMasks - i - 1) * sizeof(pMasks->papszMasks[0]));
                i--;
            }
            else if (iDiff < 0)
            {   /* Swap entries. */
                PSZ pszTmp = pMasks->papszMasks[i];
                pMasks->papszMasks[i] = pMasks->papszMasks[i-1];
                pMasks->papszMasks[i-1] = pszTmp;
            }
        }
    } while (pszTmp != NULL);
}


/**
 * Searches a mask array if there is any match for the given name.
 * @returns TRUE if found.
 *          FALSE if not found.
 * @param   pszName     Pointer to name.
 * @param   pMasks      Pointer to mask array.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL    apiFindNameInMaskArray(PSZ pszName, PMASKARRAY pMasks)
{
    return FALSE;
}


/**
 * Get the current process executable name.
 * @returns OS/2 return code.
 * @param   pszName     Pointer to output name buffer.
 * @sketch  Get current ptda.
 *          Get module handle (hmte) from current ptda.
 *          Get pmte and smte from the hmte.
 *          Check if there is any path (full filename).
 *          Parse out filename+ext from full filename and copy it to pszName.
 *          return.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET  apiGetProccessName(PSZ pszName)
{
    PPTDA pPTDA = ptdaGetCur();
    if (pPTDA)
    {
        HMTE hmte = ptdaGet_ptda_module(pPTDA);
        if (hmte)
        {
            PMTE    pmte = ldrASMpMTEFromHandle(hmte);
            PSMTE   psmte;
            if (    pmte
                && (psmte = pmte->mte_swapmte)
                &&  psmte->smte_path
                &&  *psmte->smte_path)
            {
                /*
                 * Get executable name from swap mte.
                 * We parse out the filename+ext and copies it to the output buffer.
                 */
                PCHAR   psz;
                PCHAR   pszExt;
                ldrGetFileName2(psmte->smte_path, (PCHAR*)SSToDS(&psz), (PCHAR*)SSToDS(&pszExt));
                if (!psz) psz = psmte->smte_path;
                strcpy(pszName, psz);
                return NO_ERROR;
            }
            else
                kprintf(("apiGetProcessName: failed to get pmte(0x%08x) from hmte(0x%04x) or no path.\n", pmte, hmte));
        }
        else
            kprintf(("apiGetProcessName: This PTDA has no module handle. (pptda=0x%08x, hptda=0x%04)\n", pPTDA, ptdaGet_ptda_handle(pPTDA)));
    }
    else
        kprintf(("apiGetProcessName: No current PTDA!\n"));

    return ERROR_INVALID_PARAMETER;
}

/**
 * Gets the module name from a given CS:EIP pair.
 * @returns OS/2 return code.
 * @param   pszName     Output buffer.
 * @param   usCS        CS (code segment).
 * @param   ulEIP       EIP (Extended Instruction Pointer).
 * @sketch  Get hmte from cs:eip.
 *          Get pmte and smte from the hmte.
 *          Check if there is any path (full filename).
 *          Parse out filename+ext from full filename and copy it to pszName.
 *          return.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET  apiGetModuleName(PSZ pszName, USHORT usCS, ULONG ulEIP)
{
    HMTE hmte = VMGetOwner(usCS, ulEIP);
    if (hmte)
    {
        PMTE    pmte = ldrASMpMTEFromHandle(hmte);
        PSMTE   psmte;
        if (    pmte
            && (psmte = pmte->mte_swapmte)
            &&  psmte->smte_path
            &&  *psmte->smte_path)
        {
            /*
             * Get executable name from swap mte.
             * We parse out the filename+ext and copies it to the output buffer.
             */
            PCHAR   psz;
            PCHAR   pszExt;
            ldrGetFileName2(psmte->smte_path, (PCHAR*)SSToDS(&psz), (PCHAR*)SSToDS(&pszExt));
            if (!psz) psz = psmte->smte_path;
            strcpy(pszName, psz);
            return NO_ERROR;
        }
        else
            kprintf(("apiGetModuleName: failed to get pmte(0x%08x) from hmte(0x%04x) or no path.\n", pmte, hmte));
    }
    else
        kprintf(("apiGetModuleName: failed to get hmte from cs=%04x eip=%08x\n", usCS, ulEIP));

    /*
     * We failed.
     */
    return ERROR_INVALID_PARAMETER;
}



/**
 * Checks if an api enhancement is enabled for this process or/and module.
 * Exclusion lists rulez over inclusion.
 * Excluded processes rulez over included modules.
 * @returns TRUE (!0) if it's enabled.
 *          FALSE (0) if it's disabled.
 * @param   iApi    Api data id/index.
 * @param   usCS    CS of the API caller.
 * @param   ulEIP   EIP of the API caller.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL _Optlink   APIQueryEnabled(int iApi, USHORT usCS, LONG ulEIP)
{
    PAPIDATAENTRY   pEntry;

    /*
     * Check if these enhancements are switched off globally.
     */
    if (isApiEnhDisabled())
        return FALSE;

    /*
     * Aquire read lock.
     */
    RWLockAcquireRead(&ApiInfoRWLock);

    /*
     * Get data entry pointer.
     * Check if entry is enabled.
     */
    BOOL    fRet = FALSE;
    pEntry = &aApiData[iApi];
    if (pEntry->fEnabled)
    {
        CHAR    szName[CCHMAXPATH];
        PSZ     pszName = (PSZ)SSToDS(&szName[0]);

        if (pEntry->ProcessExc.cMasks > 0 || pEntry->ProcessInc.cMasks > 0)
        {
            if (!apiGetProccessName(pszName))
            {                           /* TODO - fix this priority - it's probably wrong */
                if (pEntry->ProcessExc.cMasks)
                    fRet = !apiFindNameInMaskArray(pszName, &pEntry->ProcessExc);
                else if (pEntry->ProcessInc.cMasks)
                    fRet = apiFindNameInMaskArray(pszName, &pEntry->ProcessInc);
            }
        }

        if (    !pEntry->ProcessExc.cMasks
            &&  !fRet
            &&  (pEntry->ModuleExc.cMasks > 0 || pEntry->ModuleInc.cMasks > 0))
        {
            if (!apiGetModuleName(pszName, usCS, ulEIP))
            {                           /* TODO - fix this priority - it's probably wrong */
                if (pEntry->ModuleExc.cMasks)
                    fRet = !apiFindNameInMaskArray(pszName, &pEntry->ModuleExc);
                else if (pEntry->ProcessInc.cMasks)
                    fRet = apiFindNameInMaskArray(pszName, &pEntry->ModuleInc);
            }
        }
    }


    /*
     * Release read lock.
     */
    RWLockReleaseRead(&ApiInfoRWLock);

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

