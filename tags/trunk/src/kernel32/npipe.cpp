/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Named pipes API stub functions
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#include <os2win.h>

//TODO: Not done!!

//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekNamedPipe(HANDLE hPipe, LPVOID lpvBuffer, DWORD cbBuffer,
                                                 LPDWORD lpcbRead, LPDWORD lpcbAvail, LPDWORD lpcbMessage)
{
  dprintf(("PeekNamedPipe Not Implemented!\n"));
  return(FALSE);
}
//******************************************************************************
//LPSECURITY_ATTRIBUTES lpsa;   /* address of security attributes   */
//******************************************************************************
BOOL WIN32API CreatePipe(PHANDLE phRead, PHANDLE phWrite, LPSECURITY_ATTRIBUTES lpsa, DWORD cbPipe)
{
  dprintf(("CreatePipe Not Implemented!\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateNamedPipeA(LPCTSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                    DWORD nMaxInstances, DWORD nOutBufferSize,
                    DWORD nInBufferSize, DWORD nDefaultTimeOut,
                    void *lpSecurityAttributes)
{
  dprintf(("CreateNamedPipe Not Implemented!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateNamedPipeW(LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                    DWORD nMaxInstances, DWORD nOutBufferSize,
                    DWORD nInBufferSize, DWORD nDefaultTimeOut,
                    void *lpSecurityAttributes)
{
  dprintf(("CreateNamedPipe Not Implemented!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
