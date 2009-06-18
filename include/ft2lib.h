/* $Id: ft2lib.h,v 1.1 2004/01/19 11:29:21 sandervl Exp $ */
/** @file
 *
 * InnoTek Font Engine for OS/2 - public header
 *
 * Note: You must include os2.h with INCL_PM defined before this header.
 *
 * (C) 2003 InnoTek Systemberatung GmbH
 *
 */

#ifndef __H_FT2LIB
#define __H_FT2LIB

// Should be higher than any Odin (custom build) dll!
#define EXITLIST_FT2LIB              0x00009F00

#define FT2_ERROR                    0xffffffff

/* for GetGlyphIndices() */
#define FT2_GGI_MARK_NONEXISTING_GLYPHS  0x0001


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/

/** FT2LIB statistics .
 * Used with Ft2QueryStatistics. */
typedef struct FT2LIBSTATS
{
    /** Size of this structure. (input) */
    unsigned    cb;
    /** @name The Cache. (output)
     * @{ */
    unsigned    cbMaxWeight;
    unsigned    cbCurWeight;
    unsigned    cNodes;
    unsigned    cFacesMax;
    unsigned    cFacesCur;
    unsigned    cSizesMax;
    unsigned    cSizesCur;
    /** @} */

    /** @name The Shared Heap. (output)
     * @{ */
    unsigned    cbMaxSize;
    unsigned    cbCommitted;
    unsigned    cbUsed;
    unsigned    cbMaxFree;
    unsigned    cFrees;
    unsigned    cAllocs;
    unsigned    cUsers;
    /** @} */

    /** @name The Uconv objects. (output)
     * @{ */
    unsigned    cObjects;
    unsigned    cTotalCreateCalls;
    /** @} */
} FT2LIBSTATS, *PFT2LIBSTATS;

typedef BOOL (* PFN_WRITELOG)(char *fmt, ...);

#if defined(__WIN32TYPE_H__) || defined(_OS2WIN_H) || defined(__INCLUDE_WINUSER_H) || defined(__WINE_WINBASE_H)
#ifndef OS2_INCLUDED
typedef void *PBUNDLE;
#ifndef __DCDATA_H__
typedef void *PSIZEF;
#endif
typedef void *PPOINTL;
typedef void *PGRADIENTL;
typedef void *PKERNINGPAIRS;
typedef char *PSTR8;
typedef char *PSZ;
typedef void *PFATTRS;
typedef void *PFONTMETRICS;
typedef unsigned long HAB;
typedef unsigned long HPAL;

#endif
#else
/* for GetGlyphOutLine function */
typedef struct
{
    unsigned short  fract;
    signed short   value;
} FIXED_W;


typedef struct
{
    FIXED_W  eM11;
    FIXED_W  eM12;
    FIXED_W  eM21;
    FIXED_W  eM22;
} MAT2, *LPMAT2;

typedef struct
{
    UINT	gmBlackBoxX;
    UINT	gmBlackBoxY;
    POINTL	gmptGlyphOrigin;
    SHORT	gmCellIncX;
    SHORT	gmCellIncY;
} GLYPHMETRICS, *LPGLYPHMETRICS;

#define GGO_METRICS         0
#define GGO_BITMAP          1
#define GGO_NATIVE          2
#define GGO_BEZIER          3
#define GGO_GRAY2_BITMAP    4
#define GGO_GRAY4_BITMAP    5
#define GGO_GRAY8_BITMAP    6
#define GGO_GLYPH_INDEX     0x80

typedef struct tagPOINTFX
{
	FIXED_W x;
	FIXED_W y;
} POINTFX, *LPPOINTFX;

typedef struct tagTTPOLYCURVE
{
	USHORT wType;
	USHORT cpfx;
	POINTFX apfx[1];
} TTPOLYCURVE, *LPTTPOLYCURVE;

