/* $Id: wintls.cpp,v 1.17 2000-12-12 23:57:16 sandervl Exp $ */
/*
 * Win32 TLS API functions
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: correct errors set for Tls* apis? (verify in NT)
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

#define DBG_LOCALLOG  DBG_wintls
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsAlloc() //Allocate TLS index for this module
{
   if(!tlsAddress)
        return;

   tlsIndex = TlsAlloc();
   if(tlsIndex >= TLS_MINIMUM_AVAILABLE) {
        dprintf(("tlsAlloc: invalid tlsIndex %x!!!!", tlsIndex));
        DebugInt3();
        return;
   }
   dprintf(("Win32ImageBase::tlsAlloc (%d) for module %x", tlsIndex, hinstance));
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsDelete()  //Free TLS index for this module
{
   if(!tlsAddress)
        return;

   if(tlsIndex >= TLS_MINIMUM_AVAILABLE) {
        dprintf(("tlsDelete: invalid tlsIndex %x!!!!", tlsIndex));
        DebugInt3();
        return;
   }
   dprintf(("Win32ImageBase::tlsDestroy (%d) for module %x", tlsIndex, hinstance));
   TlsFree(tlsIndex);
   tlsIndex = -1;
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsAttachThread()  //setup TLS structures for new thread
{
 EXCEPTION_FRAME      exceptFrame;
 PIMAGE_TLS_CALLBACK *pCallback;
 LPVOID               tibmem;

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
   tibmem = VirtualAlloc(0, tlsTotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   if(tibmem == NULL) {
        dprintf(("tlsAttachThread: tibmem == NULL!!!!"));
        DebugInt3();
        return;
   }
   memset(tibmem, 0, tlsTotalSize);
   memcpy(tibmem, tlsAddress, tlsInitSize);

   TlsSetValue(tlsIndex, tibmem);
   *tlsIndexAddr = tlsIndex;

   if(tlsCallBackAddr && (ULONG)*tlsCallBackAddr != 0) 
   {
        pCallback = tlsCallBackAddr;
        while(*pCallback) {
            dprintf(("tlsAttachThread: calling TLS Callback %x", *pCallback));

            (*pCallback)((LPVOID)hinstance, DLL_THREAD_ATTACH, 0);

            dprintf(("tlsAttachThread: finished calling TLS Callback %x", *pCallback));
            pCallback++;
        }
   }
   return;
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::tlsDetachThread()  //destroy TLS structures
{
 EXCEPTION_FRAME      exceptFrame;
 PIMAGE_TLS_CALLBACK *pCallback;
 LPVOID tlsmem;

   if(!tlsAddress)
        return;

   dprintf(("Win32ImageBase::tlsDetachThread for module %x, thread id %x", hinstance, GetCurrentThreadId()));

   if(tlsCallBackAddr && (ULONG)*tlsCallBackAddr != 0) 
   {
        pCallback = tlsCallBackAddr;
        while(*pCallback) {
            dprintf(("tlsDetachThread: calling TLS Callback %x", *pCallback));

            (*pCallback)((LPVOID)hinstance, DLL_THREAD_DETACH, 0);

            dprintf(("tlsDetachThread: finished calling TLS Callback %x", *pCallback));
            pCallback++;
        }
   }
   tlsmem = TlsGetValue(tlsIndex);
   if(tlsmem) {
        VirtualFree(tlsmem, 0, MEM_RELEASE);
   }
   else {
        dprintf(("ERROR: tlsDetachThread: tlsmem == NULL!!!"));
   }
   TlsFree(tlsIndex);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API TlsAlloc()
{
 DWORD index = -1;
 TEB  *teb;
 PDB  *pdb;
 DWORD mask, tibidx;
 int   i;

  teb  = GetThreadTEB();
  pdb  = PROCESS_Current();

  EnterCriticalSection(&pdb->crit_section);
  tibidx = 0;
  if(pdb->tls_bits[0] == 0xFFFFFFFF) {
        if(pdb->tls_bits[1] == 0xFFFFFFFF) {
            LeaveCriticalSection(&pdb->crit_section);
            SetLastError(ERROR_NO_MORE_ITEMS);  //TODO: correct error?
            return -1;
        }
        tibidx = 1;
  }
  for(i=0;i<32;i++) {
        mask = (1 << i);
        if((pdb->tls_bits[tibidx] & mask) == 0) {
            pdb->tls_bits[tibidx] |= mask;
            index = (tibidx*32) + i;
            break;
        }
  }
  LeaveCriticalSection(&pdb->crit_section);
  teb->tls_array[index] = 0;

  dprintf(("KERNEL32: TlsAlloc returned %d", index));
  return index;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsFree(DWORD index)
{
 TEB   *teb;
 PDB   *pdb;
 int    tlsidx;
 DWORD  mask;

  dprintf(("KERNEL32: TlsFree %d", index));
  if(index >= TLS_MINIMUM_AVAILABLE)
  {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }

  teb = GetThreadTEB();
  pdb = PROCESS_Current();

  EnterCriticalSection(&pdb->crit_section);
  tlsidx = 0;
  if(index > 32) {
     tlsidx++;
  }
  mask = (1 << index);
  if(pdb->tls_bits[tlsidx] & mask) {
        LeaveCriticalSection(&pdb->crit_section);
        pdb->tls_bits[tlsidx] &= ~mask;
        teb->tls_array[index] = 0;
        SetLastError(ERROR_SUCCESS);
        return TRUE;
  }
  LeaveCriticalSection(&pdb->crit_section);
  SetLastError(ERROR_INVALID_PARAMETER); //TODO: correct error? (does NT even change the last error?)
  return FALSE;
}
//******************************************************************************
//******************************************************************************
LPVOID WIN32API TlsGetValue(DWORD index)
{
 LPVOID rc;
 TEB   *teb;

  if(index >= TLS_MINIMUM_AVAILABLE)
  {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }
  SetLastError(ERROR_SUCCESS);

  teb = GetThreadTEB();
  rc = teb->tls_array[index];

  dprintf2(("KERNEL32: TlsGetValue %d returned %X\n", index, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsSetValue(DWORD index, LPVOID val)
{
 TEB *teb;

  dprintf2(("KERNEL32: TlsSetValue %d %x", index, val));
  if(index >= TLS_MINIMUM_AVAILABLE)
  {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
  }
  SetLastError(ERROR_SUCCESS);

  teb = GetThreadTEB();
  teb->tls_array[index] = val;
  return TRUE;
}
//******************************************************************************
//******************************************************************************
