/* $Id: toolbar.cpp,v 1.8 2000-08-06 14:40:17 sandervl Exp $ */
/*
 * Toolbar control
 *
 * Copyright 1998,1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * TODO:
 *   - A little bug in TOOLBAR_DrawMasked()
 *   - Button wrapping (under construction).
 *   - Messages.
 *   - Notifications (under construction).
 *   - Fix TB_SETROWS.
 *   - Tooltip support (almost complete).
 *   - Unicode suppport (under construction).
 *   - Fix TOOLBAR_SetButtonInfo32A/W.
 *   - Customize dialog (under construction).
 *   - TBSTYLE_AUTOSIZE for toolbar and buttons.
 *   - I_IMAGECALLBACK support.
 *
 * Testing:
 *   - Run tests using Waite Group Windows95 API Bible Volume 2.
 *     The second cdrom contains executables addstr.exe, btncount.exe,
 *     btnstate.exe, butstrsz.exe, chkbtn.exe, chngbmp.exe, customiz.exe,
 *     enablebtn.exe, getbmp.exe, getbtn.exe, getflags.exe, hidebtn.exe,
 *     indetbtn.exe, insbtn.exe, pressbtn.exe, setbtnsz.exe, setcmdid.exe,
 *     setparnt.exe, setrows.exe, toolwnd.exe.
 *   - Microsofts controlspy examples.
 */

/*
 - (WINE 20000608 level)
*/


#include <string.h>

#include "winbase.h"
#include "commctrl.h"
#include "cache.h"
#include "comctl32.h"
#include "ccbase.h"
#include "toolbar.h"

#define SEPARATOR_WIDTH    8
#define TOP_BORDER         2
#define BOTTOM_BORDER      2

#define TOOLBAR_GetInfoPtr(hwnd) ((TOOLBAR_INFO *)GetWindowLongA(hwnd,0))

static BOOL 
TOOLBAR_IsValidBitmapIndex(TOOLBAR_INFO *infoPtr, INT index)
{
    if ((index>=0) && (index < infoPtr->nNumBitmaps))
      return TRUE;
    else
      return FALSE;
}


static void
TOOLBAR_DrawFlatSeparator (LPRECT lpRect, HDC hdc)
{
    INT x = (lpRect->left + lpRect->right) / 2 - 1;
    INT yBottom = lpRect->bottom - 3;
    INT yTop = lpRect->top + 1;

    SelectObject ( hdc, GetSysColorPen (COLOR_3DSHADOW));
    MoveToEx (hdc, x, yBottom, NULL);
    LineTo (hdc, x, yTop);
    x++;
    SelectObject ( hdc, GetSysColorPen (COLOR_3DHILIGHT));
    MoveToEx (hdc, x, yBottom, NULL);
    LineTo (hdc, x, yTop);
}

/*
 * Draw the text string for this button.
 * note: infoPtr->himlDis *SHOULD* be non-zero when infoPtr->himlDef
 * 	is non-zero, so we can simply check himlDef to see if we have
 *      an image list
 */
static void
TOOLBAR_DrawString (TOOLBAR_INFO *infoPtr, TBUTTON_INFO *btnPtr,
		    HDC hdc, INT nState, DWORD dwStyle)
{
    RECT   rcText = btnPtr->rect;
    HFONT  hOldFont;
    INT    nOldBkMode;
    COLORREF clrOld;
    LPWSTR lpText = NULL;
    HIMAGELIST himl = infoPtr->himlDef;

    // TRACE ("iString: %x\n", btnPtr->iString);

    /* get a pointer to the text */
    if (btnPtr->iString == -1)
        dprintf(("TOOLBAR_DrawString: Undocumented Index -1\n"));
    else if (HIWORD(btnPtr->iString) != 0)
        lpText = (LPWSTR)btnPtr->iString;
    else if ((btnPtr->iString >= 0) && (btnPtr->iString < infoPtr->nNumStrings))
        lpText = infoPtr->strings[btnPtr->iString];

    // TRACE ("lpText: \"%s\"\n", debugstr_w(lpText));

    /* draw text */
    if (lpText) {

	InflateRect (&rcText, -3, -3);

	if (himl && TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap)) {
		if ((dwStyle & TBSTYLE_LIST) &&
		    ((btnPtr->fsStyle & TBSTYLE_AUTOSIZE) == 0) &&
		    (btnPtr->iBitmap != I_IMAGENONE)) {
		    rcText.left += infoPtr->nBitmapWidth;
		}
		else {
		    rcText.top += infoPtr->nBitmapHeight;
		}
	}

	if (nState & (TBSTATE_PRESSED | TBSTATE_CHECKED))
	    OffsetRect (&rcText, 1, 1);

	hOldFont = SelectObject (hdc, infoPtr->hFont);
	nOldBkMode = SetBkMode (hdc, TRANSPARENT);
	if (!(nState & TBSTATE_ENABLED)) {
	    clrOld = SetTextColor (hdc, GetSysColor (COLOR_3DHILIGHT));
	    OffsetRect (&rcText, 1, 1);
	    DrawTextW (hdc, lpText, -1, &rcText, infoPtr->dwDTFlags);
	    SetTextColor (hdc, GetSysColor (COLOR_3DSHADOW));
	    OffsetRect (&rcText, -1, -1);
	    DrawTextW (hdc, lpText, -1, &rcText, infoPtr->dwDTFlags);
	}
	else if (nState & TBSTATE_INDETERMINATE) {
	    clrOld = SetTextColor (hdc, GetSysColor (COLOR_3DSHADOW));
	    DrawTextW (hdc, lpText, -1, &rcText, infoPtr->dwDTFlags);
	}
	else {
	    clrOld = SetTextColor (hdc, GetSysColor (COLOR_BTNTEXT));
	    DrawTextW (hdc, lpText, -1, &rcText, infoPtr->dwDTFlags);
	}

	SetTextColor (hdc, clrOld);
	SelectObject (hdc, hOldFont);
	if (nOldBkMode != TRANSPARENT)
	    SetBkMode (hdc, nOldBkMode);
    }
}


static void
TOOLBAR_DrawPattern (HDC hdc, LPRECT lpRect)
{
    HBRUSH hbr = SelectObject (hdc, GetPattern55AABrush ());
    INT cx = lpRect->right - lpRect->left;
    INT cy = lpRect->bottom - lpRect->top;
    PatBlt (hdc, lpRect->left, lpRect->top, cx, cy, 0x00FA0089);
    SelectObject (hdc, hbr);
}


static void
TOOLBAR_DrawMasked (TOOLBAR_INFO *infoPtr, TBUTTON_INFO *btnPtr,
		    HDC hdc, INT x, INT y)
{
    /* FIXME: this function is a hack since it uses image list
	      internals directly */

    HIMAGELIST himl = infoPtr->himlDef;
    HBITMAP hbmMask;
    HDC hdcImageList;
    HDC hdcMask;

    if (!himl)
	return;

    /* create new dc's */
    hdcImageList = CreateCompatibleDC (0);
    hdcMask = CreateCompatibleDC (0);

    /* create new bitmap */
    hbmMask = CreateBitmap (himl->cx, himl->cy, 1, 1, NULL);
    SelectObject (hdcMask, hbmMask);

    /* copy the mask bitmap */
    SelectObject (hdcImageList, himl->hbmMask);
    SetBkColor (hdcImageList, RGB(255, 255, 255));
    SetTextColor (hdcImageList, RGB(0, 0, 0));
    BitBlt (hdcMask, 0, 0, himl->cx, himl->cy,
	      hdcImageList, himl->cx * btnPtr->iBitmap, 0, SRCCOPY);

#if 0
    /* add white mask from image */
    SelectObject (hdcImageList, himl->hbmImage);
    SetBkColor (hdcImageList, RGB(0, 0, 0));
    BitBlt (hdcMask, 0, 0, himl->cx, himl->cy,
	      hdcImageList, himl->cx * btnPtr->iBitmap, 0, MERGEPAINT);
#endif

    /* draw the new mask */
    SelectObject (hdc, GetSysColorBrush (COLOR_3DHILIGHT));
    BitBlt (hdc, x+1, y+1, himl->cx, himl->cy,
	      hdcMask, 0, 0, 0xB8074A);

    SelectObject (hdc, GetSysColorBrush (COLOR_3DSHADOW));
    BitBlt (hdc, x, y, himl->cx, himl->cy,
	      hdcMask, 0, 0, 0xB8074A);

    DeleteObject (hbmMask);
    DeleteDC (hdcMask);
    DeleteDC (hdcImageList);
}


static void
TOOLBAR_DrawButton (HWND hwnd, TBUTTON_INFO *btnPtr, HDC hdc)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    RECT rc;

    if (btnPtr->fsState & TBSTATE_HIDDEN)
	return;

    rc = btnPtr->rect;

    // TRACE("iBitmap: %d\n", btnPtr->iBitmap);

    /* separator */
    if (btnPtr->fsStyle & TBSTYLE_SEP) {
        /* with the FLAT style, iBitmap is the width and has already */
        /* been taken into consideration in calculating the width    */
        /* so now we need to draw the vertical separator             */
        /* empirical tests show that iBitmap can/will be non-zero    */
        /* when drawing the vertical bar...      */
        if ((dwStyle & TBSTYLE_FLAT) /* && (btnPtr->iBitmap == 0) */)
	    TOOLBAR_DrawFlatSeparator (&rc, hdc);
	return;
    }

    /* disabled */
    if (!(btnPtr->fsState & TBSTATE_ENABLED)) {
	if (!(dwStyle & TBSTYLE_FLAT))
	    DrawEdge (hdc, &rc, EDGE_RAISED,
		      BF_SOFT | BF_RECT | BF_MIDDLE | BF_ADJUST);
	
	if (infoPtr->himlDis && 
            TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlDis, btnPtr->iBitmap, hdc,
				rc.left+1, rc.top+1, ILD_NORMAL);
	else
	    TOOLBAR_DrawMasked (infoPtr, btnPtr, hdc, rc.left+1, rc.top+1);

	TOOLBAR_DrawString (infoPtr, btnPtr, hdc, btnPtr->fsState, dwStyle);
	return;
    }

    /* pressed TBSTYLE_BUTTON */
    if (btnPtr->fsState & TBSTATE_PRESSED) {
	if (dwStyle & TBSTYLE_FLAT)
	    DrawEdge (hdc, &rc, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE | BF_ADJUST);
	else
	    DrawEdge (hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_MIDDLE | BF_ADJUST);
        if (TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlDef, btnPtr->iBitmap, hdc,
			rc.left+2, rc.top+2, ILD_NORMAL);
	TOOLBAR_DrawString (infoPtr, btnPtr, hdc, btnPtr->fsState, dwStyle);
	return;
    }

    /* checked TBSTYLE_CHECK */
    if ((btnPtr->fsStyle & TBSTYLE_CHECK) &&
	(btnPtr->fsState & TBSTATE_CHECKED)) {
	if (dwStyle & TBSTYLE_FLAT)
	    DrawEdge (hdc, &rc, BDR_SUNKENOUTER,
			BF_RECT | BF_MIDDLE | BF_ADJUST);
	else
	    DrawEdge (hdc, &rc, EDGE_SUNKEN,
			BF_RECT | BF_MIDDLE | BF_ADJUST);

	TOOLBAR_DrawPattern (hdc, &rc);
        
        if (TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlDef, btnPtr->iBitmap, hdc,
			rc.left+2, rc.top+2, ILD_NORMAL);

	TOOLBAR_DrawString (infoPtr, btnPtr, hdc, btnPtr->fsState, dwStyle);
	return;
    }

    /* indeterminate */	
    if (btnPtr->fsState & TBSTATE_INDETERMINATE) {
	DrawEdge (hdc, &rc, EDGE_RAISED,
		    BF_SOFT | BF_RECT | BF_MIDDLE | BF_ADJUST);

	TOOLBAR_DrawPattern (hdc, &rc);
	TOOLBAR_DrawMasked (infoPtr, btnPtr, hdc, rc.left+1, rc.top+1);
	TOOLBAR_DrawString (infoPtr, btnPtr, hdc, btnPtr->fsState, dwStyle);
	return;
    }

    /* normal state */
    if (dwStyle & TBSTYLE_FLAT)
    {
	if (btnPtr->bHot)
	    DrawEdge (hdc, &rc, BDR_RAISEDINNER, BF_RECT | BF_MIDDLE);
	if (btnPtr->bHot && infoPtr->himlHot && 
            TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlHot, btnPtr->iBitmap, hdc,
			    rc.left +2, rc.top +2, ILD_NORMAL);
	else if (TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlDef, btnPtr->iBitmap, hdc,
			    rc.left +2, rc.top +2, ILD_NORMAL);
    }
    else
    {
	DrawEdge (hdc, &rc, EDGE_RAISED,
		BF_SOFT | BF_RECT | BF_MIDDLE | BF_ADJUST);

        if (TOOLBAR_IsValidBitmapIndex(infoPtr,btnPtr->iBitmap))
	    ImageList_Draw (infoPtr->himlDef, btnPtr->iBitmap, hdc,
			rc.left+1, rc.top+1, ILD_NORMAL);
    }

    TOOLBAR_DrawString (infoPtr, btnPtr, hdc, btnPtr->fsState, dwStyle);
}


static void
TOOLBAR_Refresh (HWND hwnd, HDC hdc, PAINTSTRUCT* ps)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT i;
    RECT rcTemp;

    /* redraw necessary buttons */
    btnPtr = infoPtr->buttons;
    for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++)
    {
        if(IntersectRect(&rcTemp, &(ps->rcPaint), &(btnPtr->rect)))
            TOOLBAR_DrawButton (hwnd, btnPtr, hdc);
    }
}

static void
TOOLBAR_MeasureString(HWND hwnd, INT index, LPSIZE lpSize)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    HDC hdc;
    HFONT hOldFont;

    lpSize->cx = 0;
    lpSize->cy = 0;
    hdc = GetDC (0);
    hOldFont = SelectObject (hdc, infoPtr->hFont);

    btnPtr = &infoPtr->buttons[index];

    if (!(btnPtr->fsState & TBSTATE_HIDDEN) &&
         (btnPtr->iString > -1) &&
         (btnPtr->iString < infoPtr->nNumStrings)) 
    {
        LPWSTR lpText = infoPtr->strings[btnPtr->iString];
        GetTextExtentPoint32W (hdc, lpText, lstrlenW (lpText), lpSize);
    }

    SelectObject (hdc, hOldFont);
    ReleaseDC (0, hdc);

    // TRACE("string size %d x %d!\n", lpSize->cx, lpSize->cy);
}

static void
TOOLBAR_CalcStrings (HWND hwnd, LPSIZE lpSize)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT i;
    SIZE sz;


    lpSize->cx = 0;
    lpSize->cy = 0;

    btnPtr = infoPtr->buttons;
    for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++) {
        TOOLBAR_MeasureString(hwnd,i,&sz);
        if (sz.cx > lpSize->cx)
            lpSize->cx = sz.cx;
        if (sz.cy > lpSize->cy)
            lpSize->cy = sz.cy;
    }

    // TRACE("string size %d x %d!\n", lpSize->cx, lpSize->cy);
}

/***********************************************************************
* 		TOOLBAR_WrapToolbar
*
* This function walks through the buttons and seperators in the 
* toolbar, and sets the TBSTATE_WRAP flag only on those items where 
* wrapping should occur based on the width of the toolbar window.  
* It does *not* calculate button placement itself.  That task 
* takes place in TOOLBAR_CalcToolbar. If the program wants to manage 
* the toolbar wrapping on it's own, it can use the TBSTYLE_WRAPPABLE 
* flag, and set the TBSTATE_WRAP flags manually on the appropriate items.
*/ 

