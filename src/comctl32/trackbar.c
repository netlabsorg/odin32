/* $Id: trackbar.c,v 1.11 1999-07-04 21:06:00 cbratschi Exp $ */
/*
 * Trackbar control
 *
 * Copyright 1998, 1999 Eric Kohl <ekohl@abo.rhein-zeitung.de>
 * Copyright 1998,1999 Alex Priem <alexp@sci.kun.nl>
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi <cbratschi@datacomm.ch>
 *
 *
 * TODO:
 *
 *   - more notifications. (CB: should be complete)
 *   - TRACKBAR_UpdateThumb, TRACKBAR_UpdateThumbPosition:
 *     use a memory dc to avoid flickering by short movements
 */

#include "winbase.h"
#include "commctrl.h"
#include "trackbar.h"
#include <stdio.h>


#define TRACKBAR_GetInfoPtr(wndPtr) ((TRACKBAR_INFO *)GetWindowLongA (hwnd,0))


/* Used by TRACKBAR_Draw to find out which parts of the control
   need to be recalculated */

#define TB_THUMBPOSCHANGED      1
#define TB_THUMBSIZECHANGED     2
#define TB_THUMBCHANGED         (TB_THUMBPOSCHANGED | TB_THUMBPOSCHANGED)
#define TB_SELECTIONCHANGED     4
#define TB_DRAG_MODE            16     /* we're dragging the slider */
#define TB_DRAGPOSVALID         32     /* current Position is in dragPos */
#define TB_SCROLL_MODE          64     /* WM_TIMER scroll mode */
#define TB_SHOW_TOOLTIP         128    /* tooltip-style enabled and tooltip on */

/* helper defines for TRACKBAR_DrawTic */
#define TIC_LEFTEDGE            0x20
#define TIC_RIGHTEDGE           0x40
#define TIC_EDGE                (TIC_LEFTEDGE | TIC_RIGHTEDGE)
#define TIC_SELECTIONMARKMAX    0x80
#define TIC_SELECTIONMARKMIN    0x100
#define TIC_SELECTIONMARK       (TIC_SELECTIONMARKMAX | TIC_SELECTIONMARKMIN)

/* size calculation */

#define BORDER_SIZE 2

#define SCALE_SIZE  4
#define SCALE_SPACE 1

#define THUMB_LEN    23
#define THUMB_MINLEN 4

#define CHANNEL_NOSEL_HEIGHT 4 //min no sel height
#define CHANNEL_MIN_HEIGHT   6 //min sel height
#define CHANNEL_THUMB_DIFF   8 //sel thumb, channel difference
#define CHANNEL_SPACE        8
#define CHANNEL_SCALE_SPACE  SCALE_SIZE+SCALE_SPACE+BORDER_SIZE
#define CHANNEL_THUMB_SPACE  BORDER_SIZE

/* scroll mode */

#define SCROLL_TIME     500 //ms
#define SCROLL_TIMER_ID 1

/* Tooltips */

#define TOOLTIP_XSPACE 5
#define TOOLTIP_YSPACE 5

static BOOL TRACKBAR_SendNotify (HWND hwnd, UINT code);

void TRACKBAR_RecalculateTics (HWND hwnd,TRACKBAR_INFO *infoPtr,BOOL restoreOld)
{
    INT i,tic,nrTics;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    if (dwStyle & TBS_NOTICKS) //no ticks
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = NULL;

      infoPtr->uNumTics = 0;
      return;
    }

    if (restoreOld && !(dwStyle & TBS_AUTOTICKS) && infoPtr->tics != NULL)
    { //check old ticks
      LPLONG oldTics = COMCTL32_Alloc(infoPtr->uNumTics*sizeof(DWORD));
      INT count = 0;

      for (i = 0;i < infoPtr->uNumTics;i++)
      {
        if (infoPtr->tics[i] >= infoPtr->nRangeMin && infoPtr->tics[i] <= infoPtr->nRangeMax)
        {
          oldTics[count] = infoPtr->tics[i];
          count++;
        }
      }

      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = COMCTL32_ReAlloc(oldTics,count*sizeof(DWORD));
      infoPtr->uNumTics = count;

      return;
    }

    if (infoPtr->uTicFreq && infoPtr->nRangeMax > infoPtr->nRangeMin && (dwStyle & TBS_AUTOTICKS))
    {
      //Tics without start and end tic
      nrTics = (infoPtr->nRangeMax-infoPtr->nRangeMin)/infoPtr->uTicFreq-1;
      if (nrTics <= 0)
      {
        COMCTL32_Free(infoPtr->tics);
        infoPtr->tics = NULL;
        infoPtr->uNumTics = 0;
        return;
      }
    } else
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = NULL;
      infoPtr->uNumTics = 0;
      return;
    }

    if (nrTics != infoPtr->uNumTics)
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = COMCTL32_Alloc(nrTics*sizeof(DWORD));
      infoPtr->uNumTics = nrTics;
    }

    tic = infoPtr->nRangeMin+infoPtr->uTicFreq; //start not included
    for (i = 0;i < nrTics;i++)
    {
      infoPtr->tics[i] = tic;
      tic += infoPtr->uTicFreq;
    }
}


/* converts from physical (mouse) position to logical position
   (in range of trackbar) */

static DOUBLE
TRACKBAR_ConvertPlaceToPosition(TRACKBAR_INFO *infoPtr,int place,int vertical)
{
    double range,width,pos;

    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    if (vertical)
    {
      width = infoPtr->rcChannel.bottom-infoPtr->rcChannel.top;
      pos = infoPtr->nRangeMin+(range*(place-infoPtr->rcChannel.top))/width;
    } else
    {
      width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
      pos = infoPtr->nRangeMin+(range*(place-infoPtr->rcChannel.left))/width;
    }

    if (pos > infoPtr->nRangeMax) pos = infoPtr->nRangeMax;
    else if (pos < infoPtr->nRangeMin) pos = infoPtr->nRangeMin;

//    TRACE (trackbar,"%.2f\n",pos);
    return pos;
}


static VOID
TRACKBAR_CalcChannel (HWND hwnd,TRACKBAR_INFO *infoPtr)
{
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    INT channelSize;
    RECT lpRect,*channel = &infoPtr->rcChannel;
    INT thumbDiff;

    GetClientRect(hwnd,&lpRect);

    if (dwStyle & TBS_ENABLESELRANGE) channelSize = MAX(infoPtr->uThumbLen-CHANNEL_THUMB_DIFF,CHANNEL_MIN_HEIGHT);
    else channelSize = CHANNEL_NOSEL_HEIGHT;

    thumbDiff = (infoPtr->uThumbLen-channelSize)/2;

    if (dwStyle & TBS_VERT)
    {
      channel->top    = lpRect.top+CHANNEL_SPACE;
      channel->bottom = lpRect.bottom-CHANNEL_SPACE;

      if (dwStyle & TBS_BOTH || dwStyle & TBS_NOTICKS)
      { //center
        channel->left  = (lpRect.right-channelSize)/2;
        channel->right = (lpRect.right+channelSize)/2;
      } else if (dwStyle & TBS_LEFT)
      {
        channel->left  = lpRect.left+thumbDiff+CHANNEL_SCALE_SPACE;
        channel->right = channel->left+channelSize;
      } else
      { //Right, align left
        channel->left = lpRect.left+thumbDiff+CHANNEL_THUMB_SPACE;
        channel->right = channel->left+channelSize;
      }
    } else
    {
      channel->left = lpRect.left+CHANNEL_SPACE;
      channel->right = lpRect.right-CHANNEL_SPACE;
      if (dwStyle & TBS_BOTH || dwStyle & TBS_NOTICKS)
      { //center
        channel->top    = (lpRect.bottom-channelSize)/2;
        channel->bottom = (lpRect.bottom+channelSize)/2;
      } else if (dwStyle & TBS_TOP)
      {
        channel->top    = lpRect.top+thumbDiff+CHANNEL_SCALE_SPACE;
        channel->bottom = channel->top+channelSize;
      } else
      { //Bottom, align top
        channel->top    = lpRect.top+thumbDiff+CHANNEL_THUMB_SPACE;
        channel->bottom = channel->top+channelSize;
      }
    }
}

//Calculate thumb size

