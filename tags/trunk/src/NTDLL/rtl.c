/* $Id: rtl.c,v 1.6 2003-04-08 12:47:07 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * NT basis DLL
 *
 * This file contains the Rtl* API functions. These should be implementable.
 *
 * Copyright 1996-1998 Marcus Meissner
 *      1999 Alex Korobka
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <odinwrap.h>

#include "debugtools.h"

#include "winuser.h"
#include "windef.h"
#include "winerror.h"
#include "stackframe.h"
#include "ntddk.h"
#include "winreg.h"
#include "heapstring.h"

#include <misc.h>


ODINDEBUGCHANNEL(NTDLL-RTL)

static CRITICAL_SECTION peb_lock = CRITICAL_SECTION_INIT("peb_lock");

/* CRC polynomial 0xedb88320 */
static const DWORD CRC_table[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


/*
 *  WINE Adoption -  we won't support the Win64 model.
 */
#define SIZE_T UINT

/*
 * resource functions
 */

/***********************************************************************
 *           RtlInitializeResource       (NTDLL.409)
 *
 * xxxResource() functions implement multiple-reader-single-writer lock.
 * The code is based on information published in WDJ January 1999 issue.
 */
void WINAPI RtlInitializeResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlInitializeResource(%08xh)\n", rwl));

  if( rwl )
  {
    rwl->iNumberActive     = 0;
    rwl->uExclusiveWaiters = 0;
    rwl->uSharedWaiters    = 0;
    rwl->hOwningThreadId   = 0;
    rwl->dwTimeoutBoost    = 0; /* no info on this one, default value is 0 */
    InitializeCriticalSection( &rwl->rtlCS );
    rwl->hExclusiveReleaseSemaphore = CreateSemaphoreA( NULL, 0, 65535, NULL );
    rwl->hSharedReleaseSemaphore    = CreateSemaphoreA( NULL, 0, 65535, NULL );
  }
}


/***********************************************************************
 *           RtlDeleteResource              (NTDLL.330)
 */
void WINAPI RtlDeleteResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlDeleteResource(%08xh)\n",
           rwl));

  if( rwl )
  {
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive || rwl->uExclusiveWaiters || rwl->uSharedWaiters )
       dprintf(("NTDLL: RtlDeleteResource active MRSW lock (%p), expect failure\n",
                rwl));

    rwl->hOwningThreadId   = 0;
    rwl->uExclusiveWaiters = rwl->uSharedWaiters = 0;
    rwl->iNumberActive     = 0;
    CloseHandle( rwl->hExclusiveReleaseSemaphore );
    CloseHandle( rwl->hSharedReleaseSemaphore );
    LeaveCriticalSection( &rwl->rtlCS );
    DeleteCriticalSection( &rwl->rtlCS );
  }
}


/***********************************************************************
 *          RtlAcquireResourceExclusive  (NTDLL.256)
 */
BYTE WINAPI RtlAcquireResourceExclusive(LPRTL_RWLOCK rwl,
                                        BYTE         fWait)
{
  BYTE retVal = 0;

  if( !rwl )
    return 0;

  dprintf(("NTDLL: RtlAcquireResourceExclusive(%08xh,%08xh)\n",
           rwl,
           fWait));

start:
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive == 0 ) /* lock is free */
    {
   rwl->iNumberActive = -1;
   retVal = 1;
    }
    else if( rwl->iNumberActive < 0 ) /* exclusive lock in progress */
    {
    if( rwl->hOwningThreadId == GetCurrentThreadId() )
    {
        retVal = 1;
        rwl->iNumberActive--;
        goto done;
    }
wait:
    if( fWait )
    {
        rwl->uExclusiveWaiters++;

        LeaveCriticalSection( &rwl->rtlCS );
        if( WaitForSingleObject( rwl->hExclusiveReleaseSemaphore, INFINITE ) == WAIT_FAILED )
       goto done;
        goto start; /* restart the acquisition to avoid deadlocks */
    }
    }
    else  /* one or more shared locks are in progress */
    if( fWait )
        goto wait;

    if( retVal == 1 )
   rwl->hOwningThreadId = GetCurrentThreadId();
done:
    LeaveCriticalSection( &rwl->rtlCS );
    return retVal;
}

/***********************************************************************
 *          RtlAcquireResourceShared     (NTDLL.257)
 */