static void
TOOLBAR_WrapToolbar( HWND hwnd, DWORD dwStyle )
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT x, cx, i, j;
    RECT rc;
    BOOL bWrap, bButtonWrap;

    /* 	When the toolbar window style is not TBSTYLE_WRAPABLE,	*/ 
    /*	no layout is necessary. Applications may use this style */
    /*	to perform their own layout on the toolbar. 		*/
    if( !(dwStyle & TBSTYLE_WRAPABLE) )
	return;

    btnPtr = infoPtr->buttons;
    x  = infoPtr->nIndent;

    /* this can get the parents width, to know how far we can extend
     * this toolbar.  We cannot use its height, as there may be multiple
     * toolbars in a rebar control
     */
    GetClientRect( GetParent(hwnd), &rc );
    infoPtr->nWidth = rc.right - rc.left;
    bButtonWrap = FALSE;

    for (i = 0; i < infoPtr->nNumButtons; i++ )
    {
	bWrap = FALSE;
	btnPtr[i].fsState &= ~TBSTATE_WRAP;
	
	if (btnPtr[i].fsState & TBSTATE_HIDDEN)
	    continue;

	/* UNDOCUMENTED: If a separator has a non zero bitmap index, */
	/* it is the actual width of the separator. This is used for */
	/* custom controls in toolbars.                              */
	if (btnPtr[i].fsStyle & TBSTYLE_SEP)
	    cx = (btnPtr[i].iBitmap > 0) ?  
			btnPtr[i].iBitmap : SEPARATOR_WIDTH;
	else
	    cx = infoPtr->nButtonWidth;

	/* Two or more adjacent separators form a separator group.   */ 
	/* The first separator in a group should be wrapped to the   */
	/* next row if the previous wrapping is on a button.	     */
	if( bButtonWrap &&
		(btnPtr[i].fsStyle & TBSTYLE_SEP) && 
		(i + 1 < infoPtr->nNumButtons ) &&
		(btnPtr[i + 1].fsStyle & TBSTYLE_SEP) ) 
	{
	    btnPtr[i].fsState |= TBSTATE_WRAP;
	    x = infoPtr->nIndent;
	    i++;
	    bButtonWrap = FALSE;
	    continue;
	}

	/* The layout makes sure the bitmap is visible, but not the button. */
	if ( x + cx - (infoPtr->nButtonWidth - infoPtr->nBitmapWidth) / 2 
		 > infoPtr->nWidth ) 
	{
	    BOOL bFound = FALSE;

	    /* 	If the current button is a separator and not hidden,  */ 
	    /*	go to the next until it reaches a non separator.      */
	    /*	Wrap the last separator if it is before a button.     */
	    while( ( (btnPtr[i].fsStyle & TBSTYLE_SEP) || 
			(btnPtr[i].fsState & TBSTATE_HIDDEN) ) && 
			i < infoPtr->nNumButtons )
	    {
		i++;
		bFound = TRUE;
	    }
    
	    if( bFound && i < infoPtr->nNumButtons )
	    {
		i--;
		btnPtr[i].fsState |= TBSTATE_WRAP;
		x = infoPtr->nIndent;
		bButtonWrap = FALSE;
		continue;
	    }
	    else if ( i >= infoPtr->nNumButtons)
		break;

	    /* 	If the current button is not a separator, find the last  */ 
	    /*	separator and wrap it.   				 */
	    for ( j = i - 1; j >= 0  &&  !(btnPtr[j].fsState & TBSTATE_WRAP); j--)
	    {
		if ((btnPtr[j].fsStyle & TBSTYLE_SEP) &&
			!(btnPtr[j].fsState & TBSTATE_HIDDEN))
		{
		    bFound = TRUE; 
		    i = j; 
		    x = infoPtr->nIndent;
		    btnPtr[j].fsState |= TBSTATE_WRAP;
		    bButtonWrap = FALSE; 
		    break;
		}
	    }

	    /* 	If no separator available for wrapping, wrap one of 	*/
	    /*  non-hidden previous button.  			     	*/
	    if (!bFound)
	    {
		for ( j = i - 1; 
			j >= 0 && !(btnPtr[j].fsState & TBSTATE_WRAP); j--)
		{
		    if (btnPtr[j].fsState & TBSTATE_HIDDEN) 
			continue;

		    bFound = TRUE; 
		    i = j; 
		    x = infoPtr->nIndent;
		    btnPtr[j].fsState |= TBSTATE_WRAP;
		    bButtonWrap = TRUE;
		    break;
		}
	    }

	    /* If all above failed, wrap the current button. */
	    if (!bFound)  
	    {
		btnPtr[i].fsState |= TBSTATE_WRAP;
		bFound = TRUE;
		x = infoPtr->nIndent;
		if (btnPtr[i].fsState & TBSTYLE_SEP )
		    bButtonWrap = FALSE;
		else
		    bButtonWrap = TRUE;
	    }		    
	}
	else
	    x += cx;
    }
}
											
/***********************************************************************
* 		TOOLBAR_CalcToolbar
*
* This function calculates button and separator placement. It first 
* calculates the button sizes, gets the toolbar window width and then 
* calls TOOLBAR_WrapToolbar to determine which buttons we need to wrap 
* on. It assigns a new location to each item and sends this location to
* the tooltip window if appropriate. Finally, it updates the rcBound 
* rect and calculates the new required toolbar window height. 
*/  

static void
TOOLBAR_CalcToolbar (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    TBUTTON_INFO *btnPtr;
    INT i, nRows, nSepRows;
    INT x, y, cx, cy;
    SIZE  sizeString;
    BOOL bWrap;

    TOOLBAR_CalcStrings (hwnd, &sizeString);

    if (dwStyle & TBSTYLE_LIST) {
	infoPtr->nButtonHeight = max(infoPtr->nBitmapHeight, sizeString.cy) + 6;
	infoPtr->nButtonWidth = infoPtr->nBitmapWidth + sizeString.cx + 6;
    }
    else {
    BOOL usesBitmaps = FALSE;
    INT i;

    for (i = 0; i < infoPtr->nNumButtons && !usesBitmaps; i++)
	if (TOOLBAR_IsValidBitmapIndex(infoPtr,infoPtr->buttons[i].iBitmap))
	    usesBitmaps = TRUE;

    if (sizeString.cy > 0) {
        if (usesBitmaps)
	  infoPtr->nButtonHeight = sizeString.cy + infoPtr->nBitmapHeight + 6;
        else 
          infoPtr->nButtonHeight = sizeString.cy + 6;
    }
    else if (infoPtr->nButtonHeight < infoPtr->nBitmapHeight + 6)
	infoPtr->nButtonHeight = infoPtr->nBitmapHeight + 6;

    if (sizeString.cx > infoPtr->nBitmapWidth)
	infoPtr->nButtonWidth = sizeString.cx + 6;
    else if (infoPtr->nButtonWidth < infoPtr->nBitmapWidth + 6)
	infoPtr->nButtonWidth = infoPtr->nBitmapWidth + 6;
    }

    if ( infoPtr->cxMin >= 0 && infoPtr->nButtonWidth < infoPtr->cxMin )
        infoPtr->nButtonWidth = infoPtr->cxMin;
    if ( infoPtr->cxMax >= 0 && infoPtr->nButtonWidth > infoPtr->cxMax )
        infoPtr->nButtonWidth = infoPtr->cxMax;

    TOOLBAR_WrapToolbar( hwnd, dwStyle );

    x  = infoPtr->nIndent;
    y  = (dwStyle & TBSTYLE_FLAT) ? 0 : TOP_BORDER;

   /*
    * We wills et the height below, and we set the width on entry 
    * so we do not reset them here.. 
    */
#if 0
    GetClientRect( hwnd, &rc );
    /* get initial values for toolbar */
    infoPtr->nWidth  = rc.right - rc.left;
    infoPtr->nHeight = rc.bottom - rc.top;
#endif

    /* from above, minimum is a button, and possible text */
    cx = infoPtr->nButtonWidth;
    /* cannot use just ButtonHeight, we may have no buttons! */
    if (infoPtr->nNumButtons > 0)
      infoPtr->nHeight = infoPtr->nButtonHeight;
    cy = infoPtr->nHeight;

    nRows = nSepRows = 0;

    infoPtr->rcBound.top = y;
    infoPtr->rcBound.left = x;
    infoPtr->rcBound.bottom = y + cy;
    infoPtr->rcBound.right = x;

    btnPtr = infoPtr->buttons;

    /* do not base height/width on parent, if the parent is a */
    /* rebar control it could have multiple rows of toolbars  */
/*    GetClientRect( GetParent(hwnd), &rc ); */
/*    cx = rc.right - rc.left; */
/*    cy = rc.bottom - rc.top; */

    for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++ )
    {
	bWrap = FALSE;
	if (btnPtr->fsState & TBSTATE_HIDDEN)
	{
	    SetRectEmpty (&btnPtr->rect);
	    continue;
	}

	    /* UNDOCUMENTED: If a separator has a non zero bitmap index, */
	    /* it is the actual width of the separator. This is used for */
	    /* custom controls in toolbars.                              */
	if (btnPtr->fsStyle & TBSTYLE_SEP)
		cx = (btnPtr->iBitmap > 0) ?
		     btnPtr->iBitmap : SEPARATOR_WIDTH;
	else {
            if (btnPtr->fsStyle & TBSTYLE_AUTOSIZE) 
            {
              SIZE sz;
              TOOLBAR_MeasureString(hwnd,i,&sz);
              cx = sz.cx + 6;
            }
            else
	      cx = infoPtr->nButtonWidth;
	}
	cy = infoPtr->nHeight;

	if (btnPtr->fsState & TBSTATE_WRAP )
		    bWrap = TRUE;

	SetRect (&btnPtr->rect, x, y, x + cx, y + cy);

	if (infoPtr->rcBound.left > x)
	    infoPtr->rcBound.left = x;
	if (infoPtr->rcBound.right < x + cx)
	    infoPtr->rcBound.right = x + cx;
	if (infoPtr->rcBound.bottom < y + cy)
	    infoPtr->rcBound.bottom = y + cy;

	/* Set the toolTip only for non-hidden, non-separator button */
	if (infoPtr->hwndToolTip && !(btnPtr->fsStyle & TBSTYLE_SEP )) 
	{
	    TTTOOLINFOA ti;

	    ZeroMemory (&ti, sizeof(TTTOOLINFOA));
	    ti.cbSize = sizeof(TTTOOLINFOA);
	    ti.hwnd = hwnd;
	    ti.uId = btnPtr->idCommand;
	    ti.rect = btnPtr->rect;
	    SendMessageA (infoPtr->hwndToolTip, TTM_NEWTOOLRECTA,
			    0, (LPARAM)&ti);
	}

	/* btnPtr->nRow is zero based. The space between the rows is 	*/
	/* also considered as a row. 					*/
	btnPtr->nRow = nRows + nSepRows;
	if( bWrap )
	{
	    if ( !(btnPtr->fsStyle & TBSTYLE_SEP) )
	    y += cy;
	    else 
	    {   
		/* UNDOCUMENTED: If a separator has a non zero bitmap index, */
		/* it is the actual width of the separator. This is used for */
		/* custom controls in toolbars. 			     */
		y += cy + ( (btnPtr->iBitmap > 0 ) ? 
			btnPtr->iBitmap : SEPARATOR_WIDTH) * 2 /3; 
	     
		/* nSepRows is used to calculate the extra height follwoing  */ 	 
		/* the last row.					     */
		nSepRows++;
	    }
	    x = infoPtr->nIndent;
		nRows++;
	}
	else
	    x += cx;
    }

    /* infoPtr->nRows is the number of rows on the toolbar */
    infoPtr->nRows = nRows + nSepRows + 1;

    /* nSepRows * (infoPtr->nBitmapHeight + 1) is the space following 	*/
    /* the last row. 							*/
    infoPtr->nHeight = TOP_BORDER + (nRows + 1) * infoPtr->nButtonHeight + 
		       	nSepRows * (SEPARATOR_WIDTH * 2 / 3) +
			nSepRows * (infoPtr->nBitmapHeight + 1) + 
			BOTTOM_BORDER; 
    // TRACE("toolbar height %d\n", infoPtr->nHeight);
}


static INT
TOOLBAR_InternalHitTest (HWND hwnd, LPPOINT lpPt)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT i;
    
    btnPtr = infoPtr->buttons;
    for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++) {
	if (btnPtr->fsState & TBSTATE_HIDDEN)
	    continue;

	if (btnPtr->fsStyle & TBSTYLE_SEP) {
	    if (PtInRect (&btnPtr->rect, *lpPt)) {
		// TRACE(" ON SEPARATOR %d!\n", i);
		return -i;
	    }
	}
	else {
	    if (PtInRect (&btnPtr->rect, *lpPt)) {
		// TRACE(" ON BUTTON %d!\n", i);
		return i;
	    }
	}
    }

    // TRACE(" NOWHERE!\n");
    return -1;
}


static INT
TOOLBAR_GetButtonIndex (TOOLBAR_INFO *infoPtr, INT idCommand)
{
    TBUTTON_INFO *btnPtr;
    INT i;

    btnPtr = infoPtr->buttons;
    for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++) {
	if (btnPtr->idCommand == idCommand) {
	    // TRACE("command=%d index=%d\n", idCommand, i);
	    return i;
	}
    }
    // TRACE("no index found for command=%d\n", idCommand);
    return -1;
}


static INT
TOOLBAR_GetCheckedGroupButtonIndex (TOOLBAR_INFO *infoPtr, INT nIndex)
{
    TBUTTON_INFO *btnPtr;
    INT nRunIndex;

    if ((nIndex < 0) || (nIndex > infoPtr->nNumButtons))
	return -1;

    /* check index button */
    btnPtr = &infoPtr->buttons[nIndex];
    if ((btnPtr->fsStyle & TBSTYLE_CHECKGROUP) == TBSTYLE_CHECKGROUP) {
	if (btnPtr->fsState & TBSTATE_CHECKED)
	    return nIndex;
    }

    /* check previous buttons */
    nRunIndex = nIndex - 1;
    while (nRunIndex >= 0) {
	btnPtr = &infoPtr->buttons[nRunIndex];
	if ((btnPtr->fsStyle & TBSTYLE_CHECKGROUP) == TBSTYLE_CHECKGROUP) {
	    if (btnPtr->fsState & TBSTATE_CHECKED)
		return nRunIndex;
	}
	else
	    break;
	nRunIndex--;
    }

    /* check next buttons */
    nRunIndex = nIndex + 1;
    while (nRunIndex < infoPtr->nNumButtons) {
	btnPtr = &infoPtr->buttons[nRunIndex];	
	if ((btnPtr->fsStyle & TBSTYLE_CHECKGROUP) == TBSTYLE_CHECKGROUP) {
	    if (btnPtr->fsState & TBSTATE_CHECKED)
		return nRunIndex;
	}
	else
	    break;
	nRunIndex++;
    }

    return -1;
}