static VOID
TRACKBAR_CalcThumb(HWND hwnd,TRACKBAR_INFO *infoPtr)
{
    RECT *thumb;
    RECT *fullThumb;
    int range, width;
    int x,y;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    int thumbFactor = 2;

    thumb = &infoPtr->rcThumb;
    fullThumb = &infoPtr->rcFullThumb;
    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    if (dwStyle & TBS_VERT)
    {
      width = infoPtr->rcChannel.bottom-infoPtr->rcChannel.top;
      y = infoPtr->uThumbLen/thumbFactor; //thumb height
      if (y%2 == 1) y++; //for real arrow
      thumb->top    = infoPtr->rcChannel.top+(width*(infoPtr->nPos-infoPtr->nRangeMin))/range-y/2; //centered
      thumb->bottom = thumb->top+y;
      //centered, no arrows
      thumb->left  = infoPtr->rcChannel.left-(infoPtr->uThumbLen-(infoPtr->rcChannel.right-infoPtr->rcChannel.left))/2;
      thumb->right = thumb->left+infoPtr->uThumbLen;
      CopyRect(fullThumb,thumb);
      if (dwStyle & TBS_BOTH) return;
      x = y/2; //arrow width
      if (dwStyle & TBS_LEFT) thumb->left += x; else thumb->right -= x;
    } else
    {
      width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
      x = infoPtr->uThumbLen/thumbFactor; //thumb width
      if (x%2 == 1) x++; //for real arrow
      thumb->left   = infoPtr->rcChannel.left+(width*(infoPtr->nPos-infoPtr->nRangeMin))/range-x/2; //centered
      thumb->right  = thumb->left+x;
      //centered, no arrows
      thumb->top  = infoPtr->rcChannel.top-(infoPtr->uThumbLen-(infoPtr->rcChannel.bottom-infoPtr->rcChannel.top))/2;
      thumb->bottom = thumb->top+infoPtr->uThumbLen;
      CopyRect(fullThumb,thumb);
      if (dwStyle & TBS_BOTH) return;
      y = x/2; //arrow height
      if (dwStyle & TBS_TOP) thumb->top += y; else thumb->bottom -= y;
    }
}

static VOID
TRACKBAR_CalcSelection (HWND hwnd, TRACKBAR_INFO *infoPtr)
{
    RECT *selection;
    int range,width,height;
    int selMin = infoPtr->nSelMin-infoPtr->nRangeMin; //relative to nRangeMin
    int selMax = infoPtr->nSelMax-infoPtr->nRangeMin; //dito

    selection = &infoPtr->rcSelection;
    range = infoPtr->nRangeMax-infoPtr->nRangeMin;

    if (range <= 0 || selMin == selMax) SetRectEmpty(selection);
    else
        if (!(GetWindowLongA(hwnd, GWL_STYLE) & TBS_VERT))
        {   //Horizontal
            width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
            selection->left = infoPtr->rcChannel.left+(width*selMin)/range;
            selection->right  = infoPtr->rcChannel.left+(width*selMax)/range;
            selection->top    = infoPtr->rcChannel.top+2;
            selection->bottom = infoPtr->rcChannel.bottom-2;
        } else
        {   //Vertical
            height = infoPtr->rcChannel.bottom-infoPtr->rcChannel.top;
            selection->top    = infoPtr->rcChannel.top+(height*selMin)/range;
            selection->bottom = infoPtr->rcChannel.top+(height*selMax)/range;
            selection->left   = infoPtr->rcChannel.left+2;
            selection->right  = infoPtr->rcChannel.right-2;
        }
}

/* Trackbar drawing code */

/* ticPos is in tic-units, not in pixels */

static VOID
TRACKBAR_DrawHorizTic (TRACKBAR_INFO *infoPtr, HDC hdc, LONG ticPos,
                       int flags, COLORREF clrTic)
{
    RECT rcChannel = infoPtr->rcChannel;
    int x,y,width,range,side;

    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    width = rcChannel.right-rcChannel.left;

    if (flags & TBS_TOP)
    {
      y = infoPtr->rcFullThumb.top-SCALE_SPACE-1;
      side = -1;
    } else
    {
      y = infoPtr->rcFullThumb.bottom+SCALE_SPACE+1;
      side = 1;
    }
    if (flags & TIC_SELECTIONMARK)
    {
      ticPos -= infoPtr->nRangeMin; //relative to nRangeMin
      if (flags & TIC_SELECTIONMARKMIN) x = rcChannel.left+(width*ticPos)/range - 1;
      else x = rcChannel.left+(width*ticPos)/range+1;

      //first line
      SetPixel(hdc,x,y+1*side,clrTic);
      SetPixel(hdc,x,y+2*side,clrTic);
      //point
      if (flags & TIC_SELECTIONMARKMIN) x--; else x++;
      SetPixel(hdc,x,y+2*side,clrTic);

      return;
    }

    if ((ticPos > infoPtr->nRangeMin) && (ticPos < infoPtr->nRangeMax))
    {
      ticPos -= infoPtr->nRangeMin; //relative to nRangeMin
      x = rcChannel.left+(width*ticPos)/range;
      SetPixel(hdc,x,y,clrTic); //base
      SetPixel(hdc,x,y+1*side,clrTic);
      SetPixel(hdc,x,y+2*side,clrTic);
    }

    if (flags & TIC_EDGE)
    {
      if (flags & TIC_LEFTEDGE) x = rcChannel.left;
      else x = rcChannel.right;

      SetPixel(hdc,x,y,clrTic); //base
      SetPixel(hdc,x,y+1*side,clrTic);
      SetPixel(hdc,x,y+2*side,clrTic);
      SetPixel(hdc,x,y+3*side,clrTic);
    }
}

static VOID
TRACKBAR_DrawVertTic (TRACKBAR_INFO *infoPtr, HDC hdc, LONG ticPos,
                      int flags, COLORREF clrTic)
{
    RECT rcChannel = infoPtr->rcChannel;
    int x,y,width,range,side;

    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    width = rcChannel.bottom-rcChannel.top;

    if (flags & TBS_LEFT)
    {
      x = infoPtr->rcFullThumb.left-SCALE_SPACE-1;
      side = -1;
    } else
    {
      x = infoPtr->rcFullThumb.right+SCALE_SPACE+1;
      side = 1;
    }


    if (flags & TIC_SELECTIONMARK)
    {
      ticPos -= infoPtr->nRangeMin;
      if (flags & TIC_SELECTIONMARKMIN) y = rcChannel.top+(width*ticPos)/range-1;
      else y = rcChannel.top+(width*ticPos)/range+1;

      //first line
      SetPixel(hdc,x+1*side,y,clrTic);
      SetPixel(hdc,x+2*side,y,clrTic);
      //point
      if (flags & TIC_SELECTIONMARKMIN) y--; else y++;
      SetPixel(hdc,x+2*side,y,clrTic);

      return;
    }

    if ((ticPos>infoPtr->nRangeMin) && (ticPos<infoPtr->nRangeMax))
    {
      ticPos -= infoPtr->nRangeMin;
      y = rcChannel.top+(width*ticPos)/range;
      SetPixel (hdc,x, y,clrTic); //base
      SetPixel (hdc,x+1*side,y,clrTic);
      SetPixel (hdc,x+2*side,y,clrTic);
    }

    if (flags & TIC_EDGE)
    {
      if (flags & TIC_LEFTEDGE) y = rcChannel.top;
      else y = rcChannel.bottom;

      SetPixel (hdc,x,y,clrTic); //base
      SetPixel (hdc,x+1*side,y,clrTic);
      SetPixel (hdc,x+2*side,y,clrTic);
      SetPixel (hdc,x+3*side,y,clrTic);
    }
}


static VOID
TRACKBAR_DrawTics (TRACKBAR_INFO *infoPtr, HDC hdc, LONG ticPos,
                   int flags, COLORREF clrTic)
{
    if (flags & TBS_VERT)
    {
        if ((flags & TBS_LEFT) || (flags & TBS_BOTH))
            TRACKBAR_DrawVertTic (infoPtr,hdc,ticPos,flags | TBS_LEFT ,clrTic);
        //TBS_RIGHT == default
        if (!(flags & TBS_LEFT) || (flags & TBS_BOTH))
            TRACKBAR_DrawVertTic(infoPtr,hdc,ticPos,flags & ~TBS_LEFT,clrTic);
    } else
    {
      if ((flags & TBS_TOP) || (flags & TBS_BOTH))
        TRACKBAR_DrawHorizTic(infoPtr,hdc,ticPos,flags | TBS_TOP,clrTic);
      //TBS_BOTTOM == default
      if (!(flags & TBS_TOP) || (flags & TBS_BOTH))
        TRACKBAR_DrawHorizTic(infoPtr,hdc,ticPos,flags & ~TBS_TOP,clrTic);
    }
}

//draw thumb, call only from draw!