typedef struct tagTTPOLYGONHEADER
{
	ULONG cb;
	ULONG dwType;
	POINTFX pfxStart;
} TTPOLYGONHEADER, *LPTTPOLYGONHEADER;

#define TT_PRIM_LINE    1
#define TT_PRIM_QSPLINE 2
#define TT_POLYGON_TYPE 24

  /* ExtTextOut() parameters */
#define ETO_GRAYED          0x0001
#define ETO_OPAQUE          0x0002
#define ETO_CLIPPED         0x0004
#define ETO_GLYPH_INDEX     0x0010
#define ETO_RTLREADING      0x0080
#define ETO_IGNORELANGUAGE  0x1000


// Unicode functions
#define WIN32API	__stdcall

typedef unsigned long DWORD;
typedef const char *  LPCSTR;
typedef char *        LPSTR;
typedef const USHORT *LPCWSTR;
typedef USHORT       *LPWSTR;
typedef USHORT        WCHAR;

#define CP_ACP                                  0
#define CP_OEMCP                                1
#define CP_MACCP				2
#define CP_THREAD_ACP				3
#define CP_SYMBOL				42
#define CP_UTF7					65000
#define CP_UTF8					65001

#define MB_PRECOMPOSED              0x00000001
#define MB_COMPOSITE                0x00000002
#define MB_USEGLYPHCHARS            0x00000004
#define MB_ERR_INVALID_CHARS        0x00000008

/* for GetCharacterPlacement () */

#define GCP_DBCS          0x0001
#define GCP_REORDER       0x0002
#define GCP_USEKERNING    0x0008
#define GCP_GLYPHSHAPE    0x0010
#define GCP_LIGATE        0x0020
#define GCP_DIACRITIC     0x0100
#define GCP_KASHIDA       0x0200
#define GCP_ERROR         0x8000
#define FLI_MASK          0x103b
#define GCP_JUSTIFY         0x00010000L
#define FLI_GLYPHS          0x00040000L
#define GCP_CLASSIN         0x00080000L
#define GCP_MAXEXTENT       0x00100000L
#define GCP_JUSTIFYIN       0x00200000L
#define GCP_DISPLAYZWG      0x00400000L
#define GCP_SYMSWAPOFF      0x00800000L
#define GCP_NUMERICOVERRIDE 0x01000000L
#define GCP_NEUTRALOVERRIDE 0x02000000L
#define GCP_NUMERICSLATIN   0x04000000L
#define GCP_NUMERICSLOCAL   0x08000000L

#define GCPCLASS_LATIN                     1
#define GCPCLASS_HEBREW                    2
#define GCPCLASS_ARABIC                    3
#define GCPCLASS_NEUTRAL                   4
#define GCPCLASS_LOCALNUMBER               5
#define GCPCLASS_LATINNUMBER               6
#define GCPCLASS_LATINNUMERICTERMINATOR    7
#define GCPCLASS_LATINNUMERICSEPARATOR     8
#define GCPCLASS_NUMERICSEPARATOR          9
#define GCPCLASS_PREBOUNDLTR               0x80
#define GCPCLASS_PREBOUNDRLT               0x40
#define GCPCLASS_POSTBOUNDLTR              0x20
#define GCPCLASS_POSTBOUNDRTL              0x10

#define GCPGLYPH_LINKBEFORE                0x8000
#define GCPGLYPH_LINKAFTER                 0x4000

typedef struct tagGCP_RESULTSW
{
    DWORD  lStructSize;
    LPWSTR lpOutString;
    UINT   *lpOrder;
    INT    *lpDx;
    INT    *lpCaretPos;
    LPSTR  lpClass;
    LPWSTR lpGlyphs;
    UINT   nGlyphs;
    UINT   nMaxFit;
} GCP_RESULTSW, *LPGCP_RESULTSW;

#endif

// Ft2QueryFontType
#define FT2_FONTTYPE_UNKNOWN	0
#define FT2_FONTTYPE_TRUETYPE	1
#define FT2_FONTTYPE_TYPE1	2
#define FT2_FONTTYPE_BITMAP	3