static VOID
TOOLBAR_RelayEvent (HWND hwndTip, HWND hwndMsg, UINT uMsg,
		    WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.hwnd = hwndMsg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.time = GetMessageTime ();
    msg.pt.x = LOWORD(GetMessagePos ());
    msg.pt.y = HIWORD(GetMessagePos ());

    SendMessageA (hwndTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
}


/***********************************************************************
 * TOOLBAR_CustomizeDialogProc
 * This function implements the toolbar customization dialog.
 */
static BOOL WINAPI
TOOLBAR_CustomizeDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = (TOOLBAR_INFO *)GetWindowLongA (hwnd, DWL_USER);
    static HDSA hDsa = NULL;

    switch (uMsg)
    {
	case WM_INITDIALOG:
	    infoPtr = (TOOLBAR_INFO *)lParam;
	    SetWindowLongA (hwnd, DWL_USER, (DWORD)infoPtr);

	    hDsa = DSA_Create (sizeof(TBUTTON_INFO), 5);

	    if (infoPtr)
	    {
		TBUTTON_INFO *btnPtr;
		INT i;

		/* insert 'virtual' separator button into 'available buttons' list */
		SendDlgItemMessageA (hwnd, IDC_AVAILBTN_LBOX, LB_ADDSTRING, 0, (LPARAM)"");

		/* copy all buttons and append them to the right listbox */		
		btnPtr = infoPtr->buttons;
		for (i = 0; i < infoPtr->nNumButtons; i++, btnPtr++)
		{
		    DSA_InsertItem (hDsa, i, btnPtr);

		    /* FIXME: hidden buttons appear in the 'toolbar buttons' list too */
		    if (btnPtr->fsState & TBSTATE_HIDDEN)
		    {
			SendDlgItemMessageA (hwnd, IDC_AVAILBTN_LBOX, LB_ADDSTRING, 0, (LPARAM)"");
		    }
		    else
		    {
			SendDlgItemMessageA (hwnd, IDC_TOOLBARBTN_LBOX, LB_ADDSTRING, 0, (LPARAM)"");
		    }
		}

		/* append 'virtual' separator button to the 'toolbar buttons' list */
		/* TODO */
	    }
	    return TRUE;

	case WM_CLOSE:
	    EndDialog(hwnd, FALSE);
	    return TRUE;

	case WM_COMMAND:
	    switch (LOWORD(wParam))
	    {
		case IDCANCEL:
		    EndDialog(hwnd, FALSE);
		    break;
	    }
	    return TRUE;

	case WM_DESTROY:
	    if (hDsa)
		DSA_Destroy (hDsa);
	    return TRUE;

	case WM_DRAWITEM:
	    if (wParam == IDC_AVAILBTN_LBOX || wParam == IDC_TOOLBARBTN_LBOX)
	    {
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		TBUTTON_INFO btnPtr;
		RECT rcButton;
		RECT rcText;
		HPEN hOldPen;
		HBRUSH hOldBrush;
		COLORREF oldText = 0;
		COLORREF oldBk = 0;

		// FIXME("action: %x itemState: %x\n",
		//    lpdis->itemAction, lpdis->itemState);		

		DSA_GetItem (hDsa, 0 /*lpdis->itemID*/, &btnPtr);

		if (lpdis->itemState & ODS_FOCUS)
		{
		    oldBk = SetBkColor (lpdis->hDC, GetSysColor(COLOR_HIGHLIGHT));
		    oldText = SetTextColor (lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		}

		hOldPen = SelectObject (lpdis->hDC, GetSysColorPen ((lpdis->itemState & ODS_SELECTED)?COLOR_HIGHLIGHT:COLOR_WINDOW));
		hOldBrush = SelectObject (lpdis->hDC, GetSysColorBrush ((lpdis->itemState & ODS_FOCUS)?COLOR_HIGHLIGHT:COLOR_WINDOW));

		/* fill background rectangle */
		Rectangle (lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top,
			   lpdis->rcItem.right, lpdis->rcItem.bottom);

		/* calculate button and text rectangles */
		CopyRect (&rcButton, &lpdis->rcItem);
		InflateRect (&rcButton, -1, -1);
		CopyRect (&rcText, &rcButton);
		rcButton.right = rcButton.left + infoPtr->nBitmapWidth + 6;
		rcText.left = rcButton.right + 2;

		/* draw focus rectangle */
		if (lpdis->itemState & ODS_FOCUS)
		    DrawFocusRect (lpdis->hDC, &lpdis->rcItem);

		/* draw button */
		DrawEdge (lpdis->hDC, &rcButton, EDGE_RAISED, BF_RECT|BF_MIDDLE|BF_SOFT);

		/* draw image and text */
		if (wParam == IDC_AVAILBTN_LBOX && lpdis->itemID == 0)
		{
		    /* virtual separator in the 'available' list */
		    DrawTextA (lpdis->hDC, "Separator", -1, &rcText,
			       DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
		    /* real button */

		    ImageList_Draw (infoPtr->himlDef, btnPtr.iBitmap, lpdis->hDC,
				    rcButton.left+1, rcButton.top+1, ILD_NORMAL);

		    DrawTextW (lpdis->hDC,  infoPtr->strings[btnPtr.iString], -1, &rcText,
			       DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		}

		if (lpdis->itemState & ODS_FOCUS)
		{
		    SetBkColor (lpdis->hDC, oldBk);
		    SetTextColor (lpdis->hDC, oldText);
		}

		SelectObject (lpdis->hDC, hOldBrush);
		SelectObject (lpdis->hDC, hOldPen);

		return TRUE;
	    }
	    return FALSE;

	case WM_MEASUREITEM:
	    if (wParam == IDC_AVAILBTN_LBOX || wParam == IDC_TOOLBARBTN_LBOX)
	    {
		MEASUREITEMSTRUCT *lpmis = (MEASUREITEMSTRUCT*)lParam;

		if (infoPtr)
		    lpmis->itemHeight = infoPtr->nBitmapHeight + 8;
		else
		    lpmis->itemHeight = 15 + 8; /* default height */

		return TRUE;
	    }
	    return FALSE;

	default:
	    return FALSE;
    }
}


/***********************************************************************
 * TOOLBAR_AddBitmap:  Add the bitmaps to the default image list.
 *
 */
static LRESULT
TOOLBAR_AddBitmap (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBADDBITMAP lpAddBmp = (LPTBADDBITMAP)lParam;
    INT nIndex = 0, nButtons, nCount;
    HBITMAP hbmLoad;

    // TRACE("hwnd=%x wParam=%x lParam=%lx\n", hwnd, wParam, lParam);
    if (!lpAddBmp)
	return -1;

    if (lpAddBmp->hInst == HINST_COMMCTRL)
    {
	if ((lpAddBmp->nID & ~1) == IDB_STD_SMALL_COLOR)
	    nButtons = 15;
	else if ((lpAddBmp->nID & ~1) == IDB_VIEW_SMALL_COLOR)
	    nButtons = 13;
	else if ((lpAddBmp->nID & ~1) == IDB_HIST_SMALL_COLOR)
	    nButtons = 5;
	else
	    return -1;

	// TRACE ("adding %d internal bitmaps!\n", nButtons);

	/* Windows resize all the buttons to the size of a newly added standard image */
	if (lpAddBmp->nID & 1) 
	{
	    /* large icons */
	    SendMessageA (hwnd, TB_SETBITMAPSIZE, 0,
			  MAKELPARAM((WORD)26, (WORD)26));
	    SendMessageA (hwnd, TB_SETBUTTONSIZE, 0,
			  MAKELPARAM((WORD)33, (WORD)33));
	}	
	else 
	{
	    /* small icons */
	    SendMessageA (hwnd, TB_SETBITMAPSIZE, 0,
			  MAKELPARAM((WORD)16, (WORD)16));
	    SendMessageA (hwnd, TB_SETBUTTONSIZE, 0,
			  MAKELPARAM((WORD)22, (WORD)22));
	}
	
	TOOLBAR_CalcToolbar (hwnd);
    }
    else
    {
	nButtons = (INT)wParam;
	if (nButtons <= 0)
	    return -1;
	
	// TRACE ("adding %d bitmaps!\n", nButtons);
    }
    
    if (!(infoPtr->himlDef)) {
	/* create new default image list */
	// TRACE ("creating default image list!\n");

	infoPtr->himlDef =
	    ImageList_Create (infoPtr->nBitmapWidth, infoPtr->nBitmapHeight,
			      ILC_COLOR | ILC_MASK, nButtons, 2);
	infoPtr->himlInt = infoPtr->himlDef;
    }

    nCount = ImageList_GetImageCount(infoPtr->himlDef);

    /* Add bitmaps to the default image list */
    if (lpAddBmp->hInst == (HINSTANCE)0)
    {
	nIndex = 
	    ImageList_AddMasked (infoPtr->himlDef, (HBITMAP)lpAddBmp->nID,
				 CLR_DEFAULT);
    }
    else if (lpAddBmp->hInst == HINST_COMMCTRL)
    {
	/* Add system bitmaps */
	switch (lpAddBmp->nID)
    {
	    case IDB_STD_SMALL_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_STD_SMALL));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    case IDB_STD_LARGE_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_STD_LARGE));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    case IDB_VIEW_SMALL_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_VIEW_SMALL));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    case IDB_VIEW_LARGE_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_VIEW_LARGE));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    case IDB_HIST_SMALL_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_HIST_SMALL));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    case IDB_HIST_LARGE_COLOR:
		hbmLoad = LoadBitmapA (COMCTL32_hModule,
				       MAKEINTRESOURCEA(IDB_HIST_LARGE));
		nIndex = ImageList_AddMasked (infoPtr->himlDef,
					      hbmLoad, CLR_DEFAULT);
		DeleteObject (hbmLoad);
		break;

	    default:
	nIndex = ImageList_GetImageCount (infoPtr->himlDef);
	// ERR ("invalid imagelist!\n");
		break;
	}
    }
    else
    {
	hbmLoad = LoadBitmapA (lpAddBmp->hInst, (LPSTR)lpAddBmp->nID);
	nIndex = ImageList_AddMasked (infoPtr->himlDef, hbmLoad, CLR_DEFAULT);
	DeleteObject (hbmLoad);
    }

    if (nIndex != -1)
    {
       INT imagecount = ImageList_GetImageCount(infoPtr->himlDef);

       if (infoPtr->nNumBitmaps + nButtons != imagecount)
       {
         dprintf(("TOOLBAR_AddBitmap: Desired images do not match recieved images : Previous image number %i Previous images in list %i  added %i expecting total %i, Images in list %i\n",
	      infoPtr->nNumBitmaps, nCount, imagecount - nCount,
	      infoPtr->nNumBitmaps+nButtons,imagecount));

	 infoPtr->nNumBitmaps = imagecount;
       }
       else
         infoPtr->nNumBitmaps += nButtons;
    }

    return nIndex;
}


static LRESULT
TOOLBAR_AddButtonsA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTON lpTbb = (LPTBBUTTON)lParam;
    INT nOldButtons, nNewButtons, nAddButtons, nCount;

    // TRACE("adding %d buttons!\n", wParam);

    nAddButtons = (UINT)wParam;
    nOldButtons = infoPtr->nNumButtons;
    nNewButtons = nOldButtons + nAddButtons;

    if (infoPtr->nNumButtons == 0) {
	infoPtr->buttons = 
	    (TBUTTON_INFO*) COMCTL32_Alloc (sizeof(TBUTTON_INFO) * nNewButtons);
    }
    else {
	TBUTTON_INFO *oldButtons = infoPtr->buttons;
	infoPtr->buttons =
	    (TBUTTON_INFO*) COMCTL32_Alloc (sizeof(TBUTTON_INFO) * nNewButtons);
	memcpy (&infoPtr->buttons[0], &oldButtons[0],
		nOldButtons * sizeof(TBUTTON_INFO));
        COMCTL32_Free (oldButtons);
    }

    infoPtr->nNumButtons = nNewButtons;

    /* insert new button data */
    for (nCount = 0; nCount < nAddButtons; nCount++) {
	TBUTTON_INFO *btnPtr = &infoPtr->buttons[nOldButtons+nCount];
	btnPtr->iBitmap   = lpTbb[nCount].iBitmap;
	btnPtr->idCommand = lpTbb[nCount].idCommand;
	btnPtr->fsState   = lpTbb[nCount].fsState;
	btnPtr->fsStyle   = lpTbb[nCount].fsStyle;
	btnPtr->dwData    = lpTbb[nCount].dwData;
	btnPtr->iString   = lpTbb[nCount].iString;
	btnPtr->bHot      = FALSE;

	if ((infoPtr->hwndToolTip) && !(btnPtr->fsStyle & TBSTYLE_SEP)) {
	    TTTOOLINFOA ti;

	    ZeroMemory (&ti, sizeof(TTTOOLINFOA));
	    ti.cbSize   = sizeof (TTTOOLINFOA);
	    ti.hwnd     = hwnd;
	    ti.uId      = btnPtr->idCommand;
	    ti.hinst    = 0;
	    ti.lpszText = LPSTR_TEXTCALLBACKA;

	    SendMessageA (infoPtr->hwndToolTip, TTM_ADDTOOLA,
			    0, (LPARAM)&ti);
	}
    }

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect(hwnd, NULL, FALSE);

    return TRUE;
}


static LRESULT
TOOLBAR_AddButtonsW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTON lpTbb = (LPTBBUTTON)lParam;
    INT nOldButtons, nNewButtons, nAddButtons, nCount;

    // TRACE("adding %d buttons!\n", wParam);

    nAddButtons = (UINT)wParam;
    nOldButtons = infoPtr->nNumButtons;
    nNewButtons = nOldButtons + nAddButtons;

    if (infoPtr->nNumButtons == 0) {
	infoPtr->buttons =
	    (TBUTTON_INFO*)COMCTL32_Alloc (sizeof(TBUTTON_INFO) * nNewButtons);
    }
    else {
	TBUTTON_INFO *oldButtons = infoPtr->buttons;
	infoPtr->buttons =
	    (TBUTTON_INFO*)COMCTL32_Alloc (sizeof(TBUTTON_INFO) * nNewButtons);
	memcpy (&infoPtr->buttons[0], &oldButtons[0],
		nOldButtons * sizeof(TBUTTON_INFO));
        COMCTL32_Free (oldButtons);
    }

    infoPtr->nNumButtons = nNewButtons;

    /* insert new button data */
    for (nCount = 0; nCount < nAddButtons; nCount++) {
	TBUTTON_INFO *btnPtr = &infoPtr->buttons[nOldButtons+nCount];
	btnPtr->iBitmap   = lpTbb[nCount].iBitmap;
	btnPtr->idCommand = lpTbb[nCount].idCommand;
	btnPtr->fsState   = lpTbb[nCount].fsState;
	btnPtr->fsStyle   = lpTbb[nCount].fsStyle;
	btnPtr->dwData    = lpTbb[nCount].dwData;
	btnPtr->iString   = lpTbb[nCount].iString;
	btnPtr->bHot      = FALSE;

	if ((infoPtr->hwndToolTip) && !(btnPtr->fsStyle & TBSTYLE_SEP)) {
	    TTTOOLINFOW ti;

	    ZeroMemory (&ti, sizeof(TTTOOLINFOW));
	    ti.cbSize   = sizeof (TTTOOLINFOW);
	    ti.hwnd     = hwnd;
	    ti.uId      = btnPtr->idCommand;
	    ti.hinst    = 0;
	    ti.lpszText = LPSTR_TEXTCALLBACKW;

	    SendMessageW (infoPtr->hwndToolTip, TTM_ADDTOOLW,
			    0, (LPARAM)&ti);
	}
    }

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect(hwnd, NULL, FALSE);

    return TRUE;
}


