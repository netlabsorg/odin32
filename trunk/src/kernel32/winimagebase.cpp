/* $Id: winimagebase.cpp,v 1.8 2000-02-16 14:22:12 sandervl Exp $ */

/*
 * Win32 PE Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#define INCL_BASE
#include <os2wrap.h>             //Odin32 OS/2 api wrappers

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <misc.h>
#include <win32type.h>
#include <winimagebase.h>
#include <windllbase.h>
#include <winexebase.h>
#include <pefile.h>
#include <unicode.h>
#include <winres.h>
#include "oslibmisc.h"
#include "oslibdos.h"
#include "initterm.h"
#include <win\virtual.h>

#define DBG_LOCALLOG	DBG_winimagebase
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Win32ImageBase::Win32ImageBase(HINSTANCE hInstance) :
    errorState(NO_ERROR), entryPoint(0), fullpath(NULL),
    tlsAddress(0), tlsIndexAddr(0), tlsInitSize(0), tlsTotalSize(0),
    tlsCallBackAddr(0), tlsIndex(-1), winres(NULL), pResDir(NULL),
    ulRVAResourceSection(0)
{
#ifdef DEBUG
  magic = MAGIC_WINIMAGE;
#endif

  if(hInstance != -1) {
    this->hinstance = hInstance;

    char *name = OSLibGetDllName(hinstance);
    strcpy(szFileName, name);
    strupr(szFileName);

    //rename dll (os/2 -> win32) if necessary (i.e. OLE32OS2 -> OLE32)
    Win32DllBase::renameDll(szFileName, FALSE);

    name = strrchr(szFileName, '\\')+1;
    strcpy(szModule, name);

    char *dot = strrchr(szModule, '.');
    if(dot)
        *dot = 0;
  }
  else {
    szModule[0] = 0;
    this->hinstance = -1;
  }
}
//******************************************************************************
//******************************************************************************
Win32ImageBase::~Win32ImageBase()
{
 Win32Resource *res;

  while(winres)
  {
        res    = winres->next;
        delete(winres);
        winres = res;
  }
  if(fullpath)
        free(fullpath);
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::setFullPath(char *name)
{
  dassert(name, ("setFullPath, name == NULL"));
  fullpath = (char *)malloc(strlen(name)+1);
  dassert(fullpath, ("setFullPath, fullpath == NULL"));
  strcpy(fullpath, name);
}
//******************************************************************************
//Returns required OS version for this image
//******************************************************************************
ULONG Win32ImageBase::getVersion()
{
  dprintf(("Win32ImageBase::getVersion: NOT IMPLEMENTED!"));
  return 0x40000; //NT 4
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::isPEImage(char *szFileName)
{
 char   modname[CCHMAXPATH];
 char   filename[CCHMAXPATH];
 char  *syspath;
 HFILE  dllfile;
 IMAGE_FILE_HEADER     fh;
 HFILE  win32handle;
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
 APIRET rc             = NO_ERROR;            /* Return code */
 LPVOID win32file      = NULL;
 ULONG  ulRead;
 int    nSections, i;

  strcpy(filename, szFileName);
  strupr(filename);
  if(!strchr(filename, (int)'.')) {
    strcat(filename,".DLL");
  }
  dllfile = OSLibDosOpen(filename, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  if(dllfile == NULL) {//search in libpath for dll
    	strcpy(modname, kernel32Path);
    	strcat(modname, filename);
	dllfile = OSLibDosOpen(modname, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
	if(dllfile == NULL) {
		OSLibDosSearchPath(OSLIB_SEARCHENV, "PATH", filename, filename, sizeof(filename));
	}
    	else {
		strcpy(filename, modname);
		OSLibDosClose(dllfile);
	}
  }
  else  OSLibDosClose(dllfile);

  rc = DosOpen(filename,                       /* File path name */
           &win32handle,                       /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                             /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READONLY,           /* Open mode of the file */
               0L);                            /* No extended attribute */

  if (rc != NO_ERROR)
  {
        dprintf(("KERNEL32:Win32ImageBase::isPEImage(%s) failed with %u\n",
                  szFileName, rc));
        return(FALSE);
  }

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  IMAGE_DOS_HEADER *pdoshdr = (IMAGE_DOS_HEADER *)malloc(sizeof(IMAGE_DOS_HEADER));
  if(pdoshdr == NULL)   {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  rc = DosRead(win32handle, pdoshdr, sizeof(IMAGE_DOS_HEADER), &ulRead);
  if(rc != NO_ERROR) {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  ULONG hdrsize = pdoshdr->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER);
  free(pdoshdr);

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  win32file = malloc(hdrsize);
  if(win32file == NULL) {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  rc = DosRead(win32handle, win32file, hdrsize, &ulRead);
  if(rc != NO_ERROR) {
        goto failure;
  }

  if(GetPEFileHeader (win32file, &fh) == FALSE) {
        goto failure;
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
        goto failure;
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
        goto failure;
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
        goto failure;
  }
  DosClose(win32handle);
  return(TRUE);

failure:
  free(win32file);
  DosClose(win32handle);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
/**
 * Static helper which finds the Win32ImageBase object corresponding to hModule.
 * @returns   Pointer to Win32ImageBase object corresponding to hModule.
 * @param     hModule  Odin32 modulehandler. 0 and -1 is aliases for the executable module
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
Win32ImageBase * Win32ImageBase::findModule(HMODULE hModule)
{
    Win32ImageBase *pRet;

    if (hModule == -1 || hModule == 0 ||       /* note: WinNt 4, SP4 don't accept -1 as the EXE handle.*/
        (WinExe != NULL && hModule == WinExe->getInstanceHandle())
        )
        pRet = WinExe;
    else
        pRet = Win32DllBase::findModule(hModule);

    if (pRet == NULL)
    {
        if (WinExe == NULL)
            dprintf(("Win32ImageBase::findModule: Module not found. WinExe is NULL, hModule=%#x\n", hModule));
        else
            dprintf(("Win32ImageBase::findModule: Module not found, hModule=%#x\n", hModule));
    }

    return pRet;
}