typedef INT   (* WIN32API PFN_MULTIBYTETOWIDECHAR)(UINT page, DWORD flags, LPCSTR src, INT srclen,
                                                   LPWSTR dst, INT dstlen);
typedef INT   (* WIN32API PFN_WIDECHARTOMULTIBYTE)(UINT page, DWORD flags, LPCWSTR src, INT srclen,
                                                   LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used);


/*******************************************************************************
*   APIs                                                                       *
*******************************************************************************/

/** @name FT2LIB Query APIs
 * @{ */
/** Get the FT2LIB version. */
VOID APIENTRY Ft2GetVersion(LONG *plMajor, LONG *plMinor, LONG *plBuildNumber);
/** Query statistics for FT2LIB. */
BOOL APIENTRY Ft2QueryStatistics(PFT2LIBSTATS pStats);
// Write statistics to stdout
BOOL APIENTRY Ft2DumpStatistics(PFN_WRITELOG pfnWriteLog);
/** @} */


/** @name Common GPI Control APIs
 * @{ */
HPS   APIENTRY Ft2CreatePS(HAB hab, HDC hdc, PSIZEL psizlSize, ULONG flOptions);
BOOL  APIENTRY Ft2DestroyPS(HPS hps);
BOOL  APIENTRY Ft2Associate(HPS hps, HDC hdc);
BOOL  APIENTRY Ft2RestorePS(HPS hps, LONG lPSid);
LONG  APIENTRY Ft2SavePS(HPS hps);
BOOL  APIENTRY Ft2Erase(HPS hps);
/** @} */


/** @name Global Primitive APIs
 * @{ */
BOOL  APIENTRY Ft2SetColor(HPS hps, LONG lColor);
LONG  APIENTRY Ft2QueryColor(HPS hps);
BOOL  APIENTRY Ft2SetAttrs(HPS hps, LONG lPrimType, ULONG flAttrMask,
                           ULONG flDefMask, PBUNDLE ppbunAttrs);
/** @} */


/** @name Line Primitive APIs
 * @{ */
LONG  APIENTRY Ft2Box(HPS hps, LONG lControl, PPOINTL pptlPoint,
                      LONG lHRound, LONG lVRound);
BOOL  APIENTRY Ft2Move(HPS hps, PPOINTL pptlPoint);
/** @} */


/** @name Global Primitive APIs
 * @{ */
BOOL  APIENTRY Ft2SetBackColor(HPS hps, LONG lColor);
LONG  APIENTRY Ft2QueryBackColor(HPS hps);
BOOL  APIENTRY Ft2SetMix(HPS hps, LONG lMixMode);
BOOL  APIENTRY Ft2SetBackMix(HPS hps, LONG lMixMode);
/** @} */


/** @name Character Primitive APIs
 * @{ */
BOOL    APIENTRY Ft2QueryCharStringPosA(HPS hps, ULONG flOptions, LONG lCount,
                                        PCH pchString, PLONG alXincrements, PPOINTL aptlPositions);
BOOL    APIENTRY Ft2QueryCharStringPosW(HPS hps, ULONG flOptions, LONG lCount,
                                        LPWSTR pchString, PLONG alXincrements, PPOINTL aptlPositions);
