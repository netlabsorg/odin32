/* $Id: os2native.cpp,v 1.5 1999-08-22 11:11:10 sandervl Exp $ */

/*
 * Misc procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 * Copyright 1998 Peter FitzSimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __IBMC__
#include <builtin.h>
#endif
#include <win32type.h>
#include "misc.h"
#include "exceptions.h"

#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD            0x100
#define PAGE_NOCACHE          0x200
#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_TOP_DOWN       0x100000             //Ignored

extern ULONG flAllocMem;    /* Tue 03.03.1998: knut */

#ifdef DEBUG
ULONG commit = 0, reserve = 0;
#endif

//******************************************************************************
//******************************************************************************
LPVOID WIN32API VirtualAlloc(LPVOID lpvAddress, DWORD cbSize, DWORD fdwAllocationType,
                             DWORD fdwProtect)
{
 PVOID Address = lpvAddress;
 ULONG flag = 0, base;
 APIRET rc;

  dprintf(("VirtualAlloc at %X; %d bytes, fAlloc %d, fProtect %d\n", (int)lpvAddress, cbSize, fdwAllocationType, fdwProtect));

  if (cbSize > 0x7fc00000)  /* 2Gb - 4Mb */
  {
	dprintf(("VirtualAlloc: size too large"));
//        SetLastError( ERROR_OUTOFMEMORY );
        return NULL;
  }

  if (!(fdwAllocationType & (MEM_COMMIT | MEM_RESERVE)) ||
       (fdwAllocationType & ~(MEM_COMMIT | MEM_RESERVE)))
  {
	dprintf(("VirtualAlloc: Invalid parameter"));
//        SetLastError( ERROR_INVALID_PARAMETER );
        return NULL;
  }

  if(fdwAllocationType & MEM_COMMIT) {
        dprintf(("VirtualAlloc: commit\n"));
        flag = PAG_COMMIT;
  }
  if(fdwProtect & PAGE_READONLY)     flag |= PAG_READ;
  if(fdwProtect & PAGE_READWRITE)    flag |= (PAG_READ | PAG_WRITE);
  if(fdwProtect & PAGE_EXECUTE_READ) flag |= PAG_EXECUTE;
  if(fdwProtect & PAGE_GUARD)        flag |= PAG_GUARD;

  //just do this if other options are used
  if(!(flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) || flag == 0)
        flag |= PAG_READ | PAG_WRITE;

  if(fdwAllocationType & MEM_COMMIT && lpvAddress != NULL) {
        Address = lpvAddress;

        rc = DosSetMem(lpvAddress, cbSize, flag);
        //might try to commit larger part with same base address
    	if(rc == ERROR_ACCESS_DENIED && cbSize > 4096 ) {//knut: AND more than one page
        	char *newbase = (char *)lpvAddress + ((cbSize-1) & 0xFFFFF000); //knut: lets not start after the last page!
                ULONG size, os2flags;

        	while(newbase >= (char *)lpvAddress) {  //knut: should check first page to!!
                        size     = 4096;
                        os2flags = 0;
                        rc = DosQueryMem(newbase, &size, &os2flags);
                        if(rc)  break;
                        if(os2flags & PAG_COMMIT) {
                                newbase += 4096;
                                break;
                        }
                        newbase -= 4096;
                }
                if(rc == 0) {
                        //In case it wants to commit bytes that fall into the last
                        //page of the previous commit command
                        if(cbSize > ((int)newbase - (int)lpvAddress)) {
                                rc = DosSetMem(newbase, cbSize - ((int)newbase - (int)lpvAddress), flag);
                        }
                }
                else    return(NULL);
        }
	else 
	{
		if(rc == ERROR_INVALID_ADDRESS)
		{
		    	rc = DosAllocMem(&Address, cbSize, flag | flAllocMem );
		}
		else	
		if(rc)	dprintf(("Unexpected DosSetMem error %x", rc));
	}
  }
  else {
    /*knut: flAllocMem */
    rc = DosAllocMem(&Address, cbSize, flag | flAllocMem );
  }

  //TODO: Set last error in case rc != 0
  if(rc) {
        dprintf(("DosSetMem returned %d\n", rc));
//        SetLastError( ERROR_OUTOFMEMORY );
        return(NULL);
  }

  dprintf(("VirtualAlloc returned %X\n", Address));
  return(Address);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API VirtualFree(LPVOID lpvAddress, DWORD cbSize, DWORD FreeType)
{
 APIRET rc;

  dprintf(("VirtualFree at %d; %d bytes, freetype %d\n", (int)lpvAddress, cbSize, FreeType));

  if(FreeType & MEM_DECOMMIT) {
        rc = DosSetMem(lpvAddress, cbSize, PAG_DECOMMIT);
  }
  else  rc = DosFreeMem(lpvAddress);    //MEM_RELEASE, cbSize == 0 (or should be)

//TODO: Set last error in case rc != 0
  if(rc)        return(FALSE);
  return(TRUE);
}
//******************************************************************************
//LPVOID lpvAddress;            /* address of region of committed pages */
//DWORD  cbSize;                /* size of the region   */
//DWORD  fdwNewProtect;         /* desired access protection    */
//PDWORD pfdwOldProtect;        /* address of variable to get old protection    */
//TODO: Not 100% complete
//TODO: SetLastError on failure
//******************************************************************************
BOOL WIN32API VirtualProtect(LPVOID lpvAddress, DWORD cbSize, DWORD fdwNewProtect,
                                DWORD *pfdwOldProtect)
{
 APIRET rc;
 ULONG  pageFlags = 0;
 int npages;

  dprintf(("VirtualProtect %X; %d bytes, new flags %X (%X)\n", (int)lpvAddress, cbSize, fdwNewProtect, pfdwOldProtect));
//  _interrupt(3);
  if(pfdwOldProtect == NULL)
        return(FALSE);

  rc = DosQueryMem(lpvAddress, &cbSize, &pageFlags);
  if(rc) {
        dprintf(("DosQueryMem returned %d\n", rc));
        return(FALSE);
  }
  dprintf(("Old memory flags %X\n", pageFlags));
  *pfdwOldProtect = 0;
  if(pageFlags & PAG_READ && !(pageFlags & PAG_WRITE))
        *pfdwOldProtect |= PAGE_READONLY;
  if(pageFlags & (PAG_READ | PAG_WRITE))
        *pfdwOldProtect |= PAGE_READWRITE;
  if(pageFlags & PAG_EXECUTE)
        *pfdwOldProtect |= PAGE_EXECUTE_READ;
  if(pageFlags & PAG_GUARD)
        *pfdwOldProtect |= PAGE_GUARD;
  pageFlags = 0;

  if(fdwNewProtect & PAGE_READONLY)     pageFlags |= PAG_READ;
  if(fdwNewProtect & PAGE_READWRITE)    pageFlags |= (PAG_READ | PAG_WRITE);
  if(fdwNewProtect & PAGE_EXECUTE_READ) pageFlags |= PAG_EXECUTE;
  if(fdwNewProtect & PAGE_EXECUTE_READWRITE)
        pageFlags |= (PAG_EXECUTE | PAG_WRITE);
  if(fdwNewProtect & PAGE_GUARD)        pageFlags |= PAG_GUARD;
//Not supported in OS/2??
//  if(fdwNewProtect & PAGE_NOACCESS)

  dprintf(("New memory flags %X\n", pageFlags));
  if(pageFlags == 0) {
        dprintf(("pageFlags == 0\n"));
        return(TRUE);   //nothing to do
  }
  npages = ((int)lpvAddress + cbSize >> 12) - ((int)lpvAddress >> 12) + 1;

  lpvAddress = (LPVOID)((int)lpvAddress & ~0xFFF);
  cbSize     = npages*4096;
  dprintf(("lpvAddress = %X, cbSize = %d\n", lpvAddress, cbSize));

  rc = DosSetMem(lpvAddress, cbSize, pageFlags);
  if(rc) {
        dprintf(("DosSetMem returned %d\n", rc));
        return(FALSE);
  }
  return(TRUE);
}
//******************************************************************************
#define PMEMORY_BASIC_INFORMATION void *
//******************************************************************************
DWORD WIN32API VirtualQuery(LPVOID lpvAddress, PMEMORY_BASIC_INFORMATION pmbiBuffer,
                               DWORD cbLength)
{
  dprintf(("VirtualQuery - stub\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
#ifdef __WATCOMC__  /*PLF Sat  97-06-21 17:12:36*/
    extern void interrupt3( void );
    #pragma aux interrupt3= \
              "int 3"
#endif
void WIN32API DebugBreak()
{
  dprintf(("DebugBreak\n"));
#ifdef __WATCOMC__
  interrupt3();
#else
  _interrupt(3);
#endif
}
//******************************************************************************
//TODO: Implement this??
//******************************************************************************
BOOL WIN32API GetThreadContext(HANDLE hThread, LPWINCONTEXT lpContext)
{
  dprintf(("GetThreadContext NOT IMPLEMENTED!! (TRUE)\n"));
  memset(lpContext, 0, sizeof(WINCONTEXT));
  return TRUE;
}
//******************************************************************************
//TODO: Implement this??
//******************************************************************************
BOOL WIN32API SetThreadContext(HANDLE hThread, LPWINCONTEXT lpContext)
{
  dprintf(("SetThreadContext NOT IMPLEMENTED!!\n"));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API VirtualLock( LPVOID lpAddress, DWORD dwSize )
{
  dprintf(("VirtualLock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
  return TRUE;
}

//******************************************************************************
BOOL WIN32API VirtualUnlock( LPVOID lpAddress, DWORD dwSize )
{
  dprintf(("VirtualUnlock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
  return TRUE;
}


/*****************************************************************************
 * Name      : BOOL VirtualProtectEx
 * Purpose   : The VirtualProtectEx function changes the access protection on
 *             a region of committed pages in the virtual address space of a specified
 *             process. Note that this function differs from VirtualProtect,
 *             which changes the access protection on the calling process only.
 * Parameters: HANDLE hProcess       handle of process
 *             LPVOID lpvAddress     address of region of committed pages
 *             DWORD  cbSize         size of region
 *             DWORD  fdwNewProtect  desired access protection
 *             PDWORD pfdwOldProtect address of variable to get old protection
 * Variables :
 * Result    : size of target buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API VirtualProtectEx(HANDLE hProcess,
                                  LPVOID lpvAddress,
                                  DWORD  cbSize,
                                  DWORD  fdwNewProtect,
                                  LPDWORD pfdwOldProtect)
{
  dprintf(("KERNEL32: VirtualProtectEx(%08x,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           lpvAddress,
           cbSize,
           fdwNewProtect,
           pfdwOldProtect));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD VirtualQueryEx
 * Purpose   : The VirtualQueryEx function provides information about a range
 *             of pages within the virtual address space of a specified process.
 * Parameters: HANDLE  hProcess                     handle of process
 *             LPCVOID  lpvAddress                  address of region
 *             PMEMORY_BASIC_INFORMATION pmbiBuffer address of information buffer
 *             DWORD  cbLength                      size of buffer
 * Variables :
 * Result    : number of bytes returned in buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API VirtualQueryEx(HANDLE  hProcess,
                                 LPVOID  lpvAddress,
                                 PMEMORY_BASIC_INFORMATION pmbiBuffer,
                                 DWORD   cbLength)
{
  dprintf(("KERNEL32: VirtualQueryEx(%08x,%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           lpvAddress,
           pmbiBuffer,
           cbLength));

  return (0);
}

