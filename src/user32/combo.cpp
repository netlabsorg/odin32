/* $Id: combo.cpp,v 1.33 2001-02-17 20:16:31 sandervl Exp $ */
/*
 * Combo controls
 *
 * Copyright 1997 Alex Korobka
 * Copyright 1999 Christoph Bratschi
 *
 * FIXME: roll up in Netscape 3.01.
 *
 * Corel version: 20000513
 * (WINE version: 991212)
 *
 * Status:  in progress
 * Version: ?.??
 */

#include <string.h>
#include <os2win.h>
#include "controls.h"
#include "combo.h"
#include "initterm.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

#define DBG_LOCALLOG    DBG_combo
#include "dbglocal.h"

  /* bits in the dwKeyData */
#define KEYDATA_ALT             0x2000
#define KEYDATA_PREVSTATE       0x4000

/*
 * Additional combo box definitions
 */

#define CB_NOTIFY( lphc, code ) \
        (SendMessageA( (lphc)->owner, WM_COMMAND, \
                         MAKEWPARAM(GetWindowLongA((lphc)->hwndself,GWL_ID), (code)), (lphc)->hwndself))
#define CB_GETEDITTEXTLENGTH( lphc ) \
        (SendMessageA( (lphc)->hWndEdit, WM_GETTEXTLENGTH, 0, 0 ))

/*
 * Look and feel dependant "constants"
 */
#define COMBO_YBORDERGAP         5
#define COMBO_XBORDERSIZE()      ( 2 )
#define COMBO_YBORDERSIZE()      ( 2 )
#define COMBO_EDITBUTTONSPACE()  ( 0 )
#define EDIT_CONTROL_PADDING()   ( 1 )

/***********************************************************************
 *           COMBO_NCCreate
 */
static LRESULT COMBO_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO          lphc;

   if ( hwnd &&
      (lphc = (LPHEADCOMBO)HeapAlloc(GetProcessHeap(), 0, sizeof(HEADCOMBO))) )
   {
        LPCREATESTRUCTA     lpcs = (CREATESTRUCTA*)lParam;
        DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
        DWORD dwExStyle = GetWindowLongA(hwnd,GWL_EXSTYLE);

        memset( lphc, 0, sizeof(HEADCOMBO) );
        SetInfoPtr(hwnd,(DWORD)lphc);

       /* some braindead apps do try to use scrollbar/border flags */

        lphc->dwStyle = (lpcs->style & ~(WS_BORDER | WS_HSCROLL | WS_VSCROLL));
        dwStyle &= ~(WS_BORDER | WS_HSCROLL | WS_VSCROLL);
        SetWindowLongA(hwnd,GWL_STYLE,dwStyle);

        /*
         * We also have to remove the client edge style to make sure
         * we don't end-up with a non client area.
         */
        dwExStyle &= ~(WS_EX_CLIENTEDGE);
        SetWindowLongA(hwnd,GWL_EXSTYLE,dwExStyle);

        if( !(lpcs->style & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) )
              lphc->dwStyle |= CBS_HASSTRINGS;
        if( !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
              lphc->wState |= CBF_NOTIFY;

        //TRACE("[0x%08x], style = %08x\n",
        //             (UINT)lphc, lphc->dwStyle );

        return TRUE;
    }
    return FALSE;
}

/***********************************************************************
 *           COMBO_NCDestroy
 */
static LRESULT COMBO_NCDestroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   if( lphc )
   {
       //TRACE("[%04x]: freeing storage\n", CB_HWND(lphc));

       if( (CB_GETTYPE(lphc) != CBS_SIMPLE) && lphc->hWndLBox )
           DestroyWindow( lphc->hWndLBox );

       HeapFree( GetProcessHeap(), 0, lphc );
       SetInfoPtr(hwnd,0);
   }

   return DefWindowProcA(hwnd,WM_NCDESTROY,wParam,lParam);
}

/***********************************************************************
 *           CBGetTextAreaHeight
 *
 * This method will calculate the height of the text area of the
 * combobox.
 * The height of the text area is set in two ways.
 * It can be set explicitely through a combobox message of through a
 * WM_MEASUREITEM callback.
 * If this is not the case, the height is set to 13 dialog units.
 * This height was determined through experimentation.
 */
static INT CBGetTextAreaHeight(
  HWND        hwnd,
  LPHEADCOMBO lphc)
{
  INT iTextItemHeight;

  if( lphc->editHeight ) /* explicitly set height */
  {
    iTextItemHeight = lphc->editHeight;
  }
  else
  {
    TEXTMETRICA tm;
    HDC         hDC       = GetDC(hwnd);
    HFONT       hPrevFont = 0;
    INT         baseUnitY;

    if (lphc->hFont)
      hPrevFont = SelectObject( hDC, lphc->hFont );

    GetTextMetricsA(hDC, &tm);

    baseUnitY = tm.tmHeight;

    if( hPrevFont )
      SelectObject( hDC, hPrevFont );

    ReleaseDC(hwnd, hDC);

    iTextItemHeight = ((13 * baseUnitY) / 8);

    /*
     * This "formula" calculates the height of the complete control.
     * To calculate the height of the text area, we have to remove the
     * borders.
     */
    iTextItemHeight -= 2*COMBO_YBORDERSIZE();
  }

  /*
   * Check the ownerdraw case if we haven't asked the parent the size
   * of the item yet.
   */
  if ( CB_OWNERDRAWN(lphc) &&
       (lphc->wState & CBF_MEASUREITEM) )
  {
    MEASUREITEMSTRUCT measureItem;
    RECT              clientRect;
    INT               originalItemHeight = iTextItemHeight;

    /*
     * We use the client rect for the width of the item.
     */
    GetClientRect(hwnd, &clientRect);

    lphc->wState &= ~CBF_MEASUREITEM;

    /*
     * Send a first one to measure the size of the text area
     */
    measureItem.CtlType    = ODT_COMBOBOX;
    measureItem.CtlID      = GetWindowLongA(lphc->hwndself,GWL_ID);
    measureItem.itemID     = -1;
    measureItem.itemWidth  = clientRect.right;
    measureItem.itemHeight = iTextItemHeight - 6; /* ownerdrawn cb is taller */
    measureItem.itemData   = 0;
    SendMessageA(lphc->owner, WM_MEASUREITEM,
                 (WPARAM)measureItem.CtlID, (LPARAM)&measureItem);
    iTextItemHeight = 6 + measureItem.itemHeight;

    /*
     * Send a second one in the case of a fixed ownerdraw list to calculate the
     * size of the list items. (we basically do this on behalf of the listbox)
     */
    if (lphc->dwStyle & CBS_OWNERDRAWFIXED)
    {
      measureItem.CtlType    = ODT_COMBOBOX;
      measureItem.CtlID      = GetWindowLongA(lphc->hwndself,GWL_ID);
      measureItem.itemID     = 0;
      measureItem.itemWidth  = clientRect.right;
      measureItem.itemHeight = originalItemHeight;
      measureItem.itemData   = 0;
      SendMessageA(lphc->owner, WM_MEASUREITEM,
                   (WPARAM)measureItem.CtlID, (LPARAM)&measureItem);
      lphc->fixedOwnerDrawHeight = measureItem.itemHeight;
    }

    /*
     * Keep the size for the next time
     */
    lphc->editHeight = iTextItemHeight;
  }

  return iTextItemHeight;
}

/***********************************************************************
 *           CBForceDummyResize
 *
 * The dummy resize is used for listboxes that have a popup to trigger
 * a re-arranging of the contents of the combobox and the recalculation
 * of the size of the "real" control window.
 */
