/* $Id: oslibspl.h,v 1.1 2000-11-15 10:54:24 sandervl Exp $ */
#ifndef __OSLIBSPL_H__
#define __OSLIBSPL_H__

typedef struct {
         ULONG   flType;
         LPSTR   pszComputerName;
         LPSTR   pszPrintDestinationName;
         LPSTR   pszDescription;
         LPSTR   pszLocalName;
} OSLIB_PRINTERINFO, *POSLIB_PRINTERINFO;

/*** Flags for flType in  PRINTERINFO and SplEnumPrinter *************/
#define OSLIB_SPL_PR_QUEUE            0x00000001 /* include queues */
#define OSLIB_SPL_PR_DIRECT_DEVICE    0x00000002 /* unattached devices */
#define OSLIB_SPL_PR_QUEUED_DEVICE    0x00000004 /* queued devices */
#define OSLIB_SPL_PR_LOCAL_ONLY       0x00000100 /* exclude remote queues */

#define OSLIB_SPL_PR_ALL       (OSLIB_SPL_PR_QUEUE|OSLIB_SPL_PR_DIRECT_DEVICE|OSLIB_SPL_PR_QUEUED_DEVICE)

BOOL OSLibSplEnumPrinters(DWORD flType, OSLIB_PRINTERINFO *pBuf, DWORD cbSize,
                          DWORD *pcReturned, DWORD *pcTotal, DWORD *pcbNeeded);


#endif