BOOL    APIENTRY Ft2QueryTextBoxA(HPS hps, LONG lCount1, PCH pchString,LONG lCount2, PPOINTL aptlPoints);
BOOL    APIENTRY Ft2QueryTextBoxW(HPS hps, LONG lCount1, LPWSTR pchString,LONG lCount2, PPOINTL aptlPoints);
BOOL    APIENTRY Ft2SetCharSet(HPS hps, LONG llcid);
LONG    APIENTRY Ft2QueryCharSet(HPS hps);
BOOL    APIENTRY Ft2SetCharBox(HPS hps, PSIZEF psizfxBox);
BOOL    APIENTRY Ft2QueryCharBox(HPS hps, PSIZEF psizfxSize);
BOOL    APIENTRY Ft2SetCharAngle(HPS hps, PGRADIENTL pgradlAngle);
BOOL    APIENTRY Ft2QueryCharAngle(HPS hps, PGRADIENTL pgradlAngle);
BOOL    APIENTRY Ft2SetTextAlignment(HPS hps, LONG lHoriz, LONG lVert);
BOOL    APIENTRY Ft2QueryTextAlignment(HPS hps, PLONG plHoriz, PLONG plVert);
BOOL    APIENTRY Ft2SetCharBreakExtra(HPS hps, FIXED  BreakExtra);
BOOL    APIENTRY Ft2SetCharDirection(HPS hps, LONG lDirection);

/** @} */


/** @name Physical and Logical Font APIs
 * @{ */
LONG    APIENTRY Ft2CreateLogFont(HPS hps, PSTR8 pName, LONG lLcid, PFATTRS pfatAttrs);
BOOL    APIENTRY Ft2DeleteSetId(HPS hps, LONG lLcid);
BOOL    APIENTRY Ft2QueryLogicalFont(HPS PS, LONG lcid, PSTR8 name,
                                     PFATTRS attrs, LONG length);
/** @} */


/** @name Palette Manager APIs
 * @{ */
HPAL    APIENTRY Ft2CreatePalette(HAB hab, ULONG flOptions, ULONG ulFormat,
                                  ULONG ulCount, PULONG aulTable);
BOOL    APIENTRY Ft2DeletePalette(HPAL hpal);
LONG    APIENTRY Ft2QueryFonts(HPS hps, ULONG flOptions, PSZ pszFacename,
                                PLONG plReqFonts, LONG lMetricsLength, PFONTMETRICS afmMetrics);
BOOL    APIENTRY Ft2SetCurrentPosition(HPS hps, PPOINTL pptlPoint);
BOOL    APIENTRY Ft2QueryCurrentPosition(HPS hps, PPOINTL pptlPoint);
LONG    APIENTRY Ft2SetPel(HPS hps, PPOINTL pptlPoint);
LONG    APIENTRY Ft2QueryPel(HPS hps, PPOINTL pptlPoint);
BOOL    APIENTRY Ft2QueryFontMetrics(HPS hps, LONG lMetricsLength, PFONTMETRICS pfmMetrics);
LONG    APIENTRY Ft2QueryKerningPairs(HPS hps, LONG lCount, PKERNINGPAIRS akrnprData);
BOOL    APIENTRY Ft2LoadPublicFonts(HAB hab, PSZ psz);
BOOL    APIENTRY Ft2UnloadPublicFonts(HAB hab, PSZ psz);
LONG    APIENTRY Ft2CharString(HPS hps, LONG lCount, PCH pchString);
LONG    APIENTRY Ft2CharStringAt(HPS hps, PPOINTL pptlPoint, LONG lCount,
                                 PCH pchString);
LONG    APIENTRY Ft2CharStringPos(HPS hps, PRECTL prclRect, ULONG flOptions,
                                  LONG lCount, PCH pchString, PLONG alAdx);
LONG    APIENTRY Ft2CharStringPosAt(HPS hps, PPOINTL pptlStart, PRECTL prclRect,
                                    ULONG flOptions, LONG lCount, PCH pchString,
                                    PLONG alAdx);
LONG    APIENTRY Ft2CharStringPosAtA(HPS hps, PPOINTL pptlStart, PRECTL prclRect,
                                     ULONG flOptions, LONG lCount, PCH pchString, PLONG alAdx, ULONG fuWin32Options);
LONG    APIENTRY Ft2CharStringPosAtW(HPS hps, PPOINTL pptlStart, PRECTL prclRect,
                                     ULONG flOptions, LONG lCount, LPWSTR pchString, PLONG alAdx, ULONG fuWin32Options);
