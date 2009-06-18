
/*
 * Winspool support code
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_SPL
#define  INCL_SPLERRORS
#define  INCL_SPLDOSPRINT
#define  INCL_DOSERRORS
#define  INCL_GPI
#define  INCL_WIN
#define  INCL_DEV

#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <pmspl.h>
#include <string.h>
#include <win32type.h>
#include <winuser32.h>
#include <misc.h>
#include <wprocess.h>
#include "oslibspl.h"

/**
 * Enumerate printers.
 * @returns Pointer to requested data.
 * @returns NULL on failure.
 * @param   pszComputer Computer name.
 * @param   flType      Flags.
 * @param   pcReturned  Number of structures returned.
 * @param   pcTotal     Total number of structures available.
 */
void * OSLibSplEnumPrinter(LPSTR pszComputer, DWORD flType, PDWORD pcReturned, PDWORD pcTotal)
{
    ULONG   cbNeeded = 0;
    SPLERR  rc = SplEnumPrinter(NULL, 0, flType, NULL, 0, pcReturned, pcTotal, &cbNeeded, NULL);
    if (!rc || rc == ERROR_MORE_DATA || rc == NERR_BufTooSmall)
    {
        void *pv = malloc(cbNeeded+1);
        rc = SplEnumPrinter(pszComputer, 0, flType, pv, cbNeeded, pcReturned, pcTotal, &cbNeeded, NULL);
        if (!rc || rc == ERROR_MORE_DATA)
            return pv;
        free(pv);
    }
    return NULL;
}

//******************************************************************************
//******************************************************************************
ULONG OSLibShowPrinterDialog(HWND hWnd, LPSTR printerName)
{
    HAB hab;
    ULONG cReturned, cTotal, cbNeeded, flType,cbBuf, ulrc;
    int   devNum = -1;
    OSLIB_PRQINFO3 *queueInfo;
    char *pszTmp, *pszDeviceName;
    SPLERR     rc;

    hab = WinQueryAnchorBlock(Win32ToOS2Handle(hWnd));

    rc = SplEnumQueue(NULL, 3, NULL, 0, &cReturned, &cTotal, &cbNeeded, NULL);
    if (rc && rc != ERROR_MORE_DATA && rc != NERR_BufTooSmall)
    {
        dprintf(("OSLibSplEnumQueue failed"));
        return -1;
    }
    if (cTotal == 0)
    {//no printers installed
        return -1;
    }
    queueInfo = (OSLIB_PRQINFO3*)malloc(cbNeeded);

    rc = SplEnumQueue(NULL, 3, queueInfo, cbNeeded, &cReturned, &cTotal, &cbNeeded, NULL);
    if (rc)
    {
        free(queueInfo);
        dprintf(("OSLibSplEnumQueue failed"));
        return -1;
    }

    for (int i = 0; i < cReturned; i++)
        if (!strcmp(queueInfo[i].pszName, printerName))
        {
            devNum = i;
            break;
        }

    if (devNum == -1)
    {
        free(queueInfo);
        return -1;
    }
    /* Use the first device name in the PRQINFO3 structure                 */
    pszTmp = strchr(queueInfo[devNum].pszPrinters, ',');
    if (pszTmp)
        *pszTmp = '\0';

    /* Use just the driver name from the driver.device string              */
    pszDeviceName = strchr(queueInfo[devNum].pszDriverName, '.');
    if (pszDeviceName)
    {
        *pszDeviceName = '\0';
        pszDeviceName++;
    }

    /* Check size of buffer required for job properties                    */
    cbBuf = DevPostDeviceModes( hab,
                                (PDRIVDATA)NULL,
                                queueInfo[devNum].pszDriverName,
                                pszDeviceName,
                                queueInfo[devNum].pszPrinters,
                                DPDM_POSTJOBPROP
                              );

    /* Return error to caller                                              */
    if (cbBuf<=0)
    {
        free(queueInfo);
        return(-1);
    }

    /* Display job properties dialog & get updated job properties from driver */
    ulrc = DevPostDeviceModes( hab,
                               (PDRIVDATA)queueInfo[devNum].pDriverData,
                               queueInfo[devNum].pszDriverName,
                               pszDeviceName,
                               queueInfo[devNum].pszPrinters,
                               DPDM_POSTJOBPROP);

    if (pszDeviceName)
        *(--pszDeviceName) = '.';

    ulrc = SplSetQueue(NULL, printerName, 3, (PVOID)&queueInfo[devNum], cbBuf, 0);
    if (ulrc)
        dprintf(("Spooler Set Queue error %d",ulrc));

    free(queueInfo);
    return(1);
}

/**
 * Enumerate queues.
 * @returns Pointer to requested data.
 * @returns NULL on failure.
 * @param   pszComputer Computer name.
 * @param   ulLevel     Information level.
 * @param   pcReturned  Number of structures returned.
 * @param   pcTotal     Total number of structures available.
 */
