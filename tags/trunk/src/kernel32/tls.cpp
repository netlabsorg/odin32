/* $Id: tls.cpp,v 1.1 1999-06-28 07:55:33 sandervl Exp $ */
/*
 * Win32 TLS API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>

//******************************************************************************
//******************************************************************************
DWORD WIN32API TlsAlloc()
{
  dprintf(("KERNEL32:  TlsAlloc\n"));
  return(O32_TlsAlloc());
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsFree(DWORD index)
{
  dprintf(("KERNEL32:  TlsFree\n"));
  return(O32_TlsFree(index));
}
//******************************************************************************
//******************************************************************************
LPVOID WIN32API TlsGetValue(DWORD index)
{
 LPVOID rc;

  rc = O32_TlsGetValue(index);
//  dprintf(("KERNEL32:  TlsGetValue %d returned %X\n", index, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsSetValue(DWORD index, LPVOID val)
{
//  dprintf(("KERNEL32:  TlsSetValue\n"));
  return(O32_TlsSetValue(index, val));
}
//******************************************************************************
//******************************************************************************