static void CBForceDummyResize(
  LPHEADCOMBO lphc)
{
  RECT windowRect;
  int newComboHeight;

  newComboHeight = CBGetTextAreaHeight(CB_HWND(lphc),lphc) + 2*COMBO_YBORDERSIZE();

  GetWindowRect(CB_HWND(lphc), &windowRect);

  /*
   * We have to be careful, resizing a combobox also has the meaning that the
   * dropped rect will be resized. In this case, we want to trigger a resize
   * to recalculate layout but we don't want to change the dropped rectangle
   * So, we pass the height of text area of control as the height.
   * this will cancel-out in the processing of the WM_WINDOWPOSCHANGING
   * message.
   */
  SetWindowPos( CB_HWND(lphc),
                (HWND)0,
                0, 0,
                windowRect.right  - windowRect.left,
                newComboHeight,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
}

/***********************************************************************
 *           CBCalcPlacement
 *
 * Set up component coordinates given valid lphc->RectCombo.
 */
static void CBCalcPlacement(
  HWND        hwnd,
  LPHEADCOMBO lphc,
  LPRECT      lprEdit,
  LPRECT      lprButton,
  LPRECT      lprLB)
{
#if 1
  /*
   * Again, start with the client rectangle.
   */
  GetClientRect(hwnd, lprEdit);

  /*
   * Remove the borders
   */
  InflateRect(lprEdit, -COMBO_XBORDERSIZE(), -COMBO_YBORDERSIZE());

  /*
   * Chop off the bottom part to fit with the height of the text area.
   */
  lprEdit->bottom = lprEdit->top + CBGetTextAreaHeight(hwnd, lphc);

  /*
   * The button starts the same vertical position as the text area.
   */
  CopyRect(lprButton, lprEdit);

  /*
   * If the combobox is "simple" there is no button.
   */
  if( CB_GETTYPE(lphc) == CBS_SIMPLE ) 
    lprButton->left = lprButton->right = lprButton->bottom = 0;
  else
  {
    /*
     * Let's assume the combobox button is the same width as the
     * scrollbar button.
     * size the button horizontally and cut-off the text area.
     */
    lprButton->left = lprButton->right - GetSystemMetrics(SM_CXVSCROLL);
    lprEdit->right  = lprButton->left;
  }
  
  /*
   * In the case of a dropdown, there is an additional spacing between the
   * text area and the button.
   */
  if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
  {
    lprEdit->right -= COMBO_EDITBUTTONSPACE();
  }

  /*
   * If we have an edit control, we space it away from the borders slightly.
   */
  if (CB_GETTYPE(lphc) != CBS_DROPDOWNLIST)
  {
    InflateRect(lprEdit, -EDIT_CONTROL_PADDING(), -EDIT_CONTROL_PADDING());
  }
  
  /*
   * Adjust the size of the listbox popup.
   */
  if( CB_GETTYPE(lphc) == CBS_SIMPLE )
  {
    /*
     * Use the client rectangle to initialize the listbox rectangle
     */
    GetClientRect(hwnd, lprLB);

    /*
     * Then, chop-off the top part.
     */
    lprLB->top = lprEdit->bottom + COMBO_YBORDERSIZE();
  }
  else
  {
    /*
     * Make sure the dropped width is as large as the combobox itself.
     */
    if (lphc->droppedWidth < (lprButton->right + COMBO_XBORDERSIZE()))
    {
      lprLB->right  = lprLB->left + (lprButton->right + COMBO_XBORDERSIZE());

      /*
       * In the case of a dropdown, the popup listbox is offset to the right.
       * so, we want to make sure it's flush with the right side of the 
       * combobox
       */
      if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
	lprLB->right -= COMBO_EDITBUTTONSPACE();
    }
    else
       lprLB->right = lprLB->left + lphc->droppedWidth;
  }
#else
  /*
   * Again, start with the client rectangle.
   */
  GetClientRect(hwnd, lprEdit);

  /*
   * Remove the borders
   */
  InflateRect(lprEdit, -COMBO_XBORDERSIZE(), -COMBO_YBORDERSIZE());

  /*
   * Chop off the bottom part to fit with the height of the text area.
   */
  lprEdit->bottom = lprEdit->top + CBGetTextAreaHeight(hwnd, lphc);

  /*
   * The button starts the same vertical position as the text area.
   */
  CopyRect(lprButton, lprEdit);

  /*
   * If the combobox is "simple" there is no button.
   */
  if( CB_GETTYPE(lphc) == CBS_SIMPLE )
    lprButton->left = lprButton->right = lprButton->bottom = 0;
  else
  {
    /*
     * Let's assume the combobox button is the same width as the
     * scrollbar button.
     * size the button horizontally and cut-off the text area.
     */
    lprButton->left = lprButton->right - GetSystemMetrics(SM_CXVSCROLL);
    lprEdit->right  = lprButton->left;
  }

  /*
   * In the case of a dropdown, there is an additional spacing between the
   * text area and the button.
   */
  if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
  {
    lprEdit->right -= COMBO_EDITBUTTONSPACE();
  }

  /*
   * If we have an edit control, we space it away from the borders slightly.
   */
  if (CB_GETTYPE(lphc) != CBS_DROPDOWNLIST)
  {
    InflateRect(lprEdit, -EDIT_CONTROL_PADDING(), -EDIT_CONTROL_PADDING());
  }

  /*
   * Adjust the size of the listbox popup.
   */
  if( CB_GETTYPE(lphc) == CBS_SIMPLE )
  {
    /*
     * Use the client rectangle to initialize the listbox rectangle
     */
    GetClientRect(hwnd, lprLB);

    /*
     * Then, chop-off the top part.
     */
    lprLB->top = lprEdit->bottom + COMBO_YBORDERSIZE();
  }
  else
  {
    //SvL: Listbox size might have changed!
    if(lphc->hWndLBox)
        GetWindowRect(lphc->hWndLBox, lprLB);

    /*
     * Make sure the dropped width is as large as the combobox itself.
     */
    if (lphc->droppedWidth < (lprButton->right + COMBO_XBORDERSIZE()))
    {
      lprLB->right  = lprLB->left + (lprButton->right + COMBO_XBORDERSIZE());

      /*
       * In the case of a dropdown, the popup listbox is offset to the right.
       * so, we want to make sure it's flush with the right side of the
       * combobox
       */
      if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
        lprLB->right -= COMBO_EDITBUTTONSPACE();
    }
    else
       lprLB->right = lprLB->left + lphc->droppedWidth;
  }
#endif
  //TRACE("\ttext\t= (%i,%i-%i,%i)\n",
  //      lprEdit->left, lprEdit->top, lprEdit->right, lprEdit->bottom);

  //TRACE("\tbutton\t= (%i,%i-%i,%i)\n",
  //      lprButton->left, lprButton->top, lprButton->right, lprButton->bottom);

  //TRACE("\tlbox\t= (%i,%i-%i,%i)\n",
  //      lprLB->left, lprLB->top, lprLB->right, lprLB->bottom );
}

/***********************************************************************
 *           CBGetDroppedControlRect
 */
static void CBGetDroppedControlRect( LPHEADCOMBO lphc, LPRECT lpRect)
{
    /* In windows, CB_GETDROPPEDCONTROLRECT returns the upper left corner
     of the combo box and the lower right corner of the listbox */

    GetWindowRect(lphc->hwndself, lpRect);

    lpRect->right =  lpRect->left + lphc->droppedRect.right - lphc->droppedRect.left;
    lpRect->bottom = lpRect->top + lphc->droppedRect.bottom - lphc->droppedRect.top;

}

/***********************************************************************
 *           COMBO_WindowPosChanging
 */
static LRESULT COMBO_WindowPosChanging(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
  WINDOWPOS *posChanging = (WINDOWPOS*)lParam;

  dprintf(("COMBO_WindowPosChanging"));

  /*
   * We need to override the WM_WINDOWPOSCHANGING method to handle all
   * the non-simple comboboxes. The problem is that those controls are
   * always the same height. We have to make sure they are not resized
   * to another value.
   */
  if ( ( CB_GETTYPE(lphc) != CBS_SIMPLE ) &&
       ((posChanging->flags & SWP_NOSIZE) == 0) )
  {
    int newComboHeight;

    newComboHeight = CBGetTextAreaHeight(hwnd,lphc) +
                      2*COMBO_YBORDERSIZE();

    /*
     * Resizing a combobox has another side effect, it resizes the dropped
     * rectangle as well. However, it does it only if the new height for the
     * combobox is different than the height it should have. In other words,
     * if the application resizing the combobox only had the intention to resize
     * the actual control, for example, to do the layout of a dialog that is
     * resized, the height of the dropdown is not changed.
     */
    if (posChanging->cy != newComboHeight)
    {
      if (posChanging->cy > newComboHeight)
         lphc->droppedRect.bottom = lphc->droppedRect.top + posChanging->cy - newComboHeight;
      posChanging->cy = newComboHeight;
    }
  }

  return 0;
}

/***********************************************************************
 *           COMBO_Create
 */
static LRESULT COMBO_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  static char clbName[] = "ComboLBox";
  static char editName[] = "Edit";
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  LPCREATESTRUCTA  lpcs = (CREATESTRUCTA*)lParam;

  if( !CB_GETTYPE(lphc) ) lphc->dwStyle |= CBS_SIMPLE;
  if( CB_GETTYPE(lphc) != CBS_DROPDOWNLIST ) lphc->wState |= CBF_EDIT;

  lphc->hwndself  = hwnd;
  lphc->owner = lpcs->hwndParent;

  /*
   * The item height and dropped width are not set when the control
   * is created.
   */
  lphc->droppedWidth = lphc->editHeight = 0;

  /*
   * The first time we go through, we want to measure the ownerdraw item
   */
  lphc->wState |= CBF_MEASUREITEM;

  /* M$ IE 3.01 actually creates (and rapidly destroys) an ownerless combobox */

  if( lphc->owner || !(lpcs->style & WS_VISIBLE) )
  {
      UINT lbeStyle   = 0;
      UINT lbeExStyle = 0;

      /*
       * Initialize the dropped rect to the size of the client area of the
       * control and then, force all the areas of the combobox to be
       * recalculated.
       */
      GetClientRect( hwnd, &lphc->droppedRect );

      CBCalcPlacement(hwnd,
                      lphc,
                      &lphc->textRect,
                      &lphc->buttonRect,
                      &lphc->droppedRect );

      /*
       * Adjust the position of the popup listbox if it's necessary
       */
      if ( CB_GETTYPE(lphc) != CBS_SIMPLE )
      {
        lphc->droppedRect.top   = lphc->textRect.bottom + COMBO_YBORDERSIZE();

        /*
         * If it's a dropdown, the listbox is offset
         */
        if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
          lphc->droppedRect.left += COMBO_EDITBUTTONSPACE();

        ClientToScreen(hwnd, (LPPOINT)&lphc->droppedRect);
        ClientToScreen(hwnd, (LPPOINT)&lphc->droppedRect.right);
      }

      /* create listbox popup */

      lbeStyle = (LBS_NOTIFY | WS_BORDER | WS_CLIPSIBLINGS | WS_CHILD) |
                 (lpcs->style & (WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE));

      if( lphc->dwStyle & CBS_SORT )
        lbeStyle |= LBS_SORT;
      if( lphc->dwStyle & CBS_HASSTRINGS )
        lbeStyle |= LBS_HASSTRINGS;
      if( lphc->dwStyle & CBS_NOINTEGRALHEIGHT )
        lbeStyle |= LBS_NOINTEGRALHEIGHT;
      if( lphc->dwStyle & CBS_DISABLENOSCROLL )
        lbeStyle |= LBS_DISABLENOSCROLL;

      if( CB_GETTYPE(lphc) == CBS_SIMPLE )      /* child listbox */
      {
        lbeStyle |= WS_VISIBLE;

        /*
         * In win 95 look n feel, the listbox in the simple combobox has
         * the WS_EXCLIENTEDGE style instead of the WS_BORDER style.
         */
        lbeStyle   &= ~WS_BORDER;
        lbeExStyle |= WS_EX_CLIENTEDGE;
      }

      lphc->hWndLBox = CreateWindowExA(lbeExStyle,
                                       clbName,
                                       NULL,
                                       lbeStyle,
                                       lphc->droppedRect.left,
                                       lphc->droppedRect.top,
                                       lphc->droppedRect.right - lphc->droppedRect.left,
                                       lphc->droppedRect.bottom - lphc->droppedRect.top,
                                       lphc->hwndself,
                                       (HMENU)ID_CB_LISTBOX,
                                       GetWindowLongA(lphc->hwndself,GWL_HINSTANCE),
                                       (LPVOID)lphc );

      if( lphc->hWndLBox )
      {
          BOOL  bEdit = TRUE;
          lbeStyle = WS_CHILD | WS_VISIBLE | ES_NOHIDESEL | ES_LEFT | ES_COMBO;

          /*
           * In Win95 look, the border fo the edit control is
           * provided by the combobox
           */
          if( lphc->wState & CBF_EDIT )
          {
              if( lphc->dwStyle & CBS_OEMCONVERT )
                  lbeStyle |= ES_OEMCONVERT;
              if( lphc->dwStyle & CBS_AUTOHSCROLL )
                  lbeStyle |= ES_AUTOHSCROLL;
              if( lphc->dwStyle & CBS_LOWERCASE )
                  lbeStyle |= ES_LOWERCASE;
              else if( lphc->dwStyle & CBS_UPPERCASE )
                  lbeStyle |= ES_UPPERCASE;

              lphc->hWndEdit = CreateWindowExA(0,
                                               editName,
                                               NULL,
                                               lbeStyle,
                                               lphc->textRect.left, lphc->textRect.top,
                                               lphc->textRect.right - lphc->textRect.left,
                                               lphc->textRect.bottom - lphc->textRect.top,
                                               lphc->hwndself,
                                               (HMENU)ID_CB_EDIT,
                                               GetWindowLongA(lphc->hwndself,GWL_HINSTANCE),
                                               NULL );

              if( !lphc->hWndEdit )
                bEdit = FALSE;
          }

          if( bEdit )
          {
	    if( CB_GETTYPE(lphc) != CBS_SIMPLE )
	    {
              /* Now do the trick with parent */
	      SetParent(lphc->hWndLBox, HWND_DESKTOP);

              /*
               * If the combo is a dropdown, we must resize the control to fit only
               * the text area and button. To do this, we send a dummy resize and the
               * WM_WINDOWPOSCHANGING message will take care of setting the height for
               * us.
               */

              CBForceDummyResize(lphc);
            }

            //TRACE("init done\n");
            return hwnd;
          }
          //ERR("edit control failure.\n");
      } //else ERR("listbox failure.\n");
  } //else ERR("no owner for visible combo.\n");

  /* CreateWindow() will send WM_NCDESTROY to cleanup */

  return -1;
}

