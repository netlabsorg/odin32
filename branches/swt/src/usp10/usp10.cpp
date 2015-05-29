/* $Id: usp10.cpp,v 1.1 2002-04-10 18:35:18 bird Exp $
 *
 * Usp10.DLL Stubs - first implement those required by swt-os2.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <winnls.h>
#include <string.h>

#include <wingdi.h>
#include <commctrl.h>

#define DBG_LOCALLOG    DBG_usp10
#include "dbglocal.h"


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define MAX_INTLIST_COUNT   10

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef HANDLE HTHEME;

typedef enum
{
    PO_STATE = 0,                       /* Property was found in the state section. */
    PO_PART = 1,                        /* Property was found in the part section. */
    PO_CLASS = 2,                       /* Property was found in the class section. */
    PO_GLOBAL = 3,                      /* Property was found in the list of global variables. */
    PO_NOTFOUND = 4                     /* Property was not found. */
} PROPERTYORIGIN;


typedef enum
{
    TS_MIN,                             /* Receives the minimum size of a visual style part. */
    TS_TRUE,                            /* Receives the size of the visual style part that will best fit the available space. */
    TS_DRAW                             /* Receives the size that the theme manager uses to draw a part. */
} THEMESIZE, THEME_SIZE;


typedef struct _INTLIST
{
    int  iValueCount;                   /* Number of values in the list.*/
    int  iValues[MAX_INTLIST_COUNT];    /* List of integers. The constant MAX_INTLIST_COUNT, by definition, is equal to 10. */
} INTLIST, *PINTLIST;


typedef struct _MARGINS
{
    int  cxLeftWidth;
    int  cxRightWidth;
    int  cyTopHeight;
    int  cyBottomHeight;
} MARGINS, *PMARGINS;