static LRESULT
TOOLBAR_AddStringA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    if ((wParam) && (HIWORD(lParam) == 0)) {
	char szString[256];
	INT len;
	// TRACE("adding string from resource!\n");

	len = LoadStringA ((HINSTANCE)wParam, (UINT)lParam,
			     szString, 256);

	// TRACE("len=%d \"%s\"\n", len, szString);
	nIndex = infoPtr->nNumStrings;
	if (infoPtr->nNumStrings == 0) {
	    infoPtr->strings =
		(LPWSTR*) COMCTL32_Alloc (sizeof(LPWSTR));
	}
	else {
	    LPWSTR *oldStrings = infoPtr->strings;
	    infoPtr->strings = 
		(LPWSTR*) COMCTL32_Alloc (sizeof(LPWSTR) * (infoPtr->nNumStrings + 1));
	    memcpy (&infoPtr->strings[0], &oldStrings[0],
		    sizeof(LPWSTR) * infoPtr->nNumStrings);
	    COMCTL32_Free (oldStrings);
	}

	infoPtr->strings[infoPtr->nNumStrings] =
	    (WCHAR*)COMCTL32_Alloc (sizeof(WCHAR)*(len+1));
	lstrcpyAtoW (infoPtr->strings[infoPtr->nNumStrings], szString);
	infoPtr->nNumStrings++;
    }
    else {
	LPSTR p = (LPSTR)lParam;
	INT len;

	if (p == NULL)
	    return -1;
	// TRACE("adding string(s) from array!\n");

	nIndex = infoPtr->nNumStrings;
	while (*p) {
	    len = lstrlenA (p);
	    // TRACE("len=%d \"%s\"\n", len, p);

	    if (infoPtr->nNumStrings == 0) {
		infoPtr->strings =
		   (LPWSTR*) COMCTL32_Alloc (sizeof(LPWSTR));
	    }
	    else {
		LPWSTR *oldStrings = infoPtr->strings;
		infoPtr->strings =
		    (LPWSTR*) COMCTL32_Alloc (sizeof(LPWSTR) * (infoPtr->nNumStrings + 1));
		memcpy (&infoPtr->strings[0], &oldStrings[0],
			sizeof(LPWSTR) * infoPtr->nNumStrings);
		COMCTL32_Free (oldStrings);
	    }

	    infoPtr->strings[infoPtr->nNumStrings] =
		(WCHAR*)COMCTL32_Alloc (sizeof(WCHAR)*(len+1));
	    lstrcpyAtoW (infoPtr->strings[infoPtr->nNumStrings], p);
	    infoPtr->nNumStrings++;

	    p += (len+1);
	}
    }

    return nIndex;
}


static LRESULT
TOOLBAR_AddStringW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
#define MAX_RESOURCE_STRING_LENGTH 512
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    if ((wParam) && (HIWORD(lParam) == 0)) {
	WCHAR szString[MAX_RESOURCE_STRING_LENGTH];
	INT len;
	// TRACE("adding string from resource!\n");

	len = LoadStringW ((HINSTANCE)wParam, (UINT)lParam,
			     szString, MAX_RESOURCE_STRING_LENGTH);

	// TRACE("len=%d \"%s\"\n", len, debugstr_w(szString));
	// TRACE("First char: 0x%x\n", *szString);
	if (szString[0] == L'|')
	{
	    PWSTR p = szString + 1;
		
	    nIndex = infoPtr->nNumStrings;
	    while (*p != L'|') {

	    if (infoPtr->nNumStrings == 0) {
		infoPtr->strings =
		    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR));
	    }
	    else {
		LPWSTR *oldStrings = infoPtr->strings;
		infoPtr->strings =
		    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR) * (infoPtr->nNumStrings + 1));
		memcpy (&infoPtr->strings[0], &oldStrings[0],
			sizeof(LPWSTR) * infoPtr->nNumStrings);
		COMCTL32_Free (oldStrings);
	    }

	    len = COMCTL32_StrChrW (p, L'|') - p;
	    // TRACE("len=%d \"%s\"\n", len, debugstr_w(p));
	    infoPtr->strings[infoPtr->nNumStrings] =
		(WCHAR *)COMCTL32_Alloc (sizeof(WCHAR)*(len+1));
	    lstrcpynW (infoPtr->strings[infoPtr->nNumStrings], p, len);
	    infoPtr->nNumStrings++;

		p += (len+1);
	    }
	}
	else
	{
            nIndex = infoPtr->nNumStrings;
            if (infoPtr->nNumStrings == 0) {
                infoPtr->strings =
                    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR));
            }
            else {
                LPWSTR *oldStrings = infoPtr->strings;
                infoPtr->strings =
                    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR) * (infoPtr->nNumStrings + 1));
                memcpy (&infoPtr->strings[0], &oldStrings[0],
                        sizeof(LPWSTR) * infoPtr->nNumStrings);
                COMCTL32_Free (oldStrings);
            }

            infoPtr->strings[infoPtr->nNumStrings] =
                (WCHAR*)COMCTL32_Alloc (sizeof(WCHAR)*(len+1));
            lstrcpyW (infoPtr->strings[infoPtr->nNumStrings], szString);
            infoPtr->nNumStrings++;
        }
    }
    else {
	LPWSTR p = (LPWSTR)lParam;
	INT len;

	if (p == NULL)
	    return -1;
	// TRACE("adding string(s) from array!\n");
	nIndex = infoPtr->nNumStrings;
	while (*p) {
	    len = lstrlenW (p);

	    // TRACE("len=%d \"%s\"\n", len, debugstr_w(p));
	    if (infoPtr->nNumStrings == 0) {
		infoPtr->strings =
		    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR));
	    }
	    else {
		LPWSTR *oldStrings = infoPtr->strings;
		infoPtr->strings =
		    (LPWSTR*)COMCTL32_Alloc (sizeof(LPWSTR) * (infoPtr->nNumStrings + 1));
		memcpy (&infoPtr->strings[0], &oldStrings[0],
			sizeof(LPWSTR) * infoPtr->nNumStrings);
		COMCTL32_Free (oldStrings);
	    }

	    infoPtr->strings[infoPtr->nNumStrings] =
		(WCHAR*)COMCTL32_Alloc (sizeof(WCHAR)*(len+1));
	    lstrcpyW (infoPtr->strings[infoPtr->nNumStrings], p);
	    infoPtr->nNumStrings++;

	    p += (len+1);
	}
    }

    return nIndex;
}


static LRESULT
TOOLBAR_AutoSize (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    RECT parent_rect;
    RECT window_rect;
    HWND parent;
    INT  x, y;
    INT  cx, cy;
    UINT uPosFlags = SWP_NOZORDER;

    // TRACE("resize forced, style=%lx!\n", dwStyle);

    parent = GetParent (hwnd);
    GetClientRect(parent, &parent_rect);

    x = parent_rect.left;
    y = parent_rect.top;

    if (dwStyle & CCS_NORESIZE) {
	uPosFlags |= (SWP_NOSIZE | SWP_NOMOVE);
	cx = 0;
	cy = 0;
    }
    else {
	infoPtr->nWidth = parent_rect.right - parent_rect.left;
	TOOLBAR_CalcToolbar (hwnd);
	InvalidateRect( hwnd, NULL, TRUE );
	cy = infoPtr->nHeight;
	cx = infoPtr->nWidth;

	if (dwStyle & CCS_NOMOVEY) {
		GetWindowRect(hwnd, &window_rect);
		ScreenToClient(parent, (LPPOINT)&window_rect.left);
		y = window_rect.top;
	}
    }

    if (dwStyle & CCS_NOPARENTALIGN)
	uPosFlags |= SWP_NOMOVE;

    if (!(dwStyle & CCS_NODIVIDER))
	cy += GetSystemMetrics(SM_CYEDGE);

    if (dwStyle & WS_BORDER)
    {
        x = y = 1;
        cy += GetSystemMetrics(SM_CYEDGE);
        cx += GetSystemMetrics(SM_CYEDGE);
    }

    infoPtr->bAutoSize = TRUE;
    SetWindowPos (hwnd, HWND_TOP, parent_rect.left - x, parent_rect.top - y,
                        cx, cy, uPosFlags);
    /* The following line makes sure that the infoPtr->bAutoSize is turned off after
     * the setwindowpos calls */
    infoPtr->bAutoSize = FALSE;

    return 0;
}


static LRESULT
TOOLBAR_ButtonCount (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return infoPtr->nNumButtons;
}


static LRESULT
TOOLBAR_ButtonStructSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL) {
// ERR("(0x%x, 0x%x, 0x%lx)\n", hwnd, wParam, lParam);
// ERR("infoPtr == NULL!\n");
	return 0;
    }

    infoPtr->dwStructSize = (DWORD)wParam;

    return 0;
}


static LRESULT
TOOLBAR_ChangeBitmap (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    btnPtr->iBitmap = LOWORD(lParam);

    InvalidateRect(hwnd, &btnPtr->rect, TRUE);

    return TRUE;
}


static LRESULT
TOOLBAR_CheckButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;
    INT nOldIndex = -1;
    BOOL bChecked = FALSE;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];

    if (!(btnPtr->fsStyle & TBSTYLE_CHECK))
	return FALSE;

    bChecked = (btnPtr->fsState & TBSTATE_CHECKED) ? TRUE : FALSE;

    if (LOWORD(lParam) == FALSE)
	btnPtr->fsState &= ~TBSTATE_CHECKED;
    else {
	if (btnPtr->fsStyle & TBSTYLE_GROUP) {
	    nOldIndex = 
		TOOLBAR_GetCheckedGroupButtonIndex (infoPtr, nIndex);
	    if (nOldIndex == nIndex)
		return 0;
	    if (nOldIndex != -1)
		infoPtr->buttons[nOldIndex].fsState &= ~TBSTATE_CHECKED;
	}
	btnPtr->fsState |= TBSTATE_CHECKED;
    }

    if( bChecked != LOWORD(lParam) )
    {
	if (nOldIndex != -1)
            InvalidateRect(hwnd, &infoPtr->buttons[nOldIndex].rect, TRUE);
        InvalidateRect(hwnd, &btnPtr->rect, TRUE);
    }

    /* FIXME: Send a WM_NOTIFY?? */

    return TRUE;
}


static LRESULT
TOOLBAR_CommandToIndex (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
}


static LRESULT
TOOLBAR_Customize (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LRESULT ret;
    LPCVOID temp;
    HRSRC hRes;
    NMHDR nmhdr;

    /* send TBN_BEGINADJUST notification */
    nmhdr.hwndFrom = hwnd;
    nmhdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
    nmhdr.code     = TBN_BEGINADJUST;

    SendMessageA (infoPtr->hwndNotify, WM_NOTIFY,
		  (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);

    if (!(hRes = FindResourceA (COMCTL32_hModule,
                                MAKEINTRESOURCEA(IDD_TBCUSTOMIZE),
                                RT_DIALOGA)))
	return FALSE;

    if(!(temp = (LPVOID)LoadResource (COMCTL32_hModule, hRes)))
	return FALSE;

    ret = DialogBoxIndirectParamA (GetWindowLongA (hwnd, GWL_HINSTANCE),
                                   (LPDLGTEMPLATEA)temp,
                                   hwnd,
                                   (DLGPROC)TOOLBAR_CustomizeDialogProc,
                                   (LPARAM)infoPtr);

    /* send TBN_ENDADJUST notification */
    nmhdr.code = TBN_ENDADJUST;

    SendMessageA (infoPtr->hwndNotify, WM_NOTIFY,
		  (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);

    return ret;
}


static LRESULT
TOOLBAR_DeleteButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex = (INT)wParam;

    if ((nIndex < 0) || (nIndex >= infoPtr->nNumButtons))
	return FALSE;

    if ((infoPtr->hwndToolTip) && 
	!(infoPtr->buttons[nIndex].fsStyle & TBSTYLE_SEP)) {
	TTTOOLINFOA ti;

	ZeroMemory (&ti, sizeof(TTTOOLINFOA));
	ti.cbSize   = sizeof (TTTOOLINFOA);
	ti.hwnd     = hwnd;
	ti.uId      = infoPtr->buttons[nIndex].idCommand;

	SendMessageA (infoPtr->hwndToolTip, TTM_DELTOOLA, 0, (LPARAM)&ti);
    }

    if (infoPtr->nNumButtons == 1) {
	// TRACE(" simple delete!\n");
	COMCTL32_Free (infoPtr->buttons);
	infoPtr->buttons = NULL;
	infoPtr->nNumButtons = 0;
    }
    else {
	TBUTTON_INFO *oldButtons = infoPtr->buttons;
        // TRACE("complex delete! [nIndex=%d]\n", nIndex);

	infoPtr->nNumButtons--;
	infoPtr->buttons = (TBUTTON_INFO*)COMCTL32_Alloc (sizeof (TBUTTON_INFO) * infoPtr->nNumButtons);
        if (nIndex > 0) {
            memcpy (&infoPtr->buttons[0], &oldButtons[0],
                    nIndex * sizeof(TBUTTON_INFO));
        }

        if (nIndex < infoPtr->nNumButtons) {
            memcpy (&infoPtr->buttons[nIndex], &oldButtons[nIndex+1],
                    (infoPtr->nNumButtons - nIndex) * sizeof(TBUTTON_INFO));
        }

	COMCTL32_Free (oldButtons);
    }

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect (hwnd, NULL, TRUE);

    return TRUE;
}


static LRESULT
TOOLBAR_EnableButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;
    DWORD bState;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];

    bState = btnPtr->fsState & TBSTATE_ENABLED;

    /* update the toolbar button state */
    if(LOWORD(lParam) == FALSE) {
 	btnPtr->fsState &= ~(TBSTATE_ENABLED | TBSTATE_PRESSED);
    } else {
	btnPtr->fsState |= TBSTATE_ENABLED;
    }

    /* redraw the button only if the state of the button changed */
    if(bState != (btnPtr->fsState & TBSTATE_ENABLED))
        InvalidateRect(hwnd, &btnPtr->rect, TRUE);            

    return TRUE;
}


static inline LRESULT
TOOLBAR_GetAnchorHighlight (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return infoPtr->bAnchor;
}


static LRESULT
TOOLBAR_GetBitmap (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    return infoPtr->buttons[nIndex].iBitmap;
}


static inline LRESULT
TOOLBAR_GetBitmapFlags (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    return (GetDeviceCaps (0, LOGPIXELSX) >= 120) ? TBBF_LARGE : 0;
}


static LRESULT
TOOLBAR_GetButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTON lpTbb = (LPTBBUTTON)lParam;
    INT nIndex = (INT)wParam;
    TBUTTON_INFO *btnPtr;

    if (infoPtr == NULL)
	return FALSE;

    if (lpTbb == NULL)
	return FALSE;

    if ((nIndex < 0) || (nIndex >= infoPtr->nNumButtons))
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    lpTbb->iBitmap   = btnPtr->iBitmap;
    lpTbb->idCommand = btnPtr->idCommand;
    lpTbb->fsState   = btnPtr->fsState;
    lpTbb->fsStyle   = btnPtr->fsStyle;
    lpTbb->dwData    = btnPtr->dwData;
    lpTbb->iString   = btnPtr->iString;

    return TRUE;
}


static LRESULT
TOOLBAR_GetButtonInfoA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTONINFOA lpTbInfo = (LPTBBUTTONINFOA)lParam;
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    if (infoPtr == NULL)
	return -1;
    if (lpTbInfo == NULL)
	return -1;
    if (lpTbInfo->cbSize < sizeof(TBBUTTONINFOA))
	return -1;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    btnPtr = &infoPtr->buttons[nIndex];

    if (lpTbInfo->dwMask & TBIF_COMMAND)
	lpTbInfo->idCommand = btnPtr->idCommand;
    if (lpTbInfo->dwMask & TBIF_IMAGE)
	lpTbInfo->iImage = btnPtr->iBitmap;
    if (lpTbInfo->dwMask & TBIF_LPARAM)
	lpTbInfo->lParam = btnPtr->dwData;
    if (lpTbInfo->dwMask & TBIF_SIZE)
	lpTbInfo->cx = (WORD)(btnPtr->rect.right - btnPtr->rect.left);
    if (lpTbInfo->dwMask & TBIF_STATE)
	lpTbInfo->fsState = btnPtr->fsState;
    if (lpTbInfo->dwMask & TBIF_STYLE)
	lpTbInfo->fsStyle = btnPtr->fsStyle;
     if (lpTbInfo->dwMask & TBIF_TEXT) {
         if ((btnPtr->iString >= 0) && (btnPtr->iString < infoPtr->nNumStrings))
         {	
             lstrcpynWtoA (lpTbInfo->pszText,
                           (LPWSTR)infoPtr->strings[btnPtr->iString],
                           lpTbInfo->cchText);
         }
         else lpTbInfo->pszText[0]=0;
    }
    return nIndex;
}


