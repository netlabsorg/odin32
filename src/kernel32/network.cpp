/*
 * Win32 Network apis
 *
 * Copyright 1998 Peter Fitzsimmons
 *           1999 Przemyslaw Dobrowolski
 *
 */
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerNameA(LPSTR name, LPDWORD size)
{
  char *szHostname;
  char szDefault[]="NONAME";
  int  cbSize;
  
  szHostname=getenv("HOSTNAME");

  if (!szHostname) szHostname=szDefault;
     
  if (name) strncpy(name,szHostname,*size);

  *size=strlen(name);  

  dprintf(("KERNEL32 GetComputerNameA: %s (size %d)",name,*size));

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerNameW(LPWSTR name, LPDWORD size)
{
  LPSTR nameA = NULL;
  BOOL  ret;

  if (name) nameA=(LPSTR)malloc(2**size);

  ret = GetComputerNameA(nameA,size);

  if (ret) AsciiToUnicode(nameA,name);

  free(nameA);

  return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerName32W(LPWSTR name, LPDWORD size)
{
  LPSTR nameA = NULL;
  BOOL  ret;

  if (name) nameA=(LPSTR)malloc(2**size);

  ret = GetComputerNameA(nameA,size);

  if (ret) AsciiToUnicode(nameA, name);

  free(nameA);

  return ret;
}
//******************************************************************************
//******************************************************************************
