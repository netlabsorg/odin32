/* $Id */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 compatibility string functions for OS/2
 *
 * Copyright 1999 Patrick Haller
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <winnls.h>
#include <unicode.h>
#include "heap.h"
#include "heapstring.h"
#include "misc.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

static UconvObject uconv_object = NULL;

static BOOL getUconvObject( void )
{
  int rc;
  BOOL ret;

  if ( uconv_object )
    ret = TRUE;
  else
  {
    rc = UniCreateUconvObject( (UniChar*)L"",
                              &uconv_object );
    if ( rc == ULS_SUCCESS )
      ret = TRUE;
    else
    {
      uconv_object = NULL;  // to make sure
      return FALSE;
    }
    dprintf(("KERNEL32: HeapString: UniCreateUconvObject(%d)\n",
             rc));
  }
  return ret;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrlenA(LPCSTR arg1)
{
  dprintf(("KERNEL32: lstrlenA(%s)\n",
           arg1));

  return O32_lstrlen(arg1);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrlenW(LPCWSTR arg1)
{
  int rc;

  rc = UniStrlen( (UniChar*)arg1);
  dprintf(("KERNEL32: lstrlenW(%08xh) returns %d\n",
           arg1,
           rc));
  return rc;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR WIN32API lstrcatA(LPSTR arg1, LPCSTR arg2)
{
  dprintf(("KERNEL32: lstrcat(%s,%s)\n",
           arg1,
           arg2));

  return O32_lstrcat(arg1, arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR WIN32API lstrcatW(LPWSTR arg1, LPCWSTR arg2)
{
  dprintf(("KERNEL32: OS2lstrcatW(%08xh,%08xh)\n",
           arg1,
           arg2));

  UniStrcat( (UniChar*) arg1, (UniChar*) arg2 );
  return arg1;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrcmpA(LPCSTR arg1, LPCSTR  arg2)
{
  dprintf(("KERNEL32: OS2lstrcmpA(%s,%s)\n",
           arg1,
           arg2));

  return O32_lstrcmp(arg1, arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrcmpW(LPCWSTR arg1, LPCWSTR arg2)
{
    dprintf(("KERNEL32: lstrcmpW\n"));
    return UniStrcmp( (UniChar*)arg1,
                      (UniChar*)arg2 );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR WIN32API lstrcpyA(LPSTR arg1, LPCSTR  arg2)
{
  dprintf(("KERNEL32: lstrcpy(%08xh,%s)\n",
           arg1,
           arg2));

  return O32_lstrcpy(arg1, arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR WIN32API lstrcpyW(LPWSTR dest, LPCWSTR src)
{
  dprintf(("KERNEL32: lstrcpyW(%08xh,%08xh)",
           dest,
           src));

  UniStrcpy( (UniChar*)dest,
             (UniChar*)src );
  return dest;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR WIN32API lstrcpynA(LPSTR arg1, LPCSTR  arg2, int arg3)
{
  register LPSTR p1 = arg1;
  register LPSTR p2 = (LPSTR)arg2;

  dprintf(("KERNEL32: OS2lstrcpyA(%08xh, %08xh, %08xh)\n",
           arg1,
           arg2,
           arg3));

  //PH: looks like either \0 or arg3 terminate the copy
  //return strncpy(arg1, arg2, arg3);
  arg3--; // pre-decrement to avoid exceeding buffer length
          // results in better code than (arg1 > 1)

  for (;*p2 && arg3; arg3--)
    *p1++ = *p2++;

  *p1 = 0; //CB: copy arg-1, set end 0

  return arg1;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR WIN32API lstrcpynW(LPWSTR dest, LPCWSTR src, int arg3)
{
  dprintf(("KERNEL32: lstrcpynW(%08xh,%08xh,%08xh)",
           dest,
           src,
           arg3));

  if (arg3 == 0)
    return NULL;

  UniStrncpy( (UniChar*)dest,
              (UniChar*)src,
              arg3-1); //CB: copy arg3-1 characters
  dest[arg3-1] = 0; //CB: set end
  return dest;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrcmpiA(LPCSTR arg1, LPCSTR  arg2)
{
  dprintf(("KERNEL32: lstrcmpiA(%s,%s)\n",
           arg1,
           arg2));

  return O32_lstrcmpi(arg1, arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

int WIN32API lstrcmpiW(LPCWSTR arg1, LPCWSTR arg2)
{
  char *astr1, *astr2;
  int   rc;

  dprintf(("KERNEL32: lstrcmpiW(%08xh,%08xh)\n",
           arg1,
           arg2));

  // NOTE: This function has no equivalent in uunidef.h
  astr1 = UnicodeToAsciiString((LPWSTR)arg1);
  astr2 = UnicodeToAsciiString((LPWSTR)arg2);
  rc = O32_lstrcmpi(astr1, astr2);
  FreeAsciiString(astr2);
  FreeAsciiString(astr1);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

// unilen: length of astring buffer (including 0 terminator)
// returns string length

int WIN32API lstrcpynWtoA(LPSTR  astring,
                          LPWSTR ustring,
                          int    unilen)
{
  int      i;
  int      rc;
  size_t   uni_chars_left;
  size_t   out_bytes_left;
  size_t   num_subs;
  UniChar* in_buf;
  char*    out_buf;

  if (ustring == NULL)
  {
    if (astring != NULL && unilen > 0)
      astring[0] = 0;
    return 0;
  }

  if (astring == NULL || unilen <= 0)
    return 0;

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

  }
  else
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


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

// asciilen: max length of unicode buffer (including end 0)

int WIN32API lstrcpynAtoW(LPWSTR unicode,
                          LPSTR  ascii,
                          int    asciilen)
{
  int      rc;
  int      i;
  size_t   uni_chars_left;
  size_t   in_bytes_left;
  size_t   num_subs;
  UniChar* out_buf;
  char*    in_buf;

  dprintf(("KERNEL32: HeapString: lstrcpynAtoW(%s,%08xh)\n",
           ascii,
           unicode));

  //CB: no input, set at least terminator
  if (ascii == NULL)
  {
    if (unicode != NULL && asciilen > 0) unicode[0] = 0;
    return 0;
  }

  if (unicode == NULL || asciilen <= 0)
    return 0; //nothing to do

  if (getUconvObject())
  {
    if (asciilen == 1)
    {
       unicode[0] = 0;
       return 0;
    }

    in_buf        = ascii;
    in_bytes_left = asciilen-1; //buffer size in bytes
    out_buf = (UniChar*)unicode;

    uni_chars_left = in_bytes_left; //elements

    rc = UniUconvToUcs( uconv_object,
                        (void**)&in_buf, &in_bytes_left,
                        &out_buf,        &uni_chars_left,
                        &num_subs );

    unicode[asciilen-1-in_bytes_left] = 0;

    //if (rc != ULS_SUCCESS && in_bytes_left > 0) //CB: never the case during my tests
    //   dprintf(("KERNEL32: AsciiToUnicode failed, %d bytes left!\n",in_bytes_left));
    return asciilen - 1;
  }
  else
  { //poor man's conversion

    for(i = 0;i < asciilen-1;i++)
    {
      unicode[i] = ascii[i];
      if (ascii[i] == 0)
        return i-1; //work done
    }

    unicode[asciilen-1] = 0;
    return asciilen-1;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   : Converts unicode string to ascii string
 * Parameters:
 * Variables :
 * Result    : returns length of ascii string
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR WIN32API lstrcpyWtoA(LPSTR ascii, LPWSTR unicode)
{
  if (unicode == NULL)
  {
    if (unicode != NULL) unicode[0] = 0; //CB: set at least end
    return NULL;
  }

  if (unicode == NULL)
    return NULL;  /* garbage in, garbage out ! */

  /* forward to function with len parameter */
  lstrcpynWtoA(ascii,
               unicode,
               UniStrlen((UniChar*)unicode)+1); //end included

  return ascii;
}


/*****************************************************************************
 * Name      :
 * Purpose   : Copies the full string from ascii to unicode
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR WIN32API lstrcpyAtoW(LPWSTR unicode, LPSTR ascii)
{
  /* achimha for security, strlen might trap if garbage in */
  /* @@@PH 98/06/07 */
  if (ascii == NULL)
  {
    if (unicode != NULL) unicode[0] = 0; //CB: set at least end
    return NULL;
  }

  if (unicode == NULL)
    return NULL;  /* garbage in, garbage out ! */

  /* forward to call with length parameter */
  lstrcpynWtoA(ascii, unicode, strlen(ascii)+1); //end included
  return (unicode);
}




/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPVOID HEAP_xalloc( HANDLE heap, DWORD flags, DWORD size )
{
  LPVOID p = HeapAlloc( heap, flags, size );
  if (!p)
  {
    dprintf(("KERNEL32: HEAP_xalloc(%08xh,%08xh,%08xh) out of memory.\n",
             heap,
             flags,
             size));
  }
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPVOID HEAP_xrealloc( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size )
{
  LPVOID p = HeapReAlloc( heap, flags, lpMem, size );
  if (!p)
  {
    dprintf(("KERNEL32: HEAP_xrealloc(%08xh,%08xh,%08xh,%08xh) out of memory.\n",
             heap,
             flags,
             lpMem,
             size));
  }
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPVOID HEAP_malloc(DWORD size )
{
  LPVOID p = HeapAlloc( GetProcessHeap(), 0, size );
  if (!p)
  {
    dprintf(("KERNEL32: HEAP_malloc(%08xh,%08xh,%08xh) out of memory.\n",
             heap,
             flags,
             size));
  }
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPVOID HEAP_realloc(LPVOID lpMem, DWORD size )
{
  LPVOID p = HeapReAlloc( GetProcessHeap(), 0, lpMem, size );
  if (!p)
  {
    dprintf(("KERNEL32: HEAP_realloc(%08xh,%08xh,%08xh,%08xh) out of memory.\n",
             heap,
             flags,
             lpMem,
             size));
  }
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

VOID HEAP_free(LPVOID lpMem)
{
  dprintf(("KERNEL32: HEAP_free(%08xh)\n",
           lpMem));

  HeapFree( GetProcessHeap(), 0, lpMem);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR HEAP_strdupA( HANDLE heap, DWORD flags, LPCSTR str )
{
  LPSTR p = (LPSTR)HEAP_xalloc( heap, flags, strlen(str) + 1 );
  strcpy( p, str );
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR HEAP_strdupW( HANDLE heap, DWORD flags, LPCWSTR str )
{
  INT len = lstrlenW(str) + 1;
  LPWSTR p = (LPWSTR)HEAP_xalloc( heap, flags, len * sizeof(WCHAR) );
  lstrcpyW( p, str );
  return p;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPWSTR HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str )
{
  LPWSTR ret;

  if (!str) return NULL;
  ret = (LPWSTR)HEAP_xalloc( heap, flags, (strlen(str)+1) * sizeof(WCHAR) );
  lstrcpyAtoW( ret, (LPSTR)str );
  return ret;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

LPSTR HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str )
{
  LPSTR ret;

  if (!str) return NULL;
  ret = (LPSTR)HEAP_xalloc( heap, flags, lstrlenW(str) + 1 );
  lstrcpyWtoA( ret, (LPWSTR)str );
  return ret;
}


#if 0


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

// Converts unicode string to ascii string
// returns pointer to ascii string
char * WIN32API UnicodeToAsciiString(WCHAR *ustring)
{
  char *astring;

  if(ustring == NULL)  return(NULL);

  astring = (char *)malloc( 1 + UniStrlen((UniChar*)ustring) );
  UnicodeToAscii( ustring, astring );
  return(astring);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

// Converts ascii string to unicode string
// returns pointer to unicode string
WCHAR * WIN32API AsciiToUnicodeString(char *astring)
{
  WCHAR *ustring;

  if(astring == NULL)
    return(NULL);

  ustring = (WCHAR *)malloc( 1 + strlen(astring) << 1 );
  AsciiToUnicode( astring, ustring );
  return(ustring);
}

#endif

