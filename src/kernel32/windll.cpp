/* $Id: windll.cpp,v 1.8 1999-07-07 08:42:48 sandervl Exp $ */

/*
 * Win32 DLL class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <nameid.h>
#include <win32type.h>
#include <pefile.h>
#include <windll.h>
#include <wprocess.h>
#include "cio.h"
#include "os2util.h"

/***********************************
 * PH: fixups for missing os2win.h *
 ***********************************/

void _System SetLastError(ULONG ulError);

//******************************************************************************
//******************************************************************************
Win32Dll::Win32Dll(char *szDllName) : Win32Image(szDllName), referenced(0), 
				      fSkipEntryCalls(FALSE), fSystemDll(FALSE)
{
  fSystemDll   = isSystemDll(szFileName);
  fUnloaded    = FALSE;
  next = head;
  head = this;

  strcpy(szModule, StripPath(szFileName));
  UpCase(szModule);
  char *dot = strstr(szModule, ".");
  while(dot) {
	char *newdot = strstr(dot+1, ".");
	if(newdot == NULL)	break;
	dot = newdot;
  }
  if(dot)
	*dot = 0;
  dprintf(("Win32Dll::Win32Dll %s %s", szFileName, szModule));
}
//******************************************************************************
//******************************************************************************
Win32Dll::Win32Dll(HINSTANCE hinstance, int NameTableId, int Win32TableId,
		   WIN32DLLENTRY DllEntryPoint) : 
			Win32Image(hinstance, NameTableId, Win32TableId),
			referenced(0), fSkipEntryCalls(FALSE), fSystemDll(FALSE)
{
  dllEntryPoint = DllEntryPoint;
  fUnloaded     = FALSE;
  next = head;
  head = this;

  char *name = OS2GetDllName(hinstance);
  strcpy(szModule, name);
  UpCase(szModule);
  char *dot = strstr(szModule, ".");
  while(dot) {
	char *newdot = strstr(dot+1, ".");
	if(newdot == NULL)	break;
	dot = newdot;
  }
  if(dot)
	*dot = 0;

  dprintf(("Win32Dll::Win32Dll %s", szModule));
}
//******************************************************************************
//******************************************************************************
void Win32Dll::OS2DllInit(HINSTANCE hinstance, int NameTableId, int Win32TableId,
  	      	          WIN32DLLENTRY DllEntryPoint)
{
  dllEntryPoint = DllEntryPoint;
  fUnloaded     = FALSE;

  char *name = OS2GetDllName(hinstance);
  strcpy(szModule, name);
  UpCase(szModule);
  char *dot = strstr(szModule, ".");
  while(dot) {
	char *newdot = strstr(dot+1, ".");
	if(newdot == NULL)	break;
	dot = newdot;
  }
  if(dot)
	*dot = 0;

  Win32Image::OS2ImageInit(hinstance, NameTableId, Win32TableId);
}
//******************************************************************************
//******************************************************************************
Win32Dll::~Win32Dll()
{
 Win32Dll *dll = head;

  dprintf(("Win32Dll::~Win32Dll %s", szModule));

  //first remove it from the linked list so converted win32 dlls won't
  //be deleted twice (as DosFreeModule results in a call to DllExitList (wprocess.cpp)
  if(head == this) {
	head = next;
  }
  else {
	while(dll && dll->next != this) {
		dll = dll->next;
        }
	if(dll == NULL) {
		dprintf(("~Win32Dll: Can't find dll!\n"));
		return;
	}
	dll->next = next;
  }
  if(errorState == NO_ERROR && !fUnloaded) 
  {
	detachProcess();
  }
}
//******************************************************************************
//ASSUMPTION: called by FreeLibrary
//******************************************************************************
ULONG Win32Dll::Release()
{ 
 ULONG ret = --referenced;

  if(ret == 0) {
	dprintf(("Win32Dll::Release, referenced == 0\n"));
	if(fSystemDll) {
	 	DosFreeModule((HMODULE)hinstance);
		fUnloaded = TRUE;
	}
	delete this;
  }
  return(ret);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dll::init()
{
 char  *syspath;
 FILE  *dllfile;
 APIRET rc;
 BOOL   fRet;

  if(fSystemDll == FALSE && fNativePEImage == TRUE) 
  {//might be win32 dll, so check this
	dllfile = fopen(szFileName, "r");
	if(dllfile == NULL) {//search in libpath for dll
		syspath = getenv("WIN32LIBPATH");
		if(syspath) {
			strcpy(szModule, syspath);
			if(szModule[strlen(szModule)-1] != '\\') {
				strcat(szModule, "\\");
			}
			strcat(szModule, szFileName);
			szFileName = szModule;
		}
	}
	else	fclose(dllfile);
	if(isPEImage(szFileName) == TRUE) {
		fRet = Win32Image::init();
		dllEntryPoint = (WIN32DLLENTRY)entryPoint;
		return fRet;
	}
	//converted win32 dll
	fNativePEImage = FALSE;
  }
  //else load system dlls using DosLoadModule
  char szModuleFailure[CCHMAXPATH] = "";

  rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), szFileName, (HMODULE *)&hinstance);
  if(rc) {
	dprintf(("DosLoadModule returned %X for %s\n", rc, szModuleFailure));
	errorState = rc;
	return(FALSE);
  }
  return(TRUE);
}
//******************************************************************************
char *sysmodules[] = {"KERNEL32", "USER32", "GDI32", "WINMM", "DSOUND", "DDRAW",
		      "WNETAP32", "MPR", "OLE32", "ADVAPI32", "COMDLG32", 
		      "WINSPOOL", "SHELL32", "TAPI32", "CAPI32", "VERSION",
		      "WSOCK32", "COMCTL32", "NTDLL"};
