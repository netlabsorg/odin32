/* $Id: winimagepe2lx.cpp,v 1.1 1999-09-15 23:39:08 sandervl Exp $ */

/*
 * Win32 PE2LX Image base class
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
#include <winimagepe2lx.h>
#include <windllbase.h>
#include <winexebase.h>
#include <winexepe2lx.h>
#include <pefile.h>
#include <unicode.h>
#include <winres.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>

//******************************************************************************
//******************************************************************************
Win32Pe2LxImage::Win32Pe2LxImage(HINSTANCE hinstance, int NameTableId, int Win32TableId) 
               : Win32ImageBase(hinstance),
                 NameTable(NULL), Win32Table(NULL), VersionId(0)
{
 APIRET rc;

  szFileName[0] = 0;

  char *name = OSLibGetDllName(hinstance);
  strcpy(szFileName, name);
  strupr(szFileName);
  char *dot = strstr(szFileName, ".");
  while(dot) {
	char *newdot = strstr(dot+1, ".");
	if(newdot == NULL)	break;
	dot = newdot;
  }
  if(dot)
	*dot = 0;

  this->NameTableId  = NameTableId;
  this->Win32TableId = Win32TableId;

  if(NameTableId != NO_NAMETABLE) {
    //Load name table resource
    rc = DosGetResource(hinstance, RT_RCDATA, NameTableId, (PPVOID)&NameTable);
    if(rc) {
        eprintf(("Can't find converted name resource (rc %d)!!\n", rc));
        return;
    }
  }
  else  this->NameTableId = 0;

  //Load win32 id table resource
  if((Win32TableId & 0xFFFFFF) != NO_LOOKUPTABLE) {
    rc = DosGetResource(hinstance, RT_RCDATA, Win32TableId, (PPVOID)&Win32Table);
    if(rc) {
        eprintf(("Can't find win32 id resource (rc %d)!!\n", rc));
        return;
    }
  }
  else  this->Win32TableId = 0;

}
//******************************************************************************
//******************************************************************************
Win32Pe2LxImage::~Win32Pe2LxImage()
{
 Win32Resource *res;

  if(NameTable)
    	DosFreeResource((PVOID)NameTable);

  if(Win32Table)
    	DosFreeResource((PVOID)Win32Table);
}
//******************************************************************************
//******************************************************************************
int Win32Pe2LxImage::getWin32ResourceId(int id)
{
 int         nrres, i;

  if(Win32Table == NULL)
    return(-1);

  nrres = *Win32Table;
  for(i=1;i<=nrres;i++) {
    if(id == (Win32Table[i] >> 16)) {
        dprintf(("OS/2 id -> Win32 id = %d -> %d\n", id, Win32Table[i] & 0xFFFF));
        return(Win32Table[i] & 0xFFFF);
    }
  }
  dprintf(("Original resource not found!!\n"));
  return(-1);
}
//******************************************************************************
//******************************************************************************
int Win32Pe2LxImage::convertNameId(char *lpszName)
{
 NameId      *curnid;
 int          nrcvtnames, i;
 char        *upname;

  if(NameTable == NULL)
    return(0);

  nrcvtnames = *(USHORT *)NameTable;
  curnid     = (NameId *)((int)NameTable + sizeof(USHORT));;
  for(i=0;i<nrcvtnames;i++) {
    if(strcmp(lpszName, curnid->name) == 0) {
        return(curnid->id);
    }
    curnid = (NameId *)((char *)curnid + sizeof(NameId) + strlen(curnid->name));
  }

  //try upper case search
  //SvL: Copy it, since string might be located in readonly object

  upname = (char *)malloc(strlen(lpszName)+1); //CB: Trap with MFC Toolbar/UpDown samples
  strcpy(upname, lpszName);
  strupr(upname);
  dprintf(("Convert %s to id\n", upname));
  curnid     = (NameId *)((int)NameTable + sizeof(USHORT));;
  for(i=0;i<nrcvtnames;i++) {
    if(strcmp(upname, curnid->name) == 0) {
        free(upname);
        return(curnid->id);
    }
    curnid = (NameId *)((char *)curnid + sizeof(NameId) + strlen(curnid->name));
  }
  dprintf(("Converted name NOT found!\n"));
  free(upname);

  return(0);
}
//******************************************************************************
//******************************************************************************