BYTE WINAPI RtlAcquireResourceShared(LPRTL_RWLOCK rwl,
                                     BYTE         fWait)
{
  DWORD dwWait = WAIT_FAILED;
  BYTE retVal = 0;

  if( !rwl )
    return 0;

  dprintf(("NTDLL: RtlAcquireResourceShared(%08xh,%08xh)\n",
           rwl,
           fWait));

start:
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive < 0 )
    {
   if( rwl->hOwningThreadId == GetCurrentThreadId() )
   {
       rwl->iNumberActive--;
       retVal = 1;
       goto done;
   }

   if( fWait )
   {
       rwl->uSharedWaiters++;
       LeaveCriticalSection( &rwl->rtlCS );
       if( (dwWait = WaitForSingleObject( rwl->hSharedReleaseSemaphore, INFINITE )) == WAIT_FAILED )
      goto done;
       goto start;
   }
    }
    else
    {
   if( dwWait != WAIT_OBJECT_0 ) /* otherwise RtlReleaseResource() has already done it */
       rwl->iNumberActive++;
   retVal = 1;
    }
done:
    LeaveCriticalSection( &rwl->rtlCS );
    return retVal;
}


/***********************************************************************
 *           RtlReleaseResource             (NTDLL.471)
 */
void WINAPI RtlReleaseResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlReleaseResource(%08xh)\n",
           rwl));

    EnterCriticalSection( &rwl->rtlCS );

    if( rwl->iNumberActive > 0 ) /* have one or more readers */
    {
   if( --rwl->iNumberActive == 0 )
   {
       if( rwl->uExclusiveWaiters )
       {
wake_exclusive:
      rwl->uExclusiveWaiters--;
      ReleaseSemaphore( rwl->hExclusiveReleaseSemaphore, 1, NULL );
       }
   }
    }
    else
    if( rwl->iNumberActive < 0 ) /* have a writer, possibly recursive */
    {
   if( ++rwl->iNumberActive == 0 )
   {
       rwl->hOwningThreadId = 0;
       if( rwl->uExclusiveWaiters )
      goto wake_exclusive;
       else
      if( rwl->uSharedWaiters )
      {
          UINT n = rwl->uSharedWaiters;
          rwl->iNumberActive = rwl->uSharedWaiters; /* prevent new writers from joining until
                                                * all queued readers have done their thing */
          rwl->uSharedWaiters = 0;
          ReleaseSemaphore( rwl->hSharedReleaseSemaphore, n, NULL );
      }
   }
    }
    LeaveCriticalSection( &rwl->rtlCS );
}


/***********************************************************************
 *           RtlDumpResource                (NTDLL.340)
 */
void WINAPI RtlDumpResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlDumpResource(%08x)\n",
           rwl));

  if( rwl )
  {
    dprintf(("NTDLL: RtlDumpResource(%p):\n\tactive count = %i\n\twaiting readers = %i\n\twaiting writers = %i\n",
             rwl,
             rwl->iNumberActive,
             rwl->uSharedWaiters,
             rwl->uExclusiveWaiters));

    if( rwl->iNumberActive )
       dprintf(("NTDLL: \towner thread = %08x\n",
                rwl->hOwningThreadId ));
  }
}


/*
 * heap functions
 */



/*
 * misc functions
 */


/******************************************************************************
 *  RtlAcquirePebLock                       [NTDLL]
 */
VOID WINAPI RtlAcquirePebLock(void)
{
  EnterCriticalSection( &peb_lock );
}


/******************************************************************************
 *  RtlReleasePebLock                       [NTDLL]
 */
VOID WINAPI RtlReleasePebLock(void)
{
  LeaveCriticalSection( &peb_lock );
}

/******************************************************************************
 *  RtlSetEnvironmentVariable		[NTDLL.@]
 */
DWORD WINAPI RtlSetEnvironmentVariable(DWORD x1,PUNICODE_STRING key,PUNICODE_STRING val) {
	FIXME("(0x%08lx,%s,%s),stub!\n",x1,debugstr_w(key->Buffer),debugstr_w(val->Buffer));
	return 0;
}

/******************************************************************************
 *  RtlNewSecurityObject		[NTDLL.@]
 */
DWORD WINAPI RtlNewSecurityObject(DWORD x1,DWORD x2,DWORD x3,DWORD x4,DWORD x5,DWORD x6) {
	FIXME("(0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx),stub!\n",x1,x2,x3,x4,x5,x6);
	return 0;
}

/******************************************************************************
 *  RtlDeleteSecurityObject		[NTDLL.@]
 */
DWORD WINAPI RtlDeleteSecurityObject(DWORD x1) {
	FIXME("(0x%08lx),stub!\n",x1);
	return 0;
}

/**************************************************************************
 *                 RtlNormalizeProcessParams		[NTDLL.@]
 */
LPVOID WINAPI RtlNormalizeProcessParams(LPVOID x)
{
    FIXME("(%p), stub\n",x);
    return x;
}

/**************************************************************************
 *                 RtlGetNtProductType			[NTDLL.@]
 */
BOOLEAN WINAPI RtlGetNtProductType(LPDWORD type)
{
    FIXME("(%p): stub\n", type);
    *type=3; /* dunno. 1 for client, 3 for server? */
    return 1;
}