static LRESULT
TOOLBAR_GetButtonInfoW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTONINFOW lpTbInfo = (LPTBBUTTONINFOW)lParam;
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    if (infoPtr == NULL)
	return -1;
    if (lpTbInfo == NULL)
	return -1;
    if (lpTbInfo->cbSize < sizeof(TBBUTTONINFOW))
	return -1;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    btnPtr = &infoPtr->buttons[nIndex];

    if (lpTbInfo->dwMask & TBIF_COMMAND)
	lpTbInfo->idCommand = btnPtr->idCommand;
    if (lpTbInfo->dwMask & TBIF_IMAGE)
	lpTbInfo->iImage = btnPtr->iBitmap;
    if (lpTbInfo->dwMask & TBIF_LPARAM)
	lpTbInfo->lParam = btnPtr->dwData;
    if (lpTbInfo->dwMask & TBIF_SIZE)
	lpTbInfo->cx = (WORD)(btnPtr->rect.right - btnPtr->rect.left);
    if (lpTbInfo->dwMask & TBIF_STATE)
	lpTbInfo->fsState = btnPtr->fsState;
    if (lpTbInfo->dwMask & TBIF_STYLE)
	lpTbInfo->fsStyle = btnPtr->fsStyle;
    if (lpTbInfo->dwMask & TBIF_TEXT) {
	if ((btnPtr->iString >= 0) || (btnPtr->iString < infoPtr->nNumStrings))
	    lstrcpynW (lpTbInfo->pszText,
		       (LPWSTR)infoPtr->strings[btnPtr->iString],
		       lpTbInfo->cchText);
    }

    return nIndex;
}


static LRESULT
TOOLBAR_GetButtonSize (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return MAKELONG((WORD)infoPtr->nButtonWidth,
		    (WORD)infoPtr->nButtonHeight);
}


static LRESULT
TOOLBAR_GetButtonTextA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex, nStringIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    nStringIndex = infoPtr->buttons[nIndex].iString;

    // TRACE("index=%d stringIndex=%d\n", nIndex, nStringIndex);

    if ((nStringIndex < 0) || (nStringIndex >= infoPtr->nNumStrings))
	return -1;

    if (lParam == 0)
	return -1;

    lstrcpyWtoA ((LPSTR)lParam, (LPWSTR)infoPtr->strings[nStringIndex]);

    return lstrlenW ((LPWSTR)infoPtr->strings[nStringIndex]);
}


static LRESULT
TOOLBAR_GetButtonTextW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex, nStringIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    nStringIndex = infoPtr->buttons[nIndex].iString;

    // TRACE("index=%d stringIndex=%d\n", nIndex, nStringIndex);

    if ((nStringIndex < 0) || (nStringIndex >= infoPtr->nNumStrings))
	return -1;

    if (lParam == 0)
	return -1;

    lstrcpyW ((LPWSTR)lParam, (LPWSTR)infoPtr->strings[nStringIndex]);

    return lstrlenW ((LPWSTR)infoPtr->strings[nStringIndex]);
}


/* << TOOLBAR_GetColorScheme >> */


static LRESULT
TOOLBAR_GetDisabledImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->himlDis;
}


inline static LRESULT
TOOLBAR_GetExtendedStyle (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return infoPtr->dwExStyle;
}


static LRESULT
TOOLBAR_GetHotImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->himlHot;
}


static LRESULT
TOOLBAR_GetHotItem (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (!(GetWindowLongA (hwnd, GWL_STYLE) & TBSTYLE_FLAT))
	return -1;

    if (infoPtr->nHotItem < 0)
	return -1;

    return (LRESULT)infoPtr->nHotItem;
}


static LRESULT
TOOLBAR_GetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->himlDef;
}


/* << TOOLBAR_GetInsertMark >> */
/* << TOOLBAR_GetInsertMarkColor >> */


static LRESULT
TOOLBAR_GetItemRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    LPRECT     lpRect;
    INT        nIndex;

    if (infoPtr == NULL)
	return FALSE;
    nIndex = (INT)wParam;
    btnPtr = &infoPtr->buttons[nIndex];
    if ((nIndex < 0) || (nIndex >= infoPtr->nNumButtons))
	return FALSE;
    lpRect = (LPRECT)lParam;
    if (lpRect == NULL)
	return FALSE;
    if (btnPtr->fsState & TBSTATE_HIDDEN)
	return FALSE;
    
    TOOLBAR_CalcToolbar( hwnd );
    
    lpRect->left   = btnPtr->rect.left;
    lpRect->right  = btnPtr->rect.right;
    lpRect->bottom = btnPtr->rect.bottom;
    lpRect->top    = btnPtr->rect.top;

    return TRUE;
}


static LRESULT
TOOLBAR_GetMaxSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPSIZE lpSize = (LPSIZE)lParam;

    if (lpSize == NULL)
	return FALSE;

    lpSize->cx = infoPtr->rcBound.right - infoPtr->rcBound.left;
    lpSize->cy = infoPtr->rcBound.bottom - infoPtr->rcBound.top;

    // TRACE("maximum size %d x %d\n",
    // infoPtr->rcBound.right - infoPtr->rcBound.left,
    //  infoPtr->rcBound.bottom - infoPtr->rcBound.top);

    return TRUE;
}


/* << TOOLBAR_GetObject >> */
/* << TOOLBAR_GetPadding >> */


static LRESULT
TOOLBAR_GetRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    LPRECT     lpRect;
    INT        nIndex;

    if (infoPtr == NULL)
	return FALSE;
    nIndex = (INT)wParam;
    btnPtr = &infoPtr->buttons[nIndex];
    if ((nIndex < 0) || (nIndex >= infoPtr->nNumButtons))
	return FALSE;
    lpRect = (LPRECT)lParam;
    if (lpRect == NULL)
	return FALSE;
    
    lpRect->left   = btnPtr->rect.left;
    lpRect->right  = btnPtr->rect.right;
    lpRect->bottom = btnPtr->rect.bottom;
    lpRect->top    = btnPtr->rect.top;

    return TRUE;
}


static LRESULT
TOOLBAR_GetRows (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (GetWindowLongA (hwnd, GWL_STYLE) & TBSTYLE_WRAPABLE)
	return infoPtr->nRows;
    else
	return 1;
}


static LRESULT
TOOLBAR_GetState (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    return infoPtr->buttons[nIndex].fsState;
}


static LRESULT
TOOLBAR_GetStyle (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return -1;

    return infoPtr->buttons[nIndex].fsStyle;
}


static LRESULT
TOOLBAR_GetTextRows (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL)
	return 0;

    return infoPtr->nMaxTextRows;
}


static LRESULT
TOOLBAR_GetToolTips (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL)
	return 0;
    return infoPtr->hwndToolTip;
}


static LRESULT
TOOLBAR_GetUnicodeFormat (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    // TRACE("%s hwnd=0x%x stub!\n", 
//  infoPtr->bUnicode ? "TRUE" : "FALSE", hwnd);

    return infoPtr->bUnicode;
}


inline static LRESULT
TOOLBAR_GetVersion (HWND hwnd)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    return infoPtr->iVersion;
}


static LRESULT
TOOLBAR_HideButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    if (LOWORD(lParam) == FALSE)
	btnPtr->fsState &= ~TBSTATE_HIDDEN;
    else
	btnPtr->fsState |= TBSTATE_HIDDEN;

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect (hwnd, NULL, TRUE);

    return TRUE;
}


inline static LRESULT
TOOLBAR_HitTest (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    return TOOLBAR_InternalHitTest (hwnd, (LPPOINT)lParam);
}


static LRESULT
TOOLBAR_Indeterminate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    if (LOWORD(lParam) == FALSE)
	btnPtr->fsState &= ~TBSTATE_INDETERMINATE;
    else
	btnPtr->fsState |= TBSTATE_INDETERMINATE;

    InvalidateRect(hwnd, &btnPtr->rect, TRUE);

    return TRUE;
}


static LRESULT
TOOLBAR_InsertButtonA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTON lpTbb = (LPTBBUTTON)lParam;
    INT nIndex = (INT)wParam;
    TBUTTON_INFO *oldButtons;

    if (lpTbb == NULL)
	return FALSE;

    if (nIndex == -1) {
       /* EPP: this seems to be an undocumented call (from my IE4)
	* I assume in that case that:
	* - lpTbb->iString is a string pointer (not a string index in strings[] table
	* - index of insertion is at the end of existing buttons
	* I only see this happen with nIndex == -1, but it could have a special
	* meaning (like -nIndex (or ~nIndex) to get the real position of insertion).
	*/
       int	len;
       LPSTR	ptr;

       if(lpTbb->iString) {
           len = lstrlenA((char*)lpTbb->iString) + 2;
           ptr = (CHAR*)COMCTL32_Alloc(len);
           nIndex = infoPtr->nNumButtons;
           strcpy(ptr, (char*)lpTbb->iString);
           ptr[len - 1] = 0; /* ended by two '\0' */
           lpTbb->iString = TOOLBAR_AddStringA(hwnd, 0, (LPARAM)ptr);
           COMCTL32_Free(ptr);
       }
       else {
  //         ERR("lpTbb->iString is NULL\n");
           return FALSE;
       }

    } else if (nIndex < 0)
       return FALSE;

    // TRACE("inserting button index=%d\n", nIndex);
    if (nIndex > infoPtr->nNumButtons) {
	nIndex = infoPtr->nNumButtons;
	// TRACE("adjust index=%d\n", nIndex);
    }

    oldButtons = infoPtr->buttons;
    infoPtr->nNumButtons++;
    infoPtr->buttons = (TBUTTON_INFO*)COMCTL32_Alloc (sizeof (TBUTTON_INFO) * infoPtr->nNumButtons);
    /* pre insert copy */
    if (nIndex > 0) {
	memcpy (&infoPtr->buttons[0], &oldButtons[0],
		nIndex * sizeof(TBUTTON_INFO));
    }

    /* insert new button */
    infoPtr->buttons[nIndex].iBitmap   = lpTbb->iBitmap;
    infoPtr->buttons[nIndex].idCommand = lpTbb->idCommand;
    infoPtr->buttons[nIndex].fsState   = lpTbb->fsState;
    infoPtr->buttons[nIndex].fsStyle   = lpTbb->fsStyle;
    infoPtr->buttons[nIndex].dwData    = lpTbb->dwData;
    infoPtr->buttons[nIndex].iString   = lpTbb->iString;

    if ((infoPtr->hwndToolTip) && !(lpTbb->fsStyle & TBSTYLE_SEP)) {
	TTTOOLINFOA ti;

	ZeroMemory (&ti, sizeof(TTTOOLINFOA));
	ti.cbSize   = sizeof (TTTOOLINFOA);
	ti.hwnd     = hwnd;
	ti.uId      = lpTbb->idCommand;
	ti.hinst    = 0;
	ti.lpszText = LPSTR_TEXTCALLBACKA;

	SendMessageA (infoPtr->hwndToolTip, TTM_ADDTOOLA,
			0, (LPARAM)&ti);
    }

    /* post insert copy */
    if (nIndex < infoPtr->nNumButtons - 1) {
	memcpy (&infoPtr->buttons[nIndex+1], &oldButtons[nIndex],
		(infoPtr->nNumButtons - nIndex - 1) * sizeof(TBUTTON_INFO));
    }

    COMCTL32_Free (oldButtons);

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect (hwnd, NULL, FALSE);

    return TRUE;
}


static LRESULT
TOOLBAR_InsertButtonW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTON lpTbb = (LPTBBUTTON)lParam;
    INT nIndex = (INT)wParam;
    TBUTTON_INFO *oldButtons;

    if (lpTbb == NULL)
	return FALSE;
    if (nIndex < 0)
	return FALSE;

    // TRACE("inserting button index=%d\n", nIndex);
    if (nIndex > infoPtr->nNumButtons) {
	nIndex = infoPtr->nNumButtons;
	// TRACE("adjust index=%d\n", nIndex);
    }

    oldButtons = infoPtr->buttons;
    infoPtr->nNumButtons++;
    infoPtr->buttons = (TBUTTON_INFO*)COMCTL32_Alloc (sizeof (TBUTTON_INFO) * infoPtr->nNumButtons);
    /* pre insert copy */
    if (nIndex > 0) {
	memcpy (&infoPtr->buttons[0], &oldButtons[0],
		nIndex * sizeof(TBUTTON_INFO));
    }

    /* insert new button */
    infoPtr->buttons[nIndex].iBitmap   = lpTbb->iBitmap;
    infoPtr->buttons[nIndex].idCommand = lpTbb->idCommand;
    infoPtr->buttons[nIndex].fsState   = lpTbb->fsState;
    infoPtr->buttons[nIndex].fsStyle   = lpTbb->fsStyle;
    infoPtr->buttons[nIndex].dwData    = lpTbb->dwData;
    infoPtr->buttons[nIndex].iString   = lpTbb->iString;

    if ((infoPtr->hwndToolTip) && !(lpTbb->fsStyle & TBSTYLE_SEP)) {
	TTTOOLINFOW ti;

	ZeroMemory (&ti, sizeof(TTTOOLINFOW));
	ti.cbSize   = sizeof (TTTOOLINFOW);
	ti.hwnd     = hwnd;
	ti.uId      = lpTbb->idCommand;
	ti.hinst    = 0;
	ti.lpszText = LPSTR_TEXTCALLBACKW;

	SendMessageW (infoPtr->hwndToolTip, TTM_ADDTOOLW,
			0, (LPARAM)&ti);
    }

    /* post insert copy */
    if (nIndex < infoPtr->nNumButtons - 1) {
	memcpy (&infoPtr->buttons[nIndex+1], &oldButtons[nIndex],
		(infoPtr->nNumButtons - nIndex - 1) * sizeof(TBUTTON_INFO));
    }

    COMCTL32_Free (oldButtons);

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect (hwnd, NULL, FALSE);

    return TRUE;
}


/* << TOOLBAR_InsertMarkHitTest >> */


static LRESULT
TOOLBAR_IsButtonChecked (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_CHECKED);
}


static LRESULT
TOOLBAR_IsButtonEnabled (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_ENABLED);
}


static LRESULT
TOOLBAR_IsButtonHidden (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_HIDDEN);
}


static LRESULT
TOOLBAR_IsButtonHighlighted (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_MARKED);
}


static LRESULT
TOOLBAR_IsButtonIndeterminate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_INDETERMINATE);
}


static LRESULT
TOOLBAR_IsButtonPressed (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    return (infoPtr->buttons[nIndex].fsState & TBSTATE_PRESSED);
}


/* << TOOLBAR_LoadImages >> */
/* << TOOLBAR_MapAccelerator >> */
/* << TOOLBAR_MarkButton >> */
/* << TOOLBAR_MoveButton >> */


static LRESULT
TOOLBAR_PressButton (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    if (LOWORD(lParam) == FALSE)
	btnPtr->fsState &= ~TBSTATE_PRESSED;
    else
	btnPtr->fsState |= TBSTATE_PRESSED;

    InvalidateRect(hwnd, &btnPtr->rect, TRUE);

    return TRUE;
}


/* << TOOLBAR_ReplaceBitmap >> */