/*******************************************************************************
*   Functions Prototypes                                                       *
*******************************************************************************/
extern "C" {
/*
 * We doesn't have any headers for these yet.
 */
HRESULT WIN32API CloseThemeData(
    HTHEME          hTheme
);

HRESULT WIN32API DrawThemeBackground(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    const RECT*     pClipRect
);

HRESULT WIN32API DrawThemeEdge(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pDestRect,
    UINT            uEdge,
    UINT            uFlags,
    RECT*           pContentRect
);

HRESULT WIN32API DrawThemeIcon(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    HIMAGELIST      himl,
    int             iImageIndex
);

HRESULT WIN32API DrawThemeParentBackground(
    HWND            hwnd,
    HDC             hdc,
    RECT*           prc
);

HRESULT WIN32API DrawThemeText(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    LPCWSTR         pszText,
    int             iCharCount,
    DWORD           dwTextFlags,
    DWORD           dwTextFlags2,
    const RECT*     pRect
);

HRESULT WIN32API EnableThemeDialogTexture(
    HWND            hwnd,
    DWORD           dwFlags
);

HRESULT WIN32API EnableTheming(
    BOOL            fEnable
);

HRESULT WIN32API GetCurrentThemeName(
    LPWSTR          pszThemeFileName,
    int             dwMaxNameChars,
    LPWSTR          pszColorBuff,
    int             cchMaxColorChars,
    LPWSTR          pszSizeBuff,
    int             cchMaxSizeChars
);

DWORD WIN32API GetThemeAppProperties(VOID);

HRESULT WIN32API GetThemeBackgroundContentRect(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pBoundingRect,
    RECT*           pContentRect
);

HRESULT WIN32API GetThemeBackgroundExtent(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pContentRect,
    RECT*           pExtentRect
);

HRESULT WIN32API GetThemeBackgroundRegion(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    HRGN*           pRegion
);

HRESULT WIN32API GetThemeBool(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    BOOL*           pfVal
);

HRESULT WIN32API GetThemeColor(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    COLORREF*       pColor
);

HRESULT WIN32API GetThemeDocumentationProperty(
    LPCWSTR         pszThemeName,
    LPCWSTR         pszPropertyName,
    LPWSTR          pszValueBuff,
    int             cchMaxValChars
);

HRESULT WIN32API GetThemeEnumValue(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
);

HRESULT WIN32API GetThemeFilename(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LPWSTR          pszThemeFilename,
    int             cchMaxBuffChars
);

HRESULT WIN32API GetThemeFont(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LOGFONTW*       pFont
);

HRESULT WIN32API GetThemeInt(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
);

HRESULT WIN32API GetThemeIntList(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    INTLIST*        pIntList
);

HRESULT WIN32API GetThemeMargins(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    RECT*           prc,
    MARGINS*        pMargins
);

HRESULT WIN32API GetThemeMetric(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
);

HRESULT WIN32API GetThemePartSize(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    RECT*           prc,
    THEMESIZE       eSize,
    SIZE*           psz
);

HRESULT WIN32API GetThemePosition(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    POINT*          pPoint
);

HRESULT WIN32API GetThemePropertyOrigin(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    PROPERTYORIGIN* pOrigin
);

HRESULT WIN32API GetThemeRect(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    RECT*           pRect
);

HRESULT WIN32API GetThemeString(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LPWSTR          pszBuff,
    int             cchMaxBuffChars
);

BOOL WIN32API GetThemeSysBool(
    HTHEME          hTheme,
    int             iBoolID
);

COLORREF WIN32API GetThemeSysColor(
    HTHEME          hTheme,
    int             iColorID
);

HBRUSH WIN32API GetThemeSysColorBrush(
    HTHEME          hTheme,
    int             iColorID
);

HRESULT WIN32API GetThemeSysFont(
    HTHEME          hTheme,
    int             iFontID,
    LOGFONTW*       plf
);

HRESULT WIN32API GetThemeSysInt(
    HTHEME          hTheme,
    int             iIntID,
    int*            piValue
);

int WIN32API GetThemeSysSize(
    HTHEME          hTheme,
    int             iSizeID
);

HRESULT WIN32API GetThemeSysString(
    HTHEME          hTheme,
    int             iStringID,
    LPWSTR          pszStringBuff,
    int             cchMaxStringChars
);

HRESULT WIN32API GetThemeTextExtent(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    LPCWSTR         pszText,
    int             iCharCount,
    DWORD           dwTextFlags,
    const RECT*     pBoundingRect,
    RECT*           pExtentRect
);

HRESULT WIN32API GetThemeTextMetrics(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    TEXTMETRICW*     ptm
);

HTHEME WIN32API GetWindowTheme(
    HWND            hWnd
);

HRESULT WIN32API HitTestThemeBackground(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    DWORD           dwOptions,
    const RECT*     pRect,
    HRGN            hrgn,
    POINT           ptTest,
    WORD*           pwHitTestCode
);

BOOL WIN32API IsAppThemed(VOID);

BOOL WIN32API IsThemeActive(VOID);

BOOL WIN32API IsThemeBackgroundPartiallyTransparent(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId
);

BOOL WIN32API IsThemeDialogTextureEnabled(VOID);

BOOL WIN32API IsThemePartDefined(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId
);

HTHEME WIN32API OpenThemeData(
    HWND            hwnd,
    LPCWSTR         pszClassList
);

VOID WIN32API SetThemeAppProperties(
    DWORD           dwFlags
);

HRESULT WIN32API SetWindowTheme(
    HWND            hwnd,
    LPCWSTR         pszSubAppName,
    LPCWSTR         pszSubIdList
);

}