/******************************************************************************
 *  RtlFormatCurrentUserKeyPath             [NTDLL.371]
 */
NTSTATUS WINAPI RtlFormatCurrentUserKeyPath(IN OUT PUNICODE_STRING pustrKeyPath)
{
  dprintf(("NTDLL: RtlFormatCurrentUserKeyPath(%08xh) not correctly implemented.\n",
           pustrKeyPath));
  
  LPSTR Path = "\\REGISTRY\\USER\\.DEFAULT";
  ANSI_STRING AnsiPath;
  
  RtlInitAnsiString(&AnsiPath, Path);
  return RtlAnsiStringToUnicodeString(pustrKeyPath, &AnsiPath, TRUE);
}


/******************************************************************************
 *  RtlOpenCurrentUser                      [NTDLL]
 */
DWORD WINAPI RtlOpenCurrentUser(IN ACCESS_MASK DesiredAccess, OUT PHANDLE pKeyHandle)
{
  /* Note: this is not the correct solution,
   * But this works pretty good on wine and NT4.0 binaries
   */

  if (DesiredAccess == 0x2000000 )
  {
    *pKeyHandle = HKEY_CURRENT_USER;
    return TRUE;
  }
  
  return FALSE;
/*  PH 2000/08/18 currently disabled
  OBJECT_ATTRIBUTES ObjectAttributes;
  UNICODE_STRING ObjectName;
  NTSTATUS ret;
  
  RtlFormatCurrentUserKeyPath(&ObjectName);
  InitializeObjectAttributes(&ObjectAttributes,&ObjectName,OBJ_CASE_INSENSITIVE,0, NULL);
  ret = NtOpenKey(pKeyHandle, DesiredAccess, &ObjectAttributes);
  RtlFreeUnicodeString(&ObjectName);
  return ret;
  */
}


/**************************************************************************
 *                 RtlDosPathNameToNtPathName_U      [NTDLL.338]
 *
 * FIXME: convert to UNC or whatever is expected here
 */
BOOLEAN  WINAPI RtlDosPathNameToNtPathName_U(LPWSTR          from,
                                             PUNICODE_STRING us,
                                             DWORD           x2,
                                             DWORD           x3)
{
  LPSTR fromA = HEAP_strdupWtoA(GetProcessHeap(),0,from);

  dprintf(("NTDLL: RtlDosPathNameToNtPathName_U(%08xh,%08h,%08xh,%08xh) not implemented.\n",
           from,
           us,
           x2,
           x3));

  if (us)
    RtlInitUnicodeString(us,HEAP_strdupW(GetProcessHeap(),0,from));

  return TRUE;
}


/***********************************************************************
 *           RtlImageNtHeader   (NTDLL)
 */
PIMAGE_NT_HEADERS WINAPI RtlImageNtHeader(HMODULE hModule)
{
    IMAGE_NT_HEADERS *ret = NULL;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)hModule;

    if (dos->e_magic == IMAGE_DOS_SIGNATURE)
    {
        ret = (IMAGE_NT_HEADERS *)((char *)dos + dos->e_lfanew);
        if (ret->Signature != IMAGE_NT_SIGNATURE) ret = NULL;
    }
    return ret;
}

/******************************************************************************
 *  RtlCreateEnvironment                    [NTDLL]
 */
