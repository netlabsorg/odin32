/* $Id: npipe.cpp,v 1.11 2003-04-02 12:58:30 sandervl Exp $ */
/*
 * Win32 Named pipes API
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 2000 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include <stdlib.h>
#include <unicode.h>
#include <heapstring.h>
#include <options.h>
#include <HandleManager.h>
#include "debugtools.h"
#include "oslibdos.h"


#define DBG_LOCALLOG	DBG_npipe
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-NPIPE)

//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekNamedPipe(HANDLE hPipe, LPVOID lpvBuffer,
                            DWORD cbBuffer, LPDWORD lpcbRead,
                            LPDWORD lpcbAvail, LPDWORD lpcbMessage)
{
  return (HMPeekNamedPipe(hPipe,lpvBuffer,cbBuffer,lpcbRead,lpcbAvail,lpcbMessage));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreatePipe(PHANDLE phRead, PHANDLE  phWrite,
                         LPSECURITY_ATTRIBUTES lpsa, DWORD cbPipe)
{
  return (HMCreatePipe(phRead,phWrite,lpsa,cbPipe));
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateNamedPipeA(LPCTSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                                 DWORD nMaxInstances, DWORD nOutBufferSize,
                                 DWORD nInBufferSize, DWORD nDefaultTimeOut,
                                 LPSECURITY_ATTRIBUTES lpSecurityAttributes)

{
  return (HMCreateNamedPipe(lpName,
                            dwOpenMode,
                            dwPipeMode,
                            nMaxInstances,
                            nOutBufferSize,
                            nInBufferSize,
                            nDefaultTimeOut,
                            lpSecurityAttributes));

}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateNamedPipeW(LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                                 DWORD  nMaxInstances, DWORD  nOutBufferSize,
                                 DWORD  nInBufferSize, DWORD  nDefaultTimeOut,
                                 LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  char *asciiname;
  HANDLE hPipe;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpName);

  hPipe=HMCreateNamedPipe(asciiname,
                          dwOpenMode,
                          dwPipeMode,
                          nMaxInstances,
                          nOutBufferSize,
                          nInBufferSize,
                          nDefaultTimeOut,
                          lpSecurityAttributes);

  FreeAsciiString(asciiname);

  return(hPipe);
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : BOOL WIN32API ConnectNamedPipe
 * Purpose   : The ConnectNamedPipe function enables a named pipe server process
 *             to wait for a client process to connect to an instance of a
 *             named pipe. A client process connects by calling either the
 *             CreateFile or CallNamedPipe function.
 * Parameters:  HANDLE hNamedPipe      handle to named pipe to connect
 *              LPOVERLAPPED lpOverlapped  pointer to overlapped structure
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : NOT FULLY TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Sun, 2000/01/02 12:48]
 *****************************************************************************/
BOOL WIN32API ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
{
  return (HMConnectNamedPipe(hNamedPipe,lpOverlapped));
}

/*****************************************************************************
 * Name      : BOOL WIN32AOI CallNamedPipeA
 * Purpose   : The CallNamedPipe function connects to a message-type pipe
 *             (and waits if an instance of the pipe is not available),
 *             writes to and reads from the pipe, and then closes the pipe.
 * Parameters:  LPCSTR lpNamedPipeName pointer to pipe name
 *              LPVOID lpInBuffer      pointer to write buffer
 *              DWORD nInBufferSize    size, in bytes, of write buffer
 *              LPVOID lpOutBuffer     pointer to read buffer
 *              DWORD nOutBufferSize   size, in bytes, of read buffer
 *              LPDWORD lpBytesRead    pointer to number of bytes read
 *              DWORD nTimeOut         time-out time, in milliseconds
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Calling CallNamedPipe is equivalent to calling the CreateFile
 *             (or WaitNamedPipe, if CreateFile cannot open the pipe immediately),
 *             TransactNamedPipe, and CloseHandle functions. CreateFile is called
 *             with an access flag of GENERIC_READ | GENERIC_WRITE, an inherit
 *             handle flag of FALSE, and a share mode of zero (indicating no
 *             sharing of this pipe instance).
 *             If the message written to the pipe by the server process is
 *             longer than nOutBufferSize, CallNamedPipe returns FALSE, and
 *             GetLastError returns ERROR_MORE_DATA. The remainder of the
 *             message is discarded, because CallNamedPipe closes the handle
 *             to the pipe before returning.
 *
 *             CallNamedPipe fails if the pipe is a byte-type pipe.
 * Status    : NOT FULLY TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 13:32]
 *****************************************************************************/

