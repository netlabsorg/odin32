/* $Id: winimagebase.cpp,v 1.31 2001-04-02 22:51:58 sandervl Exp $ */

/*
 * Win32 PE Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
#include "oslibmisc.h"
#include "oslibdos.h"
#include "initterm.h"
#include "directory.h"
#include <win\virtual.h>
#include <winconst.h>

#define DBG_LOCALLOG    DBG_winimagebase
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Win32ImageBase::Win32ImageBase(HINSTANCE hInstance) :
    errorState(NO_ERROR), entryPoint(0), fullpath(NULL),
    tlsAddress(0), tlsIndexAddr(0), tlsInitSize(0), tlsTotalSize(0),
    tlsCallBackAddr(0), tlsIndex(-1), pResRootDir(NULL),
    ulRVAResourceSection(0)
{
  magic = MAGIC_WINIMAGE;

  if(hInstance != -1) {
    this->hinstance = hInstance;

    char *name = OSLibGetDllName(hinstance);
    strcpy(szFileName, name);
    strupr(szFileName);

    //rename dll (os/2 -> win32) if necessary (i.e. OLE32OS2 -> OLE32)
    Win32DllBase::renameDll(szFileName, FALSE);

    name = strrchr(szFileName, '\\')+1;
    strcpy(szModule, name);
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
  if(fullpath)
        free(fullpath);
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::setFullPath(char *name)
{
  if(fullpath)
        free(fullpath);
  fullpath = (char *)malloc(strlen(name)+1);
  strcpy(fullpath, name);
}
//******************************************************************************
//Add image to dependency list of this image
//******************************************************************************
void Win32ImageBase::addDependency(Win32DllBase *image)
{
  loadedDlls.Push((ULONG)image);
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::dependsOn(Win32DllBase *image)
{
 QueueItem    *item;
 BOOL          ret = FALSE;

  dlllistmutex.enter();
  item = loadedDlls.Head();
  while(item) {
    if(loadedDlls.getItem(item) == (ULONG)image) {
        ret = TRUE;
        break;
    }
    item = loadedDlls.getNext(item);
  }
  dlllistmutex.leave();
  return ret;
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
BOOL Win32ImageBase::insideModule(ULONG address)
{
  //dummy
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::insideModuleCode(ULONG address)
{
  //dummy
  return FALSE;
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getImageSize()
{
  //dummy
  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::findDll(const char *szFileName, char *szFullName,
                             int cchFullFileName, const char *pszAltPath)
{
 char   modname[CCHMAXPATH];
 HFILE  dllfile = NULL;
 char  *imagepath;

    strcpy(szFullName, szFileName);
    strupr(szFullName);
    if(!strchr(szFullName, (int)'.')) {
        strcat(szFullName, DLL_EXTENSION);
    }

    //search order:
    //1) exe dir
    //2) current dir
    //3) windows system dir (kernel32 path)
    //4) windows dir
    //5) path
    if(WinExe) {
        strcpy(modname, WinExe->getFullPath());
        //remove file name from full path
        imagepath = modname + strlen(modname) - 1;
        while(*imagepath != '\\')
            imagepath--;
        imagepath[1] = 0;
        strcat(modname, szFullName);
        dllfile = OSLibDosOpen(modname, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
    }
    if(dllfile == NULL)
    {
        strcpy(modname, szFullName);
        dllfile = OSLibDosOpen(szFullName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
        if(dllfile == NULL)
        {
            strcpy(modname, InternalGetSystemDirectoryA());
            strcat(modname, "\\");
            strcat(modname, szFullName);
            dllfile = OSLibDosOpen(modname, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
            if(dllfile == NULL)
            {
                strcpy(modname, InternalGetWindowsDirectoryA());
                strcat(modname, "\\");
                strcat(modname, szFullName);
                dllfile = OSLibDosOpen(modname, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
                if(dllfile == NULL) {
                    if(OSLibDosSearchPath(OSLIB_SEARCHENV, "PATH", szFullName, modname, sizeof(modname)) == 0)
                        return FALSE;
                }
            }
        }
    }
    strcpy(szFullName, modname);
    if(dllfile) OSLibDosClose(dllfile);
    return TRUE;
}

//******************************************************************************
//returns ERROR_SUCCESS or error code (Characteristics will contain
//the Characteristics member of the file header structure)
//******************************************************************************
ULONG Win32ImageBase::isPEImage(char *szFileName, DWORD *Characteristics, DWORD *subsystem)
{
 char   filename[CCHMAXPATH];
 char  *syspath;
 HFILE  dllfile;
 IMAGE_FILE_HEADER     fh;
 IMAGE_OPTIONAL_HEADER oh;
 HFILE  win32handle;
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
 APIRET rc             = NO_ERROR;            /* Return code */
 LPVOID win32file      = NULL;
 ULONG  ulRead;
 int    nSections, i;

  if (!findDll(szFileName, filename, sizeof(filename)))
  {
        dprintf(("KERNEL32:Win32ImageBase::isPEImage(%s) findDll failed to find the file.\n",
                 szFileName, rc));
        return ERROR_FILE_NOT_FOUND_W;
  }
  rc = DosOpen(filename,                       /* File path name */
               &win32handle,                   /* File handle */
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
        return ERROR_FILE_NOT_FOUND_W;
  }

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  IMAGE_DOS_HEADER *pdoshdr = (IMAGE_DOS_HEADER *)malloc(sizeof(IMAGE_DOS_HEADER));
  if(pdoshdr == NULL)   {
        DosClose(win32handle);                /* Close the file */
        return ERROR_INVALID_EXE_SIGNATURE_W;
  }
  rc = DosRead(win32handle, pdoshdr, sizeof(IMAGE_DOS_HEADER), &ulRead);
  if(rc != NO_ERROR || ulRead != sizeof(IMAGE_DOS_HEADER)) {
        free(pdoshdr);
        DosClose(win32handle);                /* Close the file */
        return ERROR_INVALID_EXE_SIGNATURE_W;
  }
  if(pdoshdr->e_magic != IMAGE_DOS_SIGNATURE) {
        free(pdoshdr);
        DosClose(win32handle);                /* Close the file */
        return ERROR_INVALID_EXE_SIGNATURE_W;
  }
  ULONG hdrsize = pdoshdr->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER);
  free(pdoshdr);

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  win32file = malloc(hdrsize);
  if(win32file == NULL) {
        DosClose(win32handle);                /* Close the file */
        return ERROR_NOT_ENOUGH_MEMORY_W;
  }
  rc = DosRead(win32handle, win32file, hdrsize, &ulRead);
  if(rc != NO_ERROR || ulRead != hdrsize) {
        goto failure;
  }

  if(GetPEFileHeader (win32file, &fh) == FALSE) {
        goto failure;
  }
  if(GetPEOptionalHeader (win32file, &oh) == FALSE) {
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
  if(Characteristics) {
        *Characteristics = fh.Characteristics;
  }
  if(subsystem) {
        *subsystem = oh.Subsystem;
  }
  DosClose(win32handle);
  return ERROR_SUCCESS_W;

failure:
  free(win32file);
  DosClose(win32handle);
  return ERROR_INVALID_EXE_SIGNATURE_W;
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


/**
 * Matches a given filename or module name with the module name of
 * this object.
 * @returns     TRUE:   The modulenames matches.
 *              FALSE:  Don't match.
 * @param       pszFilename     Pointer to filename or module name.
 * @status      completely implemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      Just a clarification:
 *              A module name is the filename of a executable image without
 *              path, but *with* extention.
 */
BOOL Win32ImageBase::matchModName(const char *pszFilename) const
{
    const char *pszModName;             /* Pointer to the modulename. */
    const char *pszModNameEnd = NULL;   /* Pointer to the dot starting the extention. (if any) */
    register char ch;

    /** @sketch
     * Search the filename string finding the modulename start and end.
     * The loop ends when we have passed one char left of the module name.
     */
    pszModName = pszFilename + strlen(pszFilename) - 1;
    while (pszModName >= pszFilename
           && (ch = *pszModName) != '\\'
           && ch != '/'
           && ch != ':'
           )
    {
        pszModName--;
    }
    pszModName++;

    /** @sketch
     * Compare the names caseinsensitivly.
     */
    return stricmp(pszModName, szModule) == 0;
}