DWORD WINAPI RtlCreateEnvironment(DWORD x1,
                                  DWORD x2)
{
  dprintf(("NTDLL: RtlCreateEnvironment(%08xh, %08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/******************************************************************************
 *  RtlDestroyEnvironment                   [NTDLL]
 */
DWORD WINAPI RtlDestroyEnvironment(DWORD x)
{
  dprintf(("NTDLL: RtlDestroyEnvironment(%08xh) not implemented.\n",
           x));

  return 0;
}


/******************************************************************************
 *  RtlQueryEnvironmentVariable_U           [NTDLL]
 */
DWORD WINAPI RtlQueryEnvironmentVariable_U(DWORD           x1,
                                           PUNICODE_STRING key,
                                           PUNICODE_STRING val)
{
  dprintf(("NTDLL: RtlQueryEnvironmentVariable_U(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           key,
           val));

  return 0;
}

/******************************************************************************
 *  RtlInitializeGenericTable		[NTDLL] 
 */
DWORD WINAPI RtlInitializeGenericTable(void)
{
	FIXME("\n");
	return 0;
}




/******************************************************************************
 *  RtlCopyMemory   [NTDLL] 
 * 
 */
#undef RtlCopyMemory
VOID WINAPI RtlCopyMemory( VOID *Destination, CONST VOID *Source, SIZE_T Length )
{
    memcpy(Destination, Source, Length);
}	

/******************************************************************************
 *  RtlMoveMemory   [NTDLL] 
 */
#undef RtlMoveMemory
VOID WINAPI RtlMoveMemory( VOID *Destination, CONST VOID *Source, SIZE_T Length )
{
    memmove(Destination, Source, Length);
}

/******************************************************************************
 *  RtlFillMemory   [NTDLL] 
 */
#undef RtlFillMemory
VOID WINAPI RtlFillMemory( VOID *Destination, SIZE_T Length, UINT Fill )
{
    memset(Destination, Fill, Length);
}

/******************************************************************************
 *  RtlZeroMemory   [NTDLL] 
 */
#undef RtlZeroMemory
VOID WINAPI RtlZeroMemory( VOID *Destination, SIZE_T Length )
{
    memset(Destination, 0, Length);
}

/******************************************************************************
 *  RtlCompareMemory   [NTDLL] 
 */
SIZE_T WINAPI RtlCompareMemory( const VOID *Source1, const VOID *Source2, SIZE_T Length)
{
    int i;
    for(i=0; (i<Length) && (((LPBYTE)Source1)[i]==((LPBYTE)Source2)[i]); i++);
    return i;
}

/******************************************************************************
 *  RtlAssert                           [NTDLL]
 *
 * Not implemented in non-debug versions.
 */
void WINAPI RtlAssert(LPVOID x1,LPVOID x2,DWORD x3, DWORD x4)
{
	FIXME("(%p,%p,0x%08lx,0x%08lx),stub\n",x1,x2,x3,x4);
}

/*****************************************************************************
 * Name      : RtlCopyLuid
 * Purpose   : copy local unique identifier?
 * Parameters: PLUID pluid1
 *             PLUID pluid2
 * Variables :
 * Result    :
 * Remark    : NTDLL.321
 * Status    : COMPLETELY ? IMPLEMENTED TESTED ?
 *
 * Author    : Patrick Haller [Tue, 1999/11/09 09:00]
 *****************************************************************************/

PLUID WINAPI RtlCopyLuid(PLUID pluid1,
                         PLUID pluid2)
{
  pluid2->LowPart  = pluid1->LowPart;
  pluid2->HighPart = pluid1->HighPart;
  return (pluid1);
}

/******************************************************************************
 *  RtlGetNtVersionNumbers       [NTDLL.@]
 *
 * Introduced in Windows XP (NT5.1)
 */
void WINAPI RtlGetNtVersionNumbers(LPDWORD major, LPDWORD minor, LPDWORD build)
{
	OSVERSIONINFOEXW versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	GetVersionExW((OSVERSIONINFOW*)&versionInfo);

	if (major)
	{
		*major = versionInfo.dwMajorVersion;
	}

	if (minor)
	{
		*minor = versionInfo.dwMinorVersion;
	}

	if (build)
	{
		/* FIXME: Does anybody know the real formula? */
		*build = (0xF0000000 | versionInfo.dwBuildNumber);
	}
}

/*************************************************************************
 * RtlFillMemoryUlong   [NTDLL.@]
 *
 * Fill memory with a 32 bit (dword) value.
 *
 * PARAMS
 *  lpDest  [I] Bitmap pointer
 *  ulCount [I] Number of dwords to write
 *  ulValue [I] Value to fill with
 *
 * RETURNS
 *  Nothing.
 */
VOID WINAPI RtlFillMemoryUlong(ULONG* lpDest, ULONG ulCount, ULONG ulValue)
{
  TRACE("(%p,%ld,%ld)\n", lpDest, ulCount, ulValue);

  while(ulCount--)
    *lpDest++ = ulValue;
}

/*************************************************************************
 * RtlGetLongestNtPathLength    [NTDLL.@]
 *
 * Get the longest allowed path length
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The longest allowed path length (277 characters under Win2k).
 */
DWORD WINAPI RtlGetLongestNtPathLength(void)
{
  TRACE("()\n");
  return 277;
}

/*********************************************************************
 *                  RtlComputeCrc32   [NTDLL.@]
 *
 * Calculate the CRC32 checksum of a block of bytes
 *
 * PARAMS
 *  dwInitial [I] Initial CRC value
 *  pData     [I] Data block
 *  iLen      [I] Length of the byte block
 *
 * RETURNS
 *  The cumulative CRC32 of dwInitial and iLen bytes of the pData block.
 */
DWORD WINAPI RtlComputeCrc32(DWORD dwInitial, PBYTE pData, INT iLen)
{
  DWORD crc = ~dwInitial;

  TRACE("(%ld,%p,%d)\n", dwInitial, pData, iLen);

  while (iLen > 0)
  {
    crc = CRC_table[(crc ^ *pData) & 0xff] ^ (crc >> 8);
    pData++;
    iLen--;
  }
  return ~crc;
}
