/* $Id: wintls.cpp,v 1.21 2005-02-08 20:48:51 sao2l02 Exp $ */
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
#include "asmutil.h"

#define DBG_LOCALLOG  DBG_wintls
#include "dbglocal.h"

//******************************************************************************
// Design information on TLS - AH 2002-07-18
//
// Windows TLS is very restricted in size. We implement it the same way as NT -
// as part of the thread's TEB. We do not use the OS/2 TLS facilities directly.
// The only part we use OS/2 TLS for is to store the thread's TEB pointer.
// We fully support .tls sections in PE modules with this method.
//******************************************************************************

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
 DWORD index;
 TEB  *teb;
 PDB  *pdb;

  teb  = GetThreadTEB();
  pdb  = PROCESS_Current();
  if (!teb) {
            SetLastError(ERROR_NO_MORE_ITEMS);  //TODO: correct error?
            return -1;
  } /* endif */

  EnterCriticalSection(&pdb->crit_section);
  index = search_zero_bit(TLS_MINIMUM_AVAILABLE >> 5, &pdb->tls_bits);
  if((-1 == index) || set_bit(index, &pdb->tls_bits)) {
      LeaveCriticalSection(&pdb->crit_section);
      SetLastError(ERROR_NO_MORE_ITEMS);  //TODO: correct error?
      return -1;
  }
  LeaveCriticalSection(&pdb->crit_section);
  teb->tls_array[index] = 0;
  dprintf(("KERNEL32: %x,%x TlsAlloc returned %d", pdb, teb, index));
  return index;
}

TEB *WIN32API TestTlsIndex(DWORD index)
{
  TEB *teb = NULL;
  PDB *pdb = PROCESS_Current();
  if ((index >= TLS_MINIMUM_AVAILABLE) || (index < 0) || !test_bit(index, &pdb->tls_bits) || !(teb = GetThreadTEB(),teb))
  {
        SetLastError(ERROR_INVALID_PARAMETER);
  }
  return teb;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsFree(DWORD index)
{
  TEB *teb = TestTlsIndex(index);
  PDB *pdb = PROCESS_Current();

  dprintf(("KERNEL32: TlsFree %d", index));
  if (!teb)
  {
        return FALSE;
  }

//  EnterCriticalSection(&pdb->crit_section);
  if(clear_bit(index, &pdb->tls_bits)) {
        teb->tls_array[index] = 0;
//        clear_bit(index, &pdb->tls_bits);
//        LeaveCriticalSection(&pdb->crit_section);
        SetLastError(ERROR_SUCCESS);
        return TRUE;
  }
//  LeaveCriticalSection(&pdb->crit_section);
  SetLastError(ERROR_INVALID_PARAMETER); //TODO: correct error? (does NT even change the last error?)
  return FALSE;
}

//******************************************************************************
//******************************************************************************
LPVOID WIN32API TlsGetValue(DWORD index)
{
  LPVOID rc;
  TEB   *teb = TestTlsIndex(index);
  if (!teb)
  {
        return NULL;
  }
  SetLastError(ERROR_SUCCESS);

  rc = teb->tls_array[index];

  dprintf2(("KERNEL32: TlsGetValue %d returned %X\n", index, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TlsSetValue(DWORD index, LPVOID val)
{
  TEB *teb = TestTlsIndex(index);

  dprintf2(("KERNEL32: TlsSetValue %d %x", index, val));
  if (!teb)
  {
        return FALSE;
  }
  SetLastError(ERROR_SUCCESS);

  teb->tls_array[index] = val;
  return TRUE;
}
//******************************************************************************
//******************************************************************************
