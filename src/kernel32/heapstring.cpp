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
#include <wine\unicode.h>
#include "misc.h"
#include "codepage.h"

#define DBG_LOCALLOG    DBG_heapstring
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

ODINFUNCTIONNODBG1(int, lstrlenA,
                   LPCSTR, arg1)
{
  dprintf2(("KERNEL32: lstrlenA(%s)\n",
           arg1));

  if(arg1 == NULL) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }
  return strlen(arg1);
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

ODINFUNCTIONNODBG1(int, lstrlenW,
                   LPCWSTR, arg1)
{
  int rc;

  if(arg1 == NULL) {
      SetLastError( ERROR_INVALID_PARAMETER );
      return 0;
  }

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

ODINFUNCTIONNODBG2(LPSTR, lstrcatA,
                   LPSTR, arg1, 
                   LPCSTR, arg2)
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

ODINFUNCTIONNODBG2(LPWSTR, lstrcatW,
                   LPWSTR, arg1,
                   LPCWSTR, arg2)
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

ODINFUNCTIONNODBG2(int, lstrcmpA,
                   LPCSTR, arg1,
                   LPCSTR, arg2)
{
  dprintf2(("KERNEL32: OS2lstrcmpA(%s,%s)\n",
           arg1,
           arg2));

    if(arg1 == NULL)
      return -1;
    if(arg2 == NULL)
      return 1;

    return strcmp(arg1, arg2);
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

ODINFUNCTIONNODBG3(int, lstrncmpA,
                   LPCSTR, arg1, 
                   LPCSTR, arg2,
                   int, l)
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
ODINFUNCTIONNODBG3(INT, lstrncmpiA,
                   LPCSTR, str1,
                   LPCSTR, str2,
                   INT, n )
{
  INT firstch,lastch;
  INT result = 0;

  if (n)
  {
    do
    {
      firstch = toupper(*str1);
      lastch = toupper(*str2);
      str1++;
      str2++;
    } while (--n && *str1 && *str2 && firstch == lastch);

    result = firstch - lastch;
  }

  return(result);
}
//TODO: Don't know if this is completely correct
ODINFUNCTIONNODBG3(int, lstrncmpiW,
                   LPCWSTR, str1, 
                   LPCWSTR, str2,
                   int, n)
{
  INT firstch,lastch;
  INT result = 0;

  if (n)
  {
    do
    {
      firstch = toupperW(*str1);
      lastch = toupperW(*str2);
      str1++;
      str2++;
    } while (--n && *str1 && *str2 && firstch == lastch);

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

ODINFUNCTIONNODBG2(int, lstrcmpW,
                   LPCWSTR, arg1,
                   LPCWSTR, arg2)
{
    dprintf2(("KERNEL32: lstrcmpW (%08xh, %08xh)\n",
             arg1,
             arg2));

    if(arg1 == NULL)
      return -1;
    if(arg2 == NULL)
      return 1;

    return strcmpW( arg1, arg2 );
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

ODINFUNCTIONNODBG3(int, lstrncmpW,
                   LPCWSTR, arg1,
                   LPCWSTR, arg2,
                   int, l)
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

ODINFUNCTIONNODBG2(LPSTR, lstrcpyA,
                   LPSTR, dest,
                   LPCSTR, src)
{
    if ( (src == NULL) || (dest == NULL) ) // stupid parameter checking
        return NULL;

    return strcpy(dest, src);
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

ODINFUNCTIONNODBG2(LPWSTR, lstrcpyW,
                   LPWSTR, dest,
                   LPCWSTR, src)
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

ODINFUNCTIONNODBG3(LPSTR, lstrcpynA,
                   LPSTR, arg1,
                   LPCSTR, arg2,
                   int, arg3)
{
  register LPSTR p1 = arg1;
  register LPSTR p2 = (LPSTR)arg2;

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

ODINFUNCTION3(LPWSTR, lstrcpynW,
              LPWSTR, dest,
              LPCWSTR, src,
              int, arg3)
{
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

ODINFUNCTIONNODBG2(int, lstrcmpiA,
                   LPCSTR, arg1,
                   LPCSTR, arg2)
{
  dprintf2(("KERNEL32: lstrcmpiA(%s,%s)\n",
           arg1,
           arg2));

  if(arg1 == NULL)
      return -1;

  if(arg2 == NULL)
      return 1;

  return strcmpi(arg1, arg2);
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

ODINFUNCTIONNODBG2(int, lstrcmpiW,
                   LPCWSTR, str1,
                   LPCWSTR, str2)
{
  if (!str1 || !str2) {
    
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  return strcmpiW( str1, str2 );
}

//*****************************************************************************
//lstrcpynWtoA and lstrcpynAtoW must zero-terminate the string
//because Wine code depends on this behaviour (i.e. comdlg32)
//*****************************************************************************
ODINFUNCTIONNODBG3(int, lstrcpynWtoA,
                   LPSTR,   astring,
                   LPCWSTR, ustring,
                   int,     length)
{
 int ret;

    ret = WideCharToMultiByte(CP_ACP, 0, ustring, -1, astring, length, 0, NULL);
    if(ret == 0) {
         SetLastError(ERROR_SUCCESS); //WideCharToMultiByte sets it to ERROR_INSUFFICIENT_BUFFER
         ret = length;
    }
    //Must not always set the last character to 0; some apps send the wrong
    //string size to apis that use this function (i.e. GetMenuStringW (Notes))
    //-> overwrites stack
    if(ret == length) {
         astring[length-1] = 0;
    }
    else astring[ret] = 0;

    return ret;
}

//lstrcpynWtoA and lstrcpynAtoW must zero-terminate the string
//because Wine code depends on this behaviour (i.e. comdlg32)
ODINFUNCTIONNODBG3(int,     lstrcpynAtoW,
                   LPWSTR,  unicode,
                   LPCSTR,  ascii,
                   int ,    asciilen)
{
 int ret;

    ret = MultiByteToWideChar(CP_ACP, 0, ascii, -1, unicode, asciilen);
    if(ret == 0) {
         SetLastError(ERROR_SUCCESS); //MultiByteToWideChar sets it to ERROR_INSUFFICIENT_BUFFER
         ret = asciilen;
    }

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

ODINFUNCTIONNODBG2(LPSTR, lstrcpyWtoA,
                   LPSTR, ascii,
                   LPCWSTR, unicode)
{
    //@@@PH huh? wuz dat?
    if (unicode == NULL)
    {
        DebugInt3();
        if (ascii != NULL) ((LPWSTR)ascii)[0] = 0; //CB: set at least end
        return NULL;
    }

    if (ascii == NULL) {
        DebugInt3();
        return NULL;  /* garbage in, garbage out ! */
    }

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

ODINFUNCTIONNODBG2(LPWSTR, lstrcpyAtoW,
                   LPWSTR, unicode,
                   LPCSTR, ascii)
{
    /* achimha for security, strlen might trap if garbage in */
    /* @@@PH 98/06/07 */
    if (ascii == NULL)
    {
        DebugInt3();
        if (unicode != NULL) unicode[0] = 0; //CB: set at least end
        return NULL;
    }

    if (unicode == NULL) {
        DebugInt3();
        return NULL;  /* garbage in, garbage out ! */
    }

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

ODINFUNCTIONNODBG3(LPVOID, HEAP_xalloc,
                   HANDLE, heap,
                   DWORD, flags,
                   DWORD, size )
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

ODINFUNCTIONNODBG4(LPVOID, HEAP_xrealloc,
                   HANDLE, heap,
                   DWORD, flags,
                   LPVOID, lpMem,
                   DWORD, size )
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

ODINFUNCTIONNODBG1(LPVOID, HEAP_malloc,
                   DWORD, size )
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

ODINFUNCTIONNODBG1(DWORD, HEAP_size,
                   LPVOID, lpMem)
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

ODINFUNCTIONNODBG2(LPVOID, HEAP_realloc,
                   LPVOID, lpMem,
                   DWORD, size )
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

ODINPROCEDURENODBG1(HEAP_free,
                    LPVOID, lpMem)
{
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

ODINFUNCTIONNODBG3(LPSTR, HEAP_strdupA,
                   HANDLE, heap,
                   DWORD, flags,
                   LPCSTR, str )
{
  int iLength = lstrlenA(str) + 1;
  LPSTR p = (LPSTR)HEAP_xalloc( heap, flags, iLength );
  memcpy( p, str, iLength );
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

ODINFUNCTIONNODBG3(LPWSTR, HEAP_strdupW,
                   HANDLE, heap,
                   DWORD, flags,
                   LPCWSTR, str )
{
    INT len = lstrlenW(str) + 1;
    LPWSTR p = (LPWSTR)HEAP_xalloc( heap, flags, len * sizeof(WCHAR) );
    memcpy( p, str, len );
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

ODINFUNCTIONNODBG3(LPWSTR, HEAP_strdupAtoW,
                   HANDLE, heap,
                   DWORD, flags,
                   LPCSTR, str )
{
    LPWSTR ret;
    int   len;

    if (!str) return NULL;

    len = MultiByteToWideChar( CP_ACP, 0, str, -1, NULL, 0);
    ret = (LPWSTR)HEAP_xalloc( heap, flags, len*sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, str, -1, ret, len);

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

ODINFUNCTIONNODBG3(LPSTR, HEAP_strdupWtoA,
                   HANDLE, heap,
                   DWORD, flags,
                   LPCWSTR, str )
{
    LPSTR ret;
    int   len;

    if (!str) return NULL;

    len = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, 0, NULL);
    ret = (LPSTR)HEAP_xalloc( heap, flags, len);
    WideCharToMultiByte(CP_ACP, 0, str, -1, ret, len, 0, NULL );
    return ret;
}