void * OSLibSplEnumQueue(LPSTR pszComputer, DWORD ulLevel, DWORD *pcReturned, DWORD *pcTotal)
{
    ULONG   cbNeeded = 0;
    SPLERR  rc = SplEnumQueue(pszComputer, ulLevel, NULL, 0, pcReturned, pcTotal, &cbNeeded, NULL);
    if (!rc || rc == ERROR_MORE_DATA || rc == NERR_BufTooSmall)
    {
        void *pv = malloc(cbNeeded+1);
        SPLERR  rc = SplEnumQueue(pszComputer, ulLevel, pv, cbNeeded, pcReturned, pcTotal, &cbNeeded, NULL);
        if (!rc || rc == ERROR_MORE_DATA)
            return pv;
        free(pv);
    }
    return NULL;
}

/**
 * Enumerate devices.
 * @returns Pointer to requested data.
 * @returns NULL on failure.
 * @param   pszComputer Computer name.
 * @param   ulLevel     Information level.
 * @param   pcReturned  Number of structures returned.
 * @param   pcTotal     Total number of structures available.
 */
void * OSLibSplEnumDevice(LPSTR pszComputer, DWORD ulLevel, DWORD *pcReturned, DWORD *pcTotal)
{
    ULONG   cbNeeded = 0;
    SPLERR  rc = SplEnumDevice(pszComputer, ulLevel, NULL, 0, pcReturned, pcTotal, &cbNeeded, NULL);
    if (!rc || rc == ERROR_MORE_DATA || rc == NERR_BufTooSmall)
    {
        void *pv = malloc(cbNeeded+1);
        SPLERR  rc = SplEnumDevice(pszComputer, ulLevel, pv, cbNeeded, pcReturned, pcTotal, &cbNeeded, NULL);
        if (!rc || rc == ERROR_MORE_DATA)
            return pv;
        free(pv);
    }
    return NULL;
}

//******************************************************************************
//******************************************************************************
/**
 * @remark bird: This a nasty thing. Any chance we could use the app default flag for the queue?
 */
BOOL OSLibSplFindDefaultPrinter(LPSTR lpszPortName, LPSTR lpszDriverName, LPSTR lpszQueueName)
{
        /*
         * OS/2 stores the default printer as a combination of printerdevice and queue.
         *
         *   Printer Device is related to one port. It may have multiple printer
         *       drivers because the port may serve multiple queues with different drivers.
         *   The Ports are related to multiple queues.
         *
         * So we take the default default printer+queue combination and finds the
         */
        char szDefPrnDev[20];
        char szDefPrnQue[20];
        if (PrfQueryProfileString(HINI_PROFILE, "PM_SPOOLER", "PRINTER", "",
                                  szDefPrnDev, sizeof(szDefPrnDev)) > 1
            &&  PrfQueryProfileString(HINI_PROFILE, "PM_SPOOLER", "QUEUE", "",
                                      szDefPrnQue, sizeof(szDefPrnQue)) > 1
            &&  szDefPrnDev[0]
            &&  szDefPrnQue[0]
           )
        {
            char *psz;
            /* remove everything beyond the first ';' */
            if ((psz = strchr(szDefPrnDev, ';')) != NULL)
                *psz = '\0';
            if ((psz = strchr(szDefPrnQue, ';')) != NULL)
                *psz = '\0';

            /*
             * Now we must lookup the port name from the device settings.
             *  This is a string of this form:
             *      <port>;<driver1>[,<driver2>;<queue1>[,<queue2>];?;?;
             */
            ULONG   cb = 0;
            if (PrfQueryProfileSize(HINI_SYSTEMPROFILE, "PM_SPOOLER_PRINTER", szDefPrnDev, &cb)
                &&  cb > 0)
            {
                char *pszBufD = (char*)malloc(cb + 1);
                if (pszBufD
                    &&  PrfQueryProfileString(HINI_SYSTEMPROFILE, "PM_SPOOLER_PRINTER", szDefPrnDev,
                                              NULL, pszBufD, cb + 1)
                    > 1
                   )
                {
                    /*
                     * Now get the Default printer driver for the queue.
                     *  This is stored as a ';' separated list of drivers, the first one is the default.
                     */
                    if (PrfQueryProfileSize(HINI_SYSTEMPROFILE, "PM_SPOOLER_QUEUE_DD", szDefPrnQue, &cb)
                        &&  cb > 0)
                    {
                        char *pszBufQ = (char*)malloc(cb + 1);
                        if (pszBufQ
                            &&  PrfQueryProfileString(HINI_SYSTEMPROFILE, "PM_SPOOLER_QUEUE_DD", szDefPrnQue,
                                                      NULL, pszBufQ, cb + 1)
                            > 1
                           )
                        {
                            /*
                             * We got everything now. just find the parts we need.
                             *  First printer driver from QUEUE_DD
                             *  Port name of the device.
                             */
                            if ((psz = strchr(pszBufQ, ';')) != NULL)
                                *psz = '\0';
                            if ((psz = strchr(pszBufQ, ',')) != NULL) //paranoia! in case comman separated list of some kind.
                                *psz = '\0';
                            if ((psz = strchr(pszBufD, ';')) != NULL)
                                *psz = '\0';
                            if ((psz = strchr(pszBufD, ',')) != NULL) //paranoia in case comman separated list of some kind.
                                *psz = '\0';

                            /*
                             * Now make default printer string for the win.ini.
                             */
                            strcpy(lpszPortName, pszBufD);
                            strcat(lpszQueueName, szDefPrnQue);
                            strcat(lpszDriverName, pszBufQ);
                            dprintf(("OSLibSplFindDefaultPrinter: Successfully found default printer.'%s-%s-%s'", szDefPrnQue, lpszDriverName, pszBufQ));
                            free(pszBufQ);
                            return TRUE;
                        }
                    }
                    else
                    {
                        /* OS/2 the device may exist though the default queue is destroyed.
                         * it may still exist even if there are no queues on the system at all!
                         */
                        dprintf(("OSLibSplFindDefaultPrinter: no queue driver entry for '%s'.", szDefPrnQue));
                    }

                    free(pszBufD);
                }
            }
            else
            {
                /* OS/2 doesn't remove the default settings if the default queue/printer is deleted. */
                dprintf(("OSLibSplFindDefaultPrinter: can't find device settings for '%s'.", szDefPrnDev));
            }
        }
        else
        {
            dprintf(("OSLibSplFindDefaultPrinter: no default printer? szDefPrnDev='%s' szDefPrnQue='%s'.", szDefPrnDev, szDefPrnQue));
        }
        return FALSE;
}


