/* $Id: heapstring.cpp,v 1.36 2001-01-18 18:14:16 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 compatibility string functions for OS/2
 *
 * NOTE: lstrcpyn* always appends a terminating 0 (unlike strncpy)!
 *
 * Copyright 1999 Patrick Haller
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <winnls.h>
#include <unicode.h>
#include <ctype.h>
#include <wcstr.h>
#include "heap.h"
#include <heapstring.h>
#include "misc.h"
#include "codepage.h"

#define DBG_LOCALLOG	DBG_heapstring
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(KERNEL32-HEAPSTRING)

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
  dprintf2(("KERNEL32: lstrlenA(%s)\n",
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
  dprintf2(("KERNEL32: lstrlenW(%08xh) returns %d\n",
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
  dprintf2(("KERNEL32: lstrcat(%s,%s)\n",
           arg1,
           arg2));

  if(arg2 == NULL)
    return arg1;
  strcat(arg1, arg2);
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

LPWSTR WIN32API lstrcatW(LPWSTR arg1, LPCWSTR arg2)
{
  dprintf2(("KERNEL32: OS2lstrcatW(%08xh,%08xh)\n",
           arg1,
           arg2));

  if(arg2 == NULL)
    return arg1;

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
  dprintf2(("KERNEL32: OS2lstrcmpA(%s,%s)\n",
           arg1,
           arg2));

    if(arg1 == NULL)
      return -1;
    if(arg2 == NULL)
      return 1;

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

int WIN32API lstrncmpA(LPCSTR arg1, LPCSTR  arg2, int l)
{
  dprintf2(("KERNEL32: OS2lstrncmpA(%s,%s,%d)\n",
           arg1,
           arg2,
           l));

  return strncmp(arg1, arg2, l);
}

/*****************************************************************************
 * Name      : lstrncmpiA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
INT WINAPI lstrncmpiA( LPCSTR str1, LPCSTR str2, INT n )
{
  INT firstch,lastch;
  INT result = 0;

  if (n)
  {
    do
    {
      firstch = tolower(*str1);
      lastch = tolower(*str2);
      str1++;
      str2++;
    } while (--n && str1 && str2 && firstch == lastch);

    result = firstch - lastch;
  }

  return(result);
}
//TODO: Don't know if this is completely correct
int WIN32API lstrncmpiW(LPCWSTR str1, LPCWSTR str2, int n)
{
  INT firstch,lastch;
  INT result = 0;

  if (n)
  {
    do
    {
      firstch = tolower((char)*str1);
      lastch = tolower((char)*str2);
      str1++;
      str2++;
    } while (--n && str1 && str2 && firstch == lastch);

    result = firstch - lastch;
  }

  return(result);
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
    dprintf2(("KERNEL32: lstrcmpW (%08xh, %08xh)\n",
             arg1,
             arg2));

    if(arg1 == NULL)
      return -1;
    if(arg2 == NULL)
      return 1;

    return wcscmp( (wchar_t*)arg1,
                   (wchar_t*)arg2 );
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

int WIN32API lstrncmpW(LPCWSTR arg1, LPCWSTR  arg2, int l)
{
  dprintf2(("KERNEL32: OS2lstrncmpW(%08xh,%08xh,%d)\n",
           arg1,
           arg2,
           l));

  return wcsncmp((wchar_t*)arg1,
                 (wchar_t*)arg2,
                 l);
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

LPSTR WIN32API lstrcpyA(LPSTR dest, LPCSTR src)
{
  if ( (src == NULL) || (dest == NULL) ) // stupid parameter checking
     return NULL;

  return O32_lstrcpy(dest, src);
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
  if ( (src == NULL) || (dest == NULL) ) // stupid parameter checking
     return NULL;

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

  dprintf2(("KERNEL32: OS2lstrcpyA(%08xh, %08xh, %08xh)\n",
           arg1,
           arg2,
           arg3));

  if(arg3 == 0) {
    return NULL;
  }

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
  dprintf2(("KERNEL32: lstrcpynW(%08xh,%08xh,%08xh)",
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
  dprintf2(("KERNEL32: lstrcmpiA(%s,%s)\n",
           arg1,
           arg2));

  if(arg1 == NULL)
      return -1;

  if(arg2 == NULL)
      return 1;

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

  dprintf2(("KERNEL32: lstrcmpiW(%08xh,%08xh)\n",
           arg1,
           arg2));

  if(arg1 == NULL)
      return -1;

  if(arg2 == NULL)
      return 1;

  // NOTE: This function has no equivalent in uunidef.h
  astr1 = UnicodeToAsciiString((LPWSTR)arg1);
  astr2 = UnicodeToAsciiString((LPWSTR)arg2);
  rc = lstrcmpiA(astr1, astr2);
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

int WIN32API lstrcpynCtoA(LPSTR  astring,
                          LPCWSTR ustring,
                          int    unilen,
                          UconvObject uconv_object)
{
  int      i;
  int      rc;
  size_t   uni_chars_left;
  size_t   out_bytes_left;
  size_t   num_subs;
  UniChar* in_buf;
  char*    out_buf;

  dprintf2(("KERNEL32: HeapString: lstrcpynWtoA(%08x,%08xh,%d)\n",
           astring,
           ustring,
           unilen));

  if (ustring == NULL)
  {
    if (astring != NULL && unilen > 0)
      astring[0] = 0;
    return 0;
  }

  if (astring == NULL || unilen <= 0)
    return 0;

  if (uconv_object)
  {
    if (unilen == 1)
    {
      astring[0] = 0;
      return 0; //no data
    }

    //SvL: Determine length of unicode string
    uni_chars_left = UniStrlen((UniChar*)ustring)+1;
    uni_chars_left = min(uni_chars_left, unilen);
    unilen = uni_chars_left;

    out_bytes_left = uni_chars_left; //size in bytes == elements
    in_buf  = (UniChar*)ustring;
    out_buf = astring;
    rc = UniUconvFromUcs(uconv_object,
                         &in_buf, &uni_chars_left,
                         (void**)&out_buf, &out_bytes_left,
                         &num_subs);
    
    /* @@@PH 2000/08/10
     * this causes the last character in the converted
     * target string to be chopped off. I.e. causes CMD.EXE
     * to loose the CRLF functionality.
     */
     /*
    unilen -= 1+out_bytes_left; //end + left bytes
    astring[unilen] = 0; //terminate
    */

    return unilen; //length of string (excluding terminator)
  }
  else
  {
    /* idiots unicode conversion :) */
    for (i = 0; i < unilen-1; i++)
    {
      astring[i] = (ustring[i] > 255) ? (char)0x20 : (char)ustring[i]; //CB: handle invalid characters as space
      if (ustring[i] == 0) return i; //asta la vista, baby
    }

//    astring[unilen-1] = 0; // @@@PH: 1999/06/09 fix - always terminate string
//    return(unilen-1);
    return unilen;
  }
}

