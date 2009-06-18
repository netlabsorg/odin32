#ifndef __OSLIBSPL_H__
#define __OSLIBSPL_H__


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct
{
    ULONG   flType;
    LPSTR   pszComputerName;
    LPSTR   pszPrintDestinationName;
    LPSTR   pszDescription;
    LPSTR   pszLocalName;
} OSLIB_PRINTERINFO, *POSLIB_PRINTERINFO;

typedef struct
{
    LONG    cb;
    LONG    lVersion;
    CHAR    szDeviceName[32];
    CHAR    abGeneralData[1];
} OSLIB_DRIVDATA;

typedef struct
{
    LPSTR     pszName;
    USHORT  uPriority;
    USHORT  uStartTime;
    USHORT  uUntilTime;
    USHORT  fsType;
    LPSTR     pszSepFile;
    LPSTR     pszPrProc;
    LPSTR     pszParms;
    LPSTR     pszComment;
    USHORT  fsStatus;
    USHORT  cJobs;
    LPSTR     pszPrinters;
    LPSTR     pszDriverName;
    OSLIB_DRIVDATA *pDriverData;
} OSLIB_PRQINFO3, *POSLIB_PRQINFO3;

typedef struct {
    LPSTR        pszPrinterName;  /*  Print device name. */
    LPSTR        pszUserName;     /*  User who submitted job. */
    LPSTR        pszLogAddr;      /*  Logical address (for example LPT1). */
    USHORT     uJobId;          /*  Identity of current job. */
    USHORT     fsStatus;        /*  Print destination status. */
    LPSTR        pszStatus;       /*  Print device comment while printing. */
    LPSTR        pszComment;      /*  Print device description. */
    LPSTR        pszDrivers;      /*  Drivers supported by print device. */
    USHORT     time;            /*  Time job has been printing (minutes) */
    USHORT     usTimeOut;       /*  Device timeout (seconds) */
} OSLIB_PRDINFO3, *POSLIB_PRDINFO3;

typedef struct
{
    LPSTR       pszLogAddress;
    LPSTR       pszDriverName;
    void *        pdriv;
    LPSTR       pszDataType;
    LPSTR       pszComment;
    LPSTR       pszQueueProcName;
    LPSTR       pszQueueProcParams;
    LPSTR       pszSpoolerParams;
    LPSTR       pszNetworkParams;
} OSLIB_DEVOPENSTRUC, *POSLIB_DEVOPENSTRUC;

/*** Flags for flType in  PRINTERINFO and SplEnumPrinter *************/
#define OSLIB_SPL_PR_QUEUE            0x00000001 /* include queues */
#define OSLIB_SPL_PR_DIRECT_DEVICE    0x00000002 /* unattached devices */
#define OSLIB_SPL_PR_QUEUED_DEVICE    0x00000004 /* queued devices */
#define OSLIB_SPL_PR_LOCAL_ONLY       0x00000100 /* exclude remote queues */

#define OSLIB_SPL_PR_ALL       (OSLIB_SPL_PR_QUEUE|OSLIB_SPL_PR_DIRECT_DEVICE|OSLIB_SPL_PR_QUEUED_DEVICE)

#define OSLIB_PRQ3_TYPE_RAW         0x0001 /* spools printer-specific data */
#define OSLIB_PRQ3_TYPE_BYPASS      0x0002 /* allow print while spooling   */
#define OSLIB_PRQ3_TYPE_APPDEFAULT  0x0004 /* set for Application default Q*/

void * OSLibSplEnumPrinter(LPSTR pszComputer, DWORD flType, PDWORD pcReturned, PDWORD pcTotal);
void * OSLibSplEnumDevice(LPSTR pszComputer, DWORD ulLevel, DWORD *pcReturned, DWORD *pcTotal);
void * OSLibSplEnumQueue(LPSTR pszComputer, DWORD ulLevel, DWORD *pcReturned, DWORD *pcTotal);

BOOL ExportPrintersToRegistry(void);
ULONG OSLibShowPrinterDialog(HWND, LPSTR printerName);

BOOL OSLibSplFindDefaultPrinter(LPSTR lpszPortName, LPSTR lpszDriverName, LPSTR lpszQueueName);
void * OSLibSplQueryQueue(LPSTR pszComputerName, LPSTR pszQueueName, ULONG ulLevel);

HANDLE  OSLibSplQmOpen(LPSTR pszToken, LONG lCount, LPSTR* pqmdopData);
BOOL    OSLibSplQmClose(HANDLE hspl);
BOOL    OSLibSplQmStartDoc(HANDLE hspl, LPSTR pszDocName);
BOOL    OSLibSplQmEndDoc(HANDLE hspl);
BOOL    OSLibSplQmWrite(HANDLE hspl, LONG cbData, PVOID pvData);
BOOL    OSLibSplQmNewPage(HANDLE hspl, ULONG ulPageNo);

ULONG OSLibSplWinGetLastError(void);

#endif