static VOID TRACKBAR_DrawThumb(TRACKBAR_INFO *infoPtr,HDC hdc,DWORD dwStyle)
{
    if (!(dwStyle & TBS_NOTHUMB))
    {

      HBRUSH hbr,hbrOld;
      RECT thumb = infoPtr->rcThumb;

      if (infoPtr->flags & TB_DRAG_MODE) hbr = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
      else hbr = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
      hbrOld = SelectObject(hdc,hbr);

      if (dwStyle & TBS_BOTH)
      {
        DrawEdge(hdc,&thumb,EDGE_RAISED,BF_RECT | BF_ADJUST);
        FillRect(hdc,&thumb,hbr);
      } else
      {

        POINT points[6];
        RECT triangle;  /* for correct shadows of thumb */

        if (dwStyle & TBS_VERT)
        { //Vertical

          if (dwStyle & TBS_LEFT)
          {
            HPEN oldPen,pen;

            //Outline

            SetPolyFillMode(hdc,WINDING);
            points[0].x = thumb.left;
            points[0].y = thumb.top;
            points[1].x = thumb.left-(thumb.bottom-thumb.top)/2;
            points[1].y = thumb.top+(thumb.bottom-thumb.top)/2;
            points[2].x = thumb.left;
            points[2].y = thumb.bottom;
            points[3].x = thumb.right;
            points[3].y = thumb.bottom;
            points[4].x = thumb.right;
            points[4].y = thumb.top;
            points[5].x = points[0].x;
            points[5].y = points[0].y;
            Polygon(hdc,points,6);

            //Edge

            thumb.bottom++;
            thumb.right++;
            DrawEdge(hdc,&thumb,EDGE_RAISED,BF_BOTTOM | BF_TOP | BF_RIGHT);

            //Draw notch

            triangle.right = points[0].x;
            triangle.top = points[0].y;
            triangle.left = points[1].x;
            triangle.bottom = points[1].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDTOPRIGHT);

            //draw this line direct, DrawEdge not useful
            pen = GetSysColorPen(COLOR_3DDKSHADOW);
            oldPen = SelectObject(hdc,pen);
            MoveToEx(hdc,points[1].x,points[1].y,NULL);
            LineTo(hdc,points[2].x-1,points[2].y-1);
            pen = GetSysColorPen(COLOR_BTNSHADOW);
            SelectObject(hdc,pen);
            MoveToEx(hdc,points[1].x+1,points[1].y,NULL);
            LineTo(hdc,points[2].x,points[2].y-1);
            SelectObject(hdc,oldPen);

          } else //Right
          {
            HPEN oldPen,pen;

            //Outline

            SetPolyFillMode(hdc,WINDING);
            points[0].x = thumb.left;
            points[0].y = thumb.top;
            points[1].x = thumb.left;
            points[1].y = thumb.bottom;
            points[2].x = thumb.right;
            points[2].y = thumb.bottom;
            points[3].x = thumb.right+(thumb.bottom-thumb.top)/2;
            points[3].y = thumb.bottom-(thumb.bottom-thumb.top)/2;
            points[4].x = thumb.right;
            points[4].y = thumb.top;
            points[5].x = points[0].x;
            points[5].y = points[0].y;
            Polygon(hdc,points,6);

            //Edge

            thumb.bottom++;
            DrawEdge(hdc,&thumb,EDGE_RAISED,BF_BOTTOM | BF_TOP | BF_LEFT);

            //Draw notch

            //draw this line direct, DrawEdge not useful
            pen = GetSysColorPen(COLOR_3DLIGHT);
            oldPen = SelectObject(hdc,pen);
            MoveToEx(hdc,points[4].x,points[4].y,NULL);
            LineTo(hdc,points[3].x-1,points[3].y-1);
            pen = GetSysColorPen(COLOR_BTNHIGHLIGHT);
            SelectObject(hdc,pen);
            MoveToEx(hdc,points[4].x,points[4].y+1,NULL);
            LineTo(hdc,points[3].x-2,points[3].y-1);
            SelectObject(hdc,oldPen);

            triangle.right = points[3].x;
            triangle.top = points[3].y;
            triangle.left = points[2].x;
            triangle.bottom = points[2].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDBOTTOMLEFT);
          }
        } else
        { //Horizontal

          if (dwStyle & TBS_TOP)
          {
            //Outline

            SetPolyFillMode(hdc,WINDING);
            points[0].x = thumb.left;
            points[0].y = thumb.top;
            points[1].x = thumb.left+(thumb.right-thumb.left)/2;
            points[1].y = thumb.top-(thumb.right-thumb.left)/2;
            points[2].x = thumb.right;
            points[2].y = thumb.top;
            points[3].x = thumb.right;
            points[3].y = thumb.bottom;
            points[4].x = thumb.left;
            points[4].y = thumb.bottom;
            points[5].x = points[0].x;
            points[5].y = points[0].y;
            Polygon(hdc,points,6);

            //Edge

            thumb.right++;
            thumb.bottom++;
            DrawEdge(hdc,&thumb,EDGE_RAISED,BF_BOTTOM | BF_LEFT | BF_RIGHT);

            //Draw notch

            triangle.left = points[0].x;
            triangle.bottom = points[0].y;
            triangle.right = points[1].x;
            triangle.top = points[1].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDTOPRIGHT);


            triangle.left = points[1].x;
            triangle.top = points[1].y;
            triangle.right = points[2].x;
            triangle.bottom = points[2].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDBOTTOMRIGHT);

          } else //Bottom
          {

            //Outline

            SetPolyFillMode(hdc,WINDING);
            points[0].x = thumb.left;
            points[0].y = thumb.top;
            points[1].x = thumb.right;
            points[1].y = thumb.top;
            points[2].x = thumb.right;
            points[2].y = thumb.bottom;
            points[3].x = thumb.right-(thumb.right-thumb.left)/2;
            points[3].y = thumb.bottom+(thumb.right-thumb.left)/2;
            points[4].x = thumb.left;
            points[4].y = thumb.bottom;
            points[5].x = points[0].x;
            points[5].y = points[0].y;
            Polygon(hdc,points,6);

            //Edge

            thumb.right++;
            thumb.bottom++;
            DrawEdge(hdc,&thumb,EDGE_RAISED,BF_TOP | BF_LEFT | BF_RIGHT);

            //Draw notch

            triangle.right = points[2].x;
            triangle.top = points[2].y;
            triangle.left = points[3].x;
            triangle.bottom = points[3].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDBOTTOMLEFT);
            triangle.right = points[3].x;
            triangle.bottom = points[3].y;
            triangle.left = points[4].x;
            triangle.top = points[4].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDTOPLEFT);

          }
        }
      }
      SelectObject(hdc,hbrOld);
      DeleteObject(hbr);
    }
}

//draw the trackbar

static VOID TRACKBAR_Draw(HWND hwnd,HDC hdc)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
    RECT rcClient,rcChannel,rcSelection;
    HBRUSH hBrush = CreateSolidBrush(infoPtr->clrBk);
    int i;

    GetClientRect(hwnd,&rcClient);

    //Background
    hBrush = CreateSolidBrush(infoPtr->clrBk);
    FillRect(hdc,&rcClient,hBrush);
    DeleteObject(hBrush);

    if (infoPtr->flags & TB_DRAGPOSVALID)
    {
      infoPtr->nPos = infoPtr->dragPos;
      infoPtr->flags |= TB_THUMBPOSCHANGED;
    }

    if (infoPtr->flags & TB_THUMBCHANGED)
    {
      TRACKBAR_CalcThumb(hwnd,infoPtr);
      if (infoPtr->flags & TB_THUMBSIZECHANGED) TRACKBAR_CalcChannel(hwnd,infoPtr);
    }
    if (infoPtr->flags & TB_SELECTIONCHANGED) TRACKBAR_CalcSelection(hwnd,infoPtr);
    infoPtr->flags &= ~ (TB_THUMBCHANGED | TB_SELECTIONCHANGED | TB_DRAGPOSVALID);

    /* draw channel */

    rcChannel = infoPtr->rcChannel;
    rcSelection = infoPtr->rcSelection;
    DrawEdge(hdc,&rcChannel,EDGE_SUNKEN,BF_RECT | BF_ADJUST);

    if (dwStyle & TBS_ENABLESELRANGE)           /* fill the channel */
    {
      HBRUSH hbr = CreateSolidBrush(RGB(255,255,255));
      FillRect(hdc,&rcChannel,hbr);
      DeleteObject(hbr);
      if (((dwStyle & TBS_VERT) && (rcSelection.top != rcSelection.bottom)) ||
         ((!(dwStyle & TBS_VERT)) && (rcSelection.left != rcSelection.right)))
         {
           hbr = CreateSolidBrush (COLOR_HIGHLIGHT);
           FillRect (hdc,&rcSelection,hbr);
           DeleteObject(hbr);
         }
    }

    /* draw tics */

    if (!(dwStyle & TBS_NOTICKS))
    {
      int ticFlags = dwStyle & 0x0f;
      COLORREF clrTic = GetSysColor(COLOR_3DDKSHADOW);

      for (i = 0;i < infoPtr->uNumTics;i++)
          TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->tics[i],ticFlags,clrTic);

      TRACKBAR_DrawTics(infoPtr,hdc,0,ticFlags | TIC_LEFTEDGE,clrTic);
      TRACKBAR_DrawTics(infoPtr,hdc,0,ticFlags | TIC_RIGHTEDGE,clrTic);

      if ((dwStyle & TBS_ENABLESELRANGE) &&
            ((dwStyle & TBS_VERT && rcSelection.bottom != rcSelection.top) ||
            (!(dwStyle & TBS_VERT) && rcSelection.left != rcSelection.right)))
            {
              TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->nSelMin,ticFlags | TIC_SELECTIONMARKMIN,clrTic);
              TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->nSelMax,ticFlags | TIC_SELECTIONMARKMAX,clrTic);
            }
    }

    /* draw thumb */

    TRACKBAR_DrawThumb(infoPtr,hdc,dwStyle);

    if (infoPtr->bFocus) DrawFocusRect(hdc,&rcClient);
}