/**
 *
 * @returns
 * @param   hTheme
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API CloseThemeData(
    HTHEME          hTheme
)
{
    dprintf(("USP10: %s(%x) - stub\n",  __FUNCTION__, hTheme));
    return S_OK;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pRect
 * @param   pClipRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API DrawThemeBackground(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    const RECT*     pClipRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pRect,
             pClipRect));
    return S_OK;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pDestRect
 * @param   uEdge
 * @param   uFlags
 * @param   pContentRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API DrawThemeEdge(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pDestRect,
    UINT            uEdge,
    UINT            uFlags,
    RECT*           pContentRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pDestRect,
             uEdge,
             uFlags,
             pContentRect));
    return S_OK;
}




/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pRect
 * @param   himl
 * @param   iImageIndex
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API DrawThemeIcon(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    HIMAGELIST      himl,
    int             iImageIndex
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pRect,
             himl,
             iImageIndex));
    return S_OK;
}


/**
 *
 * @returns
 * @param   hwnd
 * @param   hdc
 * @param   prc
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API    DrawThemeParentBackground(
    HWND            hwnd,
    HDC             hdc,
    RECT*           prc
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hwnd,
             hdc,
             prc));
    return S_OK;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pszText
 * @param   iCharCount
 * @param   dwTextFlags
 * @param   dwTextFlags2
 * @param   pRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API DrawThemeText(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    LPCWSTR         pszText,
    int             iCharCount,
    DWORD           dwTextFlags,
    DWORD           dwTextFlags2,
    const RECT*     pRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pszText,
             iCharCount,
             dwTextFlags,
             dwTextFlags2,
             pRect));
    return S_OK;
}


/**
 *
 * @returns
 * @param   hwnd
 * @param   dwFlags
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API    EnableThemeDialogTexture(
    HWND            hwnd,
    DWORD           dwFlags
)
{
    dprintf(("USP10: %s(%x,%x) - stub\n",
             __FUNCTION__,
             hwnd,
             dwFlags));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   fEnable
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API EnableTheming(
    BOOL            fEnable
)
{
    dprintf(("USP10: %s(%x) - stub\n",
             __FUNCTION__,
             fEnable));
    return S_OK;
}


/**
 *
 * @returns
 * @param   pszThemeFileName
 * @param   dwMaxNameChars
 * @param   pszColorBuff
 * @param   cchMaxColorChars
 * @param   pszSizeBuff
 * @param   cchMaxSizeChars
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetCurrentThemeName(
    LPWSTR          pszThemeFileName,
    int             dwMaxNameChars,
    LPWSTR          pszColorBuff,
    int             cchMaxColorChars,
    LPWSTR          pszSizeBuff,
    int             cchMaxSizeChars
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             pszThemeFileName,
             dwMaxNameChars,
             pszColorBuff,
             cchMaxColorChars,
             pszSizeBuff,
             cchMaxSizeChars));
    return S_FALSE;
}


/**
 *
 * @returns
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
DWORD WIN32API GetThemeAppProperties(VOID)
{
    dprintf(("USP10: %s() - stub\n", __FUNCTION__));
    return 0;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pBoundingRect
 * @param   pContentRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeBackgroundContentRect(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pBoundingRect,
    RECT*           pContentRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pBoundingRect,
             pContentRect));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pContentRect
 * @param   pExtentRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeBackgroundExtent(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pContentRect,
    RECT*           pExtentRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pContentRect,
             pExtentRect));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pRect
 * @param   pRegion
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeBackgroundRegion(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    const RECT*     pRect,
    HRGN*           pRegion
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pRect,
             pRegion));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pfVal
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeBool(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    BOOL*           pfVal
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pfVal));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pColor
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeColor(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    COLORREF*       pColor
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pColor));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   pszThemeName
 * @param   pszPropertyName
 * @param   pszValueBuff
 * @param   cchMaxValChars
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeDocumentationProperty(
    LPCWSTR         pszThemeName,
    LPCWSTR         pszPropertyName,
    LPWSTR          pszValueBuff,
    int             cchMaxValChars
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             pszThemeName,
             pszPropertyName,
             pszValueBuff,
             cchMaxValChars));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   piVal
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeEnumValue(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             piVal));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pszThemeFilename
 * @param   cchMaxBuffChars
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeFilename(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LPWSTR          pszThemeFilename,
    int             cchMaxBuffChars
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pszThemeFilename,
             cchMaxBuffChars));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pFont
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeFont(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LOGFONTW*       pFont
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             iPropId,
             pFont));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   piVal
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeInt(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             piVal));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pIntList
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeIntList(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    INTLIST*        pIntList
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pIntList));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   prc
 * @param   pMargins
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeMargins(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    RECT*           prc,
    MARGINS*        pMargins
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             iPropId,
             prc,
             pMargins));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   piVal
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeMetric(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    int*            piVal
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             iPropId,
             piVal));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   prc
 * @param   eSize
 * @param   psz
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemePartSize(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    RECT*           prc,
    THEMESIZE       eSize,
    SIZE*           psz
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             prc,
             eSize,
             psz));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pPoint
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemePosition(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    POINT*          pPoint
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pPoint));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pOrigin
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemePropertyOrigin(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    PROPERTYORIGIN* pOrigin
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pOrigin));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeRect(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    RECT*           pRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pRect));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @param   iPropId
 * @param   pszBuff
 * @param   cchMaxBuffChars
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeString(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId,
    int             iPropId,
    LPWSTR          pszBuff,
    int             cchMaxBuffChars
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId,
             iPropId,
             pszBuff,
             cchMaxBuffChars));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iBoolID
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
BOOL WIN32API GetThemeSysBool(
    HTHEME          hTheme,
    int             iBoolID
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iBoolID));
    return FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iColorID
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
COLORREF WIN32API GetThemeSysColor(
    HTHEME          hTheme,
    int             iColorID
)
{
    dprintf(("USP10: %s(%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iColorID));
    return 0;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iColorID
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HBRUSH WIN32API GetThemeSysColorBrush(
    HTHEME          hTheme,
    int             iColorID
)
{
    dprintf(("USP10: %s(%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iColorID));
    return NULL;
}



/**
 *
 * @returns
 * @param   hTheme
 * @param   iFontID
 * @param   plf
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeSysFont(
    HTHEME          hTheme,
    int             iFontID,
    LOGFONTW*       plf
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iFontID,
             plf));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iIntID
 * @param   piValue
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeSysInt(
    HTHEME          hTheme,
    int             iIntID,
    int*            piValue
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iIntID,
             piValue));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iSizeID
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
int WIN32API GetThemeSysSize(
    HTHEME          hTheme,
    int             iSizeID
)
{
    dprintf(("USP10: %s(%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iSizeID));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iStringID
 * @param   pszStringBuff
 * @param   cchMaxStringChars
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeSysString(
    HTHEME          hTheme,
    int             iStringID,
    LPWSTR          pszStringBuff,
    int             cchMaxStringChars
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iStringID,
             pszStringBuff,
             cchMaxStringChars));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   pszText
 * @param   iCharCount
 * @param   dwTextFlags
 * @param   pBoundingRect
 * @param   pExtentRect
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeTextExtent(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    LPCWSTR         pszText,
    int             iCharCount,
    DWORD           dwTextFlags,
    const RECT*     pBoundingRect,
    RECT*           pExtentRect
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             pszText,
             iCharCount,
             dwTextFlags,
             pBoundingRect,
             pExtentRect));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   ptm
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API GetThemeTextMetrics(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    TEXTMETRICW*    ptm
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             ptm ));
    return S_FALSE;
}


/**
 *
 * @returns
 * @param   hWnd
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HTHEME WIN32API GetWindowTheme(
    HWND            hWnd
)
{
    dprintf(("USP10: %s(%x) - stub\n",
             __FUNCTION__,
             hWnd));
    return NULL;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   hdc
 * @param   iPartId
 * @param   iStateId
 * @param   dwOptions
 * @param   pRect
 * @param   hrgn
 * @param   ptTest
 * @param   pwHitTestCode
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API HitTestThemeBackground(
    HTHEME          hTheme,
    HDC             hdc,
    int             iPartId,
    int             iStateId,
    DWORD           dwOptions,
    const RECT*     pRect,
    HRGN            hrgn,
    POINT           ptTest,
    WORD*           pwHitTestCode
)
{
    dprintf(("USP10: %s(%x,%x,%x,%x,%x,%x,%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             hdc,
             iPartId,
             iStateId,
             dwOptions,
             pRect,
             hrgn,
             ptTest,
             pwHitTestCode));
    return S_OK;
}


/**
 *
 * @returns
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
BOOL WIN32API IsAppThemed(VOID)
{
    dprintf(("USP10: %s() - stub\n", __FUNCTION__));
    return FALSE;
}


/**
 *
 * @returns
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 * @remark
 */