//******************************************************************************
BOOL Win32Dll::isSystemDll(char *szFileName)
{
 int i;
 char *dot;
 char szModule[CCHMAXPATH];

  strcpy(szModule, szFileName);
  UpCase(szModule);
  dot = strstr(szModule, ".");
  if(dot)
	*dot = 0;
 
  for(i=0;i<sizeof(sysmodules)/sizeof(char *);i++) {
	if(!strcmp(szModule, sysmodules[i]))
		return(TRUE);
  }
  return(FALSE);
}
//******************************************************************************
//Could be more efficient, but I bet this is faster than the OS/2 loader!
//******************************************************************************
ULONG Win32Dll::getApi(char *name)
{
  APIRET      rc;
  ULONG       apiaddr, i, apilen;
  char       *apiname;
  char        tmp[4];
  NameExport *curexport;
  ULONG       ulAPIOrdinal;                      /* api requested by ordinal */
  
  apilen = strlen(name) + 1;
  if(apilen < 4) 
  {
	*(ULONG *)tmp = 0;
	strcpy(tmp, name);
	apiname = tmp;
  }
  else	apiname = name;

  curexport = nameexports;
  for(i=0;
      i<nrNameExports;
      i++) 
  {
    if(apilen == curexport->nlength && 
       *(ULONG *)curexport->name == *(ULONG *)name) 
    {
      if(strcmp(curexport->name, name) == 0) 
        return(curexport->virtaddr);
    }
    curexport = (NameExport *)((ULONG)curexport->name + curexport->nlength);
  }
  
  if(fSystemDll || !fNativePEImage) 
  {
    rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
    if(rc)  
    {
      SetLastError(rc);                             /* raise error condition */
      return(0);
    }

    //Add to lookup table, so we don't have to call DosQueryProcAddr
    //multiple times for the same api (when imported by other dlls)
    AddNameExport(apiaddr, name, 0);
    return(apiaddr);
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Dll::getApi(int ordinal)
{
 APIRET      rc;
 ULONG       apiaddr, i;
 OrdExport  *curexport;
 NameExport *nexport;

  if(fSystemDll || !fNativePEImage) {
	rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
	if(rc)  return(0);
	else	return(apiaddr);
  }
  curexport = ordexports;
  for(i=0;i<nrOrdExports;i++) {
	if(curexport->ordinal == ordinal)
		return(curexport->virtaddr);
	curexport++;
  }
  //Name exports also contain an ordinal, so check this 
  nexport = nameexports;
  for(i=0;i<nrNameExports;i++) {
	if(nexport->ordinal == ordinal)
		return(nexport->virtaddr);
	nexport = (NameExport *)((ULONG)nexport->name + nexport->nlength);
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dll::attachProcess()
{
 BOOL rc;
 USHORT sel;

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  if(fSystemDll || fSkipEntryCalls) {
        dprintf(("attachProcess not required for dll %s", szModule));
	return(TRUE);
  }

  dprintf(("attachProcess to dll %s", szModule));

  sel = SetWin32TIB();
  rc = dllEntryPoint(hinstance, DLL_PROCESS_ATTACH, 0);
  SetFS(sel);
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dll::detachProcess()
{
 BOOL   rc;
 USHORT sel;

  if(fSystemDll || fSkipEntryCalls) {
        tlsDetachThread();	//destroy TLS (main thread)
	return(TRUE);
  }

  dprintf(("detachProcess from dll %s", szModule));

  fUnloaded = TRUE;
  sel = SetWin32TIB();
  rc = dllEntryPoint(hinstance, DLL_PROCESS_DETACH, 0);
  SetFS(sel);
  tlsDetachThread();	//destroy TLS (main thread)
  tlsDelete();
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dll::attachThread()
{
  if(fSystemDll || fSkipEntryCalls)
	return(TRUE);

  dprintf(("attachThread to dll %s", szModule));
  return dllEntryPoint(hinstance, DLL_THREAD_ATTACH, 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dll::detachThread()
{
  if(fSystemDll || fSkipEntryCalls)
	return(TRUE);

  dprintf(("attachThread from dll %s", szModule));
  return dllEntryPoint(hinstance, DLL_THREAD_DETACH, 0);
}
//******************************************************************************
//Send DLL_THREAD_ATTACH message to all dlls for a new thread
//******************************************************************************
void Win32Dll::attachThreadToAllDlls()
{
 Win32Dll *dll = Win32Dll::head;

  while(dll) {
	dll->attachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Send DLL_THREAD_DETACH message to all dlls for thread that's about to die
//******************************************************************************
void Win32Dll::detachThreadFromAllDlls()
{
 Win32Dll *dll = Win32Dll::head;

  while(dll) {
	dll->detachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Setup TLS structure for all dlls for a new thread
//******************************************************************************
void Win32Dll::tlsAttachThreadToAllDlls()
{
 Win32Dll *dll = Win32Dll::head;

  while(dll) {
	dll->tlsAttachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Destroy TLS structure for all dlls for a thread that's about to die
//******************************************************************************
void Win32Dll::tlsDetachThreadFromAllDlls()
{
 Win32Dll *dll = Win32Dll::head;

  while(dll) {
	dll->tlsDetachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//******************************************************************************
void Win32Dll::deleteAll()
{
  //LIFO removal
  while(Win32Dll::head) {
	delete Win32Dll::head;
  }
}
//******************************************************************************
//******************************************************************************
Win32Dll *Win32Dll::findModule(char *dllname)
{
 Win32Dll *dll = head;
 char szDllName[CCHMAXPATH];
 char *dot;

  dprintf(("findModule %s", dllname));
  strcpy(szDllName, dllname);
  UpCase(szDllName);
  dot = strstr(szDllName, ".");
  if(dot)
	*dot = 0;

  while(dll) {
	if(strcmpi(szDllName, dll->szModule) == 0)
		return(dll);

	dll = dll->next;
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32Dll *Win32Dll::findModule(WIN32DLLENTRY DllEntryPoint)
{
 Win32Dll *mod = Win32Dll::head;

   dprintf(("findModule %X", DllEntryPoint));
   while(mod != NULL) {
	dbgCheckObj(mod);
	if(mod->dllEntryPoint == DllEntryPoint)
		return(mod);
	mod = mod->next;
   }
   return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32Dll *Win32Dll::findModule(HINSTANCE hinstance)
{
 Win32Dll *mod = Win32Dll::head;

//   eprintf(("findModule inst %X", hinstance));
   while(mod != NULL) {
	dbgCheckObj(mod);
	if(mod->hinstance == hinstance)
		return(mod);
	mod = mod->next;
   }
   return(NULL);
}
//******************************************************************************
//******************************************************************************
char *Win32Dll::StripPath(char *path)
{
  /* @@@PH what does this function do ? Strip the path from a FQFN name ? */
  char *pszFilename;
  
  pszFilename = strrchr(path, '\\');                 /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  pszFilename = strrchr(path, '/');                  /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  return (path);                                     /* default return value */
  
#if 0
  char *fname = path, *prevname = path-1;
  
  while(TRUE)
  {
    @@@PH causes endless loop here !
    fname = strchr(fname, '\\');
    if(fname == NULL)
      break;
    
    prevname = fname;
  }
  return(prevname+1);
#endif
}
/******************************************************************************/
/******************************************************************************/
Win32Dll *Win32Dll::head = NULL;
