/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 PE Image class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "win32type.h"
#include "misc.h"
#include "nameid.h"
#include "win32util.h"
#define INCL_WINRES
#include "winres.h"
#include "winimage.h"
#include "windll.h"

//******************************************************************************
//******************************************************************************
int Win32Image::getWin32ResourceId(int id)
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
int Win32Image::convertNameId(char *lpszName)
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
  upname = (char *)malloc(strlen(lpszName)+1);
  strcpy(upname, lpszName);
  UpCase(upname);
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