/**
 * Does SplQueryQueue allocating a sufficent buffer of the default heap.
 *
 * @returns pointer to queue info.
 * @returns NULL on failure.
 * @param   pszComputerName See SplQueryQueue.
 * @param   pszQueueName    See SplQueryQueue.
 * @param   ulLevel         See SplQueryQueue.
 */
void * OSLibSplQueryQueue(LPSTR pszComputerName, LPSTR pszQueueName, ULONG ulLevel)
{
    ULONG   cbNeeded = 0;
    SPLERR rc = SplQueryQueue(pszComputerName, pszQueueName, ulLevel, NULL, 0, &cbNeeded);
    if (rc && rc != ERROR_MORE_DATA && rc != NERR_BufTooSmall)
        return NULL;

    void *pv = malloc(cbNeeded);
    rc = SplQueryQueue(pszComputerName, pszQueueName, ulLevel, pv, cbNeeded, &cbNeeded);
    if (rc)
    {
        free(pv);
        return NULL;
    }
    return pv;
}


/**
 * see pmref.
 * @returns see pmref.
 * @param   pszToken    see pmref.
 * @param   lCount      see pmref.
 * @param   pqmdopData  see pmref.
 */
HSPL OSLibSplQmOpen(LPSTR pszToken, LONG lCount, LPSTR* pqmdopData)
{
    return SplQmOpen(pszToken, lCount, pqmdopData);
}


/**
 * see pmref.
 * @returns see pmref.
 * @param   hspl        see pmref.
 */
BOOL OSLibSplQmClose(HANDLE hspl)
{
    return SplQmClose(hspl);
}


/**
 * see pmref.
 * @returns see pmref.
 * @param   hspl        see pmref.
 * @param   pszDocName  see pmref.
 */
BOOL OSLibSplQmStartDoc(HANDLE hspl, LPSTR pszDocName)
{
    return SplQmStartDoc(hspl, pszDocName);
}


/**
 * see pmref.
 * @returns see pmref.
 * @param   hspl        see pmref.
 */
BOOL OSLibSplQmEndDoc(HANDLE hspl)
{
    return SplQmEndDoc(hspl);
}


/**
 * see pmref
 * @returns see pmref
 * @param   hspl    see pmref
 * @param   cbData  see pmref
 * @param   pvData  see pmref
 */
BOOL OSLibSplQmWrite(HANDLE hspl, LONG cbData, PVOID pvData)
{
    return SplQmWrite(hspl, cbData, pvData);
}


/**
 * see pmref.
 * @returns see pmref.
 * @param   hspl        see pmref.
 * @param   ulPageNo    see pmref.
 */
BOOL OSLibSplQmNewPage(HANDLE hspl, ULONG ulPageNo)
{
    return SplQmNewPage(hspl, ulPageNo);
}


/**
 * Some spooler apis sets the last error.
 *
 * @returns last error.
 */
ULONG OSLibSplWinGetLastError(void)
{
    TEB *teb = GetThreadTEB();
    return WinGetLastError(teb ? teb->o.odin.hab : 0) & 0xFFFF;
}

