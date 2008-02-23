/* $Id: eventlog.cpp,v 1.3 2005-01-15 22:16:35 sao2l02 Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * NOTE: Uses registry key for service as handle
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include "misc.h"
#include "advapi32.h"
#include "unicode.h"
#include "winreg.h"
#include <heapstring.h>

ODINDEBUGCHANNEL(ADVAPI32-EVENTLOG)


BOOL WIN32API ReportEventA(     /*PLF Sat  98-03-07 00:36:43*/
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCSTR   *lpStrings,
     LPVOID     lpRawData
    )
{
    dprintf(("ReportEventA(): NIY\n"));
    return TRUE;
}


BOOL WIN32API ReportEventW(     /*PLF Sat  98-03-07 00:36:43*/
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCWSTR   *lpStrings,
     LPVOID     lpRawData
    )
{
    dprintf(("ReportEventW %x %x %x %x %x %x %x %x %x: not implemented\n",hEventLog, wType, 
              wCategory, dwEventID, lpUserSid, wNumStrings, dwDataSize, lpStrings, lpRawData));
    return TRUE;
}


/*PLF Sat  98-03-07 02:59:20*/
HANDLE WIN32API RegisterEventSourceA(LPCSTR lpUNCServerName, LPCSTR lpSourceName)
{
    dprintf(("ADVAPI32: RegisterEventSourceA() %s %s", lpUNCServerName, lpSourceName));
    
    return 0xCAFECAFE;
}

/*PLF Sat  98-03-07 02:59:20*/
HANDLE WIN32API RegisterEventSourceW(LPCWSTR lpUNCServerName, LPCWSTR lpSourceName)
{
 LPSTR lpUNCServerNameA = NULL, lpSourceNameA = NULL;
 HANDLE hEvent;

    dprintf(("ADVAPI32: RegisterEventSourceW()"));
    if(lpUNCServerName) 
    	lpUNCServerNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpUNCServerName);
    if(lpSourceName) 
    	lpSourceNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpSourceName);

    hEvent = RegisterEventSourceA(lpUNCServerNameA, lpSourceNameA);

    HeapFree(GetProcessHeap(), 0, lpUNCServerNameA);
    HeapFree(GetProcessHeap(), 0, lpSourceNameA);

    return hEvent;
}


/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API DeregisterEventSource(HANDLE hEventLog)
{
    dprintf(("DeregisterEventSource() NIY\n"));
    return FALSE;
}

/*****************************************************************************
 * Name      : BackupEventLogA
 * Purpose   : The BackupEventLog function saves the specified event log to a
 *             backup file. The function does not clear the event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API BackupEventLogA(HANDLE  hEventLog,
                                 LPCSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: BackupEventLogA(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : BackupEventLogW
 * Purpose   : The BackupEventLog function saves the specified event log to a
 *             backup file. The function does not clear the event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPCWSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API BackupEventLogW(HANDLE  hEventLog,
                                 LPCWSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: BackupEventLogW() not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ClearEventLogA
 * Purpose   : The ClearEventLog function clears the specified event log, and
 *             optionally saves the current copy of the logfile to a backup file.
 * Parameters: HANDLE  hEventLog         handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ClearEventLogA(HANDLE  hEventLog,
                                LPCSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: ClearEventLogA(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ClearEventLogW
 * Purpose   : The ClearEventLog function clears the specified event log, and
 *             optionally saves the current copy of the logfile to a backup file.
 * Parameters: HANDLE  hEventLog         handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ClearEventLogW(HANDLE  hEventLog,
                                LPCWSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: ClearEventLogW(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CloseEventLog
 * Purpose   : The CloseEventLog function closes the specified event log.
 * Parameters: HANDLE  hEventLog  handle to event log
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CloseEventLog(HANDLE hEventLog)
{
  dprintf(("ADVAPI32: CloseEventLog(%08xh) not implemented.\n",
           hEventLog));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : GetNumberOfEventLogRecords
 * Purpose   : The GetNumberOfEventLogRecords function retrieves the number of
 *             records in the specified event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPDWORD  NumberOfRecords  buffer for number of records
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetNumberOfEventLogRecords(HANDLE hEventLog,
                                            LPDWORD NumberOfRecords)
{
  dprintf(("ADVAPI32: GetNumberOfEventLogRecords(%08xh,%08xh) not implemented.\n",
           hEventLog,
           NumberOfRecords));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetOldestEventLogRecord
 * Purpose   : The GetOldestEventLogRecord function retrieves the absolute
 *             record number of the oldest record in the specified event log.
 * Parameters: HANDLE  hEventLog     handle to event log
 *             LPDWORD  OldestRecord  buffer for number of oldest record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetOldestEventLogRecord(HANDLE hEventLog,
                                         LPDWORD OldestRecord)
{
  dprintf(("ADVAPI32: GetOldestEventLogRecord(%08xh,%08xh) not implemented.\n",
           hEventLog,
           OldestRecord));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : NotifyChangeEventLog
 * Purpose   : The NotifyChangeEventLog function lets an application receive
 *             notification when an event is written to the event log file
 *             specified by hEventLog. When the event is written to the event
 *             log file, the function causes the event object specified by
 *             hEvent to become signaled.
 * Parameters: HANDLE  hEventLog  special default locale value to be converted
 *             HANDLE  hEvent     special default locale value to be converted
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API NotifyChangeEventLog(HANDLE  hEventLog,
                                      HANDLE  hEvent)
{
  dprintf(("ADVAPI32: NotifyChangeEventLog(%08xh,%08xh) not implemented.\n",
           hEventLog,
           hEvent));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : OpenBackupEventLogA
 * Purpose   : The OpenBackupEventLog function opens a handle of a backup event
 *             log. This handle can be used with the BackupEventLog function.
 * Parameters: LPCSTR  lpszUNCServerName  backup file server name
 *             LPCSTR  lpszFileName       backup filename
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenBackupEventLogA(LPCSTR  lpszUNCServerName,
                                       LPCSTR  lpszFileName)
{
  dprintf(("ADVAPI32: OpenBackupEventLogA(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszFileName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenBackupEventLogW
 * Purpose   : The OpenBackupEventLog function opens a handle of a backup event
 *             log. This handle can be used with the BackupEventLog function.
 * Parameters: LPCWSTR  lpszUNCServerName  backup file server name
 *             LPCWSTR  lpszFileName       backup filename
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenBackupEventLogW(LPCWSTR  lpszUNCServerName,
                                       LPCWSTR  lpszFileName)
{
  dprintf(("ADVAPI32: OpenBackupEventLogW(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszFileName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenEventLogA
 * Purpose   : The OpenEventLog function opens a handle of an event log.
 * Parameters: LPCSTR  lpszUNCServerName
 *             LPCSTR  lpszSourceName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenEventLogA(LPCSTR  lpszUNCServerName,
                                 LPCSTR  lpszSourceName)
{
  dprintf(("ADVAPI32: OpenEventLogA(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszSourceName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenEventLogW
 * Purpose   : The OpenEventLog function opens a handle of an event log.
 * Parameters: LPCWSTR  lpszUNCServerName
 *             LPCWSTR  lpszSourceName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenEventLogW(LPCWSTR  lpszUNCServerName,
                                 LPCWSTR  lpszSourceName)
{
  dprintf(("ADVAPI32: OpenEventLogW(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszSourceName));

  return (NULL); /* signal failure */
}