BOOL WIN32API CallNamedPipeA(LPCSTR  lpNamedPipeName,
                             LPVOID  lpInBuffer,
                             DWORD   nInBufferSize,
                             LPVOID  lpOutBuffer,
                             DWORD   nOutBufferSize,
                             LPDWORD lpBytesRead,
                             DWORD   nTimeOut)
{
  return(OSLibDosCallNamedPipe(lpNamedPipeName,
                           lpInBuffer,
                           nInBufferSize,
                           lpOutBuffer,
                           nOutBufferSize,
                           lpBytesRead,
                           nTimeOut ));
}

/*****************************************************************************
 * Name      : BOOL WIN32AOI CallNamedPipeW
 * Purpose   : The CallNamedPipe function connects to a message-type pipe
 *             (and waits if an instance of the pipe is not available),
 *             writes to and reads from the pipe, and then closes the pipe.
 * Parameters:  LPCWSTR lpNamedPipeName pointer to pipe name
 *              LPVOID lpInBuffer      pointer to write buffer
 *              DWORD nInBufferSize    size, in bytes, of write buffer
 *              LPVOID lpOutBuffer     pointer to read buffer
 *              DWORD nOutBufferSize   size, in bytes, of read buffer
 *              LPDWORD lpBytesRead    pointer to number of bytes read
 *              DWORD nTimeOut         time-out time, in milliseconds
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Calling CallNamedPipe is equivalent to calling the CreateFile
 *             (or WaitNamedPipe, if CreateFile cannot open the pipe immediately),
 *             TransactNamedPipe, and CloseHandle functions. CreateFile is called
 *             with an access flag of GENERIC_READ | GENERIC_WRITE, an inherit
 *             handle flag of FALSE, and a share mode of zero (indicating no
 *             sharing of this pipe instance).
 *             If the message written to the pipe by the server process is
 *             longer than nOutBufferSize, CallNamedPipe returns FALSE, and
 *             GetLastError returns ERROR_MORE_DATA. The remainder of the
 *             message is discarded, because CallNamedPipe closes the handle
 *             to the pipe before returning.
 *
 *             CallNamedPipe fails if the pipe is a byte-type pipe.
 * Status    : NOT FULLY TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 13:33]
 *****************************************************************************/
BOOL WIN32API CallNamedPipeW(LPCWSTR lpNamedPipeName,
                             LPVOID  lpInBuffer,
                             DWORD   nInBufferSize,
                             LPVOID  lpOutBuffer,
                             DWORD   nOutBufferSize,
                             LPDWORD lpBytesRead,
                             DWORD   nTimeOut)
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpNamedPipeName);

  rc=OSLibDosCallNamedPipe(asciiname,
                           lpInBuffer,
                           nInBufferSize,
                           lpOutBuffer,
                           nOutBufferSize,
                           lpBytesRead,
                           nTimeOut );

  FreeAsciiString(asciiname);

  return(rc);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DisconnectNamedPipe
 * Purpose   : The DisconnectNamedPipe function disconnects the server end
 *             of a named pipe instance from a client process.
 * Parameters: HANDLE hNamedPipe    handle to named pipe
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : NOT FULLY TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 13:34]
 *****************************************************************************/

BOOL WIN32API DisconnectNamedPipe(HANDLE hNamedPipe)
{
  return (HMDisconnectNamedPipe(hNamedPipe));
}

