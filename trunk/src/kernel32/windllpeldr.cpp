/* $Id: windllpeldr.cpp,v 1.3 1999-11-24 19:31:23 sandervl Exp $ */

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
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
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


//******************************************************************************
//******************************************************************************
Win32PeLdrDll::Win32PeLdrDll(char *szDllName, Win32ImageBase *parentImage) 
                : Win32ImageBase(-1), 
                  Win32DllBase(-1, 0), 
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
 	strcat(szFileName,".DLL");
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
  else	OSLibDosClose(dllfile);
  fRet = Win32PeLdrImage::init(0);
  dllEntryPoint = (WIN32DLLENTRY)entryPoint;
  return fRet;
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrDll::getApi(char *name)
{
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
  for(i=0; i<nrNameExports; i++) 
  {
    if(apilen == curexport->nlength && 
       *(ULONG *)curexport->name == *(ULONG *)name) 
    {
      	if(strcmp(curexport->name, name) == 0) 
        	return(curexport->virtaddr);
    }
    curexport = (NameExport *)((ULONG)curexport->name + curexport->nlength);
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrDll::getApi(int ordinal)
{
 ULONG       apiaddr, i;
 OrdExport  *curexport;
 NameExport *nexport;

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
BOOL Win32PeLdrDll::isLxDll()
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