/*****************************************************************************
 * Name      : ReadEventLogW
 * Purpose   : The ReadEventLog function reads a whole number of entries from
 *             the specified event log. The function can be used to read log
 *             entries in forward or reverse chronological order.
 * Parameters: HANDLE hEventLog                 handle of event log
 *             DWORD  dwReadFlags               specifies how to read log
 *             DWORD  dwRecordOffset            number of first record
 *             LPVOID lpBuffer                  address of buffer for read data
 *             DWORD  nNumberOfBytesToRead      number of bytes to read
 *             DWORD  *pnBytesRea               number of bytes read
 *             DWORD  *pnMinNumberOfBytesNeeded number of bytes required for next record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ReadEventLogW(HANDLE hEventLog,
                               DWORD  dwReadFlags,
                               DWORD  dwRecordOffset,
                               LPVOID lpBuffer,
                               DWORD  nNumberOfBytesToRead,
                               DWORD  *pnBytesRead,
                               DWORD  *pnMinNumberOfBytesNeeded)
{
  dprintf(("ADVAPI32: ReadEventLogW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hEventLog,
           dwReadFlags,
           dwRecordOffset,
           lpBuffer,
           nNumberOfBytesToRead,
           pnBytesRead,
           pnMinNumberOfBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ReadEventLogA
 * Purpose   : The ReadEventLog function reads a whole number of entries from
 *             the specified event log. The function can be used to read log
 *             entries in forward or reverse chronological order.
 * Parameters: HANDLE hEventLog                 handle of event log
 *             DWORD  dwReadFlags               specifies how to read log
 *             DWORD  dwRecordOffset            number of first record
 *             LPVOID lpBuffer                  address of buffer for read data
 *             DWORD  nNumberOfBytesToRead      number of bytes to read
 *             DWORD  *pnBytesRea               number of bytes read
 *             DWORD  *pnMinNumberOfBytesNeeded number of bytes required for next record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ReadEventLogA(HANDLE hEventLog,
                               DWORD  dwReadFlags,
                               DWORD  dwRecordOffset,
                               LPVOID lpBuffer,
                               DWORD  nNumberOfBytesToRead,
                               DWORD  *pnBytesRead,
                               DWORD  *pnMinNumberOfBytesNeeded)
{
  dprintf(("ADVAPI32: ReadEventLogA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hEventLog,
           dwReadFlags,
           dwRecordOffset,
           lpBuffer,
           nNumberOfBytesToRead,
           pnBytesRead,
           pnMinNumberOfBytesNeeded));

  return (FALSE); /* signal failure */
}