/***********************************************************************
 *           CBPaintButton
 *
 * Paint combo button (normal, pressed, and disabled states).
 */
static void CBPaintButton(
  LPHEADCOMBO lphc,
  HDC         hdc,
  RECT        rectButton)
{
    if( lphc->wState & CBF_NOREDRAW )
      return;


    UINT buttonState = DFCS_SCROLLCOMBOBOX;

    if (lphc->wState & CBF_BUTTONDOWN)
    {
      buttonState |= DFCS_PUSHED;
    }

    if (CB_DISABLED(lphc))
    {
      buttonState |= DFCS_INACTIVE;
    }

    DrawFrameControl(hdc,&rectButton,DFC_SCROLL,buttonState);
}

/***********************************************************************
 *           CBPaintText
 *
 * Paint CBS_DROPDOWNLIST text field / update edit control contents.
 */
static void CBPaintText(
  LPHEADCOMBO lphc,
  HDC         hdc,
  RECT        rectEdit)
{
   INT  id, size = 0;
   LPSTR        pText = NULL;

   if( lphc->wState & CBF_NOREDRAW ) return;

   /* follow Windows combobox that sends a bunch of text
    * inquiries to its listbox while processing WM_PAINT. */

   if( (id = SendMessageA(lphc->hWndLBox, LB_GETCURSEL, 0, 0) ) != LB_ERR )
   {
        size = SendMessageA( lphc->hWndLBox, LB_GETTEXTLEN, id, 0);
        if( (pText = (char*)HeapAlloc( GetProcessHeap(), 0, size + 1)) != NULL )
        {
            SendMessageA( lphc->hWndLBox, LB_GETTEXT, (WPARAM)id, (LPARAM)pText );
            pText[size] = '\0'; /* just in case */
        } else return;
   }

   if( lphc->wState & CBF_EDIT )
   {
        if( CB_HASSTRINGS(lphc) ) SetWindowTextA( lphc->hWndEdit, pText ? pText : "" );
        if( lphc->wState & CBF_FOCUSED )
            SendMessageA( lphc->hWndEdit, EM_SETSEL, 0, (LPARAM)(-1));
   }
   else /* paint text field ourselves */
   {
     UINT       itemState;
     HFONT      hPrevFont = (lphc->hFont) ? SelectObject(hdc, lphc->hFont) : 0;

     /*
      * Give ourselves some space.
      */
     InflateRect( &rectEdit, -1, -1 );

     if ( (lphc->wState & CBF_FOCUSED) &&
          !(lphc->wState & CBF_DROPPED) )
     {
       /* highlight */

       FillRect( hdc, &rectEdit, GetSysColorBrush(COLOR_HIGHLIGHT) );
       SetBkColor( hdc, GetSysColor( COLOR_HIGHLIGHT ) );
       SetTextColor( hdc, GetSysColor( COLOR_HIGHLIGHTTEXT ) );
       itemState = ODS_SELECTED | ODS_FOCUS;
     }
     else
       itemState = 0;

     if( CB_OWNERDRAWN(lphc) )
     {
       DRAWITEMSTRUCT dis;
       HRGN           clipRegion;
       DWORD dwStyle = GetWindowLongA(lphc->hwndself,GWL_STYLE);

       /*
        * Save the current clip region.
        * To retrieve the clip region, we need to create one "dummy"
        * clip region.
        */
       clipRegion = CreateRectRgnIndirect(&rectEdit);

       if (GetClipRgn(hdc, clipRegion)!=1)
       {
         DeleteObject(clipRegion);
         clipRegion=(HRGN)NULL;
       }

       if ( dwStyle & WS_DISABLED )
         itemState |= ODS_DISABLED;

       dis.CtlType      = ODT_COMBOBOX;
       dis.CtlID        = GetWindowLongA(lphc->hwndself,GWL_ID);
       dis.hwndItem     = lphc->hwndself;
       dis.itemAction   = ODA_DRAWENTIRE;
       dis.itemID       = id;
       dis.itemState    = itemState | ODS_COMBOBOXEDIT;
       dis.hDC          = hdc;
       dis.rcItem       = rectEdit;
       dis.itemData     = SendMessageA( lphc->hWndLBox, LB_GETITEMDATA,
                                        (WPARAM)id, 0 );

       /*
        * Clip the DC and have the parent draw the item.
        */
       IntersectClipRect(hdc,
                         rectEdit.left,  rectEdit.top,
                         rectEdit.right, rectEdit.bottom);

       SendMessageA(lphc->owner, WM_DRAWITEM,
                    GetWindowLongA(lphc->hwndself,GWL_ID), (LPARAM)&dis );

       /*
        * Reset the clipping region.
        */
       SelectClipRgn(hdc, clipRegion);
     }
     else
     {
       ExtTextOutA( hdc,
                    rectEdit.left + 1,
                    rectEdit.top + 1,
                    ETO_OPAQUE | ETO_CLIPPED,
                    &rectEdit,
                    pText ? pText : "" , size, NULL );

       if(lphc->wState & CBF_FOCUSED && !(lphc->wState & CBF_DROPPED))
         DrawFocusRect( hdc, &rectEdit );
     }

     if( hPrevFont )
       SelectObject(hdc, hPrevFont );
   }
   if (pText)
        HeapFree( GetProcessHeap(), 0, pText );
}

/***********************************************************************
 *           CBPaintBorder
 */
static void CBPaintBorder(
  HWND        hwnd,
  LPHEADCOMBO lphc,
  HDC         hdc)
{
  RECT clientRect;

  if (CB_GETTYPE(lphc) != CBS_SIMPLE)
  {
    GetClientRect(hwnd, &clientRect);
  }
  else
  {
    CopyRect(&clientRect, &lphc->textRect);

    InflateRect(&clientRect, EDIT_CONTROL_PADDING(), EDIT_CONTROL_PADDING());
    InflateRect(&clientRect, COMBO_XBORDERSIZE(), COMBO_YBORDERSIZE());
  }

  DrawEdge(hdc, &clientRect, EDGE_SUNKEN, BF_RECT);
}

