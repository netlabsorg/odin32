/* $Id: misc.cpp,v 1.5 1999-07-06 08:50:11 sandervl Exp $ */

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
#include <uniconv.h>

void convertCP(int cp, char *str);

//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(int length, WCHAR *NameString, int cp)
{
static char asciistring[256];
int i;

  if(length >= 255)     length = 255;
  for(i=0;i<length;i++) {
        asciistring[i] = NameString[i] & 0xFF;
  }
  asciistring[length] = 0;

  convertCP(cp, asciistring);
  return(asciistring);
}

//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(WCHAR *wstring, int cp)
{
 static char astring[512];
 int i;

  memset(astring, 0, sizeof(astring));

  for(i=0;i<=UniStrlen(wstring);i++) { //including 0 terminator
        astring[i] = (UCHAR)wstring[i];
  }
  convertCP(cp, astring);
  return(astring);
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
char transCP[256];
int CodePage=-1;
//******************************************************************************
BOOL prepareCP(int cp)
{
  #define SIZE_ucs_code_page 20

  static  UconvObject uconv_objFrom = NULL;
  static  UconvObject uconv_objTo = NULL;

  UniChar ucs_code_page[SIZE_ucs_code_page];
  int     rc, i, j;
  BOOL    ret;
  size_t  uni_chars_left;
  size_t  in_bytes_left;
  size_t  num_subs;
  UniChar *pout_uni_str;
  char    *pin_char_str;
  char    table[256];
  UniChar fromCP[256];
  UniChar toCP[256];

  if(cp != 0)
  {
    if(uconv_objTo == NULL)
    {
      rc = UniCreateUconvObject( (UniChar*)L"", &uconv_objTo);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniCreateUconvObject, return code = %u\n", rc);
        return FALSE;
      }
    }

    if(cp != CodePage)
    {
      if(uconv_objFrom != NULL)
      {
        rc = UniFreeUconvObject(uconv_objFrom);
        if (rc != ULS_SUCCESS)
        {
          printf("ERROR UniFreeUconvObject error: return code = %u\n", rc);
          return FALSE;
        }
      }
      rc = UniMapCpToUcsCp(cp, ucs_code_page, SIZE_ucs_code_page);
      if (rc != ULS_SUCCESS)
      {
        printf("ERROR couldn't translate Codepage ID to unistring, return code = %u\n", rc);
        return FALSE;
      }
      rc = UniCreateUconvObject( ucs_code_page, &uconv_objFrom);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniCreateUconvObject, return code = %u\n", rc);
        return FALSE;
      }
      for(i=0;i<256;i++)
        table[i] = i;

      in_bytes_left = 256;
      uni_chars_left = 256;
      pout_uni_str = fromCP;
      pin_char_str = table;
      rc = UniUconvToUcs(uconv_objFrom, (void**)&pin_char_str, &in_bytes_left,
                         &pout_uni_str, &uni_chars_left, &num_subs);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniUconvToUcs, return code = %u\n", rc);
        return FALSE;
      }

      in_bytes_left = 256;
      uni_chars_left = 256;
      pout_uni_str = toCP;
      pin_char_str = table;
      rc = UniUconvToUcs(uconv_objTo, (void**)&pin_char_str, &in_bytes_left,
                         &pout_uni_str, &uni_chars_left, &num_subs);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniUconvToUcs, return code = %u\n", rc);
        return FALSE;
      }

      CodePage = cp;

      for(i=0;i<256;i++)
      {
        transCP[i] = i;
        for(j=0;j<256;j++)
        {
          if(fromCP[i] == toCP[j])
          {
            transCP[i] = j;
            break;
          }
        }
      }
    }
  }
  else	return FALSE;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
void convertCP(int cp, char *str)
{
  if(str == NULL)
    return;

  if(prepareCP(cp) == TRUE && CodePage > 0)
    while(*str != 0)
    {
      *str = transCP[*str];
      str++;
    }
}
//******************************************************************************
//******************************************************************************
