/* $Id: unicode.cpp,v 1.11 1999-06-30 21:19:42 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Unicode functions
 * Copyright 1998 Joel Troster
 * Copyright 1999 Patrick haller
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * Read comments about the implementation before using these functions
 * Attention: length values include terminating 0
 */
#include <os2win.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"

static UconvObject uconv_object = NULL;
BOOL getUconvObject( void )
{
  int rc;
  BOOL ret;
  if ( uconv_object )
    ret = TRUE;
  else
  {
    rc = UniCreateUconvObject( (UniChar*)L"", &uconv_object );
    if ( rc == ULS_SUCCESS )
      ret = TRUE;
    else
    {
      uconv_object = NULL;  // to make sure
      return FALSE;
    }
    dprintf(("UniCreateUconvObject(%d)\n", rc ));
  }
  return ret;
}

//******************************************************************************
//Not identical, but close enough
//******************************************************************************
int WIN32API MultiByteToWideChar(UINT uCodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
                                   int  cchMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
 int i, len;

//  dprintf(("MultiByteToWideChar %s\n", lpMultiByteStr));

  if((int)lpMultiByteStr == (int)lpWideCharStr) {//not allowed
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  if(cchWideChar == 0) {//return space required for conversion
        if(cchMultiByte == -1)  cchMultiByte = strlen(lpMultiByteStr);
        return(cchMultiByte);   //return length in wide chars
  }
  if(cchMultiByte == -1)
        cchMultiByte = strlen(lpMultiByteStr);

  len = min(cchWideChar, cchMultiByte);
  for(i=0;i<=len;i++) { //including 0 terminator
        lpWideCharStr[i] = lpMultiByteStr[i];
  }
  return(len);
}
//******************************************************************************
//******************************************************************************
//Not identical, close enough
//Forget about characters that can't be mapped; just do it
//******************************************************************************
int WIN32API WideCharToMultiByte(UINT uCodePage, DWORD dwFlags, LPCWSTR lpWideCharStr,
                                   int cchWideChar, LPSTR lpMultiByteStr,
                                   int cchMultiByte, LPCSTR lpDefaultChar,
                                   LPBOOL lpfUsedDefaultChar)
{
 int i, len;

//  dprintf(("WideCharToMultiByte\n"));

  if((int)lpMultiByteStr == (int)lpWideCharStr) {//not allowed
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  if(cchMultiByte == 0) {//return space required for conversion
        if(cchWideChar == -1)   cchWideChar = UniStrlen((UniChar *)lpWideCharStr);
        return(cchWideChar);
  }
  if(cchWideChar == -1)
        cchWideChar = UniStrlen((UniChar*)lpWideCharStr);

  len = min(cchWideChar, cchMultiByte);
  for(i=0;i<=len;i++) { //including 0 terminator
        lpMultiByteStr[i] = (UCHAR)lpWideCharStr[i];
  }
  return(len);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCPInfo(UINT uCodePage, CPINFO *lpCPInfo)
{
  dprintf(("GetCPInfo (%d), not correctly implemented\n", uCodePage));
  memset(lpCPInfo, 0, sizeof(CPINFO));
  lpCPInfo->MaxCharSize    = 1;
  lpCPInfo->DefaultChar[0] = 'a';

  return(TRUE);
}
//******************************************************************************
// unilen: length of astring buffer (including 0 terminator)
// returns string length
//******************************************************************************
int WIN32API UnicodeToAsciiN(WCHAR *ustring, char *astring, int unilen)
{
  int i;
  int rc;
  size_t uni_chars_left;
  size_t out_bytes_left;
  size_t num_subs;
  UniChar * in_buf;
  char    * out_buf;

  if (ustring == NULL)
  {
    if (astring != NULL && unilen > 0) astring[0] = 0;
    return 0;
  }

  if (astring == NULL || unilen <= 0) return 0;

//  dprintf(("KERNEL32: UnicodeToAsciiN\n"));
  if (getUconvObject())
  {
    if (unilen == 1)
    {
      astring[0] = 0;
      return 0; //no data
    }

    uni_chars_left = unilen-1; //elements
    out_bytes_left = uni_chars_left; //size in bytes == elements
    in_buf  = (UniChar*)ustring;
    out_buf = astring;
    rc = UniUconvFromUcs(uconv_object,
                         &in_buf, &uni_chars_left,
                         (void**)&out_buf, &out_bytes_left,
                         &num_subs);

    unilen -= 1+out_bytes_left; //end + left bytes
    astring[unilen] = 0; //terminate

    return unilen;

//    dprintf(("KERNEL32: UnicodeToAsciiN(%d) '%s'\n", rc, astring ));
  } else
  {
    /* idiots unicode conversion :) */
    for (i = 0; i < unilen-1; i++)
    {
      astring[i] = (ustring[i] > 255) ? (char)20 : (char)ustring[i]; //CB: handle invalid characters as space
      if (ustring[i] == 0) return i; //asta la vista, baby
    }

    astring[unilen-1] = 0; // @@@PH: 1999/06/09 fix - always terminate string

    return(unilen-1);
  }
}
//******************************************************************************
// Converts unicode string to ascii string
// returns length of ascii string
//******************************************************************************
int WIN32API UnicodeToAscii(WCHAR *ustring, char *astring)
{
  /* forward to function with len parameter */
  return UnicodeToAsciiN(ustring, astring, UniStrlen((UniChar*)ustring)+1); //end included
}
//******************************************************************************
// Converts unicode string to ascii string
// returns pointer to ascii string
//******************************************************************************
char * WIN32API UnicodeToAsciiString(WCHAR *ustring)
{
  char *astring;

  if(ustring == NULL)  return(NULL);

  astring = (char *)malloc( 1 + UniStrlen((UniChar*)ustring) );
  UnicodeToAscii( ustring, astring );
  return(astring);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
void WIN32API FreeAsciiString(char *astring)
{
  if( astring )
    free( astring );
}
//******************************************************************************
// asciilen: max length of unicode buffer (including end 0)
//******************************************************************************
void WIN32API AsciiToUnicodeN(char *ascii, WCHAR *unicode, int asciilen)
{
  int rc;
  int i;
  size_t uni_chars_left;
  size_t in_bytes_left;
  size_t num_subs;
  UniChar * out_buf;
  char    * in_buf;


  dprintf(("KERNEL32: AsciiToUnicodeN(%s,%08xh)\n",
           ascii,
           unicode));

  //CB: no input, set at least terminator
  if (ascii == NULL)
  {
    if (unicode != NULL && asciilen > 0) unicode[0] = 0;
    return;
  }

  if (unicode == NULL || asciilen <= 0) return; //nothing to do

//  dprintf(("KERNEL32: AsciiToUnicodeN %s\n", ascii));
  if (getUconvObject())
  {
    if (asciilen == 1)
    {
       unicode[0] = 0;
       return;
    }

    in_buf        = ascii;
    in_bytes_left = asciilen-1; //buffer size in bytes
    out_buf = (UniChar*)unicode;

    uni_chars_left = in_bytes_left; //elements
    dprintf(("KERNEL32: AsciiToUnicode %d\n", in_bytes_left));

    rc = UniUconvToUcs( uconv_object,
                        (void**)&in_buf, &in_bytes_left,
                        &out_buf,        &uni_chars_left,
                        &num_subs );

    unicode[asciilen-1-in_bytes_left] = 0;

    //if (rc != ULS_SUCCESS && in_bytes_left > 0) //CB: never the case during my tests
    //   dprintf(("KERNEL32: AsciiToUnicode failed, %d bytes left!\n",in_bytes_left));

  } else
  { //poor man's conversion

    for(i = 0;i < asciilen-1;i++)
    {
      unicode[i] = ascii[i];
      if (ascii[i] == 0) return; //work done
    }

    unicode[asciilen-1] = 0;
  }
}
//******************************************************************************
// Copies the full string from ascii to unicode
//******************************************************************************
void WIN32API AsciiToUnicode(char *ascii, WCHAR *unicode)
{
  /* achimha for security, strlen might trap if garbage in */
  /* @@@PH 98/06/07 */
  if (ascii == NULL)
  {
    if (unicode != NULL) unicode[0] = 0; //CB: set at least end
    return;
  }

  if (unicode == NULL) return;  /* garbage in, garbage out ! */

  /* forward to call with length parameter */
  AsciiToUnicodeN(ascii, unicode, strlen(ascii)+1); //end included
}




