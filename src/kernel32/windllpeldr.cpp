/* $Id: windllpeldr.cpp,v 1.10 2002-07-18 11:52:56 achimha Exp $ */

/*
 * Win32 PE loader Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <win32type.h>
#include <pefile.h>
#include <windllpeldr.h>
#include <wprocess.h>

#include "oslibmisc.h"
#include "oslibdos.h"

#define DBG_LOCALLOG    DBG_windllpeldr
#include "dbglocal.h"

//******************************************************************************
// Design information on PE DLL memory layout - AH 2002-07-18
//
// We are currently using (high) private memory for all PE objects, including the
// read/execute code segments, constant data segments and global data segments.
// Global data segments might not be implemented correctly at all as we've never
// encountered any applictions making use of them. Therefore we are actually
// wasting memory when running multiple processes using the same PE DLLs.
//
// There are several reasons for this design decisions. Both OS/2 (LX) and
// Windows NT put all DLL segments into the shared arena. What they do for
// instance data is map it for each process to read-only pages initially. When
// a write attempt is made by a process, an exception will be triggered. This
// makes the operating system to copy the data to a new page that is read/write
// and change the page table of the process to map the linear process in the
// shared arena to private memory (this is called "copy-on-write").
// Even though an application is not guaranteed any virtual address for instance
// data segments, they always end up in the shared region and the virtual addreses
// are contiguous. An application could therefore make nasty assumptions.
// Unfortunately, it is not possible for us from ring 3 to replicate the behavior
// for our PE loader. While we can make the page read only and catch the
// exception, we have no method to remap the pages to private memory.
//
// One solution would be to create another reagion with the private region,
// i.e. define some address space range as reserved in Odin (configurable to
// workaround issues with certain PE images requiring those addresses). We
// could then load the instance data segments of PE DLLs into this private
// memory arena and still guarantee identical virtual addresses for each
// process.
//
// While the above method should work fine (assuming an application does not
// make any nasty assumptions), there is one major problem. If we enable the
// PE on-demand loader (i.e. the mmap loads each page from the PE file when
// it is accesses for the first time - very much like NT), then we would have
// nasty concurrency issues. A process could access a page for the first time
// and the exception is triggered. We commit the page read the data in using
// a call to DosRead. If the very same page is accessed from a different
// process after we have committed it but before we have finished the DosRead,
// we would run into problems. Unfortunately, there does not seem to be any
// solution for this.
//
// The bottomline is that we put everything into private memory and accept the
// drawback of wasting memory.
//******************************************************************************


//******************************************************************************
//******************************************************************************
Win32PeLdrDll::Win32PeLdrDll(char *szDllName, Win32ImageBase *parentImage)
                : Win32ImageBase(-1),
                  Win32DllBase(-1, 0, parentImage),
                  Win32PeLdrImage(szDllName, FALSE)
{
  dprintf(("Win32PeLdrDll::Win32PeLdrDll %s %s loaded by %s", szFileName, szModule,
          (parentImage) ? parentImage->getModuleName() : "Unknown"));
}
//******************************************************************************
//******************************************************************************
Win32PeLdrDll::~Win32PeLdrDll()
{
  dprintf(("Win32PeLdrDll::~Win32PeLdrDll %s", szModule));
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrDll::init(ULONG reservedMem)
{
 char   modname[CCHMAXPATH];
 char  *syspath;
 HFILE  dllfile;
 APIRET rc;
 BOOL   fRet;

  strupr(szFileName);
  if(!strchr(szFileName, (int)'.')) {
    strcat(szFileName, DLL_EXTENSION);
  }
  dllfile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  if(dllfile == NULL) {//search in libpath for dll
    syspath = getenv("WIN32LIBPATH");
    if(syspath) {
        strcpy(modname, syspath);
        if(modname[strlen(modname)-1] != '\\') {
            strcat(modname, "\\");
        }
        strcat(modname, szFileName);
        strcpy(szFileName, modname);
    }
  }
  else  OSLibDosClose(dllfile);
  fRet = Win32PeLdrImage::init(0);
  dllEntryPoint = (WIN32DLLENTRY)entryPoint;

  if(!(fh.Characteristics & IMAGE_FILE_DLL)) {
    //executable loaded as dll; don't call entrypoint
    dprintf(("WARNING: Exe %s loaded as dll; entrypoint not called", szFileName));
    dllEntryPoint = NULL;
  }
  return fRet;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrDll::isPe2LxDll() const
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrDll::isLxDll() const
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