static LRESULT
TOOLBAR_SaveRestoreA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
#if 0
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBSAVEPARAMSA lpSave = (LPTBSAVEPARAMSA)lParam;

    if (lpSave == NULL) return 0;

    if ((BOOL)wParam) {
	/* save toolbar information */
	// FIXME("save to \"%s\" \"%s\"\n",
	       lpSave->pszSubKey, lpSave->pszValueName);


    }
    else {
	/* restore toolbar information */

	// FIXME("restore from \"%s\" \"%s\"\n",
	       lpSave->pszSubKey, lpSave->pszValueName);


    }
#endif

    return 0;
}


static LRESULT
TOOLBAR_SaveRestoreW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
#if 0
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBSAVEPARAMSW lpSave = (LPTBSAVEPARAMSW)lParam;

    if (lpSave == NULL)
	return 0;

    if ((BOOL)wParam) {
	/* save toolbar information */
	// FIXME("save to \"%s\" \"%s\"\n",
	       lpSave->pszSubKey, lpSave->pszValueName);


    }
    else {
	/* restore toolbar information */

	// FIXME("restore from \"%s\" \"%s\"\n",
	       lpSave->pszSubKey, lpSave->pszValueName);


    }
#endif

    return 0;
}


static LRESULT
TOOLBAR_SetAnchorHighlight (HWND hwnd, WPARAM wParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    BOOL bOldAnchor = infoPtr->bAnchor;

    infoPtr->bAnchor = (BOOL)wParam;

    return (LRESULT)bOldAnchor;
}


static LRESULT
TOOLBAR_SetBitmapSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if ((LOWORD(lParam) <= 0) || (HIWORD(lParam)<=0))
	return FALSE;

//  if (infoPtr->nNumButtons > 0)
//      WARN("%d buttons, undoc increase to bitmap size : %d-%d -> %d-%d\n",
//           infoPtr->nNumButtons,
//           infoPtr->nBitmapWidth, infoPtr->nBitmapHeight,
//           LOWORD(lParam), HIWORD(lParam));

    infoPtr->nBitmapWidth = (INT)LOWORD(lParam);
    infoPtr->nBitmapHeight = (INT)HIWORD(lParam);

    /* uses image list internals directly */
    if (infoPtr->himlDef) {
        infoPtr->himlDef->cx = infoPtr->nBitmapWidth;
        infoPtr->himlDef->cy = infoPtr->nBitmapHeight;
    }

    return TRUE;
}


static LRESULT
TOOLBAR_SetButtonInfoA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTONINFOA lptbbi = (LPTBBUTTONINFOA)lParam;
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    if (lptbbi == NULL)
	return FALSE;
    if (lptbbi->cbSize < sizeof(TBBUTTONINFOA))
	return FALSE;
    
    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    if (lptbbi->dwMask & TBIF_COMMAND)
	btnPtr->idCommand = lptbbi->idCommand;
    if (lptbbi->dwMask & TBIF_IMAGE)
	btnPtr->iBitmap = lptbbi->iImage;
    if (lptbbi->dwMask & TBIF_LPARAM)
	btnPtr->dwData = lptbbi->lParam;
/*    if (lptbbi->dwMask & TBIF_SIZE) */
/*	btnPtr->cx = lptbbi->cx; */
    if (lptbbi->dwMask & TBIF_STATE)
	btnPtr->fsState = lptbbi->fsState;
    if (lptbbi->dwMask & TBIF_STYLE)
	btnPtr->fsStyle = lptbbi->fsStyle;

    if (lptbbi->dwMask & TBIF_TEXT) {
	if ((btnPtr->iString >= 0) || 
	    (btnPtr->iString < infoPtr->nNumStrings)) {
	   // TRACE("Ooooooch\n");
#if 0
	    WCHAR **lpString = &infoPtr->strings[btnPtr->iString];
	    INT len = lstrlenA (lptbbi->pszText);
	    *lpString = COMCTL32_ReAlloc (lpString, sizeof(WCHAR)*(len+1));
#endif

	    /* this is the ultimate sollution */
/*	    Str_SetPtrA (&infoPtr->strings[btnPtr->iString], lptbbi->pszText); */
	}
    }

    return TRUE;
}


static LRESULT
TOOLBAR_SetButtonInfoW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPTBBUTTONINFOW lptbbi = (LPTBBUTTONINFOW)lParam;
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    if (lptbbi == NULL)
	return FALSE;
    if (lptbbi->cbSize < sizeof(TBBUTTONINFOW))
	return FALSE;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    if (lptbbi->dwMask & TBIF_COMMAND)
	btnPtr->idCommand = lptbbi->idCommand;
    if (lptbbi->dwMask & TBIF_IMAGE)
	btnPtr->iBitmap = lptbbi->iImage;
    if (lptbbi->dwMask & TBIF_LPARAM)
	btnPtr->dwData = lptbbi->lParam;
/*    if (lptbbi->dwMask & TBIF_SIZE) */
/*	btnPtr->cx = lptbbi->cx; */
    if (lptbbi->dwMask & TBIF_STATE)
	btnPtr->fsState = lptbbi->fsState;
    if (lptbbi->dwMask & TBIF_STYLE)
	btnPtr->fsStyle = lptbbi->fsStyle;

    if (lptbbi->dwMask & TBIF_TEXT) {
	if ((btnPtr->iString >= 0) ||
	    (btnPtr->iString < infoPtr->nNumStrings)) {
#if 0
	    WCHAR **lpString = &infoPtr->strings[btnPtr->iString];
	    INT len = lstrlenW (lptbbi->pszText);
	    *lpString = COMCTL32_ReAlloc (lpString, sizeof(WCHAR)*(len+1));
#endif

	    /* this is the ultimate sollution */
/*	    Str_SetPtrA (&infoPtr->strings[btnPtr->iString], lptbbi->pszText); */
	}
    }

    return TRUE;
}


static LRESULT
TOOLBAR_SetButtonSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if ((LOWORD(lParam) <= 0) || (HIWORD(lParam)<=0))
    {
//      ERR("invalid parameter\n");
	return FALSE;
    }

    /* Button size can only be set before adding any button to the toolbar
       according to the documentation.  */
    /* this appears to be wrong. WINZIP32.EXE (ver 8) calls this on
       one of its buttons after adding it to the toolbar, and it
       checks that the return value is nonzero - mjm */
    if( infoPtr->nNumButtons != 0 )
    {
        // FIXME("Button size set after button in toolbar\n");
        return TRUE;
    }

    infoPtr->nButtonWidth = (INT)LOWORD(lParam);
    infoPtr->nButtonHeight = (INT)HIWORD(lParam);
    return TRUE;
}


static LRESULT
TOOLBAR_SetButtonWidth (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL)
	return FALSE;

    infoPtr->cxMin = (INT)LOWORD(lParam);
    infoPtr->cxMax = (INT)HIWORD(lParam);

    return TRUE;
}


static LRESULT
TOOLBAR_SetCmdId (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT nIndex = (INT)wParam;

    if ((nIndex < 0) || (nIndex >= infoPtr->nNumButtons))
	return FALSE;

    infoPtr->buttons[nIndex].idCommand = (INT)lParam;

    if (infoPtr->hwndToolTip) {

	// FIXME("change tool tip!\n");

    }

    return TRUE;
}


/* << TOOLBAR_SetColorScheme >> */


static LRESULT
TOOLBAR_SetDisabledImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    HIMAGELIST himlTemp;


    himlTemp = infoPtr->himlDis;
    infoPtr->himlDis = (HIMAGELIST)lParam;

    /* FIXME: redraw ? */

    return (LRESULT)himlTemp; 
}


static LRESULT
TOOLBAR_SetDrawTextFlags (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwTemp;

    dwTemp = infoPtr->dwDTFlags;
    infoPtr->dwDTFlags =
	(infoPtr->dwDTFlags & (DWORD)wParam) | (DWORD)lParam;

    return (LRESULT)dwTemp;
}


static LRESULT
TOOLBAR_SetExtendedStyle (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwTemp;

    dwTemp = infoPtr->dwExStyle;
    infoPtr->dwExStyle = (DWORD)lParam;

    return (LRESULT)dwTemp; 
}


static LRESULT
TOOLBAR_SetHotImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr(hwnd);
    HIMAGELIST himlTemp;

    himlTemp = infoPtr->himlHot;
    infoPtr->himlHot = (HIMAGELIST)lParam;

    /* FIXME: redraw ? */

    return (LRESULT)himlTemp; 
}


static LRESULT
TOOLBAR_SetHotItem (HWND hwnd, WPARAM wParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr(hwnd);
    INT nOldHotItem = infoPtr->nHotItem;

    if (GetWindowLongA (hwnd, GWL_STYLE) & TBSTYLE_FLAT)
    {
	infoPtr->nHotItem = (INT)wParam;

	/* FIXME: What else must be done ??? */

    }

    if (nOldHotItem < 0)
	return -1;

    return (LRESULT)nOldHotItem;
}


static LRESULT
TOOLBAR_SetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    HIMAGELIST himlTemp;

    himlTemp = infoPtr->himlDef;
    infoPtr->himlDef = (HIMAGELIST)lParam;

     infoPtr->nNumBitmaps = ImageList_GetImageCount(infoPtr->himlDef);
    /* FIXME: redraw ? */

    return (LRESULT)himlTemp; 
}


static LRESULT
TOOLBAR_SetIndent (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    infoPtr->nIndent = (INT)wParam;

    TOOLBAR_CalcToolbar (hwnd);

    InvalidateRect(hwnd, NULL, FALSE);

    return TRUE;
}


/* << TOOLBAR_SetInsertMark >> */


static LRESULT
TOOLBAR_SetInsertMarkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    infoPtr->clrInsertMark = (COLORREF)lParam;

    /* FIXME : redraw ??*/

    return 0;
}


static LRESULT
TOOLBAR_SetMaxTextRows (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL)
	return FALSE;

    infoPtr->nMaxTextRows = (INT)wParam;

    return TRUE;
}


/* << TOOLBAR_SetPadding >> */


static LRESULT
TOOLBAR_SetParent (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    HWND hwndOldNotify;

    if (infoPtr == NULL)
	return 0;
    hwndOldNotify = infoPtr->hwndNotify;
    infoPtr->hwndNotify = (HWND)wParam;

    return hwndOldNotify;
}


static LRESULT
TOOLBAR_SetRows (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPRECT lprc = (LPRECT)lParam;

    if (LOWORD(wParam) > 1) {

	// FIXME("multiple rows not supported!\n");

    }

    /* recalculate toolbar */
    TOOLBAR_CalcToolbar (hwnd);

    /* return bounding rectangle */
    if (lprc) {
	lprc->left   = infoPtr->rcBound.left;
	lprc->right  = infoPtr->rcBound.right;
	lprc->top    = infoPtr->rcBound.top;
	lprc->bottom = infoPtr->rcBound.bottom;
    }

    /* repaint toolbar */
    InvalidateRect(hwnd, NULL, FALSE);

    return 0;
}


static LRESULT
TOOLBAR_SetState (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    btnPtr->fsState = LOWORD(lParam);

    InvalidateRect(hwnd, &btnPtr->rect, TRUE);

    return TRUE;
}


static LRESULT
TOOLBAR_SetStyle (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    INT nIndex;

    nIndex = TOOLBAR_GetButtonIndex (infoPtr, (INT)wParam);
    if (nIndex == -1)
	return FALSE;

    btnPtr = &infoPtr->buttons[nIndex];
    btnPtr->fsStyle = LOWORD(lParam);

    InvalidateRect(hwnd, &btnPtr->rect, TRUE);

    if (infoPtr->hwndToolTip) {
	// FIXME("change tool tip!\n");
    }

    return TRUE;
}


inline static LRESULT
TOOLBAR_SetToolTips (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr == NULL)
	return 0;
    infoPtr->hwndToolTip = (HWND)wParam;
    return 0;
}


static LRESULT
TOOLBAR_SetUnicodeFormat (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    BOOL bTemp;

    // TRACE("%s hwnd=0x%04x stub!\n", 
//  ((BOOL)wParam) ? "TRUE" : "FALSE", hwnd);

    bTemp = infoPtr->bUnicode;
    infoPtr->bUnicode = (BOOL)wParam;

    return bTemp;
}


static LRESULT
TOOLBAR_SetVersion (HWND hwnd, INT iVersion)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    INT iOldVersion = infoPtr->iVersion;

    infoPtr->iVersion = iVersion;

    return iOldVersion;
}


static LRESULT
TOOLBAR_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    LOGFONTA logFont;

    /* initialize info structure */
    infoPtr->nButtonHeight = 22;
    infoPtr->nButtonWidth = 24;
    infoPtr->nBitmapHeight = 15;
    infoPtr->nBitmapWidth = 16;

    infoPtr->nHeight = infoPtr->nButtonHeight + TOP_BORDER + BOTTOM_BORDER;
    infoPtr->nRows = 1;
    infoPtr->nMaxTextRows = 1;
    infoPtr->cxMin = -1;
    infoPtr->cxMax = -1;
    infoPtr->nNumBitmaps = 0;
    infoPtr->nNumStrings = 0;

    infoPtr->bCaptured = FALSE;
    infoPtr->bUnicode = IsWindowUnicode (hwnd);
    infoPtr->nButtonDown = -1;
    infoPtr->nOldHit = -1;
    infoPtr->nHotItem = -2; /* It has to be initially different from nOldHit */
    infoPtr->hwndNotify = GetParent (hwnd);
    infoPtr->bTransparent = (dwStyle & TBSTYLE_FLAT);
    infoPtr->dwDTFlags = (dwStyle & TBSTYLE_LIST) ? DT_LEFT | DT_VCENTER | DT_SINGLELINE : DT_CENTER;
    infoPtr->bAnchor = FALSE; /* no anchor highlighting */
    infoPtr->iVersion = 0;

    SystemParametersInfoA (SPI_GETICONTITLELOGFONT, 0, &logFont, 0);
    infoPtr->hFont = CreateFontIndirectA (&logFont);

    if (dwStyle & TBSTYLE_TOOLTIPS) {
	/* Create tooltip control */
	infoPtr->hwndToolTip =
	    CreateWindowExA (0, TOOLTIPS_CLASSA, NULL, 0,
			       CW_USEDEFAULT, CW_USEDEFAULT,
			       CW_USEDEFAULT, CW_USEDEFAULT,
			       hwnd, 0, 0, 0);

	/* Send NM_TOOLTIPSCREATED notification */
	if (infoPtr->hwndToolTip) {
	    NMTOOLTIPSCREATED nmttc;

	    nmttc.hdr.hwndFrom = hwnd;
	    nmttc.hdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
	    nmttc.hdr.code = NM_TOOLTIPSCREATED;
	    nmttc.hwndToolTips = infoPtr->hwndToolTip;

	    SendMessageA (infoPtr->hwndNotify, WM_NOTIFY,
			  (WPARAM)nmttc.hdr.idFrom, (LPARAM)&nmttc);
	}
    }

    return 0;
}


static LRESULT
TOOLBAR_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    /* delete tooltip control */
    if (infoPtr->hwndToolTip)
	DestroyWindow (infoPtr->hwndToolTip);

    /* delete button data */
    if (infoPtr->buttons)
	COMCTL32_Free (infoPtr->buttons);

    /* delete strings */
    if (infoPtr->strings) {
	INT i;
	for (i = 0; i < infoPtr->nNumStrings; i++)
	    if (infoPtr->strings[i])
		COMCTL32_Free (infoPtr->strings[i]);

	COMCTL32_Free (infoPtr->strings);
    }

    /* destroy internal image list */
    if (infoPtr->himlInt)
	ImageList_Destroy (infoPtr->himlInt);

    /* delete default font */
    if (infoPtr->hFont)
	DeleteObject (infoPtr->hFont);

    /* free toolbar info data */
    COMCTL32_Free (infoPtr);
    SetWindowLongA (hwnd, 0, 0);

    return 0;
}