//update thumb position

static VOID TRACKBAR_UpdateThumbPosition(HWND hwnd,INT lastPos)
{
   HDC hdc;
   TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr(hwnd);
   DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
   RECT lastRect,newRect;
   HRGN hrgn,hrgnLast,hrgnNew;

   //last
   lastRect = infoPtr->rcFullThumb;

   //new
   if (infoPtr->flags & TB_DRAGPOSVALID)
   {
     infoPtr->nPos = infoPtr->dragPos;
     infoPtr->flags &= ~TB_DRAGPOSVALID;
   }
   if (infoPtr->nPos == lastPos) return;

   if (dwStyle & TBS_NOTHUMB) return;

   TRACKBAR_CalcThumb(hwnd,infoPtr);
   infoPtr->flags &= ~TB_THUMBCHANGED;
   newRect = infoPtr->rcFullThumb;


   //3D frame adjustation
   lastRect.right++;
   lastRect.bottom++;
   newRect.right++;
   newRect.bottom++;

   hdc = GetDC(hwnd);
   hrgnLast = CreateRectRgnIndirect(&lastRect);
   hrgnNew = CreateRectRgnIndirect(&newRect);
   hrgn = CreateRectRgn(0,0,0,0);
   CombineRgn(hrgn,hrgnLast,hrgnNew,RGN_OR);
   SelectClipRgn(hdc,hrgn);
   TRACKBAR_Draw(hwnd,hdc);
   SelectClipRgn(hdc,0);
   DeleteObject(hrgnLast);
   DeleteObject(hrgnNew);
   DeleteObject(hrgn);
   ReleaseDC(hwnd,hdc);
}

//redraw thumb at same position

static VOID TRACKBAR_UpdateThumb(HWND hwnd)
{
   HDC hdc;
   TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
   DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
   HRGN hrgn;

   if (dwStyle & TBS_NOTHUMB) return;

   hdc = GetDC(hwnd);
   hrgn = CreateRectRgnIndirect(&infoPtr->rcFullThumb);
   SelectClipRgn(hdc,hrgn);
   TRACKBAR_Draw(hwnd,hdc);
   SelectClipRgn(hdc,0);
   DeleteObject(hrgn);
   ReleaseDC(hwnd,hdc);
}

//redraw everything

static VOID TRACKBAR_Refresh (HWND hwnd)
{
   HDC hdc;

   hdc = GetDC (hwnd);
   TRACKBAR_Draw(hwnd,hdc);
   ReleaseDC(hwnd,hdc);
}

static VOID
TRACKBAR_AlignBuddies (HWND hwnd, TRACKBAR_INFO *infoPtr)
{
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    HWND hwndParent = GetParent (hwnd);
    RECT rcSelf, rcBuddy;
    INT x, y;

    GetWindowRect (hwnd, &rcSelf);
    MapWindowPoints (HWND_DESKTOP, hwndParent, (LPPOINT)&rcSelf, 2);

    /* align buddy left or above */
    if (infoPtr->hwndBuddyLA) {
        GetWindowRect (infoPtr->hwndBuddyLA, &rcBuddy);
        MapWindowPoints (HWND_DESKTOP, hwndParent, (LPPOINT)&rcBuddy, 2);

        if (dwStyle & TBS_VERT) {
            x = (infoPtr->rcChannel.right + infoPtr->rcChannel.left) / 2 -
                (rcBuddy.right - rcBuddy.left) / 2 + rcSelf.left;
            y = rcSelf.top - (rcBuddy.bottom - rcBuddy.top);
        }
        else {
            x = rcSelf.left - (rcBuddy.right - rcBuddy.left);
            y = (infoPtr->rcChannel.bottom + infoPtr->rcChannel.top) / 2 -
                (rcBuddy.bottom - rcBuddy.top) / 2 + rcSelf.top;
        }

        SetWindowPos (infoPtr->hwndBuddyLA, 0, x, y, 0, 0,
                      SWP_NOZORDER | SWP_NOSIZE);
    }


    /* align buddy right or below */
    if (infoPtr->hwndBuddyRB) {
        GetWindowRect (infoPtr->hwndBuddyRB, &rcBuddy);
        MapWindowPoints (HWND_DESKTOP, hwndParent, (LPPOINT)&rcBuddy, 2);

        if (dwStyle & TBS_VERT) {
            x = (infoPtr->rcChannel.right + infoPtr->rcChannel.left) / 2 -
                (rcBuddy.right - rcBuddy.left) / 2 + rcSelf.left;
            y = rcSelf.bottom;
        }
        else {
            x = rcSelf.right;
            y = (infoPtr->rcChannel.bottom + infoPtr->rcChannel.top) / 2 -
                (rcBuddy.bottom - rcBuddy.top) / 2 + rcSelf.top;
        }
        SetWindowPos (infoPtr->hwndBuddyRB, 0, x, y, 0, 0,
                      SWP_NOZORDER | SWP_NOSIZE);
    }
}


static LRESULT
TRACKBAR_ClearSel (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (infoPtr->nSelMin != infoPtr->nSelMax)
    {
      infoPtr->nSelMin = 0;
      infoPtr->nSelMax = 0;
      infoPtr->flags |= TB_SELECTIONCHANGED;

      if ((BOOL)wParam) TRACKBAR_Refresh(hwnd);
    }

    return 0;
}


static LRESULT
TRACKBAR_ClearTics (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (!(GetWindowLongA(hwnd, GWL_STYLE) & (TBS_AUTOTICKS | TBS_NOTICKS))) return 0;

    if (infoPtr->tics)
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = NULL;
      infoPtr->uNumTics = 0;

      if (wParam) TRACKBAR_Refresh(hwnd);
    }

    return 0;
}


static LRESULT
TRACKBAR_GetBuddy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (wParam)  /* buddy is left or above */
        return (LRESULT)infoPtr->hwndBuddyLA;

    /* buddy is right or below */
    return (LRESULT) infoPtr->hwndBuddyRB;
}


static LRESULT
TRACKBAR_GetChannelRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    LPRECT lprc = (LPRECT)lParam;

    if (lprc == NULL) return 0;

    CopyRect(lprc,&infoPtr->rcChannel);

    return 0;
}


static LRESULT
TRACKBAR_GetLineSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nLineSize;
}


static LRESULT
TRACKBAR_GetNumTics (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (GetWindowLongA(hwnd, GWL_STYLE) & TBS_NOTICKS) return 0;

    return infoPtr->uNumTics+2; //includes last and first tick
}


static LRESULT
TRACKBAR_GetPageSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nPageSize;
}


static LRESULT
TRACKBAR_GetPos (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nPos;
}


static LRESULT
TRACKBAR_GetRangeMax (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nRangeMax;
}


static LRESULT
TRACKBAR_GetRangeMin (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nRangeMin;
}


static LRESULT
TRACKBAR_GetSelEnd (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nSelMax;
}


static LRESULT
TRACKBAR_GetSelStart (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->nSelMin;
}


static LRESULT
TRACKBAR_GetThumbLength (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return infoPtr->uThumbLen;
}

static LRESULT
TRACKBAR_GetPTics (HWND hwnd)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->tics;
}

static LRESULT
TRACKBAR_GetThumbRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    LPRECT lprc = (LPRECT)lParam;

    if (lprc == NULL) return 0;

    CopyRect(lprc,&infoPtr->rcFullThumb);

    return 0;
}


static LRESULT
TRACKBAR_GetTic (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT iTic;

    iTic = (INT)wParam;
    if ((iTic < 0) || (iTic > infoPtr->uNumTics)) return -1;

    return (LRESULT)infoPtr->tics[iTic];

}


