/* $Id: unicode.cpp,v 1.1 1999-05-24 20:19:43 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Unicode functions
 *
 * Copyright 1998 Joel Troster
 *
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
//******************************************************************************
int WIN32API UnicodeToAscii(WCHAR *ustring, char *astring)
{
  int ulen, i;
  int rc;
  size_t uni_chars_left;
  size_t out_bytes_left;
  size_t num_subs;
  UniChar * in_buf;
  char    * out_buf;

  if(ustring == NULL)
    return(NULL);

//  dprintf(("KERNEL32: UnicodeToAscii\n"));
  ulen = UniStrlen((UniChar*)ustring);
  if ( getUconvObject() )
  {
    uni_chars_left = ulen + 1;
    out_bytes_left = uni_chars_left;
    in_buf  = (UniChar*)ustring;
    out_buf = astring;
    rc = UniUconvFromUcs( uconv_object,
                          &in_buf,  &uni_chars_left,
                          (void**)&out_buf, &out_bytes_left,
                          &num_subs );
//    dprintf(("KERNEL32: UnicodeToAscii(%d) '%s'\n", rc, astring ));
  }
  else
  {
   for(i=0;i<ulen;i++) {
        astring[i] = (char)ustring[i];
   }
   astring[ulen] = 0;
  }
  return(ulen);
}
//******************************************************************************
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
//******************************************************************************
void WIN32API AsciiToUnicode(char *ascii, WCHAR *unicode)
{
  int rc;
  int i, len;
  size_t uni_chars_left;
  size_t in_bytes_left;
  size_t num_subs;
  UniChar * out_buf;
  char    * in_buf;


  dprintf(("KERNEL32: AsciiToUnicode(%s,%08xh)\n",
           ascii,
           unicode));

  /* @@@PH 98/06/07 */
  if ( (ascii   == NULL) ||                     /* garbage in, garbage out ! */
       (unicode == NULL) )
    return;

  /* determine length of ascii string */
  len = strlen(ascii);

//  dprintf(("KERNEL32: AsciiToUnicode %s\n", ascii));
  if ( getUconvObject() )
  {
    in_buf        = ascii;
    in_bytes_left = len;
    out_buf = (UniChar*)unicode;

    uni_chars_left = in_bytes_left +1;
    dprintf(("KERNEL32: AsciiToUnicode %d\n", in_bytes_left));

    rc = UniUconvToUcs( uconv_object,
                        (void**)&in_buf, &in_bytes_left,
                        &out_buf,        &uni_chars_left,
                        &num_subs );
  }
  else
  {
    for(i=0;
        i<len;
        i++)
      unicode[i] = ascii[i];
  }

  unicode[len] = 0;
//SvL: Messes up the heap
//  unicode[len+1] = 0; /* @@@PH 98/06/07 */
}
//******************************************************************************
//TODO: use OS/2 unicode stuff
//******************************************************************************
char *UnicodeToAscii(int length, WCHAR *NameString)
{
static char asciistring[256];
int i;

  if(length >= 255) length = 255;
  for(i=0;i<length;i++) {
    asciistring[i] = NameString[i] & 0xFF;
  }
  asciistring[length] = 0;
  return(asciistring);
}
//******************************************************************************
//******************************************************************************
