/*
 * PE2LX ascii to unicode conversion
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
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "misc.h"
#include <versionos2.h>    /*PLF Wed  98-03-18 01:47:26*/


//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(int length, WCHAR *NameString)
{
static char asciistring[256];
int i;

  if(length >= 255)     length = 255;
  for(i=0;i<length;i++) {
        asciistring[i] = NameString[i] & 0xFF;
  }
  asciistring[length] = 0;
  return(asciistring);
}
//******************************************************************************
//******************************************************************************
int UniStrlen(WCHAR *wstring)
{
 int i = 0;

  while(wstring[i] != 0)        i++;
  return(i);
}
//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(WCHAR *wstring)
{
 static char astring[512];
 int i;

  memset(astring, 0, sizeof(astring));

  for(i=0;i<=UniStrlen(wstring);i++) { //including 0 terminator
        astring[i] = (UCHAR)wstring[i];
  }
  return(astring);
}
//******************************************************************************
//******************************************************************************
void UpCase(char *mixedcase)
{
 int i;

  for(i=0;i<strlen(mixedcase);i++) {
        if(mixedcase[i] >= 'a' && mixedcase[i] <= 'z') {
                mixedcase[i] += 'A' - 'a';
        }
  }
}
//******************************************************************************
//******************************************************************************