/***********************************************************************
 *           COMBO_PrepareColors
 *
 * This method will sent the appropriate WM_CTLCOLOR message to
 * prepare and setup the colors for the combo's DC.
 *
 * It also returns the brush to use for the background.
 */
static HBRUSH COMBO_PrepareColors(
  HWND        hwnd,
  LPHEADCOMBO lphc,
  HDC         hDC)
{
  HBRUSH  hBkgBrush;

  /*
   * Get the background brush for this control.
   */
  if (CB_DISABLED(lphc))
  {
    hBkgBrush = SendMessageA( lphc->owner, WM_CTLCOLORSTATIC,
                              hDC, lphc->hwndself );

    /*
     * We have to change the text color since WM_CTLCOLORSTATIC will
     * set it to the "enabled" color. This is the same behavior as the
     * edit control
     */
    SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));
  }
  else
  {
    if (lphc->wState & CBF_EDIT)
    {
      hBkgBrush = SendMessageA( lphc->owner, WM_CTLCOLOREDIT,
                                hDC, lphc->hwndself );
    }
    else
    {
      hBkgBrush = SendMessageA( lphc->owner, WM_CTLCOLORLISTBOX,
                                hDC, lphc->hwndself );
    }
  }

  /*
   * Catch errors.
   */
  if( !hBkgBrush )
    hBkgBrush = GetSysColorBrush(COLOR_WINDOW);

  return hBkgBrush;
}

/***********************************************************************
 *           COMBO_EraseBackground
 */
static LRESULT COMBO_EraseBackground(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
  HBRUSH  hBkgBrush;
  RECT    clientRect;
  HDC     hDC;

  hDC = (wParam) ? (HDC)wParam:GetDC(hwnd);

  /*
   * Calculate the area that we want to erase.
   */
  if (CB_GETTYPE(lphc) != CBS_SIMPLE)
  {
    GetClientRect(hwnd, &clientRect);
  }
  else
  {
    CopyRect(&clientRect, &lphc->textRect);

    InflateRect(&clientRect, COMBO_XBORDERSIZE(), COMBO_YBORDERSIZE());
  }

  /*
   * Retrieve the background brush
   */
  hBkgBrush = COMBO_PrepareColors(hwnd, lphc, hDC);

  FillRect(hDC, &clientRect, hBkgBrush);

  if (!wParam)
    ReleaseDC(hwnd, hDC);

  return TRUE;
}

static LRESULT COMBO_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
  LRESULT result = DLGC_WANTARROWS | DLGC_WANTCHARS;

  if (lParam && (((LPMSG)lParam)->message == WM_KEYDOWN))
  {
    int vk = (int)((LPMSG)lParam)->wParam;

    if ((vk == VK_RETURN || vk == VK_ESCAPE) && (lphc->wState & CBF_DROPPED))
      result |= DLGC_WANTMESSAGE;
  }
  return  result;
}

/***********************************************************************
 *           COMBO_Paint
 */
static LRESULT COMBO_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
  PAINTSTRUCT ps;
  HDC   hDC;

  hDC = (wParam) ? (HDC)wParam:BeginPaint(hwnd,&ps);


  if( hDC && !(lphc->wState & CBF_NOREDRAW) )
  {
      HBRUSH    hPrevBrush, hBkgBrush;

      /*
       * Retrieve the background brush and select it in the
       * DC.
       */
      hBkgBrush = COMBO_PrepareColors(hwnd,lphc,hDC);

      hPrevBrush = SelectObject( hDC, hBkgBrush );

      /*
       * In non 3.1 look, there is a sunken border on the combobox
       */
      CBPaintBorder(CB_HWND(lphc), lphc, hDC);

      if( !IsRectEmpty(&lphc->buttonRect) )
      {
        CBPaintButton(lphc, hDC, lphc->buttonRect);
      }

      if( !(lphc->wState & CBF_EDIT) )
      {
        /*
         * The text area has a border only in Win 3.1 look.
         */

        CBPaintText( lphc, hDC, lphc->textRect);
      }

      if( hPrevBrush )
        SelectObject( hDC, hPrevBrush );
  }

  if(!wParam) EndPaint(hwnd,&ps);

  return 0;
}

static LRESULT COMBO_PrintClient(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (lParam & PRF_ERASEBKGND) COMBO_EraseBackground(hwnd,wParam,lParam);

  return COMBO_Paint(hwnd,wParam,lParam);
}

/***********************************************************************
 *           CBUpdateLBox
 *
 * Select listbox entry according to the contents of the edit control.
 */
static INT CBUpdateLBox( LPHEADCOMBO lphc, BOOL bSelect )
{
   INT  length, idx;
   LPSTR        pText = NULL;

   idx = LB_ERR;
   length = CB_GETEDITTEXTLENGTH( lphc );

   if( length > 0 )
       pText = (LPSTR) HeapAlloc( GetProcessHeap(), 0, length + 1);

   //TRACE("\t edit text length %i\n", length );

   if( pText )
   {
       if( length ) GetWindowTextA( lphc->hWndEdit, pText, length + 1);
       else pText[0] = '\0';
       idx = SendMessageA( lphc->hWndLBox, LB_FINDSTRING,
                             (WPARAM)(-1), (LPARAM)pText );
       HeapFree( GetProcessHeap(), 0, pText );
   }

   SendMessageA( lphc->hWndLBox, LB_SETCURSEL, (WPARAM)(bSelect ? idx : -1), 0 );

   /* the LB_SETCARETINDEX message should scroll the list box if required
      and sending LB_SETTOPINDEX can generate annoying behaviors  */
   /* probably superfluous but Windows sends this too */
   SendMessageA( lphc->hWndLBox, LB_SETCARETINDEX, (WPARAM)(idx < 0 ? 0 : idx), 0 );
   SendMessageA( lphc->hWndLBox, LB_SETTOPINDEX, (WPARAM)(idx < 0 ? 0 : idx), 0 );

   return idx;
}

/***********************************************************************
 *           CBUpdateEdit
 *
 * Copy a listbox entry to the edit control.
 */
static void CBUpdateEdit( LPHEADCOMBO lphc , INT index )
{
   INT  length;
   LPSTR        pText = NULL;

   //TRACE("\t %i\n", index );

   if( index >= 0 ) /* got an entry */
   {
       length = SendMessageA( lphc->hWndLBox, LB_GETTEXTLEN, (WPARAM)index, 0);
       if( length )
       {
           pText = (LPSTR) HeapAlloc( GetProcessHeap(), 0, length + 1);
           if(pText)
           {
                SendMessageA( lphc->hWndLBox, LB_GETTEXT,
                                (WPARAM)index, (LPARAM)pText );
           }
       }
   }
   lphc->wState |= CBF_NOEDITNOTIFY;

   SendMessageA( lphc->hWndEdit, WM_SETTEXT, 0, pText ? (LPARAM)pText : (LPARAM)"" );

   lphc->wState &= ~CBF_NOEDITNOTIFY;

   if( lphc->wState & CBF_FOCUSED )
     SendMessageA( lphc->hWndEdit, EM_SETSEL, 0, (LPARAM)(-1) );


   if( pText )
       HeapFree( GetProcessHeap(), 0, pText );
}

/***********************************************************************
 *           CBDropDown
 *
 * Show listbox popup.
 */
static void CBDropDown( LPHEADCOMBO lphc )
{
   RECT rect;
   int nItems = 0;
   int nDroppedHeight;

   HWND hWnd = 0;
   RECT DesktopRect;

   //TRACE("[%04x]: drop down\n", CB_HWND(lphc));

   CB_NOTIFY( lphc, CBN_DROPDOWN );

   /* set selection */

   lphc->wState |= CBF_DROPPED;
   if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
   {
       lphc->droppedIndex = CBUpdateLBox( lphc,TRUE );

       if( !(lphc->wState & CBF_CAPTURE) )
         CBUpdateEdit( lphc, lphc->droppedIndex );
   }
   else
   {
       lphc->droppedIndex = SendMessageA( lphc->hWndLBox, LB_GETCURSEL, 0, 0 );

       if( lphc->droppedIndex == LB_ERR )
         lphc->droppedIndex = 0;

       SendMessageA( lphc->hWndLBox, LB_SETTOPINDEX,
                     (WPARAM)(lphc->droppedIndex == LB_ERR ? 0 : lphc->droppedIndex), 0 );
       SendMessageA( lphc->hWndLBox, LB_CARETON, 0, 0 );
   }

   /* now set popup position */
   GetWindowRect( lphc->hwndself, &rect );

   /*
    * If it's a dropdown, the listbox is offset
    */
   if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
     rect.left += COMBO_EDITBUTTONSPACE();

   /* if the dropped height is greater than the total height of the dropped
     items list, then force the drop down list height to be the total height
     of the items in the dropped list */

  /* And Remove any extra space (Best Fit) */
   nDroppedHeight = lphc->droppedRect.bottom - lphc->droppedRect.top;
   nItems = (int)SendMessageA (lphc->hWndLBox, LB_GETCOUNT, 0, 0);

   if (nItems > 0)
   {
      int nHeight;

      nHeight = (int)SendMessageA (lphc->hWndLBox, LB_GETITEMHEIGHT, 0, 0);
      nHeight *= nItems;

      if (nHeight < nDroppedHeight - COMBO_YBORDERSIZE())
         nDroppedHeight = nHeight + COMBO_YBORDERSIZE();
   }

   /*Now we should know screen size. */
   hWnd = GetDesktopWindow();

   if( hWnd )
   {
     GetClientRect( hWnd, &DesktopRect );
   }


   /*If height of dropped rectangle gets beyond a screen size it should go up, otherwise down.*/
   if((rect.bottom + nDroppedHeight) >= DesktopRect.bottom)
      rect.bottom = rect.top - nDroppedHeight;

   SetWindowPos( lphc->hWndLBox, HWND_TOP, rect.left, rect.bottom,
		 lphc->droppedRect.right - lphc->droppedRect.left,
		 nDroppedHeight,
		 SWP_NOACTIVATE | SWP_SHOWWINDOW);


   if( !(lphc->wState & CBF_NOREDRAW) )
     RedrawWindow( lphc->hwndself, NULL, 0, RDW_INVALIDATE |
			   RDW_ERASE | RDW_UPDATENOW | RDW_NOCHILDREN );

   EnableWindow( lphc->hWndLBox, TRUE );
   if (GetCapture() != lphc->hwndself)
      SetCapture(lphc->hWndLBox);
}