BOOL    APIENTRY Ft2QueryCharStringPosAtA(HPS hps, PPOINTL pptlStart, ULONG flOptions, LONG lCount,
                                          PCH pchString, PLONG alXincrements, PPOINTL aptlPositions);
BOOL    APIENTRY Ft2QueryCharStringPosAtW(HPS hps, PPOINTL pptlStart, ULONG flOptions, LONG lCount,
                                          LPWSTR pchString, PLONG alXincrements, PPOINTL aptlPositions);
HBITMAP APIENTRY Ft2SetBitmap(HPS hps, HBITMAP hbm);
BOOL    APIENTRY Ft2SetCp(HPS hps, ULONG ulCodePage);
ULONG   APIENTRY Ft2QueryCp(HPS hps);

/** @name Additional APIs
 * @{ */
BOOL APIENTRY Ft2FontSupportsUnicodeChar(HPS hps, LONG lLcid, BOOL isUnicode, WCHAR ch);
BOOL APIENTRY Ft2FontSupportsUnicodeChar1(PSTR8 pName, PFATTRS pfatAttrs, BOOL isUnicode, WCHAR ch);
LONG APIENTRY Ft2AddFontResourceEx(PSZ lpszFilename, ULONG fl, PVOID pdv);
LONG APIENTRY Ft2RemoveFontResourceEx(PSZ lpszFilename, ULONG fl, PVOID pdv);
ULONG APIENTRY Ft2GetFontData(HPS hps, ULONG dwTable, ULONG dwOffset,
                              PVOID lpvBuffer, ULONG cbData);
ULONG APIENTRY Ft2GetGlyphIndices(HPS hps, LPWSTR str, int c,
                                  USHORT *pgi, ULONG fl);

ULONG APIENTRY Ft2GetGlyphOutline(HPS hps, UINT glyph, UINT format,
	                          LPGLYPHMETRICS lpgm, ULONG buflen, void* buf,
		                  const MAT2* lpmat);

BOOL  APIENTRY Ft2GetTextExtentW(HPS hps, LONG lCount1, LPWSTR pchString,
                                 LONG lCount2, PPOINTL aptlPoints);

ULONG APIENTRY Ft2QueryFontType(HPS hps, PSZ lpszFontName);

BOOL APIENTRY Ft2LoadPrivateFonts(HAB hab, PSZ psz);
BOOL APIENTRY Ft2UnloadPrivateFonts(HAB hab, PSZ psz);

void APIENTRY Ft2RegisterUconv(PFN_WIDECHARTOMULTIBYTE pfnWideCharToMultiByte, PFN_MULTIBYTETOWIDECHAR pfnMultiByteToWideChar);

BOOL APIENTRY Ft2QueryStringWidthW(HPS hps, LPWSTR lpszString, UINT cbString, LONG *pWidthArray);

DWORD APIENTRY Ft2GetCharacterPlacementW(HPS hps, LPCWSTR lpString, int uCount,
                                         int nMaxExtent, GCP_RESULTSW *lpResults,
                                         DWORD dwFlags);

/* PMWIN replacements */
HPS  APIENTRY Ft2BeginPaint(HWND hwnd, HPS hps, PRECTL prclPaint);
BOOL APIENTRY Ft2EndPaint(HPS hps);
HPS  APIENTRY Ft2GetPS(HWND hwnd);
BOOL APIENTRY Ft2ReleasePS(HPS hps);
HPS  APIENTRY Ft2GetClipPS(HWND hwnd, HWND hwndClip, ULONG fl);
HPS  APIENTRY Ft2GetScreenPS(HWND hwndDesktop);
LONG APIENTRY Ft2DrawTextA(HPS hps, LONG cchText, PCH lpchText, PRECTL prcl, LONG clrFore, LONG clrBack, ULONG flCmd);

BOOL APIENTRY Ft2EnableFontEngine(BOOL fEnable);

/** @} */

#endif // __H_FT2LIB