/*****************************************************************************
 * Name      : BOOL GetNamedPipeHandleStateA
 * Purpose   : The GetNamedPipeHandleStateA function retrieves information about
 *             a specified named pipe. The information returned can vary during
 *             the lifetime of an instance of the named pipe.
 * Parameters: HANDLE  hNamedPipe           handle of named pipe
 *             LPDWORD lpState              address of flags indicating pipe state
 *             LPDWORD lpCurInstances       address of number of current pipe instances
 *             LPDWORD lpMaxCollectionCount address of max. bytes before remote transmission
 *             LPDWORD lpCollectDataTimeout address of max. time before remote transmission
 *             LPTSTR  lpUserName           address of user name of client process
 *             DWORD   nMaxUserNameSize     size, in characters, of user name buffer
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeHandleStateA(HANDLE  hNamedPipe,
                                       LPDWORD lpState,
                                       LPDWORD lpCurInstances,
                                       LPDWORD lpMaxCollectionCount,
                                       LPDWORD lpCollectDataTimeout,
                                       LPTSTR  lpUserName,
                                       DWORD   nMaxUserNameSize)
{
  // Not implemented but waiting to implementation in hmnpipe.cpp
  return ( HMGetNamedPipeHandleState( hNamedPipe,
                                       lpState,
                                       lpCurInstances,
                                       lpMaxCollectionCount,
                                       lpCollectDataTimeout,
                                       lpUserName,
                                       nMaxUserNameSize));
}


/*****************************************************************************
 * Name      : BOOL GetNamedPipeHandleStateW
 * Purpose   : The GetNamedPipeHandleStateW function retrieves information about
 *             a specified named pipe. The information returned can vary during
 *             the lifetime of an instance of the named pipe.
 * Parameters: HANDLE  hNamedPipe           handle of named pipe
 *             LPDWORD lpState              address of flags indicating pipe state
 *             LPDWORD lpCurInstances       address of number of current pipe instances
 *             LPDWORD lpMaxCollectionCount address of max. bytes before remote transmission
 *             LPDWORD lpCollectDataTimeout address of max. time before remote transmission
 *             LPWSTR  lpUserName           address of user name of client process
 *             DWORD   nMaxUserNameSize     size, in characters, of user name buffer
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeHandleStateW(HANDLE  hNamedPipe,
                                       LPDWORD lpState,
                                       LPDWORD lpCurInstances,
                                       LPDWORD lpMaxCollectionCount,
                                       LPDWORD lpCollectDataTimeout,
                                       LPWSTR  lpUserName,
                                       DWORD   nMaxUserNameSize)
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpUserName);

  // Not implemented but waiting to implementation in hmnpipe.cpp
  rc= HMGetNamedPipeHandleState( hNamedPipe,
                                  lpState,
                                  lpCurInstances,
                                  lpMaxCollectionCount,
                                  lpCollectDataTimeout,
                                  asciiname,
                                  nMaxUserNameSize);


  FreeAsciiString(asciiname);

  return (rc);
}


/*****************************************************************************
 * Name      : BOOL GetNamedPipeInfo
 * Purpose   : The GetNamedPipeInfo function retrieves information about the specified named pipe.
 * Parameters: HANDLE  hNamedPipe      handle of named pipe
 *             LPDWORD lpFlags         address of flags indicating type of pipe
 *             LPDWORD lpOutBufferSize address of size, in bytes, of pipe's output buffer
 *             LPDWORD lpInBufferSize  address of size, in bytes, of pipe's input buffer
 *             LPDWORD lpMaxInstances  address of max. number of pipe instances
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeInfo(HANDLE  hNamedPipe,
                               LPDWORD lpFlags,
                               LPDWORD lpOutBufferSize,
                               LPDWORD lpInBufferSize,
                               LPDWORD lpMaxInstances)
{
  // Not implemented but waiting to implementation in hmnpipe.cpp
  return ( HMGetNamedPipeInfo( hNamedPipe,
                               lpFlags,
                               lpOutBufferSize,
                               lpInBufferSize,
                               lpMaxInstances));

}

/*****************************************************************************
 * Name      : BOOL SetNamedPipeHandleState
 * Purpose   : The SetNamedPipeHandleState function sets the read mode and the
 *             blocking mode of the specified named pipe. If the specified handle
 *             is to the client end of a named pipe and if the named pipe server
 *             process is on a remote computer, the function can also be used to
 *             control local buffering.
 * Parameters: HANDLE  hNamedPipe     handle of named pipe
 *             LPDWORD lpdwMode       address of new pipe mode
 *             LPDWORD lpcbMaxCollect address of max. bytes before remote transmission
 *             LPDWORD lpdwCollectDataTimeout address of max. time before remote transmission
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetNamedPipeHandleState(HANDLE  hNamedPipe,
                                      LPDWORD lpdwMode,
                                      LPDWORD lpcbMaxCollect,
                                      LPDWORD lpdwCollectDataTimeout)
{
  // Not implemented but waiting to implementation in hmnpipe.cpp
  return ( HMSetNamedPipeHandleState( hNamedPipe,
                               lpdwMode,
                               lpcbMaxCollect,
                               lpdwCollectDataTimeout));
}

/*****************************************************************************
 * Name      : BOOL TransactNamedPipe
 * Purpose   : The TransactNamedPipe function combines into a single network
 *             operation the functions that write a message to and read a
 *             message from the specified named pipe.
 * Parameters: HANDLE       hNamedPipe  handle of named pipe
 *             LPVOID       lpvWriteBuf address of write buffer
 *             DWORD        cbWriteBuf  size of the write buffer, in bytes
 *             LPVOID       lpvReadBuf  address of read buffer
 *             DWORD        cbReadBuf   size of read buffer, in bytes
 *             LPDWORD      lpcbRead    address of variable for bytes actually read
 *             LPOVERLAPPED lpo         address of overlapped structure
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : NOT FULLY TESTED (YET!)
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 08:48]
 *****************************************************************************/