static LRESULT
TOOLBAR_EraseBackground (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (infoPtr->bTransparent)
	return SendMessageA (GetParent (hwnd), WM_ERASEBKGND, wParam, lParam);

    return DefWindowProcA (hwnd, WM_ERASEBKGND, wParam, lParam);
}


static LRESULT
TOOLBAR_GetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    return infoPtr->hFont;
}


static LRESULT
TOOLBAR_LButtonDblClk (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    POINT pt;
    INT   nHit;

    pt.x = (INT)LOWORD(lParam);
    pt.y = (INT)HIWORD(lParam);
    nHit = TOOLBAR_InternalHitTest (hwnd, &pt);

    if (nHit >= 0) {
	btnPtr = &infoPtr->buttons[nHit];
	if (!(btnPtr->fsState & TBSTATE_ENABLED))
	    return 0;
	SetCapture (hwnd);
	infoPtr->bCaptured = TRUE;
	infoPtr->nButtonDown = nHit;

	btnPtr->fsState |= TBSTATE_PRESSED;

        InvalidateRect(hwnd, &btnPtr->rect, TRUE);
    }
    else if (GetWindowLongA (hwnd, GWL_STYLE) & CCS_ADJUSTABLE)
	TOOLBAR_Customize (hwnd);

    return 0;
}


static LRESULT
TOOLBAR_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    POINT pt;
    INT   nHit;

    if (infoPtr->hwndToolTip)
	TOOLBAR_RelayEvent (infoPtr->hwndToolTip, hwnd,
			    WM_LBUTTONDOWN, wParam, lParam);

    pt.x = (INT)LOWORD(lParam);
    pt.y = (INT)HIWORD(lParam);
    nHit = TOOLBAR_InternalHitTest (hwnd, &pt);

    if (nHit >= 0) {
	btnPtr = &infoPtr->buttons[nHit];
	if (!(btnPtr->fsState & TBSTATE_ENABLED))
	    return 0;

	if (btnPtr->fsStyle &  TBSTYLE_DROPDOWN)
	{
	    NMTOOLBARA nmtb;

	    nmtb.hdr.hwndFrom = hwnd;
	    nmtb.hdr.idFrom = GetWindowLongA (hwnd, GWL_ID);
	    nmtb.hdr.code = TBN_DROPDOWN;
	    nmtb.iItem = btnPtr->idCommand;

	    SendMessageA (infoPtr->hwndNotify, WM_NOTIFY,
			  (WPARAM)nmtb.hdr.idFrom, (LPARAM)&nmtb);
	}

	SetCapture (hwnd);
	infoPtr->bCaptured = TRUE;
	infoPtr->nButtonDown = nHit;
	infoPtr->nOldHit = nHit;

	btnPtr->fsState |= TBSTATE_PRESSED;
        btnPtr->bHot = FALSE;

        InvalidateRect(hwnd, &btnPtr->rect, TRUE);
    }

    return 0;
}

static LRESULT
TOOLBAR_LButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *btnPtr;
    POINT pt;
    INT   nHit;
    INT   nOldIndex = -1;
    BOOL  bSendMessage = TRUE;

    if (infoPtr->hwndToolTip)
	TOOLBAR_RelayEvent (infoPtr->hwndToolTip, hwnd,
			    WM_LBUTTONUP, wParam, lParam);

    pt.x = (INT)LOWORD(lParam);
    pt.y = (INT)HIWORD(lParam);
    nHit = TOOLBAR_InternalHitTest (hwnd, &pt);

    /* restore hot effect to hot button disabled by TOOLBAR_LButtonDown() */
    if(infoPtr->nHotItem >= 0)
        infoPtr->buttons[infoPtr->nHotItem].bHot = TRUE;

    if ((infoPtr->bCaptured) && (infoPtr->nButtonDown >= 0)) {
	infoPtr->bCaptured = FALSE;
	ReleaseCapture ();
	btnPtr = &infoPtr->buttons[infoPtr->nButtonDown];
	btnPtr->fsState &= ~TBSTATE_PRESSED;

	if (nHit == infoPtr->nButtonDown) {
	    if (btnPtr->fsStyle & TBSTYLE_CHECK) {
		if (btnPtr->fsStyle & TBSTYLE_GROUP) {
		    nOldIndex = TOOLBAR_GetCheckedGroupButtonIndex (infoPtr,
			infoPtr->nButtonDown);
		    if (nOldIndex == infoPtr->nButtonDown)
			bSendMessage = FALSE;
		    if ((nOldIndex != infoPtr->nButtonDown) && 
			(nOldIndex != -1))
			infoPtr->buttons[nOldIndex].fsState &= ~TBSTATE_CHECKED;
		    btnPtr->fsState |= TBSTATE_CHECKED;
		}
		else {
		    if (btnPtr->fsState & TBSTATE_CHECKED)
			btnPtr->fsState &= ~TBSTATE_CHECKED;
		    else
			btnPtr->fsState |= TBSTATE_CHECKED;
		}
	    }
	}
	else
	    bSendMessage = FALSE;

	if (nOldIndex != -1)
            InvalidateRect(hwnd, &infoPtr->buttons[nOldIndex].rect, TRUE); 
        
        InvalidateRect(hwnd, &btnPtr->rect, TRUE);

	if (bSendMessage) {
	    SendMessageA (GetParent(hwnd), WM_COMMAND,
			  MAKEWPARAM(btnPtr->idCommand, 0), (LPARAM)hwnd);

//	    if ((GetWindowLongA(hwnd, GWL_STYLE) & TBSTYLE_DROPDOWN) ||
//		(btnPtr->fsStyle & 0x08/* BTNS_DROPDOWN */)) {
       /* 
        * This appears to be an error. Instead of checking the style of the
        * button in question wine was checking the style of the toolbar 
        * itself. This caused a number of strange behaviors. In my 
        * invistigation i think the whole dropdown thing is still fairly
        * broken. but this helps fix some of the problems.
        */

	if (btnPtr->fsStyle &  TBSTYLE_DROPDOWN) {
	       NMTOOLBARW	nmtb;

	       nmtb.hdr.hwndFrom = hwnd;
	       nmtb.hdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
	       nmtb.hdr.code     = TBN_DROPDOWN;
	       nmtb.iItem        = nHit;
	       /* nmtb.tbButton not used with TBN_DROPDOWN */
	       if ((btnPtr->iString >= 0) && (btnPtr->iString < infoPtr->nNumStrings)) {
		  nmtb.pszText      = infoPtr->strings[btnPtr->iString];
		  nmtb.cchText      = lstrlenW(nmtb.pszText);
	       } else {
		  nmtb.pszText      = NULL;
		  nmtb.cchText      = 0;
	       }
	       nmtb.rcButton     = btnPtr->rect;

	       SendMessageW(infoPtr->hwndNotify, WM_NOTIFY,
			    (WPARAM)nmtb.hdr.idFrom, (LPARAM)&nmtb);
	    }
	}
	infoPtr->nButtonDown = -1;
	infoPtr->nOldHit = -1;
    }

    return 0;
}

static LRESULT
TOOLBAR_MouseLeave (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    TBUTTON_INFO *hotBtnPtr;

    if (infoPtr->nOldHit < 0)
      return TRUE;

    hotBtnPtr = &infoPtr->buttons[infoPtr->nOldHit];

    /* Redraw the button if the last button we were over is the hot button and it
       is enabled */
    if((infoPtr->nOldHit == infoPtr->nHotItem) && (hotBtnPtr->fsState & TBSTATE_ENABLED))
    {
	hotBtnPtr->bHot = FALSE;
		    
        InvalidateRect (hwnd, &hotBtnPtr->rect, TRUE);
    }

    infoPtr->nOldHit = -1; /* reset the old hit index as we've left the toolbar */
    infoPtr->nHotItem = -2; /* It has to be initially different from nOldHit */

    return TRUE;
}

static LRESULT
TOOLBAR_MouseMove (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TBUTTON_INFO *btnPtr, *oldBtnPtr;
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    POINT pt;
    INT   nHit;
    TRACKMOUSEEVENT trackinfo;

    /* fill in the TRACKMOUSEEVENT struct */
    trackinfo.cbSize = sizeof(TRACKMOUSEEVENT);
    trackinfo.dwFlags = TME_QUERY;
    trackinfo.hwndTrack = hwnd;
    trackinfo.dwHoverTime = HOVER_DEFAULT;

    /* call _TrackMouseEvent to see if we are currently tracking for this hwnd */
    _TrackMouseEvent(&trackinfo);

    /* Make sure tracking is enabled so we recieve a WM_MOUSELEAVE message */
    if(!(trackinfo.dwFlags & TME_LEAVE)) {
        trackinfo.dwFlags = TME_LEAVE; /* notify upon leaving */

        /* call TRACKMOUSEEVENT so we recieve a WM_MOUSELEAVE message */
        /* and can properly deactivate the hot toolbar button */
        _TrackMouseEvent(&trackinfo);
   }

    if (infoPtr->hwndToolTip)
	TOOLBAR_RelayEvent (infoPtr->hwndToolTip, hwnd,
			    WM_MOUSEMOVE, wParam, lParam);

    pt.x = (INT)LOWORD(lParam);
    pt.y = (INT)HIWORD(lParam);

    nHit = TOOLBAR_InternalHitTest (hwnd, &pt);

    if (infoPtr->nOldHit != nHit)
    {
	/* Remove the effect of an old hot button if the button was enabled and was
	   drawn with the hot button effect */
	if(infoPtr->nOldHit >= 0 && infoPtr->nOldHit == infoPtr->nHotItem && 
		(infoPtr->buttons[infoPtr->nOldHit].fsState & TBSTATE_ENABLED))
	{
	    oldBtnPtr = &infoPtr->buttons[infoPtr->nOldHit];
	    oldBtnPtr->bHot = FALSE;
		    
	    InvalidateRect (hwnd, &oldBtnPtr->rect, TRUE);
	}

	/* It's not a separator or in nowhere. It's a hot button. */
	if (nHit >= 0)
	{
	    btnPtr = &infoPtr->buttons[nHit];
	    btnPtr->bHot = TRUE;

	    infoPtr->nHotItem = nHit;

            /* only enabled buttons show hot effect */            
            if(infoPtr->buttons[nHit].fsState & TBSTATE_ENABLED)
                InvalidateRect(hwnd, &btnPtr->rect, TRUE);
	}

    if (infoPtr->bCaptured) {
	    btnPtr = &infoPtr->buttons[infoPtr->nButtonDown];
	    if (infoPtr->nOldHit == infoPtr->nButtonDown) {
		btnPtr->fsState &= ~TBSTATE_PRESSED;
                InvalidateRect(hwnd, &btnPtr->rect, TRUE);
	    }
	    else if (nHit == infoPtr->nButtonDown) {
		btnPtr->fsState |= TBSTATE_PRESSED;
                InvalidateRect(hwnd, &btnPtr->rect, TRUE);
	    }
	}
	infoPtr->nOldHit = nHit;
    }
    return 0;
}


inline static LRESULT
TOOLBAR_NCActivate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
/*    if (wndPtr->dwStyle & CCS_NODIVIDER) */
	return DefWindowProcA (hwnd, WM_NCACTIVATE, wParam, lParam);
/*    else */
/*	return TOOLBAR_NCPaint (wndPtr, wParam, lParam); */
}


inline static LRESULT
TOOLBAR_NCCalcSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (!(GetWindowLongA (hwnd, GWL_STYLE) & CCS_NODIVIDER))
	((LPRECT)lParam)->top += GetSystemMetrics(SM_CYEDGE);

    return DefWindowProcA (hwnd, WM_NCCALCSIZE, wParam, lParam);
}


static LRESULT
TOOLBAR_NCCreate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr;

    /* allocate memory for info structure */
    infoPtr = (TOOLBAR_INFO *)COMCTL32_Alloc (sizeof(TOOLBAR_INFO));
    SetWindowLongA (hwnd, 0, (DWORD)infoPtr);

    /* paranoid!! */
    infoPtr->dwStructSize = sizeof(TBBUTTON);

    /* fix instance handle, if the toolbar was created by CreateToolbarEx() */
    if (!GetWindowLongA (hwnd, GWL_HINSTANCE)) {
        HINSTANCE hInst = (HINSTANCE)GetWindowLongA (GetParent (hwnd), GWL_HINSTANCE);
	SetWindowLongA (hwnd, GWL_HINSTANCE, (DWORD)hInst);
    }

    return DefWindowProcA (hwnd, WM_NCCREATE, wParam, lParam);
}


static LRESULT
TOOLBAR_NCPaint (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    RECT rcWindow;
    HDC hdc;

    if (dwStyle & WS_MINIMIZE)
	return 0; /* Nothing to do */

    DefWindowProcA (hwnd, WM_NCPAINT, wParam, lParam);

    if (!(hdc = GetDCEx (hwnd, 0, DCX_USESTYLE | DCX_WINDOW)))
	return 0;

    if (!(dwStyle & CCS_NODIVIDER))
    {
	GetWindowRect (hwnd, &rcWindow);
	OffsetRect (&rcWindow, -rcWindow.left, -rcWindow.top);
	if( dwStyle & WS_BORDER )
	    OffsetRect (&rcWindow, 1, 1);
	DrawEdge (hdc, &rcWindow, EDGE_ETCHED, BF_TOP);
    }

    ReleaseDC( hwnd, hdc );

    return 0;
}


inline static LRESULT
TOOLBAR_Notify (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    LPNMHDR lpnmh = (LPNMHDR)lParam;

    // TRACE("passing WM_NOTIFY!\n");

    if ((infoPtr->hwndToolTip) && (lpnmh->hwndFrom == infoPtr->hwndToolTip)) {
	SendMessageA (infoPtr->hwndNotify, WM_NOTIFY,	wParam, lParam);

#if 0
	if (lpnmh->code == TTN_GETDISPINFOA) {
	    LPNMTTDISPINFOA lpdi = (LPNMTTDISPINFOA)lParam;

	    // FIXME("retrieving ASCII string\n");

	}
	else if (lpnmh->code == TTN_GETDISPINFOW) {
	    LPNMTTDISPINFOW lpdi = (LPNMTTDISPINFOW)lParam;

	    // FIXME("retrieving UNICODE string\n");

	}
#endif
    }

    return 0;
}


static LRESULT
TOOLBAR_Paint (HWND hwnd, WPARAM wParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr(hwnd);
    HDC hdc;
    PAINTSTRUCT ps;

    /* fill ps.rcPaint with a default rect */
    memcpy(&(ps.rcPaint), &(infoPtr->rcBound), sizeof(infoPtr->rcBound)); 

    TOOLBAR_CalcToolbar( hwnd );
    hdc = wParam==0 ? BeginPaint(hwnd, &ps) : (HDC)wParam;
    TOOLBAR_Refresh (hwnd, hdc, &ps);
    if (!wParam) EndPaint (hwnd, &ps);

    return 0;
}


