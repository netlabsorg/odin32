/* $Id: unicode.cpp,v 1.25 2001-04-03 14:10:48 sandervl Exp $ */

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
#include "codepage.h"
#include <unicode.h>

#define DBG_LOCALLOG	DBG_unicode
#include "dbglocal.h"

/*static UconvObject uconv_object = NULL;*/
/*
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
*/
//******************************************************************************
//******************************************************************************
// unilen: length of astring buffer (including 0 terminator)
// returns string length
//******************************************************************************
int UnicodeToCodepageN(LPCWSTR ustring, char *astring, int unilen, UconvObject uconv_object)
{
  int i;
  int rc, length;
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

//  length = UniStrlen(ustring)+1;
//  unilen = min(length, unilen);

  if (uconv_object)
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

int WIN32API UnicodeToAsciiN(LPCWSTR ustring, char *astring, int unilen)
{
    return UnicodeToCodepageN(ustring, astring, unilen, GetWindowsUconvObject());
}

//******************************************************************************
// Converts unicode string to ascii string
// returns length of ascii string
//******************************************************************************
int WIN32API UnicodeToAscii(LPCWSTR ustring, char *astring)
{
  /* forward to function with len parameter */
  return UnicodeToAsciiN(ustring, astring, UniStrlen((UniChar*)ustring)+1); //end included
}
//******************************************************************************
// Converts unicode string to ascii string
// returns pointer to ascii string
//******************************************************************************
char * WIN32API UnicodeToAsciiString(LPCWSTR ustring)
{
  char *astring;

  if(ustring == NULL)  return(NULL);

  astring = (char *)malloc( 1 + UniStrlen((UniChar*)ustring) );
  UnicodeToAscii( ustring, astring );
  return(astring);
}
//******************************************************************************
// length = characters to convert
//******************************************************************************
char * WIN32API UnicodeToAsciiStringN(LPCWSTR ustring, ULONG length)
{
  char *astring;

  if(ustring == NULL)  return(NULL);

  astring = (char *)malloc( 1 + length );
  UnicodeToAsciiN( ustring, astring, length+1 ); //terminating 0 always added
  return(astring);
}
//******************************************************************************
// Converts ascii string to unicode string
// returns pointer to unicode string
//******************************************************************************
WCHAR * WIN32API AsciiToUnicodeString(const char *astring)
{
  WCHAR *ustring;

  if(astring == NULL)
    return(NULL);

  ustring = (WCHAR *)malloc( 1 + strlen(astring) << 1 );
  AsciiToUnicode( astring, ustring );
  return(ustring);
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
void CodepageToUnicodeN(const char *ascii, WCHAR *unicode, int asciilen, UconvObject uconv_object)
{
  int rc;
  int i;
  size_t uni_chars_left;
  size_t in_bytes_left;
  size_t num_subs;
  UniChar     * out_buf;
  const char  * in_buf;


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
  if (uconv_object)
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
    dprintf2(("KERNEL32: AsciiToUnicode %d\n", in_bytes_left));

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

void WIN32API AsciiToUnicodeN(const char *ascii, WCHAR *unicode, int asciilen)
{
    CodepageToUnicodeN(ascii, unicode, asciilen, GetWindowsUconvObject());
}


//******************************************************************************
// Copies the full string from ascii to unicode
//******************************************************************************
void WIN32API AsciiToUnicode(const char *ascii, WCHAR *unicode)
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