static LRESULT
TRACKBAR_GetTicPos (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT iTic, range, width, pos;


    iTic = (INT)wParam;
    if ((iTic < 0) || (iTic > infoPtr->uNumTics)) return -1;

    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
    pos = infoPtr->rcChannel.left+(width*(infoPtr->tics[iTic]-infoPtr->nRangeMin))/range;

    return (LRESULT) pos;
}


static LRESULT
TRACKBAR_GetToolTips (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (GetWindowLongA (hwnd, GWL_STYLE) & TBS_TOOLTIPS)
        return (LRESULT)infoPtr->hwndToolTip;
    return 0;
}


/*      case TBM_GETUNICODEFORMAT: */


static LRESULT
TRACKBAR_SetBuddy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    HWND hwndTemp;

    if (wParam) {
        /* buddy is left or above */
        hwndTemp = infoPtr->hwndBuddyLA;
        infoPtr->hwndBuddyLA = (HWND)lParam;

//      FIXME (trackbar, "move buddy!\n");
    }
    else {
        /* buddy is right or below */
        hwndTemp = infoPtr->hwndBuddyRB;
        infoPtr->hwndBuddyRB = (HWND)lParam;

//        FIXME (trackbar, "move buddy!\n");
    }

    TRACKBAR_AlignBuddies (hwnd, infoPtr);

    return (LRESULT)hwndTemp;
}


static LRESULT
TRACKBAR_SetLineSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT nTemp = infoPtr->nLineSize;

    infoPtr->nLineSize = (INT)lParam;

    return nTemp;
}


static LRESULT
TRACKBAR_SetPageSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT nTemp = infoPtr->nPageSize;

    infoPtr->nPageSize = (INT)lParam;

    return nTemp;
}


static LRESULT
TRACKBAR_SetPos (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT lastPos = infoPtr->nPos;

    infoPtr->nPos = (INT)LOWORD(lParam);

    if (lastPos == infoPtr->nPos) return 0; //nothing changed

    if (infoPtr->nPos < infoPtr->nRangeMin)
        infoPtr->nPos = infoPtr->nRangeMin;

    if (infoPtr->nPos > infoPtr->nRangeMax)
        infoPtr->nPos = infoPtr->nRangeMax;
    infoPtr->flags |= TB_THUMBPOSCHANGED;

    if (wParam) TRACKBAR_UpdateThumbPosition(hwnd,lastPos);

    return 0;
}


static LRESULT
TRACKBAR_SetRange (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    int newMin,newMax;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    newMin = (INT)LOWORD(lParam);
    newMax = (INT)HIWORD(lParam);

    if (newMin == newMax) return 0;
    if (newMin > newMax)
    { //exchange
      int x;

      x = newMin;
      newMin = newMax;
      newMax = x;
    }
    if (newMin == infoPtr->nRangeMin && newMax == infoPtr->nRangeMax) return 0;

    infoPtr->nRangeMin = newMin;
    infoPtr->nRangeMax = newMax;

    if (infoPtr->nPos < infoPtr->nRangeMin)
    {
      infoPtr->nPos = infoPtr->nRangeMin;
      infoPtr->flags |= TB_THUMBPOSCHANGED;
    }
    if (infoPtr->nPos > infoPtr->nRangeMax)
    {
      infoPtr->nPos = infoPtr->nRangeMax;
      infoPtr->flags |= TB_THUMBPOSCHANGED;
    }

    if (infoPtr->nSelMin < infoPtr->nRangeMin)
    {
      infoPtr->nSelMin = infoPtr->nRangeMin;
      infoPtr->flags |= TB_SELECTIONCHANGED;
    }
    if (infoPtr->nSelMin > infoPtr->nRangeMax)
    {
      infoPtr->nSelMin = infoPtr->nRangeMax;
      infoPtr->flags |= TB_SELECTIONCHANGED;
    }
    if (infoPtr->nSelMax < infoPtr->nRangeMin)
    {
      infoPtr->nSelMax = infoPtr->nRangeMin;
      infoPtr->flags |= TB_SELECTIONCHANGED;
    }
    if (infoPtr->nSelMax > infoPtr->nRangeMax)
    {
      infoPtr->nSelMax = infoPtr->nRangeMax;
      infoPtr->flags |= TB_SELECTIONCHANGED;
    }

    infoPtr->nPageSize = (infoPtr->nRangeMax-infoPtr->nRangeMin)/5;
    if (infoPtr->nPageSize == 0) infoPtr->nPageSize = 1;
    TRACKBAR_RecalculateTics(hwnd,infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(hwnd);

    return 0;
}


static LRESULT
TRACKBAR_SetRangeMax (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if ((INT)lParam <= infoPtr->nRangeMin) return 0;
    if (infoPtr->nRangeMax == (INT)lParam) return 0;

    infoPtr->nRangeMax = (INT)lParam;
    if (infoPtr->nPos > infoPtr->nRangeMax)
    {
      infoPtr->nPos = infoPtr->nRangeMax;
      infoPtr->flags |=TB_THUMBPOSCHANGED;
    }

    infoPtr->nPageSize = (infoPtr->nRangeMax-infoPtr->nRangeMin)/5;
    if (infoPtr->nPageSize == 0) infoPtr->nPageSize = 1;
    TRACKBAR_RecalculateTics(hwnd,infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(hwnd);

    return 0;
}


static LRESULT
TRACKBAR_SetRangeMin (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if ((INT)lParam >= infoPtr->nRangeMax) return 0;
    if (infoPtr->nRangeMin == (INT)lParam) return 0;

    infoPtr->nRangeMin = (INT)lParam;
    if (infoPtr->nPos < infoPtr->nRangeMin)
    {
      infoPtr->nPos = infoPtr->nRangeMin;
      infoPtr->flags |=TB_THUMBPOSCHANGED;
    }

    infoPtr->nPageSize = (infoPtr->nRangeMax-infoPtr->nRangeMin)/5;
    if (infoPtr->nPageSize == 0) infoPtr->nPageSize = 1;
    TRACKBAR_RecalculateTics(hwnd,infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(hwnd);

    return 0;
}


static LRESULT
TRACKBAR_SetTicFreq (HWND hwnd, WPARAM wParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (infoPtr->uTicFreq == (UINT)wParam) return 0;

    if (!(GetWindowLongA(hwnd,GWL_STYLE) & TBS_AUTOTICKS)) return 0;

    infoPtr->uTicFreq = (UINT)wParam;

    TRACKBAR_RecalculateTics(hwnd,infoPtr,FALSE);

    TRACKBAR_Refresh(hwnd);

    return 0;
}


static LRESULT
TRACKBAR_SetSel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT newMin,newMax,oldMin,oldMax;

    oldMin = infoPtr->nSelMin;
    oldMax = infoPtr->nSelMax;
    newMin = (INT)LOWORD(lParam);
    newMax = (INT)HIWORD(lParam);

    if (infoPtr->nSelMin == newMin && infoPtr->nSelMax == newMax) return 0;
    infoPtr->nSelMin = newMin;
    infoPtr->nSelMax = newMax;

    if (infoPtr->nSelMin < infoPtr->nRangeMin) infoPtr->nSelMin = infoPtr->nRangeMin;
    if (infoPtr->nSelMin > infoPtr->nRangeMax) infoPtr->nSelMin = infoPtr->nRangeMax;
    if (infoPtr->nSelMax > infoPtr->nRangeMax) infoPtr->nSelMax = infoPtr->nRangeMax;
    if (infoPtr->nSelMax < infoPtr->nRangeMin) infoPtr->nSelMax = infoPtr->nRangeMin;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!GetWindowLongA(hwnd, GWL_STYLE) & TBS_ENABLESELRANGE) return 0;

    if (oldMin != newMin || oldMax != newMax)
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(hwnd);
    }

    return 0;
}


static LRESULT
TRACKBAR_SetSelEnd (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT oldMax;

    if (infoPtr->nSelMax == (INT)lParam) return 0;

    oldMax = infoPtr->nSelMax;
    infoPtr->nSelMax = (INT)lParam;

    if (infoPtr->nSelMax > infoPtr->nRangeMax) infoPtr->nSelMax = infoPtr->nRangeMax;
    if (infoPtr->nSelMax < infoPtr->nRangeMin) infoPtr->nSelMax = infoPtr->nRangeMin;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!GetWindowLongA(hwnd,GWL_STYLE) & TBS_ENABLESELRANGE) return 0;

    if (oldMax != infoPtr->nSelMax)
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(hwnd);
    }

    return 0;
}


