/* $Id: iphlpapi.cpp,v 1.1 2001-05-24 17:35:34 sandervl Exp $ */
/*
 *	IPHLPAPI library
 *
 */

#include <os2win.h>
#include <odinwrap.h>
#include <winversion.h>


//******************************************************************************
//******************************************************************************
DWORD WIN32API GetAdaptersInfo(DWORD arg1, DWORD arg2)
{
  dprintf(("GetAdaptersInfo %x %x - STUB", arg1, arg2));
  return ERROR_NOT_SUPPORTED; //NT returns this 
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetNetworkParams(DWORD arg1, DWORD arg2)
{
  dprintf(("GetNetworkParams %x %x - STUB", arg1, arg2));
  return ERROR_NOT_SUPPORTED; //NT returns this 
}
//******************************************************************************
//******************************************************************************