//lstrcpynWtoA and lstrcpynAtoW must zero-terminate the string
//because Wine code depends on this behaviour (i.e. comdlg32)
int WIN32API lstrcpynWtoA(LPSTR  astring,
                          LPCWSTR ustring,
                          int    unilen)
{
 int ret;

    ret = lstrcpynCtoA(astring, ustring, unilen, GetWindowsUconvObject());
    //Must not always set the last character to 0; some apps send the wrong
    //string size to apis that use this function (i.e. GetMenuStringW (Notes))
    //-> overwrites stack
    if(ret == unilen) {
         astring[unilen-1] = 0;
    }
    else astring[ret] = 0;
    
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

// asciilen: max length of unicode buffer (including end 0)
// @@@PH 0 termination is NOT necessarily included !
int lstrcpynAtoC(LPWSTR unicode,
                 LPCSTR  ascii,
                 int    asciilen,
                 UconvObject uconv_object)
{
  int      rc;
  int      i;
  size_t   uni_chars_left;
  size_t   in_bytes_left;
  size_t   num_subs;
  UniChar* out_buf;
  char*    in_buf;

  dprintf2(("KERNEL32: HeapString: lstrcpynAtoW(%s,%08xh,%d)\n",
           ascii,
           unicode,
           asciilen));

  //CB: no input, set at least terminator
  if (ascii == NULL)
  {
    if (unicode != NULL && asciilen > 0) unicode[0] = 0;
    return 0;
  }

  if (unicode == NULL || asciilen <= 0)
    return 0; //nothing to do

  if (uconv_object)
  {
    //@@@PH what's this?
    if ((asciilen == 1) && (*ascii == '\0') )
    {
       unicode[0] = 0;
       return 0;
    }

    in_buf        = (LPSTR)ascii;

    //@@@PH what's this?
    //in_bytes_left = asciilen-1; //buffer size in bytes

    //SvL: Determine length of ascii string
    in_bytes_left = strlen(in_buf)+1;
    in_bytes_left = asciilen = min(in_bytes_left, asciilen); //buffer size in bytes

    out_buf = (UniChar*)unicode;

    uni_chars_left = in_bytes_left; //elements

    rc = UniUconvToUcs( uconv_object,
                        (void**)&in_buf, &in_bytes_left,
                        &out_buf,        &uni_chars_left,
                        &num_subs );
    
    /* @@@PH 2000/08/10
     * this causes the last character in the converted
     * target string to be chopped off. I.e. causes CMD.EXE
     * to enter command correctly.
     */
    /*
    asciilen -= 1+uni_chars_left; //end + left bytes

    unicode[asciilen] = 0; // always terminate string
    */
      
    return asciilen; //length of string (excluding terminator)
  }
  else
  { //poor man's conversion

//    for(i = 0;i < asciilen-1;i++)
    for(i = 0;i < asciilen;i++)
    {
      unicode[i] = ascii[i];
      if (ascii[i] == 0)
        //return i-1; //work done
        return i; //work done
    }

//    unicode[asciilen-1] = 0;
//    return asciilen-1;
    return asciilen;
  }
}

//lstrcpynWtoA and lstrcpynAtoW must zero-terminate the string
//because Wine code depends on this behaviour (i.e. comdlg32)
int WIN32API lstrcpynAtoW(LPWSTR unicode,
                          LPCSTR  ascii,
                          int    asciilen)
{
 int ret;

    ret = lstrcpynAtoC(unicode, ascii, asciilen, GetWindowsUconvObject());
    //Must not always set the last character to 0; some apps send the wrong
    //string size to apis that use this function (i.e. GetMenuStringW (Notes))
    //-> overwrites stack
    if(ret == asciilen) {
         unicode[asciilen-1] = 0;
    }
    else unicode[ret] = 0;
    return ret;
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

LPSTR WIN32API lstrcpyWtoA(LPSTR ascii, LPCWSTR unicode)
{
  //@@@PH huh? wuz dat?
  if (unicode == NULL)
  {
    if (unicode != NULL) ((LPWSTR)unicode)[0] = 0; //CB: set at least end
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

LPWSTR WIN32API lstrcpyAtoW(LPWSTR unicode, LPCSTR ascii)
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
  lstrcpynAtoW(unicode, ascii, strlen(ascii)+1); //end included
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

LPVOID WIN32API HEAP_xalloc( HANDLE heap, DWORD flags, DWORD size )
{
  LPVOID p = HeapAlloc( heap, flags, size );
  if (!p)
  {
    dprintf2(("KERNEL32: HEAP_xalloc(%08xh,%08xh,%08xh) out of memory.\n",
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

LPVOID WIN32API HEAP_xrealloc( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size )
{
  LPVOID p = HeapReAlloc( heap, flags, lpMem, size );
  if (!p)
  {
    dprintf2(("KERNEL32: HEAP_xrealloc(%08xh,%08xh,%08xh,%08xh) out of memory.\n",
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

LPVOID WIN32API HEAP_malloc(DWORD size )
{
  LPVOID p = HeapAlloc( GetProcessHeap(), 0, size );
  if (!p)
  {
    dprintf2(("KERNEL32: HEAP_malloc(%08xh) out of memory.\n",
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

DWORD WIN32API HEAP_size(LPVOID lpMem)
{
  return HeapSize( GetProcessHeap(), 0, lpMem );
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

LPVOID WIN32API HEAP_realloc(LPVOID lpMem, DWORD size )
{
  LPVOID p = HeapReAlloc( GetProcessHeap(), 0, lpMem, size );
  if (!p)
  {
    dprintf2(("KERNEL32: HEAP_realloc(%08xh,%08xh) out of memory.\n",
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

VOID WIN32API HEAP_free(LPVOID lpMem)
{
  dprintf2(("KERNEL32: HEAP_free(%08xh)\n",
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

LPSTR WIN32API HEAP_strdupA( HANDLE heap, DWORD flags, LPCSTR str )
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

LPWSTR WIN32API HEAP_strdupW( HANDLE heap, DWORD flags, LPCWSTR str )
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

LPWSTR WIN32API HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str )
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

LPSTR WIN32API HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str )
{
  LPSTR ret;

  if (!str) return NULL;
  ret = (LPSTR)HEAP_xalloc( heap, flags, lstrlenW(str) + 1 );
  lstrcpyWtoA( ret, (LPWSTR)str );
  return ret;
}


/*****************************************************************************
 * Name      : WideCharToLocal
 * Purpose   : similar lstrcpyWtoA, should handle codepages properly
 * Parameters:
 * Variables :
 * Result    : strlen of the destination string
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

INT WIN32API WideCharToLocal(LPSTR pLocal, LPWSTR pWide, INT dwChars)
{
  dprintf2(("KERNEL32: WideCharToLocal(%08xh,%08xh,%08xh)\n",
           pLocal,
           pWide,
           dwChars));

  *pLocal = 0;
  WideCharToMultiByte(CP_ACP,
                      0,
                      pWide,
                      -1,
                      pLocal,
                      dwChars,
                      NULL,
                      NULL);

  return strlen(pLocal);
}


/*****************************************************************************
 * Name      : LocalToWideChar
 * Purpose   : similar lstrcpyAtoW, should handle codepages properly
 * Parameters:
 * Variables :
 * Result    : strlen of the destination string
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/08/05 20:46]
 *****************************************************************************/

INT WIN32API LocalToWideChar(LPWSTR pWide, LPSTR pLocal, INT dwChars)
{
  *pWide = 0;

  dprintf2(("KERNEL32: LocalToWideChar(%08xh,%08xh,%08xh)\n",
           pLocal,
           pWide,
           dwChars));

  MultiByteToWideChar(CP_ACP,
                      0,
                      pLocal,
                      -1,
                      pWide,
                      dwChars);

  return lstrlenW(pWide);
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



/**************************************************************************
 *           This function is used just locally !
 *  Description: Inverts a string.
 */ 
static void OLE_InvertString(char* string)
{    
    char    sTmpArray[128];
    INT     counter, i = 0;

    for (counter = strlen(string); counter > 0; counter--)
    {
        memcpy(sTmpArray + i, string + counter-1, 1);
        i++;
    }
    memcpy(sTmpArray + i, "\0", 1);
    strcpy(string, sTmpArray);
}

/***************************************************************************************
 *           This function is used just locally !
 *  Description: Test if the given string (psNumber) is valid or not.
 *               The valid characters are the following:
 *               - Characters '0' through '9'. 
 *               - One decimal point (dot) if the number is a floating-point value. 
 *               - A minus sign in the first character position if the number is 
 *                 a negative value.
 *              If the function succeeds, psBefore/psAfter will point to the string
 *              on the right/left of the decimal symbol. pbNegative indicates if the 
 *              number is negative.
 */
static INT OLE_GetNumberComponents(char* pInput, char* psBefore, char* psAfter, BOOL* pbNegative)
{
char	sNumberSet[] = "0123456789";
BOOL	bInDecimal = FALSE;

	/* Test if we do have a minus sign */
	if ( *pInput == '-' )
	{
		*pbNegative = TRUE;
		pInput++; /* Jump to the next character. */
	}
	
	while(*pInput != '\0')
	{
		/* Do we have a valid numeric character */
		if ( strchr(sNumberSet, *pInput) != NULL )
		{
			if (bInDecimal == TRUE)
                *psAfter++ = *pInput;
			else
                *psBefore++ = *pInput;
		}
		else
		{
			/* Is this a decimal point (dot) */
			if ( *pInput == '.' )
			{
				/* Is it the first time we find it */
				if ((bInDecimal == FALSE))
					bInDecimal = TRUE;
				else
					return -1; /* ERROR: Invalid parameter */
			}
			else
			{
				/* It's neither a numeric character, nor a decimal point.
				 * Thus, return an error.
                 */
				return -1;
			}
		}
        pInput++;
	}
	
	/* Add an End of Line character to the output buffers */
	*psBefore = '\0';
	*psAfter = '\0';

	return 0; 
}

/**************************************************************************
 *           This function is used just locally !
 *  Description: A number could be formatted using different numbers 
 *               of "digits in group" (example: 4;3;2;0).
 *               The first parameter of this function is an array
 *               containing the rule to be used. It's format is the following:
 *               |NDG|DG1|DG2|...|0|
 *               where NDG is the number of used "digits in group" and DG1, DG2,
 *               are the corresponding "digits in group".
 *               Thus, this function returns the grouping value in the array
 *               pointed by the second parameter.
 */
static INT OLE_GetGrouping(char* sRule, INT index)
{
    char    sData[2], sRuleSize[2];
    INT     nData, nRuleSize;

    memcpy(sRuleSize, sRule, 1);
    memcpy(sRuleSize+1, "\0", 1);
    nRuleSize = atoi(sRuleSize);

    if (index > 0 && index < nRuleSize)
    {
        memcpy(sData, sRule+index, 1);
        memcpy(sData+1, "\0", 1);
        nData = atoi(sData);            
    }
        
    else
    {
        memcpy(sData, sRule+nRuleSize-1, 1);
        memcpy(sData+1, "\0", 1);
        nData = atoi(sData);            
    }
    
    return nData;
}