static LRESULT
TRACKBAR_SetSelStart (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT oldMin;

    if (infoPtr->nSelMin == (INT)lParam) return 0;

    oldMin = infoPtr->nSelMin;
    infoPtr->nSelMin = (INT)lParam;

    if (infoPtr->nSelMin < infoPtr->nRangeMin) infoPtr->nSelMin = infoPtr->nRangeMin;
    if (infoPtr->nSelMin > infoPtr->nRangeMax) infoPtr->nSelMin = infoPtr->nRangeMax;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!GetWindowLongA(hwnd,GWL_STYLE) & TBS_ENABLESELRANGE) return 0;

    if (oldMin != infoPtr->nSelMin)
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(hwnd);
    }

    return 0;
}


static LRESULT
TRACKBAR_SetThumbLength (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (infoPtr->uThumbLen == (UINT)wParam) return 0;

    if (!(GetWindowLongA (hwnd, GWL_STYLE) & TBS_FIXEDLENGTH)) return 0;

    infoPtr->uThumbLen = (UINT)wParam;
    infoPtr->flags |= TB_THUMBSIZECHANGED;

    TRACKBAR_Refresh(hwnd);

    return 0;
}

static void TRACKBAR_QuickSort(LPLONG list,INT lo,INT hi)
{
  INT i,j,x,y;

  i = lo;
  j = hi;
  x = list[(lo+hi)/2];
  do
  {
    while (list[i] < x) i++;
    while (x < list[j]) j--;
    if (i <= j)
    {
      y = list[i];
      list[i] = list[j];
      list[j] = y;
      i++;
      j--;
    }
  } while (i <= j);
  if (lo < j) TRACKBAR_QuickSort(list,lo,j);
  if (i < hi) TRACKBAR_QuickSort(list,i,hi);
}

static LRESULT
TRACKBAR_SetTic (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT nPos = (INT)lParam;
    INT x;

    if (!(GetWindowLongA(hwnd, GWL_STYLE) & (TBS_AUTOTICKS | TBS_NOTICKS))) return 0;

    if ((nPos < infoPtr->nRangeMin) || (nPos > infoPtr->nRangeMax)) return FALSE;

    //Check if tick exists
    for (x = 0;x < infoPtr->uNumTics;x++)
    {
      if (infoPtr->tics[x] == nPos) return TRUE; //value is ok
    }

    infoPtr->uNumTics++;
    infoPtr->tics = COMCTL32_ReAlloc(infoPtr->tics,infoPtr->uNumTics*sizeof(DWORD));
    infoPtr->tics[infoPtr->uNumTics-1] = nPos;

    //Quicksort the list
    TRACKBAR_QuickSort(infoPtr->tics,0,infoPtr->uNumTics-1);

    TRACKBAR_Refresh(hwnd);

    return TRUE;
}


static LRESULT
TRACKBAR_SetTipSide (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    INT fTemp = infoPtr->fLocation;

    infoPtr->fLocation = (INT)wParam;

    return fTemp;
}


static LRESULT
TRACKBAR_SetToolTips (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    infoPtr->hwndToolTip = (HWND)wParam;

    return 0;
}


/*      case TBM_SETUNICODEFORMAT: */


static LRESULT
TRACKBAR_InitializeThumb (HWND hwnd)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    RECT clientRect;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    INT scaleSize;

    GetClientRect(hwnd,&clientRect);
    infoPtr->uThumbLen = THUMB_LEN;   /* initial thumb length */

    scaleSize = 2*BORDER_SIZE;
    if (dwStyle & TBS_NOTICKS) scaleSize += 0;
    else if (dwStyle & TBS_BOTH) scaleSize += 2*(SCALE_SIZE+SCALE_SPACE);
    else scaleSize += SCALE_SIZE+SCALE_SPACE;

    if (dwStyle & TBS_VERT)
    {
      INT width = clientRect.right-clientRect.left;

      if (infoPtr->uThumbLen+scaleSize > width) infoPtr->uThumbLen = MAX(width-scaleSize,THUMB_MINLEN);
    } else
    {
      INT height = clientRect.bottom-clientRect.top;

      if (infoPtr->uThumbLen+scaleSize > height) infoPtr->uThumbLen = MAX(height-scaleSize,THUMB_MINLEN);
    }

    TRACKBAR_CalcChannel(hwnd,infoPtr);
    TRACKBAR_CalcThumb(hwnd,infoPtr);

    infoPtr->flags &= ~TB_SELECTIONCHANGED;

    return 0;
}


static LRESULT
TRACKBAR_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr;

    infoPtr = (TRACKBAR_INFO *)COMCTL32_Alloc (sizeof(TRACKBAR_INFO));
    SetWindowLongA (hwnd, 0, (DWORD)infoPtr);

    /* set default values */
    infoPtr->nRangeMin  = 0;
    infoPtr->nRangeMax  = 100;
    infoPtr->nLineSize  = 1;
    infoPtr->nPageSize  = 20;
    infoPtr->nSelMin    = 0;
    infoPtr->nSelMax    = 0;
    infoPtr->nPos       = 0;

    infoPtr->uNumTics   = 0;    /* start and end tic are not included in count*/
    infoPtr->uTicFreq   = 1;
    infoPtr->tics       = NULL;
    infoPtr->clrBk      = GetSysColor(COLOR_3DFACE);
    infoPtr->hwndNotify = GetParent(hwnd);

    TRACKBAR_InitializeThumb (hwnd);

    /* Create tooltip control */
    if (GetWindowLongA(hwnd,GWL_STYLE) & TBS_TOOLTIPS)
    {
      TTTOOLINFOA ti;

      infoPtr->hwndToolTip =
          CreateWindowExA (WS_EX_TOOLWINDOW,TOOLTIPS_CLASSA,NULL,WS_POPUP,
                           CW_USEDEFAULT,CW_USEDEFAULT,
                           CW_USEDEFAULT,CW_USEDEFAULT,
                           hwnd,0,0,0);

      /* Send NM_TOOLTIPSCREATED notification */
      if (infoPtr->hwndToolTip)
      {
        NMTOOLTIPSCREATED nmttc;

        nmttc.hdr.hwndFrom = hwnd;
        nmttc.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
        nmttc.hdr.code = NM_TOOLTIPSCREATED;
        nmttc.hwndToolTips = infoPtr->hwndToolTip;

        SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmttc.hdr.idFrom,(LPARAM)&nmttc);
      }

      ZeroMemory(&ti,sizeof(TTTOOLINFOA));
      ti.cbSize   = sizeof(TTTOOLINFOA);
      ti.uFlags   = TTF_TRACK | TTF_CENTERTIP;
      ti.hwnd     = hwnd;
      ti.uId      = 0;
      ti.lpszText = "Test"; /* LPSTR_TEXTCALLBACK */
      SetRectEmpty(&ti.rect);

      SendMessageA(infoPtr->hwndToolTip,TTM_ADDTOOLA,0,(LPARAM)&ti);
    }

    return 0;
}


static LRESULT
TRACKBAR_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    /* delete tooltip control */
    if (infoPtr->hwndToolTip) DestroyWindow(infoPtr->hwndToolTip);

    COMCTL32_Free(infoPtr->tics);
    COMCTL32_Free(infoPtr);

    return 0;
}

/* pt intialized with cursor position */

static VOID TRACKBAR_CalcToolTipPos(HWND hwnd,DWORD dwStyle,TRACKBAR_INFO *infoPtr,POINT *pt)
{
  if (dwStyle & TBS_VERT)
  {
    pt->x += 5;
    pt->y += 15;
  } else
  {
    pt->x += 15;
    pt->y += 5;
  }
}

