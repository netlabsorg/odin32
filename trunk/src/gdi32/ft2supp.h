/*
 * GDI32 FreeType2 Support Class
 *
 * Copyright 2003 Innotek Systemberatung GmbH (stauff@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __FT2SUPP_H__
#define __FT2SUPP_H__

#include <odin.h>
#include "oslibgpi.h"
#include <winconst.h>
#include <win32type.h>
#include <ft2lib.h>

#ifndef GDI_ERROR
#define GDI_ERROR			0xffffffff
#endif

typedef int ( SYSTEM _FT2FN) ();
typedef _FT2FN    *PFT2FN;

#define FT2LIBNAME 		"ft2lib.dll"
#define FT2LIB_MAJOR_VERSION	2
#define FT2LIB_MINOR_VERSION    3

// Add here function pointer typedefs to funcs that we will use
typedef ULONG (* SYSTEM PFN_FT2GETGLYPHINDICES)(HPS, WCHAR *, int, ULONG *, ULONG );

typedef BOOL  (* SYSTEM PFN_FT2GETTEXTEXTENTW)(HPS hps, LONG lCount1,LPCWSTR pchString,LONG lCount2,PPOINTLOS2 aptlPoints);
typedef BOOL  (* SYSTEM PFN_FT2CHARSTRINGPOSATA)(HPS hps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx, ULONG fuWin32Options);
typedef BOOL  (* SYSTEM PFN_FT2CHARSTRINGPOSATW)(HPS hps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCWSTR pchString,CONST INT *alAdx, ULONG fuWin32Options);
typedef DWORD (* SYSTEM PFN_FT2GETGLYPHOUTLINE)(HPS hps, UINT glyph, UINT format, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf, const MAT2* lpmat);
typedef DWORD (* SYSTEM PFN_FT2GETFONTDATA)(HPS hps, DWORD dwTable, DWORD dwOffset, LPVOID lpvBuffer, DWORD cbData);
typedef DWORD (* SYSTEM PFN_FT2QUERYFONTTYPE)(HPS hps, LPCSTR lpszFontName);
typedef BOOL  (* SYSTEM PFN_FT2QUERYSTRINGWIDTHW)(HPS hps, LPWSTR lpszString, UINT cbString, LONG *pWidthArray);
typedef DWORD (* SYSTEM PFN_FT2GETCHARACTERPLACEMENTW)(HDC hdc, LPCWSTR lpString, int uCount,
                                                       int nMaxExtent, GCP_RESULTSW *lpResults,
                                                       DWORD dwFlags);

typedef INT   (* WIN32API PFN_MULTIBYTETOWIDECHAR)(UINT page, DWORD flags, LPCSTR src, INT srclen,
                                                   LPWSTR dst, INT dstlen);
typedef INT   (* WIN32API PFN_WIDECHARTOMULTIBYTE)(UINT page, DWORD flags, LPCWSTR src, INT srclen,
                                                   LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used);
typedef void  (* SYSTEM PFN_FT2REGISTERUCONV)(PFN_WIDECHARTOMULTIBYTE pfnWideCharToMultiByte, PFN_MULTIBYTETOWIDECHAR pfnMultiByteToWideChar);

typedef BOOL  (* SYSTEM PFN_FT2ENABLEFONTENGINE)(BOOL fEnable);

typedef VOID  (* SYSTEM PFN_FT2GETVERSION)(LONG *plMajor, LONG *plMinor, LONG *plBuildNumber);

class CFT2Module {
 private:
   // Handle for our module
   HMODULE hftModule;
   char   *pszModuleName;

   PFN_FT2GETGLYPHINDICES  pfnGetGlyphIndices;
   PFN_FT2GETTEXTEXTENTW   pfnFt2GetTextExtentW;
   PFN_FT2CHARSTRINGPOSATA pfnFt2CharStringPosAtA;
   PFN_FT2CHARSTRINGPOSATW pfnFt2CharStringPosAtW;
   PFN_FT2GETGLYPHOUTLINE  pfnFt2GetGlyphOutline;
   PFN_FT2GETFONTDATA      pfnFt2GetFontData;
   PFN_FT2QUERYFONTTYPE    pfnFt2QueryFontType;
   PFN_FT2REGISTERUCONV    pfnFt2RegisterUconv;
   PFN_FT2QUERYSTRINGWIDTHW pfnFt2QueryStringWidthW;
   PFN_FT2GETCHARACTERPLACEMENTW pfnFt2GetCharacterPlacementW;
   PFN_FT2ENABLEFONTENGINE pfnFt2EnableFontEngine;
   PFN_FT2GETVERSION pfnFt2GetVersion;

   // Indicator of enabled state
   BOOL    bEnabled;
   PFT2FN QueryProcAddress( int );
   PFT2FN QueryProcAddress( char* );

 public:
   // Constructor
   CFT2Module( char* modulename = FT2LIBNAME );
   ~CFT2Module();
   void init();


   // Functions we may call
 
   BOOL  isEnabled() { return bEnabled; };
    
   DWORD Ft2GetGlyphIndices(HPS hps, LPCWSTR str, int c, LPWORD pgi, DWORD fl);
   DWORD Ft2GetGlyphOutline(HPS hps, UINT glyph, UINT format, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf, const MAT2* lpmat); 

   BOOL  Ft2GetTextExtentW(HPS hps, LONG lCount1,LPCWSTR pchString, PPOINTLOS2 pwidthHeight);
   BOOL  Ft2CharStringPosAtA(HPS hps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx, ULONG fuWin32Options);
   BOOL  Ft2CharStringPosAtW(HPS hps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCWSTR pchString,CONST INT *alAdx, ULONG fuWin32Options);

   BOOL  Ft2GetStringWidthW(HDC hdc, LPWSTR lpszString, UINT cbString, PINT pWidthArray);

   DWORD Ft2GetCharacterPlacementW(HDC hdc, LPCWSTR lpString, int uCount,
                                   int nMaxExtent, GCP_RESULTSW *lpResults,
                                   DWORD dwFlags);

   DWORD Ft2GetFontData(HPS hps, DWORD dwTable, DWORD dwOffset, LPVOID lpvBuffer, DWORD cbData);

   DWORD Ft2QueryFontType(HPS hps, LPCSTR lpszFontName);

};

extern CFT2Module FT2Module;

#endif // __FT2SUPP_H__