BOOL WIN32API TransactNamedPipe(HANDLE  hNamedPipe,
                                LPVOID  lpvWriteBuf,
                                DWORD   cbWriteBuf,
                                LPVOID  lpvReadBuf,
                                DWORD   cbReadBuf,
                                LPDWORD lpcbRead,
                                LPOVERLAPPED lpo)
{
  return(HMTransactNamedPipe( hNamedPipe,
                              lpvWriteBuf,
                              cbWriteBuf,
                              lpvReadBuf,
                              cbReadBuf,
                              lpcbRead,
                              lpo));
}

/*****************************************************************************
 * Name      : BOOL WaitNamedPipeA
 * Purpose   : The WaitNamedPipe function waits until either a time-out interval
 *             elapses or an instance of the specified named pipe is available
 *             to be connected to (that is, the pipe's server process has a
 *             pending ConnectNamedPipe operation on the pipe).
 * Parameters: LPCTSTR lpszNamedPipeName
 *             DWORD   dwTimeout
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED (YET)
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 13:52]
 *****************************************************************************/

BOOL WIN32API WaitNamedPipeA(LPCSTR lpszNamedPipeName, DWORD dwTimeout)
{
  return(OSLibDosWaitNamedPipe(lpszNamedPipeName, dwTimeout));
}


/*****************************************************************************
 * Name      : BOOL WaitNamedPipeW
 * Purpose   : The WaitNamedPipe function waits until either a time-out interval
 *             elapses or an instance of the specified named pipe is available
 *             to be connected to (that is, the pipe's server process has a
 *             pending ConnectNamedPipe operation on the pipe).
 * Parameters: LPCWSTR lpszNamedPipeName
 *             DWORD   dwTimeout
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED (YET)
 *
 * Author    : Przemyslaw Dobrowolski [Mon, 2000/01/03 13:44]
 *****************************************************************************/

BOOL WIN32API WaitNamedPipeW(LPCWSTR lpszNamedPipeName, DWORD dwTimeout)
{
  char  *asciiname;
  DWORD rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpszNamedPipeName);

  rc=OSLibDosWaitNamedPipe(asciiname, dwTimeout);

  FreeAsciiString(asciiname);

  return(rc);
}