static LRESULT
TRACKBAR_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    int clickPlace,prevPos,vertical;
    DOUBLE clickPos;
    RECT thumb;
    POINT clickPoint;

    SetFocus (hwnd);

    vertical = dwStyle & TBS_VERT;
    clickPoint.x = (INT)LOWORD(lParam);
    clickPoint.y = (INT)HIWORD(lParam);


    if (vertical) clickPlace = clickPoint.y;
    else clickPlace = clickPoint.x;

    //Button down on thumb?
    thumb = infoPtr->rcFullThumb; //approzimative
    if ((vertical &&
         (clickPoint.y >= thumb.top) &&
         (clickPoint.y <= thumb.bottom) &&
         (clickPoint.x >= thumb.left) &&
         (clickPoint.x <= thumb.right))   ||
        (!vertical &&
         (clickPoint.x >= thumb.left) &&
         (clickPoint.x <= thumb.right) &&
         (clickPoint.y >= thumb.top) &&
         (clickPoint.y <= thumb.bottom)))
       {
         infoPtr->flags |= TB_DRAG_MODE;
         if (dwStyle & TBS_TOOLTIPS)
         {  /* enable tooltip */
           TTTOOLINFOA ti;
           POINT pt;
           char buf[80];

           GetCursorPos(&pt);
           TRACKBAR_CalcToolTipPos(hwnd,dwStyle,infoPtr,&pt);
           SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));

           ti.cbSize   = sizeof(TTTOOLINFOA);
           ti.uId      = 0;
           ti.hwnd     = (UINT)hwnd;
           ti.hinst = 0;
           sprintf (buf,"%d",infoPtr->nPos);
           ti.lpszText = (LPSTR)buf;

           infoPtr->flags |= TB_SHOW_TOOLTIP;
           SetCapture(hwnd);

           SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTA,0,(LPARAM)&ti);
           SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);
         }
         SetCapture(hwnd);
         TRACKBAR_UpdateThumb(hwnd); //change arrow color
         return 0;
       }
    else if ((vertical &&
              (clickPoint.y >= thumb.top) &&
              (clickPoint.y <= thumb.bottom))   ||
             (!vertical &&
              (clickPoint.x >= thumb.left) &&
              (clickPoint.x <= thumb.right)))
            {
              //ScrollMode
              infoPtr->flags |= TB_SCROLL_MODE;
              SetCapture(hwnd);
              SetTimer(hwnd,SCROLL_TIMER_ID,SCROLL_TIME,NULL);

              return 0;
            }

    clickPos = TRACKBAR_ConvertPlaceToPosition(infoPtr,clickPlace,vertical);
    prevPos = infoPtr->nPos;

    if (clickPos > prevPos)
    {  /* similar to VK_NEXT */
      infoPtr->nPos += infoPtr->nPageSize;
      if (infoPtr->nPos > infoPtr->nRangeMax) infoPtr->nPos = infoPtr->nRangeMax;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(hwnd,TB_PAGEUP);
    } else
    {  /* similar to VK_PRIOR */
      infoPtr->nPos -= infoPtr->nPageSize;
      if (infoPtr->nPos < infoPtr->nRangeMin) infoPtr->nPos = infoPtr->nRangeMin;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(hwnd,TB_PAGEDOWN);
    }

    if (prevPos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(hwnd,prevPos);
    }

    //ScrollMode
    infoPtr->flags |= TB_SCROLL_MODE;
    SetCapture(hwnd);
    SetTimer(hwnd,SCROLL_TIMER_ID,SCROLL_TIME,NULL);

    return 0;
}


static LRESULT
TRACKBAR_LButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (infoPtr->flags & TB_DRAG_MODE)
    {
      TRACKBAR_SendNotify(hwnd,TB_ENDTRACK);

      infoPtr->flags &= ~TB_DRAG_MODE;
      ReleaseCapture();
      TRACKBAR_UpdateThumb(hwnd); //change arrow color
    }

    if (infoPtr->flags & TB_SCROLL_MODE)
    {
      infoPtr->flags &= ~TB_SCROLL_MODE;
      ReleaseCapture();
      KillTimer(hwnd,SCROLL_TIMER_ID);
    }

    if (GetWindowLongA (hwnd, GWL_STYLE) & TBS_TOOLTIPS)
    { /* disable tooltip */
      TTTOOLINFOA ti;

      ti.cbSize   = sizeof(TTTOOLINFOA);
      ti.uId      = 0;
      ti.hwnd     = (UINT)hwnd;

      infoPtr->flags &= ~TB_SHOW_TOOLTIP;
      SendMessageA (infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&ti);
    }

    return 0;
}


static LRESULT TRACKBAR_Timer(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  POINT mousePoint;
  INT mousePlace,prevPos,newPos,vertical;
  DOUBLE mousePos;

  GetCursorPos(&mousePoint);
  ScreenToClient(hwnd,&mousePoint);

  vertical = dwStyle & TBS_VERT;
  if (vertical) mousePlace = mousePoint.y;
  else mousePlace = mousePoint.x;

  mousePos = TRACKBAR_ConvertPlaceToPosition(infoPtr,mousePlace,vertical);
  prevPos = infoPtr->nPos;

  if (mousePos > (INT)mousePos+0.5) newPos = mousePos+1;
  else newPos = mousePos;
  if (newPos == prevPos) return 0;

  if (newPos > prevPos)
  {  /* similar to VK_NEXT */
      infoPtr->nPos += infoPtr->nPageSize;
      if (infoPtr->nPos > infoPtr->nRangeMax) infoPtr->nPos = infoPtr->nRangeMax;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(hwnd,TB_PAGEUP);
    } else
    {  /* similar to VK_PRIOR */
      infoPtr->nPos -= infoPtr->nPageSize;
      if (infoPtr->nPos < infoPtr->nRangeMin) infoPtr->nPos = infoPtr->nRangeMin;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(hwnd,TB_PAGEDOWN);
    }

    if (prevPos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(hwnd,prevPos);
    }

  return 0;
}

static LRESULT
TRACKBAR_CaptureChanged (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    if (infoPtr->flags & TB_DRAGPOSVALID)
    {
      int lastPos = infoPtr->nPos;
      infoPtr->nPos = infoPtr->dragPos;
      if (lastPos != infoPtr->nPos) TRACKBAR_UpdateThumbPosition(hwnd,lastPos);
    }

    infoPtr->flags &= ~ TB_DRAGPOSVALID;

    if (infoPtr->flags & TB_SCROLL_MODE)
    {
      infoPtr->flags &= ~TB_SCROLL_MODE;
      KillTimer(hwnd,SCROLL_TIMER_ID);
    }

    TRACKBAR_SendNotify(hwnd,TB_ENDTRACK);
    return 0;
}


static LRESULT
TRACKBAR_Paint (HWND hwnd, WPARAM wParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = wParam == 0 ? BeginPaint(hwnd,&ps) : (HDC)wParam;
    TRACKBAR_Draw(hwnd,hdc);
    if (!wParam) EndPaint(hwnd,&ps);
    return 0;
}


static LRESULT
TRACKBAR_SetFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    HDC hdc;
    RECT rcClient;

//    TRACE (trackbar,"\n");
    if (!infoPtr->bFocus)
    {
      infoPtr->bFocus = TRUE;

      GetClientRect (hwnd,&rcClient);
      hdc = GetDC (hwnd);
      DrawFocusRect (hdc,&rcClient);
      ReleaseDC(hwnd,hdc);

    }
    return 0;
}

static LRESULT
TRACKBAR_KillFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    HDC hdc;
    RECT rcClient;

//    TRACE (trackbar,"\n");

    infoPtr->flags &= ~TB_DRAG_MODE;
    if (infoPtr->bFocus)
    {
      infoPtr->bFocus = FALSE;

      GetClientRect(hwnd,&rcClient);
      hdc = GetDC (hwnd);
      DrawFocusRect(hdc,&rcClient); //XOR removes
      ReleaseDC(hwnd,hdc);
    }

    return 0;
}

static LRESULT
TRACKBAR_Size (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);

    TRACKBAR_CalcChannel (hwnd, infoPtr);
    TRACKBAR_AlignBuddies (hwnd, infoPtr);

    return 0;
}


static BOOL
TRACKBAR_SendNotify (HWND hwnd, UINT code)
{
//    TRACE (trackbar, "%x\n",code);

    if (GetWindowLongA(hwnd, GWL_STYLE) & TBS_VERT)
    {
      return (BOOL)SendMessageA(GetParent(hwnd),WM_VSCROLL,(WPARAM)code,(LPARAM)hwnd);
    } else
    {
      return (BOOL)SendMessageA(GetParent(hwnd),WM_HSCROLL,(WPARAM)code,(LPARAM)hwnd);
    }
}


static LRESULT
TRACKBAR_MouseMove (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    SHORT clickPlace;
    DOUBLE dragPos;

//    TRACE (trackbar, "%x\n",wParam);

    if (!(infoPtr->flags & TB_DRAG_MODE)) return TRUE;

    if (dwStyle & TBS_VERT) clickPlace = (SHORT)HIWORD(lParam);
    else clickPlace = (SHORT)LOWORD(lParam);

    dragPos = TRACKBAR_ConvertPlaceToPosition(infoPtr,clickPlace,dwStyle & TBS_VERT);
    if (dragPos > ((INT)dragPos)+0.5) infoPtr->dragPos = dragPos + 1;
    else infoPtr->dragPos = dragPos;

    if (infoPtr->nPos == infoPtr->dragPos) return TRUE; //nothing changed

    infoPtr->flags |= TB_DRAGPOSVALID;

    TRACKBAR_UpdateThumbPosition(hwnd,infoPtr->nPos); //infoPtr->nPos now set

    TRACKBAR_SendNotify(hwnd,TB_THUMBTRACK | (infoPtr->nPos >> 16));

    if (infoPtr->flags & TB_SHOW_TOOLTIP)
    {
      POINT pt;
      TTTOOLINFOA ti;
      char buf[80];

      ti.cbSize = sizeof(TTTOOLINFOA);
      ti.hwnd = hwnd;
      ti.uId = 0;
      ti.hinst = 0;
      sprintf (buf,"%d",infoPtr->nPos);
      ti.lpszText = (LPSTR)buf;
      GetCursorPos(&pt);
      TRACKBAR_CalcToolTipPos(hwnd,dwStyle,infoPtr,&pt);

      SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTA,0,(LPARAM)&ti);
      SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));
    }

    return TRUE;
}