static LRESULT
TOOLBAR_Size (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    RECT parent_rect;
    RECT window_rect;
    HWND parent;
    INT  x, y;
    INT  cx, cy;
    INT  flags;
    UINT uPosFlags = 0;

    /* Resize deadlock check */
    if (infoPtr->bAutoSize) {
	infoPtr->bAutoSize = FALSE;
	return 0;
    }

    flags = (INT) wParam;

    /* FIXME for flags =
     * SIZE_MAXIMIZED, SIZE_MAXSHOW, SIZE_MINIMIZED
     */

    // TRACE("sizing toolbar!\n");

    if (flags == SIZE_RESTORED) {
	/* width and height don't apply */
	parent = GetParent (hwnd);
	GetClientRect(parent, &parent_rect);
	x = parent_rect.left;
	y = parent_rect.top;

	if (dwStyle & CCS_NORESIZE) {
	    uPosFlags |= (SWP_NOSIZE | SWP_NOMOVE);

	    /*
             * this sets the working width of the toolbar, and
             * Calc Toolbar will not adjust it, only the height
             */
	    infoPtr->nWidth = parent_rect.right - parent_rect.left; 
	    cy = infoPtr->nHeight;
	    cx = infoPtr->nWidth;
	    TOOLBAR_CalcToolbar (hwnd);
	    infoPtr->nWidth = cx;
	    infoPtr->nHeight = cy;
	}
	else {
	    infoPtr->nWidth = parent_rect.right - parent_rect.left;
	    TOOLBAR_CalcToolbar (hwnd);
	    cy = infoPtr->nHeight;
	    cx = infoPtr->nWidth;

	    if (dwStyle & CCS_NOMOVEY) {
		GetWindowRect(hwnd, &window_rect);
		ScreenToClient(parent, (LPPOINT)&window_rect.left);
		y = window_rect.top;
	    }
	}

	if (dwStyle & CCS_NOPARENTALIGN) {
	    uPosFlags |= SWP_NOMOVE;
	    cy = infoPtr->nHeight;
	    cx = infoPtr->nWidth;
	}

	if (!(dwStyle & CCS_NODIVIDER))
	    cy += GetSystemMetrics(SM_CYEDGE);

	if (dwStyle & WS_BORDER)
	{
	    x = y = 1;
	    cy += GetSystemMetrics(SM_CYEDGE);
	    cx += GetSystemMetrics(SM_CYEDGE);
	}

	SetWindowPos (hwnd, 0, parent_rect.left - x, parent_rect.top - y,
			cx, cy, uPosFlags | SWP_NOZORDER);
    }
    return 0;
}


static LRESULT
TOOLBAR_StyleChanged (HWND hwnd, INT nType, LPSTYLESTRUCT lpStyle)
{
    TOOLBAR_INFO *infoPtr = TOOLBAR_GetInfoPtr (hwnd);

    if (nType == GWL_STYLE) {
	if (lpStyle->styleNew & TBSTYLE_LIST) {
	    infoPtr->dwDTFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	}
	else {
	    infoPtr->dwDTFlags = DT_CENTER;
	}
    }

    TOOLBAR_AutoSize (hwnd);

    InvalidateRect(hwnd, NULL, FALSE);

    return 0;
}



static LRESULT WINAPI
ToolbarWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
       case WM_DESTROY:
           return TOOLBAR_Destroy (hwnd, wParam, lParam);

       case WM_NCCREATE:
           return TOOLBAR_NCCreate (hwnd, wParam, lParam);
    }

    if (!TOOLBAR_GetInfoPtr (hwnd))
    {
       return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
	case TB_ADDBITMAP:
	    return TOOLBAR_AddBitmap (hwnd, wParam, lParam);

	case TB_ADDBUTTONSA:
	    return TOOLBAR_AddButtonsA (hwnd, wParam, lParam);

	case TB_ADDBUTTONSW:
	    return TOOLBAR_AddButtonsW (hwnd, wParam, lParam);

	case TB_ADDSTRINGA:
	    return TOOLBAR_AddStringA (hwnd, wParam, lParam);

	case TB_ADDSTRINGW:
	    return TOOLBAR_AddStringW (hwnd, wParam, lParam);

	case TB_AUTOSIZE:
	    return TOOLBAR_AutoSize (hwnd);

	case TB_BUTTONCOUNT:
	    return TOOLBAR_ButtonCount (hwnd, wParam, lParam);

	case TB_BUTTONSTRUCTSIZE:
	    return TOOLBAR_ButtonStructSize (hwnd, wParam, lParam);

	case TB_CHANGEBITMAP:
	    return TOOLBAR_ChangeBitmap (hwnd, wParam, lParam);

	case TB_CHECKBUTTON:
	    return TOOLBAR_CheckButton (hwnd, wParam, lParam);

	case TB_COMMANDTOINDEX:
	    return TOOLBAR_CommandToIndex (hwnd, wParam, lParam);

	case TB_CUSTOMIZE:
	    return TOOLBAR_Customize (hwnd);

	case TB_DELETEBUTTON:
	    return TOOLBAR_DeleteButton (hwnd, wParam, lParam);

	case TB_ENABLEBUTTON:
	    return TOOLBAR_EnableButton (hwnd, wParam, lParam);

	case TB_GETANCHORHIGHLIGHT:
	    return TOOLBAR_GetAnchorHighlight (hwnd);

	case TB_GETBITMAP:
	    return TOOLBAR_GetBitmap (hwnd, wParam, lParam);

	case TB_GETBITMAPFLAGS:
	    return TOOLBAR_GetBitmapFlags (hwnd, wParam, lParam);

	case TB_GETBUTTON:
	    return TOOLBAR_GetButton (hwnd, wParam, lParam);

	case TB_GETBUTTONINFOA:
	    return TOOLBAR_GetButtonInfoA (hwnd, wParam, lParam);

	case TB_GETBUTTONINFOW:
	    return TOOLBAR_GetButtonInfoW (hwnd, wParam, lParam);

	case TB_GETBUTTONSIZE:
	    return TOOLBAR_GetButtonSize (hwnd);

	case TB_GETBUTTONTEXTA:
	    return TOOLBAR_GetButtonTextA (hwnd, wParam, lParam);

	case TB_GETBUTTONTEXTW:
	    return TOOLBAR_GetButtonTextW (hwnd, wParam, lParam);

/*	case TB_GETCOLORSCHEME:			*/ /* 4.71 */

	case TB_GETDISABLEDIMAGELIST:
	    return TOOLBAR_GetDisabledImageList (hwnd, wParam, lParam);

	case TB_GETEXTENDEDSTYLE:
	    return TOOLBAR_GetExtendedStyle (hwnd);

	case TB_GETHOTIMAGELIST:
	    return TOOLBAR_GetHotImageList (hwnd, wParam, lParam);

	case TB_GETHOTITEM:
	    return TOOLBAR_GetHotItem (hwnd);

	case TB_GETIMAGELIST:
	    return TOOLBAR_GetImageList (hwnd, wParam, lParam);

/*	case TB_GETINSERTMARK:			*/ /* 4.71 */
/*	case TB_GETINSERTMARKCOLOR:		*/ /* 4.71 */

	case TB_GETITEMRECT:
	    return TOOLBAR_GetItemRect (hwnd, wParam, lParam);

	case TB_GETMAXSIZE:
	    return TOOLBAR_GetMaxSize (hwnd, wParam, lParam);

/*	case TB_GETOBJECT:			*/ /* 4.71 */
/*	case TB_GETPADDING:			*/ /* 4.71 */

	case TB_GETRECT:
	    return TOOLBAR_GetRect (hwnd, wParam, lParam);

	case TB_GETROWS:
	    return TOOLBAR_GetRows (hwnd, wParam, lParam);

	case TB_GETSTATE:
	    return TOOLBAR_GetState (hwnd, wParam, lParam);

	case TB_GETSTYLE:
	    return TOOLBAR_GetStyle (hwnd, wParam, lParam);

	case TB_GETTEXTROWS:
	    return TOOLBAR_GetTextRows (hwnd, wParam, lParam);

	case TB_GETTOOLTIPS:
	    return TOOLBAR_GetToolTips (hwnd, wParam, lParam);

	case TB_GETUNICODEFORMAT:
	    return TOOLBAR_GetUnicodeFormat (hwnd, wParam, lParam);

	case CCM_GETVERSION:
	    return TOOLBAR_GetVersion (hwnd);

	case TB_HIDEBUTTON:
	    return TOOLBAR_HideButton (hwnd, wParam, lParam);

	case TB_HITTEST:
	    return TOOLBAR_HitTest (hwnd, wParam, lParam);

	case TB_INDETERMINATE:
	    return TOOLBAR_Indeterminate (hwnd, wParam, lParam);

	case TB_INSERTBUTTONA:
	    return TOOLBAR_InsertButtonA (hwnd, wParam, lParam);

	case TB_INSERTBUTTONW:
	    return TOOLBAR_InsertButtonW (hwnd, wParam, lParam);

/*	case TB_INSERTMARKHITTEST:		*/ /* 4.71 */

	case TB_ISBUTTONCHECKED:
	    return TOOLBAR_IsButtonChecked (hwnd, wParam, lParam);

	case TB_ISBUTTONENABLED:
	    return TOOLBAR_IsButtonEnabled (hwnd, wParam, lParam);

	case TB_ISBUTTONHIDDEN:
	    return TOOLBAR_IsButtonHidden (hwnd, wParam, lParam);

	case TB_ISBUTTONHIGHLIGHTED:
	    return TOOLBAR_IsButtonHighlighted (hwnd, wParam, lParam);

	case TB_ISBUTTONINDETERMINATE:
	    return TOOLBAR_IsButtonIndeterminate (hwnd, wParam, lParam);

	case TB_ISBUTTONPRESSED:
	    return TOOLBAR_IsButtonPressed (hwnd, wParam, lParam);

	case TB_LOADIMAGES:			   /* 4.70 */
	    // FIXME("missing standard imagelists\n");
	    return 0;

/*	case TB_MAPACCELERATORA:		*/ /* 4.71 */
/*	case TB_MAPACCELERATORW:		*/ /* 4.71 */
/*	case TB_MARKBUTTON:			*/ /* 4.71 */
/*	case TB_MOVEBUTTON:			*/ /* 4.71 */

	case TB_PRESSBUTTON:
	    return TOOLBAR_PressButton (hwnd, wParam, lParam);

/*	case TB_REPLACEBITMAP: */

	case TB_SAVERESTOREA:
	    return TOOLBAR_SaveRestoreA (hwnd, wParam, lParam);

	case TB_SAVERESTOREW:
	    return TOOLBAR_SaveRestoreW (hwnd, wParam, lParam);

	case TB_SETANCHORHIGHLIGHT:
	    return TOOLBAR_SetAnchorHighlight (hwnd, wParam);

	case TB_SETBITMAPSIZE:
	    return TOOLBAR_SetBitmapSize (hwnd, wParam, lParam);

	case TB_SETBUTTONINFOA:
	    return TOOLBAR_SetButtonInfoA (hwnd, wParam, lParam);

	case TB_SETBUTTONINFOW:
	    return TOOLBAR_SetButtonInfoW (hwnd, wParam, lParam);

	case TB_SETBUTTONSIZE:
	    return TOOLBAR_SetButtonSize (hwnd, wParam, lParam);

	case TB_SETBUTTONWIDTH:
	    return TOOLBAR_SetButtonWidth (hwnd, wParam, lParam);

	case TB_SETCMDID:
	    return TOOLBAR_SetCmdId (hwnd, wParam, lParam);

/*	case TB_SETCOLORSCHEME:			*/ /* 4.71 */

	case TB_SETDISABLEDIMAGELIST:
	    return TOOLBAR_SetDisabledImageList (hwnd, wParam, lParam);

	case TB_SETDRAWTEXTFLAGS:
	    return TOOLBAR_SetDrawTextFlags (hwnd, wParam, lParam);

	case TB_SETEXTENDEDSTYLE:
	    return TOOLBAR_SetExtendedStyle (hwnd, wParam, lParam);

	case TB_SETHOTIMAGELIST:
	    return TOOLBAR_SetHotImageList (hwnd, wParam, lParam);

	case TB_SETHOTITEM:
	    return TOOLBAR_SetHotItem (hwnd, wParam);

	case TB_SETIMAGELIST:
	    return TOOLBAR_SetImageList (hwnd, wParam, lParam);

	case TB_SETINDENT:
	    return TOOLBAR_SetIndent (hwnd, wParam, lParam);

/*	case TB_SETINSERTMARK:			*/ /* 4.71 */

	case TB_SETINSERTMARKCOLOR:
	    return TOOLBAR_SetInsertMarkColor (hwnd, wParam, lParam);

	case TB_SETMAXTEXTROWS:
	    return TOOLBAR_SetMaxTextRows (hwnd, wParam, lParam);

/*	case TB_SETPADDING:			*/ /* 4.71 */

	case TB_SETPARENT:
	    return TOOLBAR_SetParent (hwnd, wParam, lParam);

	case TB_SETROWS:
	    return TOOLBAR_SetRows (hwnd, wParam, lParam);

	case TB_SETSTATE:
	    return TOOLBAR_SetState (hwnd, wParam, lParam);

	case TB_SETSTYLE:
	    return TOOLBAR_SetStyle (hwnd, wParam, lParam);

	case TB_SETTOOLTIPS:
	    return TOOLBAR_SetToolTips (hwnd, wParam, lParam);

	case TB_SETUNICODEFORMAT:
	    return TOOLBAR_SetUnicodeFormat (hwnd, wParam, lParam);

	case CCM_SETVERSION:
	    return TOOLBAR_SetVersion (hwnd, (INT)wParam);


/*	case WM_CHAR: */

	case WM_CREATE:
	    return TOOLBAR_Create (hwnd, wParam, lParam);

	case WM_ERASEBKGND:
	    return TOOLBAR_EraseBackground (hwnd, wParam, lParam);

	case WM_GETFONT:
		return TOOLBAR_GetFont (hwnd, wParam, lParam);

/*	case WM_KEYDOWN: */
/*	case WM_KILLFOCUS: */

	case WM_LBUTTONDBLCLK:
	    return TOOLBAR_LButtonDblClk (hwnd, wParam, lParam);

	case WM_LBUTTONDOWN:
	    return TOOLBAR_LButtonDown (hwnd, wParam, lParam);

	case WM_LBUTTONUP:
	    return TOOLBAR_LButtonUp (hwnd, wParam, lParam);

	case WM_MOUSEMOVE:
	    return TOOLBAR_MouseMove (hwnd, wParam, lParam);

	case WM_MOUSELEAVE:
	    return TOOLBAR_MouseLeave (hwnd, wParam, lParam);	

	case WM_NCACTIVATE:
	    return TOOLBAR_NCActivate (hwnd, wParam, lParam);

	case WM_NCCALCSIZE:
	    return TOOLBAR_NCCalcSize (hwnd, wParam, lParam);

	case WM_NCPAINT:
	    return TOOLBAR_NCPaint (hwnd, wParam, lParam);

	case WM_NOTIFY:
	    return TOOLBAR_Notify (hwnd, wParam, lParam);

/*	case WM_NOTIFYFORMAT: */

	case WM_PAINT:
	    return TOOLBAR_Paint (hwnd, wParam);

	case WM_SIZE:
	    return TOOLBAR_Size (hwnd, wParam, lParam);

	case WM_STYLECHANGED:
	    return TOOLBAR_StyleChanged (hwnd, (INT)wParam, (LPSTYLESTRUCT)lParam);

/*	case WM_SYSCOLORCHANGE: */

/*	case WM_WININICHANGE: */

	case WM_CHARTOITEM:
	case WM_COMMAND:
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
	case WM_VKEYTOITEM:
	    return SendMessageA (GetParent (hwnd), uMsg, wParam, lParam);

        default:
//            if (uMsg >= WM_USER)
//              ERR (toolbar, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
TOOLBAR_Register (void)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)ToolbarWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(TOOLBAR_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = TOOLBARCLASSNAMEA;
 
    RegisterClassA (&wndClass);
}


VOID
TOOLBAR_Unregister (void)
{
    UnregisterClassA (TOOLBARCLASSNAMEA, (HINSTANCE)NULL);
}

