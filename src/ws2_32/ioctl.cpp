/* $Id: ioctl.cpp,v 1.1 2001-09-18 16:58:59 phaller Exp $ */
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

#define INCL_BASE
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <misc.h>
#include <win32api.h>

#define DBG_LOCALLOG	DBG_wsa
#include "dbglocal.h"

// PH: ugly ...
#include "../wsock32/wsock32.h"


ODINDEBUGCHANNEL(WS2_32-IOCTL)


typedef LPVOID LPWSAOVERLAPPED;
typedef LPVOID LPWSAOVERLAPPED_COMPLETION_ROUTINE;


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