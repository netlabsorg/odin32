/* $Id: wintls.cpp,v 1.9 2000-02-16 14:22:13 sandervl Exp $ */
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
#include <string.h>
#include <winimagebase.h>
#include <thread.h>
#include <wprocess.h>
#include "exceptutil.h"

#define DBG_LOCALLOG	DBG_wintls
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsAlloc()	//Allocate TLS index for this module
{
   if(!tlsAddress) 
	return;

   tlsIndex = TlsAlloc();
   if(tlsIndex >= TLS_MINIMUM_AVAILABLE) {
	dprintf(("tlsAttachThread: invalid tlsIndex %x!!!!", tlsIndex));
	DebugInt3();
	return;  	
   }
   dprintf(("Win32ImageBase::tlsAlloc (%d) for module %x", tlsIndex, hinstance));
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsDelete()	//Free TLS index for this module
{
   if(!tlsAddress) 
	return;

   if(tlsIndex >= TLS_MINIMUM_AVAILABLE) {
	dprintf(("tlsAttachThread: invalid tlsIndex %x!!!!", tlsIndex));
	DebugInt3();
	return;  	
   }
   dprintf(("Win32ImageBase::tlsDestroy (%d) for module %x", tlsIndex, hinstance));
   TlsFree(tlsIndex);
   tlsIndex = -1;
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsAttachThread()	//setup TLS structures for new thread
{
 EXCEPTION_FRAME exceptFrame;
 PIMAGE_TLS_CALLBACK *pCallback;
 TEB   *winteb;
 char  *tibmem;

   if(!tlsAddress) 
	return;

   if(tlsIndex >= TLS_MINIMUM_AVAILABLE) {
	dprintf(("tlsAttachThread: invalid tlsIndex %x!!!!", tlsIndex));
	DebugInt3();
	return;  	
   }

   dprintf(("Win32ImageBase::tlsAttachThread for module %x, thread id %x", hinstance, GetCurrentThreadId()));
   dprintf(("tlsAddress:      %x", tlsAddress));
   dprintf(("tlsInitSize:     %x", tlsInitSize));
   dprintf(("tlsTotalSize     %x", tlsTotalSize));
   dprintf(("tlsIndexAddr     %x", tlsIndexAddr));
   dprintf(("tlsCallbackAddr  %x", tlsCallBackAddr));
   dprintf(("*tlsCallbackAddr %x", (tlsCallBackAddr) ? *tlsCallBackAddr : 0));
   tibmem = (char *)VirtualAlloc(0, tlsTotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   if(tibmem == NULL) {
	dprintf(("tlsAttachThread: tibmem == NULL!!!!"));
	DebugInt3();
	return;
   }
   memset(tibmem, 0, tlsTotalSize);
   memcpy(tibmem, tlsAddress, tlsInitSize);

   winteb = (TEB *)*TIBFlatPtr;
   winteb->tls_ptr[tlsIndex] = tibmem;
   *tlsIndexAddr = tlsIndex;

   if(tlsCallBackAddr && (ULONG)*tlsCallBackAddr != 0) {
	pCallback = tlsCallBackAddr;
	while(*pCallback) {
		dprintf(("tlsAttachThread: calling TLS Callback %x", *pCallback));

		(*pCallback)((LPVOID)hinstance, DLL_THREAD_ATTACH, 0);

		dprintf(("tlsAttachThread: finished calling TLS Callback %x", *pCallback));
		*pCallback++;
	}
   }
   return;
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsDetachThread()	//destroy TLS structures
{
 EXCEPTION_FRAME exceptFrame;
 PIMAGE_TLS_CALLBACK *pCallback;
 TEB   *winteb;

   if(!tlsAddress) 
	return;

   dprintf(("Win32ImageBase::tlsDetachThread for module %x, thread id %x", hinstance, GetCurrentThreadId()));

   if(tlsCallBackAddr && (ULONG)*tlsCallBackAddr != 0) {
	pCallback = tlsCallBackAddr;
	while(*pCallback) {
		dprintf(("tlsDetachThread: calling TLS Callback %x", *pCallback));

		(*pCallback)((LPVOID)hinstance, DLL_THREAD_DETACH, 0);

		dprintf(("tlsDetachThread: finished calling TLS Callback %x", *pCallback));
		*pCallback++;
	}
   }
   winteb = (TEB *)*TIBFlatPtr;
   VirtualFree(winteb->tls_ptr[tlsIndex], tlsTotalSize, MEM_RELEASE);
   winteb->tls_ptr[tlsIndex] = 0;
}
//******************************************************************************
//******************************************************************************

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