/***********************************************************************
 *           CBRollUp
 *
 * Hide listbox popup.
 */
static void CBRollUp( LPHEADCOMBO lphc, BOOL ok, BOOL bButton )
{
   HWND hWnd = lphc->hwndself;

   CB_NOTIFY( lphc, (ok) ? CBN_SELENDOK : CBN_SELENDCANCEL );

   if( IsWindow( hWnd ) && CB_GETTYPE(lphc) != CBS_SIMPLE )
   {

       //TRACE("[%04x]: roll up [%i]\n", CB_HWND(lphc), (INT)ok );

       if( lphc->wState & CBF_DROPPED )
       {
           RECT rect;

           lphc->wState &= ~CBF_DROPPED;
	   /* By hiding the window, the capture by lphc->hWndLBox will be released too */
	
           ShowWindow( lphc->hWndLBox, SW_HIDE );

           if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
           {
	       rect = lphc->buttonRect;
           }
           else
           {
               if( bButton )
               {
                 UnionRect( &rect,
                            &lphc->buttonRect,
                            &lphc->textRect);
               }
               else
                 rect = lphc->textRect;

               bButton = TRUE;
           }

           if( bButton && !(lphc->wState & CBF_NOREDRAW) )
               RedrawWindow( hWnd, &rect, 0, RDW_INVALIDATE |
                               RDW_ERASE | RDW_UPDATENOW | RDW_NOCHILDREN );
           CB_NOTIFY( lphc, CBN_CLOSEUP );
       }
   }
}

/***********************************************************************
 *           COMBO_FlipListbox
 *
 * Used by the ComboLBox to show/hide itself in response to VK_F4, etc...
 */
BOOL COMBO_FlipListbox( LPHEADCOMBO lphc, BOOL ok, BOOL bRedrawButton )
{
   if( lphc->wState & CBF_DROPPED )
   {
       CBRollUp( lphc, ok, bRedrawButton );
       return FALSE;
   }

   CBDropDown( lphc );
   return TRUE;
}

/***********************************************************************
 *           CBRepaintButton
 */
static void CBRepaintButton( LPHEADCOMBO lphc )
   {
  InvalidateRect(CB_HWND(lphc), &lphc->buttonRect, TRUE);
  UpdateWindow(CB_HWND(lphc));
}

static VOID COMBO_EditSetFocus(LPHEADCOMBO lphc)
{
  if(!(lphc->wState & CBF_FOCUSED))
  {
    if( CB_GETTYPE(lphc) == CBS_DROPDOWNLIST )
      SendMessageA( lphc->hWndLBox, LB_CARETON, 0, 0 );

    lphc->wState |= CBF_FOCUSED;

    if( !(lphc->wState & CBF_EDIT) )
      InvalidateRect(CB_HWND(lphc), &lphc->textRect, TRUE);

    CB_NOTIFY( lphc, CBN_SETFOCUS );
  }
}

/***********************************************************************
 *           COMBO_SetFocus
 */
static LRESULT COMBO_SetFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

//SvL: This doesn't work. Example:
//     Click on combo box in Abiword (when it doesn't have the focus)
//     COMBO_LButtonDown checks focus, not set -> SetFocus to combo box
//     Next thing it does is check if it has the focus (CBF_FOCUSED flag).
//     This check fails as SetFocus(lphc->hWndEdit) doesn't change this flag.
//     Removing this check doesn't work as the listbox is not removed after it
//     loses the focus.
//   if(lphc->wState & CBF_EDIT)
//     SetFocus(lphc->hWndEdit);
//   else
     COMBO_EditSetFocus(lphc);

   return 0;
}

static VOID COMBO_EditKillFocus(LPHEADCOMBO lphc)
{
  if( lphc->wState & CBF_FOCUSED )
  {
    CBRollUp( lphc, FALSE, TRUE );
    if( IsWindow( lphc->hwndself ) )
    {
      if( CB_GETTYPE(lphc) == CBS_DROPDOWNLIST )
        SendMessageA( lphc->hWndLBox, LB_CARETOFF, 0, 0 );

      lphc->wState &= ~CBF_FOCUSED;

      /* redraw text */
      if( !(lphc->wState & CBF_EDIT) )
        InvalidateRect(CB_HWND(lphc), &lphc->textRect, TRUE);

      CB_NOTIFY( lphc, CBN_KILLFOCUS );
    }
  }
}

/***********************************************************************
 *           COMBO_KillFocus
 */
static LRESULT COMBO_KillFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   if(!wParam || (wParam != lphc->hWndEdit && wParam != lphc->hWndLBox ))
   {
     COMBO_EditKillFocus(lphc);
   }

   return 0;
}

/***********************************************************************
 *           COMBO_Command
 */
static LRESULT COMBO_Command(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   if ( lphc->wState & CBF_EDIT && lphc->hWndEdit == lParam )
   {
       /* ">> 8" makes gcc generate jump-table instead of cmp ladder */

       switch( HIWORD(wParam) >> 8 )
       {
           case (EN_SETFOCUS >> 8):

                //TRACE("[%04x]: edit [%04x] got focus\n",
                //             CB_HWND(lphc), lphc->hWndEdit );

                COMBO_EditSetFocus(lphc);
                break;

           case (EN_KILLFOCUS >> 8):

                //TRACE("[%04x]: edit [%04x] lost focus\n",
                //             CB_HWND(lphc), lphc->hWndEdit );

                /* NOTE: it seems that Windows' edit control sends an
                 * undocumented message WM_USER + 0x1B instead of this
                 * notification (only when it happens to be a part of
                 * the combo). ?? - AK.
                 */

                COMBO_EditKillFocus(lphc);
                break;


           case (EN_CHANGE >> 8):
               /*
                * In some circumstances (when the selection of the combobox
                * is changed for example) we don't wans the EN_CHANGE notification
                * to be forwarded to the parent of the combobox. This code
                * checks a flag that is set in these occasions and ignores the
                * notification.
                */
                if (lphc->wState & CBF_NOEDITNOTIFY)
                {
                  lphc->wState &= ~CBF_NOEDITNOTIFY;
                }
                else
                {
                  CB_NOTIFY( lphc, CBN_EDITCHANGE );
                }

                if (lphc->wState & CBF_NOLBSELECT)
                {
                  lphc->wState &= ~CBF_NOLBSELECT;
                }
                else
                {
                  CBUpdateLBox( lphc, lphc->wState & CBF_DROPPED );
                }
                break;

           case (EN_UPDATE >> 8):
                //SvL: Don't send updates either. (Realplayer 7 infinite loops)
                //CB: note: EN_UPDATE changes in Corel 20000317
               /*
                * In some circumstances (when the selection of the combobox
                * is changed for example) we don't wans the EN_CHANGE notification
                * to be forwarded to the parent of the combobox. This code
                * checks a flag that is set in these occasions and ignores the
                * notification.
                */
                if (lphc->wState & CBF_NOEDITNOTIFY)
                {
                  lphc->wState &= ~CBF_NOEDITNOTIFY;
                }
                else
                {
                  CB_NOTIFY( lphc, CBN_EDITUPDATE );
                }
                break;

           case (EN_ERRSPACE >> 8):
                CB_NOTIFY( lphc, CBN_ERRSPACE );
       }
   }
   else if( lphc->hWndLBox == lParam )
   {
       switch( HIWORD(wParam) )
       {
           case LBN_ERRSPACE:
                CB_NOTIFY( lphc, CBN_ERRSPACE );
                break;

           case LBN_DBLCLK:
                CB_NOTIFY( lphc, CBN_DBLCLK );
                break;

           case LBN_SELCHANGE:
           case LBN_SELCANCEL:

                //TRACE("[%04x]: lbox selection change [%04x]\n",
                //             CB_HWND(lphc), lphc->wState );

                /* do not roll up if selection is being tracked
                 * by arrowkeys in the dropdown listbox */

                if( (CB_GETTYPE(lphc) == CBS_SIMPLE) ||
                    ((lphc->wState & CBF_DROPPED) && !(lphc->wState & CBF_NOROLLUP)) )
                {
                   CBRollUp( lphc, (HIWORD(wParam) == LBN_SELCHANGE), TRUE );
                }
                else lphc->wState &= ~CBF_NOROLLUP;

		CB_NOTIFY( lphc, CBN_SELCHANGE );

		if( HIWORD(wParam) == LBN_SELCHANGE)
		{
                  if( lphc->wState & CBF_EDIT )
                  {
                      INT index = SendMessageA(lphc->hWndLBox, LB_GETCURSEL, 0, 0);

                      lphc->wState |= CBF_NOLBSELECT;
                      CBUpdateEdit( lphc, index );
                      /* select text in edit, as Windows does */
                      SendMessageA( lphc->hWndEdit, EM_SETSEL, 0, (LPARAM)(-1) );
                  }
                  else
                      InvalidateRect(CB_HWND(lphc), &lphc->textRect, TRUE);
                }

                /* fall through */

           case LBN_SETFOCUS:
           case LBN_KILLFOCUS:
                /* nothing to do here since ComboLBox always resets the focus to its
                 * combo/edit counterpart */
                 break;
       }
   }
   return 0;
}