BOOL WIN32API IsThemeActive(VOID)
{
    dprintf(("USP10: %s() - stub\n", __FUNCTION__));
    return FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @status  stub.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
BOOL WIN32API IsThemeBackgroundPartiallyTransparent(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId));
    return FALSE;
}

/**
 *
 * @returns
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
BOOL WIN32API IsThemeDialogTextureEnabled(VOID)
{
    dprintf(("USP10: %s() - stub\n", __FUNCTION__));
    return FALSE;
}


/**
 *
 * @returns
 * @param   hTheme
 * @param   iPartId
 * @param   iStateId
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
BOOL WIN32API IsThemePartDefined(
    HTHEME          hTheme,
    int             iPartId,
    int             iStateId
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hTheme,
             iPartId,
             iStateId));
    return FALSE;
}


/**
 *
 * @returns
 * @param   hwnd
 * @param   pszClassList
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HTHEME OpenThemeData(
    HWND            hwnd,
    LPCWSTR         pszClassList
)
{
    dprintf(("USP10: %s(%x, %x) - stub - returns NULL\n",
             __FUNCTION__,
             hwnd,
             pszClassList));
    return NULL;
}


/**
 *
 * @returns
 * @param   dwFlags
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
VOID WIN32API SetThemeAppProperties(
    DWORD           dwFlags
)
{
    dprintf(("USP10: %s(%x) - stub\n",
             __FUNCTION__,
             dwFlags));
}


/**
 *
 * @returns
 * @param   hwnd
 * @param   pszSubAppName
 * @param   pszSubIdList
 * @status  stub
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HRESULT WIN32API SetWindowTheme(
    HWND            hwnd,
    LPCWSTR         pszSubAppName,
    LPCWSTR         pszSubIdList
)
{
    dprintf(("USP10: %s(%x,%x,%x) - stub\n",
             __FUNCTION__,
             hwnd,
             pszSubAppName,
             pszSubIdList));
    return S_FALSE;
}
