/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define ICOM_CINTERFACE 1

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h> 

#include "winnls.h"
#include "winerror.h"
#include "debugtools.h"
#include "heap.h"

#include "shellapi.h"
#include "shell32_main.h"
#include "wine/undocshell.h"
//#include "wine/unicode.h"


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(shell32-shellstring)


/************************* STRRET functions ****************************/

/*************************************************************************
 * StrRetToStrN					[SHELL32.96]
 * 
 * converts a STRRET to a normal string
 *
 * NOTES
 *  the pidl is for STRRET OFFSET
 */
HRESULT WINAPI StrRetToStrNA (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
        return StrRetToBufA( src, pidl, (LPSTR)dest, len );
}

HRESULT WINAPI StrRetToStrNW (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
        return StrRetToBufW( src, pidl, (LPWSTR)dest, len );
}

HRESULT WINAPI StrRetToStrNAW (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
	if(SHELL_OsIsUnicode())
	  return StrRetToStrNW (dest, len, src, pidl);
	return StrRetToStrNA (dest, len, src, pidl);
}

/************************* OLESTR functions ****************************/

/************************************************************************
 *	StrToOleStr			[SHELL32.163]
 *
 */
int WINAPI StrToOleStrA (LPWSTR lpWideCharStr, LPCSTR lpMultiByteString)
{
	//TRACE("(%p, %p %s)\n", lpWideCharStr, lpMultiByteString, debugstr_a(lpMultiByteString));

	return MultiByteToWideChar(0, 0, lpMultiByteString, -1, lpWideCharStr, MAX_PATH);

}
int WINAPI StrToOleStrW (LPWSTR lpWideCharStr, LPCWSTR lpWString)
{
	//TRACE("(%p, %p %s)\n", lpWideCharStr, lpWString, debugstr_w(lpWString));

	lstrcpyW (lpWideCharStr, lpWString );
	return lstrlenW(lpWideCharStr);
}

BOOL WINAPI StrToOleStrAW (LPWSTR lpWideCharStr, LPCVOID lpString)
{
	if (SHELL_OsIsUnicode())
	  return StrToOleStrW (lpWideCharStr, (LPCWSTR)lpString);
	return StrToOleStrA (lpWideCharStr, (LPCSTR)lpString);
}

/*************************************************************************
 * StrToOleStrN					[SHELL32.79]
 *  lpMulti, nMulti, nWide [IN]
 *  lpWide [OUT]
 */
BOOL WINAPI StrToOleStrNA (LPWSTR lpWide, INT nWide, LPCSTR lpStrA, INT nStr) 
{
  //TRACE("(%p, %x, %s, %x)\n", lpWide, nWide, debugstr_an(lpStrA,nStr), nStr);
  return MultiByteToWideChar (0, 0, lpStrA, nStr, lpWide, nWide);
}
BOOL WINAPI StrToOleStrNW (LPWSTR lpWide, INT nWide, LPCWSTR lpStrW, INT nStr) 
{
  //TRACE("(%p, %x, %s, %x)\n", lpWide, nWide, debugstr_wn(lpStrW, nStr), nStr);

  if (lstrcpynW (lpWide, lpStrW, nWide))
  { return lstrlenW (lpWide);
  }
  return 0;
}

BOOL WINAPI StrToOleStrNAW (LPWSTR lpWide, INT nWide, LPCVOID lpStr, INT nStr) 
{
	if (SHELL_OsIsUnicode())
	  return StrToOleStrNW (lpWide, nWide, (LPCWSTR)lpStr, nStr);
	return StrToOleStrNA (lpWide, nWide, (LPCSTR)lpStr, nStr);
}

/*************************************************************************
 * OleStrToStrN					[SHELL32.78]
 */
BOOL WINAPI OleStrToStrNA (LPSTR lpStr, INT nStr, LPCWSTR lpOle, INT nOle) 
{
  //TRACE("(%p, %x, %s, %x)\n", lpStr, nStr, debugstr_wn(lpOle,nOle), nOle);
  return WideCharToMultiByte (0, 0, lpOle, nOle, lpStr, nStr, NULL, NULL);
}

BOOL WINAPI OleStrToStrNW (LPWSTR lpwStr, INT nwStr, LPCWSTR lpOle, INT nOle) 
{
 // TRACE("(%p, %x, %s, %x)\n", lpwStr, nwStr, debugstr_wn(lpOle,nOle), nOle);

  if (lstrcpynW ( lpwStr, lpOle, nwStr))
  { return lstrlenW (lpwStr);
  }
  return 0;
}

BOOL WINAPI OleStrToStrNAW (LPVOID lpOut, INT nOut, LPCVOID lpIn, INT nIn) 
{
	if (SHELL_OsIsUnicode())
	  return OleStrToStrNW ((LPWSTR)lpOut, nOut, (LPWSTR)lpIn, nIn);
	return OleStrToStrNA ((LPSTR)lpOut, nOut, (LPCWSTR)lpIn, nIn);
}