/***********************************************************************
 *           COMBO_HandleItem
 *
 * Fixup an ownerdrawn item operation and pass it up to the combobox owner.
 */
static LRESULT COMBO_HandleItem(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   //TRACE("[%04x]: ownerdraw op %04x\n", CB_HWND(lphc), msg );

#define lpIS    ((LPDELETEITEMSTRUCT)lParam)

   /* two first items are the same in all 4 structs */
   lpIS->CtlType = ODT_COMBOBOX;
   lpIS->CtlID   = GetWindowLongA(hwnd,GWL_ID);

   switch( msg )        /* patch window handle */
   {
        case WM_DELETEITEM:
             lpIS->hwndItem = hwnd;
#undef  lpIS
             break;
        case WM_DRAWITEM:
#define lpIS    ((LPDRAWITEMSTRUCT)lParam)
             lpIS->hwndItem = hwnd;
#undef  lpIS
             break;
        case WM_COMPAREITEM:
#define lpIS    ((LPCOMPAREITEMSTRUCT)lParam)
             lpIS->hwndItem = hwnd;
#undef  lpIS
             break;
   }
   return SendMessageA( GetParent(hwnd), msg, GetWindowLongA(hwnd,GWL_ID), lParam );
}

/***********************************************************************
 *           COMBO_GetText
 */
static LRESULT COMBO_GetText(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   if( lphc->wState & CBF_EDIT )
       return SendMessageA( lphc->hWndEdit, WM_GETTEXT,
                             wParam,lParam);

   /* get it from the listbox */

   if( lphc->hWndLBox )
   {
       INT idx = SendMessageA( lphc->hWndLBox, LB_GETCURSEL, 0, 0 );
       if( idx != LB_ERR )
       {
           LPSTR        lpBuffer;
           INT  length = SendMessageA( lphc->hWndLBox, LB_GETTEXTLEN,
                                                (WPARAM)idx, 0 );

           /* 'length' is without the terminating character */
           if( length >= (UINT)wParam )
               lpBuffer = (LPSTR) HeapAlloc( GetProcessHeap(), 0, length + 1 );
           else
               lpBuffer = (LPSTR)lParam;

           if( lpBuffer )
           {
               INT    n = SendMessageA( lphc->hWndLBox, LB_GETTEXT,
                                           (WPARAM)idx, (LPARAM)lpBuffer );

               /* truncate if buffer is too short */

               if( length >= (UINT)wParam )
               {
                   if ((UINT)wParam && lParam) {
                   if( n != LB_ERR ) memcpy( (LPSTR)lParam, lpBuffer, ((UINT)wParam>n) ? n+1 : (UINT)wParam-1 );
                   ((LPSTR)lParam)[(UINT)wParam - 1] = '\0';
                   }
                   HeapFree( GetProcessHeap(), 0, lpBuffer );
               }
               return (LRESULT)n;
           }
       }
   }
   return 0;
}

static LRESULT COMBO_GetTextLength(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if (!(lphc->wState & CBF_EDIT))
  {
    int idx = SendMessageA( lphc->hWndLBox, LB_GETCURSEL, 0, 0 );

    if (idx == -1) return 0;
    return SendMessageA( lphc->hWndLBox, LB_GETTEXTLEN, idx, 0 );
  } else if (lphc->wState & CBF_EDIT)
  {
    /* Don't bother turning on CBF_NOEDITNOTIFY. */
    return SendMessageA( lphc->hWndEdit, WM_GETTEXTLENGTH, wParam, lParam );
  } else return CB_ERR;
}

static LRESULT COMBO_HandleText(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if (lphc == NULL)
  {
    dprintf(("COMBO_HandleText Info Pointer NULL!\n"));
    return CB_ERR;
  }

  if (lphc->wState & CBF_EDIT)
  {
    LRESULT res;

    lphc->wState |= CBF_NOEDITNOTIFY;
    res = SendMessageA( lphc->hWndEdit, message, wParam, lParam );
    lphc->wState &= ~CBF_NOEDITNOTIFY;
    return res;
  } else return CB_ERR;
}

/***********************************************************************
 *           CBResetPos
 *
 * This function sets window positions according to the updated
 * component placement struct.
 */
static void CBResetPos(
  LPHEADCOMBO lphc,
  LPRECT      rectEdit,
  LPRECT      rectLB,
  BOOL        bRedraw)
{
   BOOL bDrop = (CB_GETTYPE(lphc) != CBS_SIMPLE);

   /* NOTE: logs sometimes have WM_LBUTTONUP before a cascade of
    * sizing messages */

   if( lphc->wState & CBF_EDIT )
     SetWindowPos( lphc->hWndEdit, 0,
                   rectEdit->left, rectEdit->top,
                   rectEdit->right - rectEdit->left,
                   rectEdit->bottom - rectEdit->top,
                       SWP_NOZORDER | SWP_NOACTIVATE | ((bDrop) ? SWP_NOREDRAW : 0) );

   SetWindowPos( lphc->hWndLBox, 0,
                 rectLB->left, rectLB->top,
                 rectLB->right - rectLB->left,
                 rectLB->bottom - rectLB->top,
                   SWP_NOACTIVATE | SWP_NOZORDER | ((bDrop) ? SWP_NOREDRAW : 0) );

   if( bDrop )
   {
       if( lphc->wState & CBF_DROPPED )
       {
           lphc->wState &= ~CBF_DROPPED;
           ShowWindow( lphc->hWndLBox, SW_HIDE );
       }

       if( bRedraw && !(lphc->wState & CBF_NOREDRAW) )
           RedrawWindow( lphc->hwndself, NULL, 0,
                           RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW );
   }
}


/***********************************************************************
 *           COMBO_Size
 */
static LRESULT COMBO_Size(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  dprintf(("COMBO_Size"));

  if(lphc->hWndLBox && !(lphc->wState & CBF_NORESIZE))
  {
    CBCalcPlacement(hwnd,
                    lphc,
                    &lphc->textRect,
                    &lphc->buttonRect,
                    &lphc->droppedRect);

    CBResetPos( lphc, &lphc->textRect, &lphc->droppedRect, TRUE );
  }

  return 0;
}


/***********************************************************************
 *           COMBO_Font
 */
static LRESULT COMBO_SetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  /*
   * Set the font
   */
  lphc->hFont = wParam;

  /*
   * Propagate to owned windows.
   */
  if( lphc->wState & CBF_EDIT )
      SendMessageA(lphc->hWndEdit,WM_SETFONT,wParam,lParam);
  SendMessageA(lphc->hWndLBox,WM_SETFONT,wParam,lParam);

  /*
   * Redo the layout of the control.
   */
  if ( CB_GETTYPE(lphc) == CBS_SIMPLE)
  {
    CBCalcPlacement(hwnd,
                    lphc,
                    &lphc->textRect,
                    &lphc->buttonRect,
                    &lphc->droppedRect);

    CBResetPos( lphc, &lphc->textRect, &lphc->droppedRect, TRUE );
  }
  else
  {
    CBForceDummyResize(lphc);
  }

  return 0;
}

static LRESULT COMBO_GetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return lphc->hFont;
}


/***********************************************************************
 *           COMBO_LButtonDown
 */
static LRESULT COMBO_LButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
   POINT     pt;
   BOOL      bButton;

   if(!(lphc->wState & CBF_FOCUSED)) SetFocus(hwnd);
   if(!(lphc->wState & CBF_FOCUSED)) return 0;

   pt.x = LOWORD(lParam);
   pt.y = HIWORD(lParam);
   bButton = PtInRect(&lphc->buttonRect, pt);

   if( (CB_GETTYPE(lphc) == CBS_DROPDOWNLIST) ||
       (bButton && (CB_GETTYPE(lphc) == CBS_DROPDOWN)) )
   {
       lphc->wState |= CBF_BUTTONDOWN;
       if( lphc->wState & CBF_DROPPED )
       {
           /* got a click to cancel selection */

           lphc->wState &= ~CBF_BUTTONDOWN;
           CBRollUp( lphc, TRUE, FALSE );
           if( !IsWindow( hwnd ) ) return 0;

           if( lphc->wState & CBF_CAPTURE )
           {
               lphc->wState &= ~CBF_CAPTURE;
               ReleaseCapture();
           }
       }
       else
       {
           /* drop down the listbox and start tracking */

           lphc->wState |= CBF_CAPTURE;
           SetCapture( hwnd );
           CBDropDown( lphc );
       }
       if( bButton ) CBRepaintButton( lphc );
   }

   return 0;
}

/***********************************************************************
 *           COMBO_LButtonUp
 *
 * Release capture and stop tracking if needed.
 */
