/* $Id: ioctl.cpp,v 1.3 2001-11-18 14:32:13 sandervl Exp $ */
/*
 * based on Windows Sockets 1.1 specs
 * (ftp.microsoft.com:/Advsys/winsock/spec11/WINSOCK.TXT)
 * 
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 * NOTE: If you make any changes to fix a particular app, make sure 
 * they don't break something else like Netscape or telnet and ftp 
 * clients and servers (www.winsite.com got a lot of those).
 *
 * NOTE 2: Many winsock structs such as servent, hostent, protoent, ...
 * are used with 1-byte alignment for Win16 programs and 4-byte alignment
 * for Win32 programs in winsock.h. winsock2.h uses forced 4-byte alignment.
 * So we have non-forced (just as MSDN) ws_XXXXent (winsock.h), 4-byte forced
 * ws_XXXXent32 (winsock2.h) and 1-byte forced ws_XXXXent16 (winsock16.h).
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

// Note: this file is currently incompatible with performance tracing
#undef PROFILE

#include <odin.h>
#include <odinwrap.h>

#include <windows.h>
#include <winsock2.h>
#include <misc.h>


#define DBG_LOCALLOG	DBG_wsa
#include "dbglocal.h"


ODINDEBUGCHANNEL(WS2_32-IOCTL)


/*****************************************************************************
 * Name      : int WSAIoctl
 * Purpose   : direct socket / stack ioctls
 * Parameters: SOCKET  s
 *             DWORD   dwIoControlCode
 *             LPVOID  lpvInBuffer
 *             DWORD   cbInBuffer
 *             LPVOID  lpvOutBuffer
 *             DWORD   cbOutBuffer
 *             LPDWORD lpcbBytesReturned
 *             LPWSAOVERLAPPED lpOverlapped (unsupported, must be NULL)
 *             LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine (unsupported, must be NULL)
 * Variables :
 * Result    : 0 indicates success, SOCKET_ERROR indicates failure in case
 *             WSAGetLastError provides closer information
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [2001-09-17]
 *****************************************************************************/

ODINFUNCTION9(int,     WSAIoctl,
              SOCKET,  s,
              DWORD,   dwIoControlCode,
              LPVOID,  lpvInBuffer,
              DWORD,   cbInBuffer,
              LPVOID,  lpvOutBuffer,
              DWORD,   cbOutBuffer,
              LPDWORD, lpcbBytesReturned,
              LPWSAOVERLAPPED, lpOverlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE, lpCompletionRoutine)
{
  dprintf(("not implemented"));
  
  WSASetLastError(ERROR_NOT_SUPPORTED);
  return SOCKET_ERROR;
}