static LRESULT
TRACKBAR_KeyDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr(hwnd);
    INT pos;

//    TRACE (trackbar, "%x\n",wParam);

    if (infoPtr->flags & TB_DRAG_MODE) return TRUE;

    pos = infoPtr->nPos;
    switch (wParam) {
    case VK_LEFT:
    case VK_UP:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos -= infoPtr->nLineSize;
        if (infoPtr->nPos < infoPtr->nRangeMin)
            infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify(hwnd,TB_LINEUP);
        break;
    case VK_RIGHT:
    case VK_DOWN:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos += infoPtr->nLineSize;
        if (infoPtr->nPos > infoPtr->nRangeMax)
            infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify (hwnd, TB_LINEDOWN);
        break;
    case VK_NEXT:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos += infoPtr->nPageSize;
        if (infoPtr->nPos > infoPtr->nRangeMax)
            infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify (hwnd, TB_PAGEUP);
        break;
    case VK_PRIOR:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos -= infoPtr->nPageSize;
        if (infoPtr->nPos < infoPtr->nRangeMin)
            infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify (hwnd, TB_PAGEDOWN);
        break;
    case VK_HOME:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify (hwnd, TB_TOP);
        break;
    case VK_END:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify (hwnd, TB_BOTTOM);
        break;
    }

    if (pos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(hwnd,pos);
    }

    return TRUE;
}


static LRESULT
TRACKBAR_KeyUp (HWND hwnd, WPARAM wParam)
{
    TRACKBAR_INFO *infoPtr = TRACKBAR_GetInfoPtr(hwnd);

    if (infoPtr->flags & TB_DRAG_MODE) return TRUE;

    switch (wParam) {
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
    case VK_NEXT:
    case VK_PRIOR:
    case VK_HOME:
    case VK_END:
        TRACKBAR_SendNotify (hwnd, TB_ENDTRACK);
    }
    return TRUE;
}


LRESULT WINAPI
TRACKBAR_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case TBM_CLEARSEL:
        return TRACKBAR_ClearSel (hwnd, wParam, lParam);

    case TBM_CLEARTICS:
        return TRACKBAR_ClearTics (hwnd, wParam, lParam);

    case TBM_GETBUDDY:
        return TRACKBAR_GetBuddy (hwnd, wParam, lParam);

    case TBM_GETCHANNELRECT:
        return TRACKBAR_GetChannelRect (hwnd, wParam, lParam);

    case TBM_GETLINESIZE:
        return TRACKBAR_GetLineSize (hwnd, wParam, lParam);

    case TBM_GETNUMTICS:
        return TRACKBAR_GetNumTics (hwnd, wParam, lParam);

    case TBM_GETPAGESIZE:
        return TRACKBAR_GetPageSize (hwnd, wParam, lParam);

    case TBM_GETPOS:
        return TRACKBAR_GetPos (hwnd, wParam, lParam);

    case TBM_GETPTICS:
        return TRACKBAR_GetPTics (hwnd);

    case TBM_GETRANGEMAX:
        return TRACKBAR_GetRangeMax (hwnd, wParam, lParam);

    case TBM_GETRANGEMIN:
        return TRACKBAR_GetRangeMin (hwnd, wParam, lParam);

    case TBM_GETSELEND:
        return TRACKBAR_GetSelEnd (hwnd, wParam, lParam);

    case TBM_GETSELSTART:
        return TRACKBAR_GetSelStart (hwnd, wParam, lParam);

    case TBM_GETTHUMBLENGTH:
        return TRACKBAR_GetThumbLength (hwnd, wParam, lParam);

    case TBM_GETTHUMBRECT:
        return TRACKBAR_GetThumbRect (hwnd, wParam, lParam);

    case TBM_GETTIC:
        return TRACKBAR_GetTic (hwnd, wParam, lParam);

    case TBM_GETTICPOS:
        return TRACKBAR_GetTicPos (hwnd, wParam, lParam);

    case TBM_GETTOOLTIPS:
        return TRACKBAR_GetToolTips (hwnd, wParam, lParam);

/*      case TBM_GETUNICODEFORMAT: */

    case TBM_SETBUDDY:
        return TRACKBAR_SetBuddy (hwnd, wParam, lParam);

    case TBM_SETLINESIZE:
        return TRACKBAR_SetLineSize (hwnd, wParam, lParam);

    case TBM_SETPAGESIZE:
        return TRACKBAR_SetPageSize (hwnd, wParam, lParam);

    case TBM_SETPOS:
        return TRACKBAR_SetPos (hwnd, wParam, lParam);

    case TBM_SETRANGE:
        return TRACKBAR_SetRange (hwnd, wParam, lParam);

    case TBM_SETRANGEMAX:
        return TRACKBAR_SetRangeMax (hwnd, wParam, lParam);

    case TBM_SETRANGEMIN:
        return TRACKBAR_SetRangeMin (hwnd, wParam, lParam);

    case TBM_SETSEL:
        return TRACKBAR_SetSel (hwnd, wParam, lParam);

    case TBM_SETSELEND:
        return TRACKBAR_SetSelEnd (hwnd, wParam, lParam);

    case TBM_SETSELSTART:
        return TRACKBAR_SetSelStart (hwnd, wParam, lParam);

    case TBM_SETTHUMBLENGTH:
        return TRACKBAR_SetThumbLength (hwnd, wParam, lParam);

    case TBM_SETTIC:
        return TRACKBAR_SetTic (hwnd, wParam, lParam);

    case TBM_SETTICFREQ:
        return TRACKBAR_SetTicFreq (hwnd, wParam);

    case TBM_SETTIPSIDE:
        return TRACKBAR_SetTipSide (hwnd, wParam, lParam);

    case TBM_SETTOOLTIPS:
        return TRACKBAR_SetToolTips (hwnd, wParam, lParam);

/*      case TBM_SETUNICODEFORMAT: */


    case WM_CAPTURECHANGED:
        return TRACKBAR_CaptureChanged (hwnd, wParam, lParam);

    case WM_CREATE:
        return TRACKBAR_Create (hwnd, wParam, lParam);

    case WM_DESTROY:
        return TRACKBAR_Destroy (hwnd, wParam, lParam);

/*      case WM_ENABLE: */

/*      case WM_ERASEBKGND: */
/*          return 0; */

    case WM_GETDLGCODE:
        return DLGC_WANTARROWS;

    case WM_KEYDOWN:
        return TRACKBAR_KeyDown (hwnd, wParam, lParam);

    case WM_KEYUP:
        return TRACKBAR_KeyUp (hwnd, wParam);

    case WM_LBUTTONDOWN:
        return TRACKBAR_LButtonDown (hwnd, wParam, lParam);

    case WM_LBUTTONUP:
        return TRACKBAR_LButtonUp (hwnd, wParam, lParam);

    case WM_TIMER:
        return TRACKBAR_Timer(hwnd,wParam,lParam);

    case WM_MOUSEMOVE:
        return TRACKBAR_MouseMove (hwnd, wParam, lParam);

    case WM_PAINT:
        return TRACKBAR_Paint (hwnd, wParam);

    case WM_SETFOCUS:
        return TRACKBAR_SetFocus (hwnd, wParam, lParam);

    case WM_KILLFOCUS:
        return TRACKBAR_KillFocus (hwnd, wParam, lParam);

    case WM_SIZE:
        return TRACKBAR_Size (hwnd, wParam, lParam);

    case WM_WININICHANGE:
        return TRACKBAR_InitializeThumb (hwnd);

    default:
//        if (uMsg >= WM_USER)
//            ERR (trackbar, "unknown msg %04x wp=%08x lp=%08lx\n",
//                 uMsg, wParam, lParam);
        return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
TRACKBAR_Register (VOID)
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA (TRACKBAR_CLASSA)) return;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)TRACKBAR_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(TRACKBAR_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = TRACKBAR_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
TRACKBAR_Unregister (VOID)
{
    if (GlobalFindAtomA (TRACKBAR_CLASSA))
        UnregisterClassA (TRACKBAR_CLASSA, (HINSTANCE)NULL);
}