static LRESULT COMBO_LButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

   if( lphc->wState & CBF_CAPTURE )
   {
       lphc->wState &= ~CBF_CAPTURE;
       if( CB_GETTYPE(lphc) == CBS_DROPDOWN )
       {
           INT index = CBUpdateLBox( lphc, TRUE );

           lphc->wState |= CBF_NOLBSELECT;
	   CBUpdateEdit( lphc, index );
           lphc->wState &= ~CBF_NOLBSELECT;
       }
       SetCapture(lphc->hWndLBox);
   }

   if( lphc->wState & CBF_BUTTONDOWN )
   {
       lphc->wState &= ~CBF_BUTTONDOWN;
       CBRepaintButton( lphc );
   }

   return 0;
}

/***********************************************************************
 *           COMBO_MouseMove
 *
 * Two things to do - track combo button and release capture when
 * pointer goes into the listbox.
 */
static LRESULT COMBO_MouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
   POINT  pt;
   RECT   lbRect;

   if(!(lphc->wState & CBF_CAPTURE)) return 0;

   pt.x = LOWORD(lParam);
   pt.y = HIWORD(lParam);

   if( lphc->wState & CBF_BUTTONDOWN )
   {
     BOOL bButton;

     bButton = PtInRect(&lphc->buttonRect, pt);

     if( !bButton )
     {
       lphc->wState &= ~CBF_BUTTONDOWN;
       CBRepaintButton( lphc );
     }
   }

   GetClientRect( lphc->hWndLBox, &lbRect );
   MapWindowPoints(hwnd, lphc->hWndLBox, &pt, 1 );
   if( PtInRect(&lbRect, pt) )
   {
       lphc->wState &= ~CBF_CAPTURE;
       ReleaseCapture();
       if( CB_GETTYPE(lphc) == CBS_DROPDOWN ) CBUpdateLBox( lphc,TRUE );

       /* hand over pointer tracking */
       SendMessageA( lphc->hWndLBox, WM_LBUTTONDOWN, wParam, lParam );
   }

   return 0;
}

static LRESULT COMBO_MouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam & (MK_SHIFT | MK_CONTROL))
    return DefWindowProcA(hwnd,WM_MOUSEWHEEL,wParam,lParam);

  if ((short) HIWORD(wParam) > 0) return SendMessageA(hwnd,WM_KEYDOWN,VK_UP,0);
  if ((short) HIWORD(wParam) < 0) return SendMessageA(hwnd,WM_KEYDOWN,VK_DOWN,0);

  return TRUE;
}

static LRESULT COMBO_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( lphc->wState & CBF_EDIT )
    EnableWindow( lphc->hWndEdit, (BOOL)wParam );
  EnableWindow( lphc->hWndLBox, (BOOL)wParam );

  /* Force the control to repaint when the enabled state changes. */
  InvalidateRect(CB_HWND(lphc), NULL, TRUE);

  return 0;
}

static LRESULT COMBO_SetRedraw(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( wParam )
    lphc->wState &= ~CBF_NOREDRAW;
  else
    lphc->wState |= CBF_NOREDRAW;

  if( lphc->wState & CBF_EDIT )
    SendMessageA( lphc->hWndEdit, WM_SETREDRAW, wParam, lParam );
  SendMessageA( lphc->hWndLBox, WM_SETREDRAW, wParam, lParam );

  return 0;
}

static LRESULT COMBO_SysKeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( KEYDATA_ALT & HIWORD(lParam) )
    if( wParam == VK_UP || wParam == VK_DOWN )
      COMBO_FlipListbox( lphc,FALSE,FALSE );

  return 0;
}

static LRESULT COMBO_HandleKey(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if (((CHAR)wParam == VK_RETURN || (CHAR)wParam == VK_ESCAPE) && (lphc->wState & CBF_DROPPED))
  {
    CBRollUp( lphc, (CHAR)wParam == VK_RETURN, FALSE );
    return TRUE;
  }

  if( lphc->wState & CBF_EDIT )
    return  SendMessageA( lphc->hWndEdit, message, wParam, lParam );
  else
    return  SendMessageA( lphc->hWndLBox, message, wParam, lParam );
}

/* combobox messages */

static LRESULT COMBO_AddString(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA(lphc->hWndLBox,LB_ADDSTRING,0,lParam);
}

static LRESULT COMBO_InsertString(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA(lphc->hWndLBox,LB_INSERTSTRING,wParam,lParam);
}

static LRESULT COMBO_DeleteString(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA(lphc->hWndLBox,LB_DELETESTRING,wParam,0);
}

/***********************************************************************
 *           COMBO_SelectString
 */
static LRESULT COMBO_SelectString(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
  INT CurSel = SendMessageA( lphc->hWndLBox, LB_GETCURSEL, 0, 0 );
  INT index = SendMessageA(lphc->hWndLBox,LB_SELECTSTRING,wParam,lParam);

  if( index >= 0 )
  {
    if( lphc->wState & CBF_EDIT )
    {
      if (CurSel != index)
        CBUpdateEdit( lphc, index );
    } else
        InvalidateRect(CB_HWND(lphc), &lphc->textRect, TRUE);
  }

  return (LRESULT)index;
}

static LRESULT COMBO_FindString(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA(lphc->hWndLBox,LB_FINDSTRING,wParam,lParam);
}

static LRESULT COMBO_FindStringExact(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA(lphc->hWndLBox,LB_FINDSTRINGEXACT,wParam,lParam);
}

/***********************************************************************
 *           COMBO_SetItemHeight
 */
static LRESULT COMBO_SetItemHeight(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
   LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);
   LRESULT      lRet = CB_ERR;

   if( wParam == -1 ) /* set text field height */
   {
       if( lParam < 32768 )
       {
           lphc->editHeight = lParam;

         /*
          * Redo the layout of the control.
          */
         if ( CB_GETTYPE(lphc) == CBS_SIMPLE)
         {
           CBCalcPlacement(hwnd,
                           lphc,
                           &lphc->textRect,
                           &lphc->buttonRect,
                           &lphc->droppedRect);

           CBResetPos( lphc, &lphc->textRect, &lphc->droppedRect, TRUE );
         }
         else
         {
           CBForceDummyResize(lphc);
         }

           lRet = lParam;
       }
   }
   else if ( CB_OWNERDRAWN(lphc) )      /* set listbox item height */
        lRet = SendMessageA( lphc->hWndLBox, LB_SETITEMHEIGHT,
                              wParam,lParam);
   return lRet;
}

static LRESULT COMBO_GetItemHeight(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( (INT)wParam >= 0 )  /* listbox item */
    return SendMessageA( lphc->hWndLBox, LB_GETITEMHEIGHT, wParam, 0);

  return CBGetTextAreaHeight(hwnd, lphc);
}

static LRESULT COMBO_ResetContent(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  SendMessageA( lphc->hWndLBox, LB_RESETCONTENT, 0, 0 );
  InvalidateRect(CB_HWND(lphc), NULL, TRUE);

  return  TRUE;
}

static LRESULT COMBO_InitStorage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_INITSTORAGE, wParam, lParam);
}

static LRESULT COMBO_GetHorizontalExtent(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETHORIZONTALEXTENT, 0, 0);
}

static LRESULT COMBO_SetHorizontalExtent(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_SETHORIZONTALEXTENT, wParam, 0);
}

static LRESULT COMBO_GetTopIndex(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETTOPINDEX, 0, 0);
}

static LRESULT COMBO_GetLocale(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETLOCALE, 0, 0);
}

static LRESULT COMBO_SetLocale(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_SETLOCALE, wParam, 0);
}

static LRESULT COMBO_GetDroppedWidth(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( lphc->droppedWidth )
    return  lphc->droppedWidth;

  return lphc->droppedRect.right - lphc->droppedRect.left;
}

static LRESULT COMBO_SetDroppedWidth(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( (CB_GETTYPE(lphc) != CBS_SIMPLE) &&
      (INT)wParam < 32768 ) lphc->droppedWidth = (INT)wParam;

  return CB_ERR;
}

static LRESULT COMBO_GetDroppedControlRect(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( lParam ) CBGetDroppedControlRect(lphc, (LPRECT)lParam );

  return CB_OKAY;
}

static LRESULT COMBO_GetDroppedState(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return (lphc->wState & CBF_DROPPED) ? TRUE : FALSE;
}

static LRESULT COMBO_Dir(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return COMBO_Directory( lphc, (UINT)wParam,
                          (LPSTR)lParam,TRUE);
}

static LRESULT COMBO_ShowDropDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( CB_GETTYPE(lphc) != CBS_SIMPLE )
  {
    if( wParam )
    {
      if( !(lphc->wState & CBF_DROPPED) )
        CBDropDown( lphc );
    }
    else
      if( lphc->wState & CBF_DROPPED )
        CBRollUp( lphc, FALSE, TRUE );
  }

  return TRUE;
}

static LRESULT COMBO_GetCount(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETCOUNT, 0, 0);
}

static LRESULT COMBO_GetCurSel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETCURSEL, 0, 0);
}

static LRESULT COMBO_SetCurSel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  lParam = SendMessageA( lphc->hWndLBox, LB_SETCURSEL, wParam, 0);
  if( lParam >= 0 )
    SendMessageA( lphc->hWndLBox, LB_SETTOPINDEX, wParam, 0);
  if( lphc->wState & CBF_SELCHANGE )
  {
    //SvL: Clear the flag here; doing it after calling CBUpdateEdit causes
    //     an infinite loop in RealPlayer 7
    lphc->wState &= ~CBF_SELCHANGE;

    /* no LBN_SELCHANGE in this case, update manually */
    if( lphc->wState & CBF_EDIT )
        CBUpdateEdit( lphc, (INT)wParam );
    else
        InvalidateRect(CB_HWND(lphc), &lphc->textRect, TRUE);
  }

  return lParam;
}

static LRESULT COMBO_GetLBText(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETTEXT, wParam, lParam);
}

static LRESULT COMBO_GetLBTextLen(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETTEXTLEN, wParam, 0);
}

static LRESULT COMBO_GetItemData(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_GETITEMDATA, wParam, 0);
}

static LRESULT COMBO_SetItemData(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return SendMessageA( lphc->hWndLBox, LB_SETITEMDATA, wParam, lParam);
}

static LRESULT COMBO_GetEditSel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( lphc->wState & CBF_EDIT )
  {
    INT a, b;

    return  SendMessageA( lphc->hWndEdit, EM_GETSEL,
                          (wParam) ? wParam : (WPARAM)&a,
                          (lParam) ? lParam : (LPARAM)&b );
  }

  return CB_ERR;
}

static LRESULT COMBO_SetEditSel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( lphc->wState & CBF_EDIT )
    return  SendMessageA( lphc->hWndEdit, EM_SETSEL,
                          (INT)(INT16)LOWORD(lParam), (INT)(INT16)HIWORD(lParam) );

  return CB_ERR;
}

static LRESULT COMBO_SetExtendedUI(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  if( CB_GETTYPE(lphc) == CBS_SIMPLE )
    return  CB_ERR;
  if( wParam )
    lphc->wState |= CBF_EUI;
  else lphc->wState &= ~CBF_EUI;
    return CB_OKAY;
}

static LRESULT COMBO_GetExtendedUI(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPHEADCOMBO lphc = (LPHEADCOMBO)GetInfoPtr(hwnd);

  return (lphc->wState & CBF_EUI) ? TRUE : FALSE;
}

/***********************************************************************
 *           ComboWndProc
 *
 * http://www.microsoft.com/msdn/sdk/platforms/doc/sdk/win32/ctrl/src/combobox_15.htm
 */
LRESULT WINAPI ComboWndProc( HWND hwnd, UINT message,
                             WPARAM wParam, LPARAM lParam )
{
//      dprintf(("ComboWndProc hwnd: %04x, msg %s, wp %08x lp %08lx\n",
//               hwnd, GetMsgText(message), wParam, lParam));

      switch(message)
      {

        /* System messages */

        case WM_NCCREATE:
          return COMBO_NCCreate(hwnd,wParam,lParam);

        case WM_NCDESTROY:
          return COMBO_NCDestroy(hwnd,wParam,lParam);

        case WM_CREATE:
          return COMBO_Create(hwnd,wParam,lParam);

        case WM_PRINTCLIENT:
          return COMBO_PrintClient(hwnd,wParam,lParam);

        case WM_PAINT:
          return COMBO_Paint(hwnd,wParam,lParam);

        case WM_ERASEBKGND:
          return COMBO_EraseBackground(hwnd,wParam,lParam);

        case WM_GETDLGCODE:
          return COMBO_GetDlgCode(hwnd,wParam,lParam);

        case WM_WINDOWPOSCHANGING:
          return COMBO_WindowPosChanging(hwnd,wParam,lParam);

        case WM_SIZE:
          return COMBO_Size(hwnd,wParam,lParam);

        case WM_SETFONT:
          return COMBO_SetFont(hwnd,wParam,lParam);

        case WM_GETFONT:
          return COMBO_GetFont(hwnd,wParam,lParam);

        case WM_SETFOCUS:
          return COMBO_SetFocus(hwnd,wParam,lParam);

        case WM_KILLFOCUS:
          return COMBO_KillFocus(hwnd,wParam,lParam);

        case WM_COMMAND:
          return COMBO_Command(hwnd,wParam,lParam);

        case WM_GETTEXT:
          return COMBO_GetText(hwnd,wParam,lParam);

        case WM_GETTEXTLENGTH:
          return COMBO_GetTextLength(hwnd,wParam,lParam);

        case WM_SETTEXT:
        case WM_CLEAR:
        case WM_CUT:
        case WM_PASTE:
        case WM_COPY:
          return COMBO_HandleText(hwnd,message,wParam,lParam);

        case WM_DRAWITEM:
        case WM_DELETEITEM:
        case WM_COMPAREITEM:
        case WM_MEASUREITEM:
          return COMBO_HandleItem(hwnd,message,wParam,lParam);

        case WM_ENABLE:
          return COMBO_Enable(hwnd,wParam,lParam);

        case WM_SETREDRAW:
          return COMBO_SetRedraw(hwnd,wParam,lParam);

        case WM_SYSKEYDOWN:
          return COMBO_SysKeyDown(hwnd,wParam,lParam);

        case WM_CHAR:
        case WM_KEYDOWN:
          return COMBO_HandleKey(hwnd,message,wParam,lParam);

        case WM_LBUTTONDOWN:
          return COMBO_LButtonDown(hwnd,wParam,lParam);

        case WM_LBUTTONUP:
          return COMBO_LButtonUp(hwnd,wParam,lParam);

        case WM_MOUSEMOVE:
          return COMBO_MouseMove(hwnd,wParam,lParam);

        case WM_MOUSEWHEEL:
          return COMBO_MouseWheel(hwnd,wParam,lParam);

        /* Combo messages */

        case CB_ADDSTRING:
          return COMBO_AddString(hwnd,wParam,lParam);

        case CB_INSERTSTRING:
          return COMBO_InsertString(hwnd,wParam,lParam);

        case CB_DELETESTRING:
          return COMBO_DeleteString(hwnd,wParam,lParam);

        case CB_SELECTSTRING:
          return COMBO_SelectString(hwnd,wParam,lParam);

        case CB_FINDSTRING:
          return COMBO_FindString(hwnd,wParam,lParam);

        case CB_FINDSTRINGEXACT:
          return COMBO_FindStringExact(hwnd,wParam,lParam);

        case CB_SETITEMHEIGHT:
          return COMBO_SetItemHeight(hwnd,wParam,lParam);

        case CB_GETITEMHEIGHT:
          return COMBO_GetItemHeight(hwnd,wParam,lParam);

        case CB_RESETCONTENT:
          return COMBO_ResetContent(hwnd,wParam,lParam);

        case CB_INITSTORAGE:
          return COMBO_InitStorage(hwnd,wParam,lParam);

        case CB_GETHORIZONTALEXTENT:
          return COMBO_GetHorizontalExtent(hwnd,wParam,lParam);

        case CB_SETHORIZONTALEXTENT:
          return COMBO_SetHorizontalExtent(hwnd,wParam,lParam);

        case CB_GETTOPINDEX:
          return COMBO_GetTopIndex(hwnd,wParam,lParam);

        case CB_GETLOCALE:
          return COMBO_GetLocale(hwnd,wParam,lParam);

        case CB_SETLOCALE:
          return COMBO_SetLocale(hwnd,wParam,lParam);

        case CB_GETDROPPEDWIDTH:
          return COMBO_GetDroppedWidth(hwnd,wParam,lParam);

        case CB_SETDROPPEDWIDTH:
          return COMBO_SetDroppedWidth(hwnd,wParam,lParam);

        case CB_GETDROPPEDCONTROLRECT:
          return COMBO_GetDroppedControlRect(hwnd,wParam,lParam);

        case CB_GETDROPPEDSTATE:
          return COMBO_GetDroppedState(hwnd,wParam,lParam);

        case CB_DIR:
          return COMBO_Dir(hwnd,wParam,lParam);

        case CB_SHOWDROPDOWN:
          return COMBO_ShowDropDown(hwnd,wParam,lParam);

        case CB_GETCOUNT:
          return COMBO_GetCount(hwnd,wParam,lParam);

        case CB_GETCURSEL:
          return COMBO_GetCurSel(hwnd,wParam,lParam);

        case CB_SETCURSEL:
          return COMBO_SetCurSel(hwnd,wParam,lParam);

        case CB_GETLBTEXT:
          return COMBO_GetLBText(hwnd,wParam,lParam);

        case CB_GETLBTEXTLEN:
          return COMBO_GetLBTextLen(hwnd,wParam,lParam);

        case CB_GETITEMDATA:
          return COMBO_GetItemData(hwnd,wParam,lParam);

        case CB_SETITEMDATA:
          return COMBO_SetItemData(hwnd,wParam,lParam);

        case CB_GETEDITSEL:
          return COMBO_GetEditSel(hwnd,wParam,lParam);

        case CB_SETEDITSEL:
          return COMBO_SetEditSel(hwnd,wParam,lParam);

        case CB_SETEXTENDEDUI:
          return COMBO_SetExtendedUI(hwnd,wParam,lParam);

        case CB_GETEXTENDEDUI:
          return COMBO_GetExtendedUI(hwnd,wParam,lParam);

        //case (WM_USER + 0x1B):
        //        WARN("[%04x]: undocumented msg!\n", hwnd );
    }
    return DefWindowProcA(hwnd, message, wParam, lParam);
}

BOOL COMBOBOX_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(COMBOBOXCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_PARENTDC;
    wndClass.lpfnWndProc   = (WNDPROC)ComboWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(VOID*);
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = COMBOBOXCLASSNAME;

    return RegisterClassA(&wndClass);
}

BOOL COMBOBOX_Unregister()
{
    if (GlobalFindAtomA(COMBOBOXCLASSNAME))
        return UnregisterClassA(COMBOBOXCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}

