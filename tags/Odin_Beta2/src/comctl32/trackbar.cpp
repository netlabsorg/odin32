/* $Id: trackbar.cpp,v 1.6 2001-03-31 13:25:27 sandervl Exp $ */
/*
 * Trackbar control
 *
 * Copyright 1998, 1999 Eric Kohl <ekohl@abo.rhein-zeitung.de>
 * Copyright 1998,1999 Alex Priem <alexp@sci.kun.nl>
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi <cbratschi@datacomm.ch>
 *
 *
 * Status: complete
 * Version: 5.80
 *
 * Note: TBM_SETTHUMBLENGTH implemented, COMCTL32 5.00 ignores it
 *       Microsoft(TM) should fix this bug :)
 */

#include "winbase.h"
#include "commctrl.h"
#include "ccbase.h"
#include "trackbar.h"
#include "comctl32.h"
#include <stdio.h>

#define TRACKBAR_GetInfoPtr(hwnd) ((TRACKBAR_INFO*)getInfoPtr(hwnd))


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

#define THUMB_LEN_SEL 23
#define THUMB_LEN     21
#define THUMB_MINLEN   4

#define CHANNEL_NOSEL_HEIGHT 4 //min no sel height
#define CHANNEL_MIN_HEIGHT   6 //min sel height
#define CHANNEL_THUMB_DIFF   8 //sel thumb, channel difference
#define CHANNEL_SPACE        8
#define CHANNEL_SCALE_SPACE  SCALE_SIZE+SCALE_SPACE+BORDER_SIZE
#define CHANNEL_THUMB_SPACE  BORDER_SIZE

#define TOOLTIP_SPACE 5

/* scroll mode */

#define SCROLL_TIME     500 //ms
#define SCROLL_TIMER_ID 1

/* Tooltips */

#define TOOLTIP_XSPACE 5
#define TOOLTIP_YSPACE 5

static const WORD pattern[8] =
     {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

static BOOL TRACKBAR_SendNotify(TRACKBAR_INFO *infoPtr,UINT code);

static void TRACKBAR_RecalculateTics(TRACKBAR_INFO *infoPtr,BOOL restoreOld)
{
    INT i,tic,nrTics;

    if (infoPtr->dwStyle & TBS_NOTICKS) //no ticks
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = NULL;

      infoPtr->uNumTics = 0;
      return;
    }

    if (restoreOld && !(infoPtr->dwStyle & TBS_AUTOTICKS) && infoPtr->tics)
    { //check old ticks
      LPLONG oldTics = (LONG*)COMCTL32_Alloc(infoPtr->uNumTics*sizeof(DWORD));
      INT count = 0;

      for (i = 0;i < infoPtr->uNumTics;i++)
      {
        if ((infoPtr->tics[i] >= infoPtr->nRangeMin) && (infoPtr->tics[i] <= infoPtr->nRangeMax))
        {
          oldTics[count] = infoPtr->tics[i];
          count++;
        }
      }

      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = (LONG*)COMCTL32_ReAlloc(oldTics,count*sizeof(DWORD));
      infoPtr->uNumTics = count;

      return;
    }

    if (infoPtr->uTicFreq && (infoPtr->nRangeMax > infoPtr->nRangeMin) && (infoPtr->dwStyle & TBS_AUTOTICKS))
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
      infoPtr->tics = (LONG*)COMCTL32_Alloc(nrTics*sizeof(DWORD));
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

    return pos;
}


static VOID
TRACKBAR_CalcChannel(TRACKBAR_INFO *infoPtr)
{
    INT channelSize;
    RECT lpRect,*channel = &infoPtr->rcChannel;
    INT thumbDiff;

    GetClientRect(infoPtr->hwnd,&lpRect);

    if (infoPtr->dwStyle & TBS_ENABLESELRANGE) channelSize = MAX(infoPtr->uThumbLen-CHANNEL_THUMB_DIFF,CHANNEL_MIN_HEIGHT);
    else channelSize = CHANNEL_NOSEL_HEIGHT;

    thumbDiff = (infoPtr->uThumbLen-channelSize)/2;

    if (infoPtr->dwStyle & TBS_VERT)
    {
      channel->top    = lpRect.top+CHANNEL_SPACE;
      channel->bottom = lpRect.bottom-CHANNEL_SPACE;

      if ((infoPtr->dwStyle & TBS_BOTH) || (infoPtr->dwStyle & TBS_NOTICKS))
      { //center
        channel->left  = (lpRect.right-channelSize)/2;
        channel->right = (lpRect.right+channelSize)/2;
      } else if (infoPtr->dwStyle & TBS_LEFT)
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
      if ((infoPtr->dwStyle & TBS_BOTH) || (infoPtr->dwStyle & TBS_NOTICKS))
      { //center
        channel->top    = (lpRect.bottom-channelSize)/2;
        channel->bottom = (lpRect.bottom+channelSize)/2;
      } else if (infoPtr->dwStyle & TBS_TOP)
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

static VOID
TRACKBAR_CalcThumb(TRACKBAR_INFO *infoPtr)
{
    RECT *thumb;
    RECT *fullThumb;
    int range, width;
    int x,y;
    int thumbFactor = 2;

    thumb = &infoPtr->rcThumb;
    fullThumb = &infoPtr->rcFullThumb;
    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    if (infoPtr->dwStyle & TBS_VERT)
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
      if (infoPtr->dwStyle & TBS_BOTH) return;
      x = y/2; //arrow width
      if (infoPtr->dwStyle & TBS_LEFT) thumb->left += x; else thumb->right -= x;
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
      if (infoPtr->dwStyle & TBS_BOTH) return;
      y = x/2; //arrow height
      if (infoPtr->dwStyle & TBS_TOP) thumb->top += y; else thumb->bottom -= y;
    }
}

static VOID
TRACKBAR_CalcSelection(TRACKBAR_INFO *infoPtr)
{
    RECT *selection;
    int range,width,height;
    int selMin = infoPtr->nSelMin-infoPtr->nRangeMin; //relative to nRangeMin
    int selMax = infoPtr->nSelMax-infoPtr->nRangeMin; //dito

    selection = &infoPtr->rcSelection;
    range = infoPtr->nRangeMax-infoPtr->nRangeMin;

    if ((range <= 0) || (selMin == selMax)) SetRectEmpty(selection);
    else
        if (!(infoPtr->dwStyle & TBS_VERT))
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
TRACKBAR_DrawHorizTic(TRACKBAR_INFO *infoPtr,HDC hdc,LONG ticPos,int flags,COLORREF clrTic)
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

static VOID TRACKBAR_FillHorzTics(TRACKBAR_INFO *infoPtr,HDC hdc,int flags,COLORREF clrTic)
{
  RECT rect;
  INT range = infoPtr->nRangeMax-infoPtr->nRangeMin;
  INT width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
  HBRUSH hbr = CreateSolidBrush(clrTic);

  if (flags & TBS_TOP)
  {
    rect.bottom = infoPtr->rcFullThumb.top-SCALE_SPACE-1+1;
    rect.top = rect.bottom-3;
  } else
  {
    rect.top = infoPtr->rcFullThumb.bottom+SCALE_SPACE+1;
    rect.bottom = rect.top+3;
  }
  rect.left = infoPtr->rcChannel.left+(width*(infoPtr->tics[0]-infoPtr->nRangeMin))/range;
  rect.right = infoPtr->rcChannel.left+(width*(infoPtr->tics[infoPtr->uNumTics-1]-infoPtr->nRangeMin))/range+1;

  FillRect(hdc,&rect,hbr);
  DeleteObject(hbr);
}

static VOID
TRACKBAR_DrawVertTic(TRACKBAR_INFO *infoPtr,HDC hdc,LONG ticPos,int flags,COLORREF clrTic)
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

static VOID TRACKBAR_FillVertTics(TRACKBAR_INFO *infoPtr,HDC hdc,int flags,COLORREF clrTic)
{
  RECT rect;
  INT range = infoPtr->nRangeMax-infoPtr->nRangeMin;
  INT width = infoPtr->rcChannel.bottom-infoPtr->rcChannel.top;
  HBRUSH hbr = CreateSolidBrush(clrTic);

  if (flags & TBS_TOP)
  {
    rect.right = infoPtr->rcFullThumb.left-SCALE_SPACE-1+1;
    rect.left = rect.right-3;
  } else
  {
    rect.left = infoPtr->rcFullThumb.right+SCALE_SPACE+1;
    rect.right = rect.left+3;
  }
  rect.top = infoPtr->rcChannel.top+(width*(infoPtr->tics[0]-infoPtr->nRangeMin))/range;
  rect.bottom = infoPtr->rcChannel.top+(width*(infoPtr->tics[infoPtr->uNumTics-1]-infoPtr->nRangeMin))/range+1;

  FillRect(hdc,&rect,hbr);
  DeleteObject(hbr);
}

static VOID
TRACKBAR_DrawTics(TRACKBAR_INFO *infoPtr,HDC hdc,LONG ticPos,int flags,COLORREF clrTic)
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

static VOID TRACKBAR_DrawThumb(TRACKBAR_INFO *infoPtr,HDC hdc)
{
    if (!(infoPtr->dwStyle & TBS_NOTHUMB))
    {
      HBRUSH hbr,hbrOld;
      RECT thumb = infoPtr->rcThumb;

      if ((infoPtr->flags & TB_DRAG_MODE) || !IsWindowEnabled(infoPtr->hwnd)) hbr = CreateSolidBrush(infoPtr->clrHighlightedThumb);
      else hbr = infoPtr->hbrThumb;
      hbrOld = SelectObject(hdc,hbr);

      if (infoPtr->dwStyle & TBS_BOTH)
      {
        DrawEdge(hdc,&thumb,EDGE_RAISED,BF_RECT | BF_ADJUST);
        FillRect(hdc,&thumb,hbr);
      } else
      {
        POINT points[6];
        RECT triangle;  /* for correct shadows of thumb */

        if (infoPtr->dwStyle & TBS_VERT)
        { //Vertical

          if (infoPtr->dwStyle & TBS_LEFT)
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
            LineTo(hdc,points[3].x,points[3].y);
            pen = GetSysColorPen(COLOR_BTNHIGHLIGHT);
            SelectObject(hdc,pen);
            MoveToEx(hdc,points[4].x,points[4].y+1,NULL);
            LineTo(hdc,points[3].x-1,points[3].y);
            SelectObject(hdc,oldPen);

            triangle.right = points[3].x;
            triangle.top = points[3].y;
            triangle.left = points[2].x;
            triangle.bottom = points[2].y;
            DrawEdge(hdc,&triangle,EDGE_RAISED,BF_DIAGONAL | BF_DIAGONAL_ENDBOTTOMLEFT);
          }
        } else
        { //Horizontal

          if (infoPtr->dwStyle & TBS_TOP)
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
      if (hbr != infoPtr->hbrThumb) DeleteObject(hbr);
    }
}

//draw the trackbar

static VOID TRACKBAR_Draw(TRACKBAR_INFO *infoPtr,HDC hdc)
{
    RECT rcClient,rcChannel,rcSelection;
    HBRUSH hBrush;
    int i;
    NMCUSTOMDRAW cdraw;
    LRESULT cdctlres,cdres;

    GetClientRect(infoPtr->hwnd,&rcClient);

    //Custom draw
    cdraw.dwDrawStage  = CDDS_PREPAINT;
    cdraw.hdc          = hdc;
    cdraw.dwItemSpec   = 0;
    cdraw.uItemState   = infoPtr->bFocus ? CDIS_FOCUS:CDIS_DEFAULT;
    cdraw.rc           = rcClient;
    cdraw.lItemlParam  = 0;

    cdctlres = sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);

    if (cdctlres & CDRF_SKIPDEFAULT) return;

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
      if (infoPtr->flags & TB_THUMBSIZECHANGED)
      {
        TRACKBAR_CalcChannel(infoPtr);
        TRACKBAR_CalcSelection(infoPtr);
      }
      TRACKBAR_CalcThumb(infoPtr);
    }
    if (infoPtr->flags & TB_SELECTIONCHANGED) TRACKBAR_CalcSelection(infoPtr);
    infoPtr->flags &= ~ (TB_THUMBCHANGED | TB_SELECTIONCHANGED | TB_DRAGPOSVALID);

    /* draw channel */

    if (cdctlres & CDRF_NOTIFYITEMDRAW)
    {
      cdraw.dwDrawStage    = CDDS_ITEMPREPAINT;
      cdraw.dwItemSpec     = TBCD_CHANNEL;
      cdraw.rc             = infoPtr->rcChannel;

      cdres = sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
    } else cdres = 0;

    if (!(cdres & CDRF_SKIPDEFAULT))
    {
      rcChannel = infoPtr->rcChannel;
      rcSelection = infoPtr->rcSelection;
      DrawEdge(hdc,&rcChannel,EDGE_SUNKEN,BF_RECT | BF_ADJUST);

      if (infoPtr->dwStyle & TBS_ENABLESELRANGE)           /* fill the channel */
      {
        HBRUSH hbr = CreateSolidBrush(RGB(255,255,255));
        FillRect(hdc,&rcChannel,hbr);
        DeleteObject(hbr);
        if (((infoPtr->dwStyle & TBS_VERT) && (rcSelection.top != rcSelection.bottom)) ||
           ((!(infoPtr->dwStyle & TBS_VERT)) && (rcSelection.left != rcSelection.right)))
           {
             hbr = CreateSolidBrush (COLOR_HIGHLIGHT);
             FillRect (hdc,&rcSelection,hbr);
             DeleteObject(hbr);
           }
      }

      if (cdctlres & CDRF_NOTIFYITEMDRAW)
      {
        cdraw.dwDrawStage    = CDDS_ITEMPOSTPAINT;

        sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
      }
    }

    /* draw tics */

    if (cdctlres & CDRF_NOTIFYITEMDRAW)
    {
      cdraw.dwDrawStage    = CDDS_ITEMPREPAINT;
      cdraw.dwItemSpec     = TBCD_TICS;
      SetRectEmpty(&cdraw.rc);

      cdres = sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
    } else cdres = 0;

    if (!(cdres & CDRF_SKIPDEFAULT))
    {
      if (!(infoPtr->dwStyle & TBS_NOTICKS))
      {
        int ticFlags = infoPtr->dwStyle & 0x0f;
        COLORREF clrTic = GetSysColor(COLOR_3DDKSHADOW);
        INT range = infoPtr->nRangeMax-infoPtr->nRangeMin;
        INT width = (infoPtr->dwStyle & TBS_VERT) ? rcChannel.bottom-rcChannel.top:rcChannel.right-rcChannel.left;

        //check if maximum of visible marks is reached
        if ((infoPtr->dwStyle & TBS_AUTOTICKS) && (infoPtr->uNumTics > 1) && ((INT)(width*infoPtr->tics[0]/range) == (INT)(width*infoPtr->tics[1]/range)))
        {
          //draw all pixels at once -> much faster
          if (infoPtr->dwStyle & TBS_VERT)
          {
            if ((ticFlags & TBS_LEFT) || (ticFlags & TBS_BOTH))
              TRACKBAR_FillVertTics(infoPtr,hdc,ticFlags | TBS_LEFT,clrTic);
            if (!(ticFlags & TBS_LEFT) || (ticFlags & TBS_BOTH))
              TRACKBAR_FillVertTics(infoPtr,hdc,ticFlags & ~TBS_LEFT,clrTic);
          } else
          {
            if ((ticFlags & TBS_TOP) || (ticFlags & TBS_BOTH))
              TRACKBAR_FillHorzTics(infoPtr,hdc,ticFlags | TBS_TOP,clrTic);
            if (!(ticFlags & TBS_TOP) || (ticFlags & TBS_BOTH))
              TRACKBAR_FillHorzTics(infoPtr,hdc,ticFlags & ~TBS_TOP,clrTic);
          }
        } else
        {
          for (i = 0;i < infoPtr->uNumTics;i++)
              TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->tics[i],ticFlags,clrTic);
        }

        TRACKBAR_DrawTics(infoPtr,hdc,0,ticFlags | TIC_LEFTEDGE,clrTic);
        TRACKBAR_DrawTics(infoPtr,hdc,0,ticFlags | TIC_RIGHTEDGE,clrTic);

        if ((infoPtr->dwStyle & TBS_ENABLESELRANGE) &&
              ((infoPtr->dwStyle & TBS_VERT && rcSelection.bottom != rcSelection.top) ||
              (!(infoPtr->dwStyle & TBS_VERT) && rcSelection.left != rcSelection.right)))
              {
                TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->nSelMin,ticFlags | TIC_SELECTIONMARKMIN,clrTic);
                TRACKBAR_DrawTics(infoPtr,hdc,infoPtr->nSelMax,ticFlags | TIC_SELECTIONMARKMAX,clrTic);
              }
      }

      if (cdctlres & CDRF_NOTIFYITEMDRAW)
      {
        cdraw.dwDrawStage    = CDDS_ITEMPOSTPAINT;

        sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
      }
    }

    /* draw thumb */

    if (cdctlres & CDRF_NOTIFYITEMDRAW)
    {
      cdraw.dwDrawStage    = CDDS_ITEMPREPAINT;
      cdraw.dwItemSpec     = TBCD_THUMB;
      cdraw.rc             = infoPtr->rcFullThumb;

      cdres = sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
    } else cdres = 0;

    if (!(cdres & CDRF_SKIPDEFAULT))
    {
      TRACKBAR_DrawThumb(infoPtr,hdc);

      if (cdctlres & CDRF_NOTIFYITEMDRAW)
      {
        cdraw.dwDrawStage    = CDDS_ITEMPOSTPAINT;

        sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
      }

    }

    if (infoPtr->bFocus && IsWindowEnabled(infoPtr->hwnd)) DrawFocusRect(hdc,&rcClient);

    if (cdctlres & CDRF_NOTIFYPOSTPAINT)
    {
      cdraw.dwDrawStage    = CDDS_POSTPAINT;
      cdraw.dwItemSpec     = 0;
      GetClientRect(infoPtr->hwnd,&cdraw.rc);

      sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&cdraw.hdr);
    }

}

//update thumb position

static VOID TRACKBAR_UpdateThumbPosition(TRACKBAR_INFO *infoPtr,INT lastPos,BOOL mustRedraw)
{
   HDC hdc;
   RECT lastRect,newRect,updateRect;
   HDC hdcCompatible;
   HBITMAP bitmap,oldbmp;

   //last
   lastRect = infoPtr->rcFullThumb;

   //new
   if (infoPtr->flags & TB_DRAGPOSVALID)
   {
     infoPtr->nPos = infoPtr->dragPos;
     infoPtr->flags &= ~TB_DRAGPOSVALID;
   }
   if (!mustRedraw && (infoPtr->nPos == lastPos)) return;

   if (infoPtr->dwStyle & TBS_NOTHUMB) return;

   TRACKBAR_CalcThumb(infoPtr);
   infoPtr->flags &= ~TB_THUMBCHANGED;
   newRect = infoPtr->rcFullThumb;

   //same rect?
   if (!mustRedraw && EqualRect(&lastRect,&newRect)) return;

   //3D frame adjustation
   lastRect.right++;
   lastRect.bottom++;
   newRect.right++;
   newRect.bottom++;

   //BitBlt from memory -> no flickering
   updateRect.left = MIN(lastRect.left,newRect.left);
   updateRect.right = MAX(lastRect.right,newRect.right);
   updateRect.top = MIN(lastRect.top,newRect.top);
   updateRect.bottom = MAX(lastRect.bottom,newRect.bottom);
   hdc = GetDC(infoPtr->hwnd);
   hdcCompatible = CreateCompatibleDC(hdc);
   bitmap = CreateCompatibleBitmap(hdc,updateRect.right,updateRect.bottom);
   oldbmp = SelectObject(hdcCompatible,bitmap);
   TRACKBAR_Draw(infoPtr,hdcCompatible);
   if (infoPtr->dwStyle & TBS_VERT)
   {
     if ((lastRect.top > newRect.top) && (lastRect.top < newRect.bottom))
       BitBlt(hdc,newRect.left,newRect.top,newRect.right-newRect.left,lastRect.bottom-newRect.top,hdcCompatible,newRect.left,newRect.top,SRCCOPY);
     else if ((lastRect.bottom < newRect.bottom) && (lastRect.bottom > newRect.top))
       BitBlt(hdc,lastRect.left,lastRect.top,lastRect.right-lastRect.left,newRect.bottom-lastRect.top,hdcCompatible,lastRect.left,lastRect.top,SRCCOPY);
     else
     {
       BitBlt(hdc,lastRect.left,lastRect.top,lastRect.right-lastRect.left,lastRect.bottom-lastRect.top,hdcCompatible,lastRect.left,lastRect.top,SRCCOPY);
       BitBlt(hdc,newRect.left,newRect.top,newRect.right-newRect.left,newRect.bottom-newRect.top,hdcCompatible,newRect.left,newRect.top,SRCCOPY);
     }
   } else
   {
     if ((lastRect.right > newRect.left) && (lastRect.right < newRect.right))
       BitBlt(hdc,lastRect.left,lastRect.top,newRect.right-lastRect.left,lastRect.bottom-lastRect.top,hdcCompatible,lastRect.left,lastRect.top,SRCCOPY);
     else if ((lastRect.left < newRect.right) && (lastRect.left > newRect.left))
       BitBlt(hdc,newRect.left,newRect.top,lastRect.right-newRect.left,newRect.bottom-newRect.top,hdcCompatible,newRect.left,newRect.top,SRCCOPY);
     else
     {
       BitBlt(hdc,lastRect.left,lastRect.top,lastRect.right-lastRect.left,lastRect.bottom-lastRect.top,hdcCompatible,lastRect.left,lastRect.top,SRCCOPY);
       BitBlt(hdc,newRect.left,newRect.top,newRect.right-newRect.left,newRect.bottom-newRect.top,hdcCompatible,newRect.left,newRect.top,SRCCOPY);
     }
   }
   SelectObject(hdcCompatible,oldbmp);
   DeleteObject(bitmap);
   DeleteDC(hdcCompatible);
   ReleaseDC(infoPtr->hwnd,hdc);
}

//redraw everything

static VOID TRACKBAR_Refresh(TRACKBAR_INFO *infoPtr)
{
   HDC hdc;

   hdc = GetDC(infoPtr->hwnd);
   TRACKBAR_Draw(infoPtr,hdc);
   ReleaseDC(infoPtr->hwnd,hdc);
}

static VOID
TRACKBAR_AlignBuddies(TRACKBAR_INFO *infoPtr)
{
    HWND hwndParent = GetParent(infoPtr->hwnd);
    RECT rcSelf,rcBuddy;
    INT x, y;

    GetWindowRect(infoPtr->hwnd,&rcSelf);
    MapWindowPoints(HWND_DESKTOP,hwndParent,(LPPOINT)&rcSelf,2);

    /* align buddy left or above */
    if (infoPtr->hwndBuddyLA)
    {
      GetWindowRect(infoPtr->hwndBuddyLA,&rcBuddy);

      if (infoPtr->dwStyle & TBS_VERT)
      { //above
        x = (infoPtr->rcChannel.right+infoPtr->rcChannel.left)/2-(rcBuddy.right-rcBuddy.left)/2+rcSelf.left;
        y = rcSelf.top-(rcBuddy.bottom-rcBuddy.top);
      } else
      { //left
        x = rcSelf.left-(rcBuddy.right-rcBuddy.left);
        y = (infoPtr->rcChannel.bottom+infoPtr->rcChannel.top)/2-(rcBuddy.bottom-rcBuddy.top)/2+rcSelf.top;
      }

      SetWindowPos(infoPtr->hwndBuddyLA,0,x,y,0,0,SWP_NOZORDER | SWP_NOSIZE);
    }


    /* align buddy right or below */
    if (infoPtr->hwndBuddyRB)
    {
      GetWindowRect(infoPtr->hwndBuddyRB,&rcBuddy);

      if (infoPtr->dwStyle & TBS_VERT)
      { //below
        x = (infoPtr->rcChannel.right+infoPtr->rcChannel.left)/2-(rcBuddy.right-rcBuddy.left)/2+rcSelf.left;
        y = rcSelf.bottom;
      } else
      { //right
        x = rcSelf.right;
        y = (infoPtr->rcChannel.bottom+infoPtr->rcChannel.top)/2-(rcBuddy.bottom-rcBuddy.top)/2+rcSelf.top;
      }

      SetWindowPos(infoPtr->hwndBuddyRB,0,x,y,0,0,SWP_NOZORDER | SWP_NOSIZE);
    }
}

static LRESULT
TRACKBAR_ClearSel(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->nSelMin != infoPtr->nSelMax)
    {
      infoPtr->nSelMin = 0;
      infoPtr->nSelMax = 0;
      infoPtr->flags |= TB_SELECTIONCHANGED;

      if ((BOOL)wParam) TRACKBAR_Refresh(infoPtr);
    }

    return 0;
}

static LRESULT
TRACKBAR_ClearTics(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (!(infoPtr->dwStyle & (TBS_AUTOTICKS | TBS_NOTICKS))) return 0;

    if (infoPtr->tics)
    {
      COMCTL32_Free(infoPtr->tics);
      infoPtr->tics = NULL;
      infoPtr->uNumTics = 0;

      if (wParam) TRACKBAR_Refresh(infoPtr);
    }

    return 0;
}


static LRESULT
TRACKBAR_GetBuddy(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (wParam)
      return (LRESULT)infoPtr->hwndBuddyLA; //left or above
    else
      return (LRESULT)infoPtr->hwndBuddyRB; //right or below
}

static LRESULT
TRACKBAR_GetChannelRect(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    LPRECT lprc = (LPRECT)lParam;

    if (lprc == NULL) return 0;

    CopyRect(lprc,&infoPtr->rcChannel);

    return 0;
}

static LRESULT
TRACKBAR_GetLineSize(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nLineSize;
}


static LRESULT
TRACKBAR_GetNumTics(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->dwStyle & TBS_NOTICKS) return 0;

    return infoPtr->uNumTics+2; //includes last and first tick
}

static LRESULT
TRACKBAR_GetPageSize(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nPageSize;
}


static LRESULT
TRACKBAR_GetPos(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nPos;
}


static LRESULT
TRACKBAR_GetRangeMax(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nRangeMax;
}


static LRESULT
TRACKBAR_GetRangeMin(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nRangeMin;
}


static LRESULT
TRACKBAR_GetSelEnd(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nSelMax;
}


static LRESULT
TRACKBAR_GetSelStart(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->nSelMin;
}


static LRESULT
TRACKBAR_GetThumbLength(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    return infoPtr->uThumbLen;
}

static LRESULT
TRACKBAR_GetPTics(TRACKBAR_INFO *infoPtr)
{
    return (LRESULT)infoPtr->tics;
}

static LRESULT
TRACKBAR_GetThumbRect(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    LPRECT lprc = (LPRECT)lParam;

    if (lprc == NULL) return 0;

    CopyRect(lprc,&infoPtr->rcFullThumb);

    return 0;
}


static LRESULT
TRACKBAR_GetTic(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT iTic;

    iTic = (INT)wParam;
    if ((iTic < 0) || (iTic > infoPtr->uNumTics)) return -1;

    return (LRESULT)infoPtr->tics[iTic];

}


static LRESULT
TRACKBAR_GetTicPos(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT iTic, range, width, pos;


    iTic = (INT)wParam;
    if ((iTic < 0) || (iTic > infoPtr->uNumTics)) return -1;

    range = infoPtr->nRangeMax-infoPtr->nRangeMin;
    width = infoPtr->rcChannel.right-infoPtr->rcChannel.left;
    pos = infoPtr->rcChannel.left+(width*(infoPtr->tics[iTic]-infoPtr->nRangeMin))/range;

    return (LRESULT) pos;
}


static LRESULT
TRACKBAR_GetToolTips(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->dwStyle & TBS_TOOLTIPS)
        return (LRESULT)infoPtr->hwndToolTip;
    return 0;
}


static LRESULT
TRACKBAR_SetBuddy(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    HWND hwndTemp;

    if (!(GetParent(lParam) == GetParent(infoPtr->hwnd))) return wParam ? infoPtr->hwndBuddyLA:infoPtr->hwndBuddyRB;

    if (wParam)
    {
      /* buddy is left or above */
      hwndTemp = infoPtr->hwndBuddyLA;
      infoPtr->hwndBuddyLA = (HWND)lParam;
    } else
    {
      /* buddy is right or below */
      hwndTemp = infoPtr->hwndBuddyRB;
      infoPtr->hwndBuddyRB = (HWND)lParam;
    }

    TRACKBAR_AlignBuddies(infoPtr);

    return (LRESULT)hwndTemp;
}


static LRESULT
TRACKBAR_SetLineSize(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT nTemp = infoPtr->nLineSize;

    infoPtr->nLineSize = (INT)lParam;

    return nTemp;
}


static LRESULT
TRACKBAR_SetPageSize(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT nTemp = infoPtr->nPageSize;

    infoPtr->nPageSize = (INT)lParam;

    return nTemp;
}


static LRESULT
TRACKBAR_SetPos(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT lastPos = infoPtr->nPos;

    infoPtr->nPos = (INT)LOWORD(lParam);

    if (lastPos == infoPtr->nPos) return 0; //nothing changed

    if (infoPtr->nPos < infoPtr->nRangeMin)
        infoPtr->nPos = infoPtr->nRangeMin;

    if (infoPtr->nPos > infoPtr->nRangeMax)
        infoPtr->nPos = infoPtr->nRangeMax;
    infoPtr->flags |= TB_THUMBPOSCHANGED;

    if (wParam) TRACKBAR_UpdateThumbPosition(infoPtr,lastPos,FALSE);

    return 0;
}


static LRESULT
TRACKBAR_SetRange(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    int newMin,newMax;

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
    if ((newMin == infoPtr->nRangeMin) && (newMax == infoPtr->nRangeMax)) return 0;

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
    TRACKBAR_RecalculateTics(infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(infoPtr);

    return 0;
}


static LRESULT
TRACKBAR_SetRangeMax(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
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
    TRACKBAR_RecalculateTics(infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(infoPtr);

    return 0;
}


static LRESULT
TRACKBAR_SetRangeMin(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
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
    TRACKBAR_RecalculateTics(infoPtr,TRUE);

    if (wParam) TRACKBAR_Refresh(infoPtr);

    return 0;
}


static LRESULT
TRACKBAR_SetTicFreq(TRACKBAR_INFO *infoPtr,WPARAM wParam)
{
    if (infoPtr->uTicFreq == (UINT)wParam) return 0;

    if (!(infoPtr->dwStyle & TBS_AUTOTICKS)) return 0;

    infoPtr->uTicFreq = (UINT)wParam;

    TRACKBAR_RecalculateTics(infoPtr,FALSE);

    TRACKBAR_Refresh(infoPtr);

    return 0;
}

static LRESULT
TRACKBAR_SetSel(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT newMin,newMax,oldMin,oldMax;

    oldMin = infoPtr->nSelMin;
    oldMax = infoPtr->nSelMax;
    newMin = (INT)LOWORD(lParam);
    newMax = (INT)HIWORD(lParam);

    if ((infoPtr->nSelMin == newMin) && (infoPtr->nSelMax == newMax)) return 0;
    infoPtr->nSelMin = newMin;
    infoPtr->nSelMax = newMax;

    if (infoPtr->nSelMin < infoPtr->nRangeMin) infoPtr->nSelMin = infoPtr->nRangeMin;
    if (infoPtr->nSelMin > infoPtr->nRangeMax) infoPtr->nSelMin = infoPtr->nRangeMax;
    if (infoPtr->nSelMax > infoPtr->nRangeMax) infoPtr->nSelMax = infoPtr->nRangeMax;
    if (infoPtr->nSelMax < infoPtr->nRangeMin) infoPtr->nSelMax = infoPtr->nRangeMin;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!(infoPtr->dwStyle & TBS_ENABLESELRANGE)) return 0;

    if ((oldMin != newMin) || (oldMax != newMax))
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(infoPtr);
    }

    return 0;
}

static LRESULT
TRACKBAR_SetSelEnd(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT oldMax;

    if (infoPtr->nSelMax == (INT)lParam) return 0;

    oldMax = infoPtr->nSelMax;
    infoPtr->nSelMax = (INT)lParam;

    if (infoPtr->nSelMax > infoPtr->nRangeMax) infoPtr->nSelMax = infoPtr->nRangeMax;
    if (infoPtr->nSelMax < infoPtr->nRangeMin) infoPtr->nSelMax = infoPtr->nRangeMin;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!(infoPtr->dwStyle & TBS_ENABLESELRANGE)) return 0;

    if (oldMax != infoPtr->nSelMax)
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(infoPtr);
    }

    return 0;
}

static LRESULT
TRACKBAR_SetSelStart(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT oldMin;

    if (infoPtr->nSelMin == (INT)lParam) return 0;

    oldMin = infoPtr->nSelMin;
    infoPtr->nSelMin = (INT)lParam;

    if (infoPtr->nSelMin < infoPtr->nRangeMin) infoPtr->nSelMin = infoPtr->nRangeMin;
    if (infoPtr->nSelMin > infoPtr->nRangeMax) infoPtr->nSelMin = infoPtr->nRangeMax;

    if (infoPtr->nSelMin > infoPtr->nSelMax) infoPtr->nSelMin = infoPtr->nSelMax;

    if (!(infoPtr->dwStyle & TBS_ENABLESELRANGE)) return 0;

    if (oldMin != infoPtr->nSelMin)
    {
      infoPtr->flags |= TB_SELECTIONCHANGED;
      if (wParam) TRACKBAR_Refresh(infoPtr);
    }

    return 0;
}

static LRESULT
TRACKBAR_SetThumbLength(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->uThumbLen == (UINT)wParam) return 0;

    if (infoPtr->dwStyle & TBS_FIXEDLENGTH) return 0;

    infoPtr->uThumbLen = MAX((UINT)wParam,THUMB_MINLEN);
    infoPtr->flags |= TB_THUMBSIZECHANGED | TB_THUMBCHANGED;

    TRACKBAR_Refresh(infoPtr);

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
TRACKBAR_SetTic(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT nPos = (INT)lParam;
    INT x;

    if (!(infoPtr->dwStyle & (TBS_AUTOTICKS | TBS_NOTICKS))) return 0;

    if ((nPos < infoPtr->nRangeMin) || (nPos > infoPtr->nRangeMax)) return FALSE;

    //Check if tick exists
    for (x = 0;x < infoPtr->uNumTics;x++)
    {
      if (infoPtr->tics[x] == nPos) return TRUE; //value is ok
    }

    infoPtr->uNumTics++;
    infoPtr->tics = (LONG*)COMCTL32_ReAlloc(infoPtr->tics,infoPtr->uNumTics*sizeof(DWORD));
    infoPtr->tics[infoPtr->uNumTics-1] = nPos;

    //Quicksort the list
    TRACKBAR_QuickSort(infoPtr->tics,0,infoPtr->uNumTics-1);

    TRACKBAR_Refresh(infoPtr);

    return TRUE;
}

static LRESULT
TRACKBAR_SetTipSide(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT fTemp = infoPtr->fLocation;

    if (infoPtr->dwStyle & TBS_VERT)
    {
      if ((wParam == TBTS_LEFT) || (wParam == TBTS_RIGHT)) infoPtr->fLocation = (INT)wParam;
    } else
    {
      if ((wParam == TBTS_TOP) || (wParam == TBTS_BOTTOM)) infoPtr->fLocation = (INT)wParam;
    }

    return fTemp;
}

static LRESULT
TRACKBAR_SetToolTips(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    infoPtr->hwndToolTip = (HWND)wParam;

    return 0;
}

static LRESULT
TRACKBAR_InitializeThumb(TRACKBAR_INFO *infoPtr)
{
    RECT clientRect;
    INT scaleSize;

    GetClientRect(infoPtr->hwnd,&clientRect);
    infoPtr->uThumbLen = (infoPtr->dwStyle & TBS_ENABLESELRANGE) ? THUMB_LEN_SEL:THUMB_LEN;   /* initial thumb length */

    scaleSize = 2*BORDER_SIZE;
    if (infoPtr->dwStyle & TBS_NOTICKS) scaleSize += 0;
    else if (infoPtr->dwStyle & TBS_BOTH) scaleSize += 2*(SCALE_SIZE+SCALE_SPACE);
    else scaleSize += SCALE_SIZE+SCALE_SPACE;

    if (infoPtr->dwStyle & TBS_VERT)
    {
      INT width = clientRect.right-clientRect.left;

      if (infoPtr->uThumbLen+scaleSize > width) infoPtr->uThumbLen = MAX(width-scaleSize,THUMB_MINLEN);
    } else
    {
      INT height = clientRect.bottom-clientRect.top;

      if (infoPtr->uThumbLen+scaleSize > height) infoPtr->uThumbLen = MAX(height-scaleSize,THUMB_MINLEN);
    }

    TRACKBAR_CalcChannel(infoPtr);
    TRACKBAR_CalcThumb(infoPtr);

    infoPtr->flags &= ~TB_SELECTIONCHANGED;

    return 0;
}

static LRESULT
TRACKBAR_InitColors(TRACKBAR_INFO *infoPtr,BOOL redraw)
{
  infoPtr->clrBk = GetSysColor(COLOR_3DFACE);
  infoPtr->clrHighlightedThumb = GetSysColor(COLOR_3DHILIGHT);

  if (redraw) TRACKBAR_Refresh(infoPtr);

  return 0;
}

static LRESULT
TRACKBAR_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    TRACKBAR_INFO *infoPtr;
    LOGBRUSH lb;

    infoPtr = (TRACKBAR_INFO*)initControl(hwnd,sizeof(TRACKBAR_INFO));

    /* set default values */
    infoPtr->hwnd    = hwnd;
    infoPtr->dwStyle = GetWindowLongA(hwnd,GWL_STYLE);;

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

    /* Create dotted brush */
    lb.lbStyle = BS_PATTERN;
    lb.lbHatch = (INT)CreateBitmap(8, 8, 1, 1, pattern);
    infoPtr->hbrThumb = CreateBrushIndirect(&lb);
    DeleteObject((HGDIOBJ)lb.lbHatch);

    TRACKBAR_InitColors(infoPtr,FALSE);

    infoPtr->hwndBuddyLA = 0;
    infoPtr->hwndBuddyRB = 0;
    infoPtr->flags       = 0;
    infoPtr->bFocus      = FALSE;

    if (infoPtr->dwStyle & TBS_VERT)
    {
      infoPtr->fLocation = (infoPtr->dwStyle & TBS_LEFT) ? TBTS_RIGHT : TBTS_LEFT;
    } else
    {
      infoPtr->fLocation = (infoPtr->dwStyle & TBS_TOP) ? TBTS_BOTTOM : TBTS_TOP;
    }

    TRACKBAR_InitializeThumb(infoPtr);

    /* Create tooltip control */
    if (infoPtr->dwStyle & TBS_TOOLTIPS)
    {
      UINT uFlags = TTF_TRACK | TTF_ABSOLUTE;

      if (infoPtr->dwStyle & TBS_VERT)
        if (infoPtr->fLocation == TBTS_RIGHT)
          uFlags |= TTF_ALIGNRIGHT | TTF_VCENTER;
        else
          uFlags |= TTF_ALIGNLEFT | TTF_VCENTER;
      else
        if (infoPtr->fLocation == TBTS_TOP)
          uFlags |= TTF_ALIGNTOP | TTF_HCENTER;
        else
          uFlags |= TTF_ALIGNBOTTOM | TTF_HCENTER;

      infoPtr->hwndToolTip = createToolTip(hwnd,uFlags,TRUE);
    }

    return 0;
}

static LRESULT
TRACKBAR_Destroy(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->hbrThumb)
      DeleteObject((HGDIOBJ)infoPtr->hbrThumb);

    /* delete tooltip control */
    destroyToolTip(infoPtr->hwndToolTip);

    COMCTL32_Free(infoPtr->tics);
    doneControl(infoPtr->hwnd);

    return 0;
}

static VOID TRACKBAR_CalcToolTipPos(TRACKBAR_INFO *infoPtr,POINT *pt)
{
  if (infoPtr->dwStyle & TBS_VERT)
  {
    if (infoPtr->fLocation == TBTS_RIGHT)
    {
      pt->x = infoPtr->rcFullThumb.right+TOOLTIP_SPACE;
      pt->y = infoPtr->rcFullThumb.top+(infoPtr->rcFullThumb.bottom-infoPtr->rcFullThumb.top)/2;
    } else
    {
      pt->x = infoPtr->rcFullThumb.left-TOOLTIP_SPACE;
      pt->y = infoPtr->rcFullThumb.top+(infoPtr->rcFullThumb.bottom-infoPtr->rcFullThumb.top)/2;
    }

  } else
  {
    if (infoPtr->fLocation == TBTS_TOP)
    {
      pt->x = infoPtr->rcFullThumb.left+(infoPtr->rcFullThumb.right-infoPtr->rcFullThumb.left)/2;
      pt->y = infoPtr->rcFullThumb.top-TOOLTIP_SPACE;
    } else
    {
      pt->x = infoPtr->rcFullThumb.left+(infoPtr->rcFullThumb.right-infoPtr->rcFullThumb.left)/2;
      pt->y = infoPtr->rcFullThumb.bottom+TOOLTIP_SPACE;
    }
  }
  ClientToScreen(infoPtr->hwnd,pt);
}

static LRESULT
TRACKBAR_LButtonDown(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    int clickPlace,prevPos,vertical;
    DOUBLE clickPos;
    RECT thumb,fullThumb;
    POINT clickPoint;

    SetFocus(infoPtr->hwnd);

    vertical = infoPtr->dwStyle & TBS_VERT;
    clickPoint.x = (INT)LOWORD(lParam);
    clickPoint.y = (INT)HIWORD(lParam);


    if (vertical) clickPlace = clickPoint.y;
    else clickPlace = clickPoint.x;

    //Button down on thumb?
    thumb = infoPtr->rcThumb;
    fullThumb = infoPtr->rcFullThumb;
    if ((vertical &&
         (clickPoint.y >= thumb.top) &&
         (clickPoint.y <= thumb.bottom) &&
         (((infoPtr->dwStyle & TBS_BOTH) &&
           (clickPoint.x >= thumb.left) &&
           (clickPoint.x <= thumb.right)
          ) ||
          ((infoPtr->dwStyle & TBS_LEFT) &&
           (clickPoint.x <= thumb.right) &&
           ((clickPoint.x >= thumb.left) ||
            ((clickPoint.x >= fullThumb.left) &&
             ((thumb.left-clickPoint.x <= clickPoint.y-thumb.top) &&
              (thumb.left-clickPoint.x <= thumb.bottom-clickPoint.y)
             )
            )
           )
          ) ||
          (!(infoPtr->dwStyle & (TBS_BOTH | TBS_LEFT)) &&
           (clickPoint.x >= thumb.left) &&
           ((clickPoint.x <= thumb.right) ||
            ((clickPoint.x <= fullThumb.right) &&
             ((clickPoint.x-thumb.right <= clickPoint.y-thumb.top) &&
              (clickPoint.x-thumb.right <= thumb.bottom-clickPoint.y)
             )
            )
           )
         ))
        ) ||
        (!vertical &&
         (clickPoint.x >= thumb.left) &&
         (clickPoint.x <= thumb.right) &&
         (((infoPtr->dwStyle & TBS_BOTH) &&
           (clickPoint.y >= thumb.top) &&
           (clickPoint.y <= thumb.bottom)
          ) ||
          ((infoPtr->dwStyle & TBS_TOP) &&
           (clickPoint.y <= thumb.bottom) &&
           ((clickPoint.y >= thumb.top) ||
            ((clickPoint.y >= fullThumb.top) &&
             ((thumb.top-clickPoint.y <= clickPoint.x-thumb.left) &&
              (thumb.top-clickPoint.y <= thumb.right-clickPoint.x)
             )
            )
           )
          ) ||
          (!(infoPtr->dwStyle & (TBS_BOTH | TBS_TOP)) &&
           (clickPoint.y >= thumb.top) &&
           ((clickPoint.y <= thumb.bottom) ||
            ((clickPoint.y <= fullThumb.bottom) &&
             ((clickPoint.y-thumb.bottom <= clickPoint.x-thumb.left) &&
              (clickPoint.y-thumb.bottom <= thumb.right-clickPoint.x)
             )
            )
           )
         ))
       ))
       {
         infoPtr->flags |= TB_DRAG_MODE;
         if (infoPtr->dwStyle & TBS_TOOLTIPS)
         {  /* enable tooltip */
           TTTOOLINFOA ti;
           POINT pt;
           char buf[80];

           ti.cbSize   = sizeof(TTTOOLINFOA);
           ti.uId      = 0;
           ti.hwnd     = (UINT)infoPtr->hwnd;
           ti.hinst    = 0;
           sprintf (buf,"%ld",infoPtr->nPos);
           ti.lpszText = (LPSTR)buf;

           infoPtr->flags |= TB_SHOW_TOOLTIP;
           SetCapture(infoPtr->hwnd);

           SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTA,0,(LPARAM)&ti);
           TRACKBAR_CalcToolTipPos(infoPtr,&pt);
           SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));

           SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);
         }

         SetCapture(infoPtr->hwnd);
         TRACKBAR_UpdateThumbPosition(infoPtr,infoPtr->nPos,TRUE); //change arrow color
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
              SetCapture(infoPtr->hwnd);
              SetTimer(infoPtr->hwnd,SCROLL_TIMER_ID,SCROLL_TIME,NULL);

              return 0;
            }

    clickPos = TRACKBAR_ConvertPlaceToPosition(infoPtr,clickPlace,vertical);
    prevPos = infoPtr->nPos;

    if (clickPos > prevPos)
    {  /* similar to VK_NEXT */
      infoPtr->nPos += infoPtr->nPageSize;
      if (infoPtr->nPos > infoPtr->nRangeMax) infoPtr->nPos = infoPtr->nRangeMax;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(infoPtr,TB_PAGEUP);
    } else
    {  /* similar to VK_PRIOR */
      infoPtr->nPos -= infoPtr->nPageSize;
      if (infoPtr->nPos < infoPtr->nRangeMin) infoPtr->nPos = infoPtr->nRangeMin;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(infoPtr,TB_PAGEDOWN);
    }

    if (prevPos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(infoPtr,prevPos,TRUE);
    }

    //ScrollMode
    infoPtr->flags |= TB_SCROLL_MODE;
    SetCapture(infoPtr->hwnd);
    SetTimer(infoPtr->hwnd,SCROLL_TIMER_ID,SCROLL_TIME,NULL);

    return 0;
}


static LRESULT
TRACKBAR_LButtonUp(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    if (infoPtr->flags & TB_DRAG_MODE)
    {
      TRACKBAR_SendNotify(infoPtr,TB_ENDTRACK);

      infoPtr->flags &= ~TB_DRAG_MODE;

      if (GetCapture() == infoPtr->hwnd)
      {
        sendNotify(infoPtr->hwnd,NM_RELEASEDCAPTURE);

        ReleaseCapture();
      }

      TRACKBAR_UpdateThumbPosition(infoPtr,infoPtr->nPos,TRUE); //change arrow color
    }

    if (infoPtr->flags & TB_SCROLL_MODE)
    {
      infoPtr->flags &= ~TB_SCROLL_MODE;

      if (GetCapture() == infoPtr->hwnd)
      {
        sendNotify(infoPtr->hwnd,NM_RELEASEDCAPTURE);

        ReleaseCapture();
      }

      KillTimer(infoPtr->hwnd,SCROLL_TIMER_ID);
    }

    if (infoPtr->dwStyle & TBS_TOOLTIPS)
    { /* disable tooltip */
      TTTOOLINFOA ti;

      ti.cbSize   = sizeof(TTTOOLINFOA);
      ti.uId      = 0;
      ti.hwnd     = (UINT)infoPtr->hwnd;

      infoPtr->flags &= ~TB_SHOW_TOOLTIP;
      SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&ti);
    }

    return 0;
}

static LRESULT TRACKBAR_Timer(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  POINT mousePoint;
  INT mousePlace,prevPos,newPos,vertical;
  DOUBLE mousePos;

  GetCursorPos(&mousePoint);
  ScreenToClient(infoPtr->hwnd,&mousePoint);

  vertical = infoPtr->dwStyle & TBS_VERT;
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
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(infoPtr,TB_PAGEUP);
    } else
    {  /* similar to VK_PRIOR */
      infoPtr->nPos -= infoPtr->nPageSize;
      if (infoPtr->nPos < infoPtr->nRangeMin) infoPtr->nPos = infoPtr->nRangeMin;
      if (prevPos != infoPtr->nPos) TRACKBAR_SendNotify(infoPtr,TB_PAGEDOWN);
    }

    if (prevPos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(infoPtr,prevPos,FALSE);
    }

  return 0;
}

static LRESULT
TRACKBAR_CaptureChanged(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    sendNotify(infoPtr->hwnd,NM_RELEASEDCAPTURE);

    if (infoPtr->flags & TB_DRAGPOSVALID)
    {
      int lastPos = infoPtr->nPos;
      infoPtr->nPos = infoPtr->dragPos;
      if (lastPos != infoPtr->nPos) TRACKBAR_UpdateThumbPosition(infoPtr,lastPos,TRUE);
    }

    infoPtr->flags &= ~TB_DRAGPOSVALID;

    if (infoPtr->flags & TB_SCROLL_MODE)
    {
      infoPtr->flags &= ~TB_SCROLL_MODE;
      KillTimer(infoPtr->hwnd,SCROLL_TIMER_ID);
    }

    if (infoPtr->flags & TB_DRAG_MODE)
    {
      TRACKBAR_SendNotify(infoPtr,TB_ENDTRACK);

      infoPtr->flags &= ~TB_DRAG_MODE;
    }

    return 0;
}


static LRESULT
TRACKBAR_Paint(TRACKBAR_INFO *infoPtr,WPARAM wParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = wParam == 0 ? BeginPaint(infoPtr->hwnd,&ps):(HDC)wParam;
    TRACKBAR_Draw(infoPtr,hdc);
    if (!wParam) EndPaint(infoPtr->hwnd,&ps);
    return 0;
}


static LRESULT
TRACKBAR_SetFocus(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    HDC hdc;
    RECT rcClient;

    if (!infoPtr->bFocus && IsWindowEnabled(infoPtr->hwnd))
    {
      infoPtr->bFocus = TRUE;

      GetClientRect(infoPtr->hwnd,&rcClient);
      hdc = GetDC (infoPtr->hwnd);
      DrawFocusRect(hdc,&rcClient);
      ReleaseDC(infoPtr->hwnd,hdc);
    }
    return 0;
}

static LRESULT
TRACKBAR_KillFocus(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    HDC hdc;
    RECT rcClient;


    infoPtr->flags &= ~TB_DRAG_MODE;
    if (infoPtr->bFocus && IsWindowEnabled(infoPtr->hwnd))
    {
      infoPtr->bFocus = FALSE;

      GetClientRect(infoPtr->hwnd,&rcClient);
      hdc = GetDC(infoPtr->hwnd);
      DrawFocusRect(hdc,&rcClient); //XOR removes
      ReleaseDC(infoPtr->hwnd,hdc);
    }

    return 0;
}

static LRESULT
TRACKBAR_Size(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  if (!(infoPtr->dwStyle & TBS_FIXEDLENGTH))
  {
    /* If trackbar doesn't have TBS_FIXEDLENGTH style
     * it sets thumb length according to new width / height.
     */
    LONG h;
    LONG hmin = (infoPtr->dwStyle & TBS_ENABLESELRANGE) ? THUMB_LEN_SEL:THUMB_LEN;

    if (infoPtr->dwStyle & TBS_VERT)
      h = (LONG)(INT16)LOWORD(lParam);  /* width */
    else
      h = (LONG)(INT16)HIWORD(lParam);  /* height */

    if (h < hmin)
    {
      /* focus rect + tic size + thumb offset */

       h -= (BORDER_SIZE+SCALE_SPACE)*2;
    }

    h = MAX(h,THUMB_MINLEN);
    h = MIN(h,hmin);

    infoPtr->uThumbLen = h;
  }

  TRACKBAR_CalcChannel(infoPtr);
  TRACKBAR_AlignBuddies(infoPtr);

  return 0;
}

static BOOL TRACKBAR_SendNotify(TRACKBAR_INFO *infoPtr,UINT code)
{
  if (infoPtr->dwStyle & TBS_VERT)
    return sendVScroll(infoPtr->hwnd,code);
  else
    return sendHScroll(infoPtr->hwnd,code);
}

static LRESULT
TRACKBAR_MouseMove(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    SHORT clickPlace;
    DOUBLE dragPos;

    if (!(infoPtr->flags & TB_DRAG_MODE)) return TRUE;

    if (infoPtr->dwStyle & TBS_VERT) clickPlace = (SHORT)HIWORD(lParam);
    else clickPlace = (SHORT)LOWORD(lParam);

    dragPos = TRACKBAR_ConvertPlaceToPosition(infoPtr,clickPlace,infoPtr->dwStyle & TBS_VERT);
    if (dragPos > ((INT)dragPos)+0.5) infoPtr->dragPos = dragPos + 1;
    else infoPtr->dragPos = dragPos;

    if (infoPtr->nPos == infoPtr->dragPos) return TRUE; //nothing changed

    infoPtr->flags |= TB_DRAGPOSVALID;

    TRACKBAR_UpdateThumbPosition(infoPtr,infoPtr->nPos,FALSE); //infoPtr->nPos now set

    TRACKBAR_SendNotify(infoPtr,TB_THUMBTRACK | (infoPtr->nPos << 16));

    if (infoPtr->flags & TB_SHOW_TOOLTIP)
    {
      POINT pt;
      TTTOOLINFOA ti;
      char buf[80];

      ti.cbSize = sizeof(TTTOOLINFOA);
      ti.hwnd = infoPtr->hwnd;
      ti.uId = 0;
      ti.hinst = 0;
      sprintf(buf,"%d",infoPtr->nPos);
      ti.lpszText = (LPSTR)buf;

      SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTA,0,(LPARAM)&ti);
      TRACKBAR_CalcToolTipPos(infoPtr,&pt);
      SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));
    }

    return 0;
}

static LRESULT
TRACKBAR_Enable(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  BOOL oldFocus = infoPtr->bFocus;

  if (wParam) infoPtr->bFocus = (GetFocus() == infoPtr->hwnd);
  else infoPtr->bFocus = FALSE;

  if (oldFocus != infoPtr->bFocus) TRACKBAR_Refresh(infoPtr);
  else TRACKBAR_UpdateThumbPosition(infoPtr,infoPtr->nPos,TRUE);

  return 0;
}

static LRESULT
TRACKBAR_KeyDown(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
    INT pos;

    if (infoPtr->flags & TB_DRAG_MODE) return TRUE;

    pos = infoPtr->nPos;
    switch (wParam) {
    case VK_LEFT:
    case VK_UP:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos -= infoPtr->nLineSize;
        if (infoPtr->nPos < infoPtr->nRangeMin)
            infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify(infoPtr,TB_LINEUP);
        break;
    case VK_RIGHT:
    case VK_DOWN:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos += infoPtr->nLineSize;
        if (infoPtr->nPos > infoPtr->nRangeMax)
            infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify(infoPtr,TB_LINEDOWN);
        break;
    case VK_NEXT:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos += infoPtr->nPageSize;
        if (infoPtr->nPos > infoPtr->nRangeMax)
            infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify(infoPtr,TB_PAGEUP);
        break;
    case VK_PRIOR:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos -= infoPtr->nPageSize;
        if (infoPtr->nPos < infoPtr->nRangeMin)
            infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify(infoPtr,TB_PAGEDOWN);
        break;
    case VK_HOME:
        if (infoPtr->nPos == infoPtr->nRangeMin) return FALSE;
        infoPtr->nPos = infoPtr->nRangeMin;
        TRACKBAR_SendNotify(infoPtr,TB_TOP);
        break;
    case VK_END:
        if (infoPtr->nPos == infoPtr->nRangeMax) return FALSE;
        infoPtr->nPos = infoPtr->nRangeMax;
        TRACKBAR_SendNotify(infoPtr,TB_BOTTOM);
        break;

      default:
        return FALSE;
    }

    if (pos != infoPtr->nPos)
    {
      infoPtr->flags |= TB_THUMBPOSCHANGED;
      TRACKBAR_UpdateThumbPosition(infoPtr,pos,FALSE);
    }

    return TRUE;
}


static LRESULT
TRACKBAR_KeyUp(TRACKBAR_INFO *infoPtr,WPARAM wParam)
{
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
        TRACKBAR_SendNotify(infoPtr,TB_ENDTRACK);
    }
    return TRUE;
}

static LRESULT
TRACKBAR_StyleChanged(TRACKBAR_INFO *infoPtr,WPARAM wParam,LPSTYLESTRUCT lParam)
{
  TRACKBAR_CalcChannel(infoPtr);

  return 0;
}

static LRESULT WINAPI
TRACKBAR_WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  TRACKBAR_INFO *infoPtr;

  if (uMsg == WM_CREATE)
    return TRACKBAR_Create(hwnd,wParam,lParam);

  infoPtr = TRACKBAR_GetInfoPtr(hwnd);

  if (!infoPtr)
    goto defproc;

  switch (uMsg)
  {
    case TBM_CLEARSEL:
        return TRACKBAR_ClearSel(infoPtr,wParam,lParam);

    case TBM_CLEARTICS:
        return TRACKBAR_ClearTics(infoPtr,wParam,lParam);

    case TBM_GETBUDDY:
        return TRACKBAR_GetBuddy(infoPtr,wParam,lParam);

    case TBM_GETCHANNELRECT:
        return TRACKBAR_GetChannelRect(infoPtr,wParam,lParam);

    case TBM_GETLINESIZE:
        return TRACKBAR_GetLineSize(infoPtr,wParam,lParam);

    case TBM_GETNUMTICS:
        return TRACKBAR_GetNumTics(infoPtr,wParam,lParam);

    case TBM_GETPAGESIZE:
        return TRACKBAR_GetPageSize(infoPtr,wParam,lParam);

    case TBM_GETPOS:
        return TRACKBAR_GetPos(infoPtr,wParam,lParam);

    case TBM_GETPTICS:
        return TRACKBAR_GetPTics(infoPtr);

    case TBM_GETRANGEMAX:
        return TRACKBAR_GetRangeMax(infoPtr,wParam,lParam);

    case TBM_GETRANGEMIN:
        return TRACKBAR_GetRangeMin(infoPtr,wParam,lParam);

    case TBM_GETSELEND:
        return TRACKBAR_GetSelEnd(infoPtr,wParam,lParam);

    case TBM_GETSELSTART:
        return TRACKBAR_GetSelStart(infoPtr,wParam,lParam);

    case TBM_GETTHUMBLENGTH:
        return TRACKBAR_GetThumbLength(infoPtr,wParam,lParam);

    case TBM_GETTHUMBRECT:
        return TRACKBAR_GetThumbRect(infoPtr,wParam,lParam);

    case TBM_GETTIC:
        return TRACKBAR_GetTic(infoPtr,wParam,lParam);

    case TBM_GETTICPOS:
        return TRACKBAR_GetTicPos(infoPtr,wParam,lParam);

    case TBM_GETTOOLTIPS:
        return TRACKBAR_GetToolTips(infoPtr,wParam,lParam);

    case TBM_SETBUDDY:
        return TRACKBAR_SetBuddy(infoPtr,wParam,lParam);

    case TBM_SETLINESIZE:
        return TRACKBAR_SetLineSize(infoPtr,wParam,lParam);

    case TBM_SETPAGESIZE:
        return TRACKBAR_SetPageSize(infoPtr,wParam,lParam);

    case TBM_SETPOS:
        return TRACKBAR_SetPos(infoPtr,wParam,lParam);

    case TBM_SETRANGE:
        return TRACKBAR_SetRange(infoPtr,wParam,lParam);

    case TBM_SETRANGEMAX:
        return TRACKBAR_SetRangeMax(infoPtr,wParam,lParam);

    case TBM_SETRANGEMIN:
        return TRACKBAR_SetRangeMin(infoPtr,wParam,lParam);

    case TBM_SETSEL:
        return TRACKBAR_SetSel(infoPtr,wParam,lParam);

    case TBM_SETSELEND:
        return TRACKBAR_SetSelEnd(infoPtr,wParam,lParam);

    case TBM_SETSELSTART:
        return TRACKBAR_SetSelStart(infoPtr,wParam,lParam);

    case TBM_SETTHUMBLENGTH:
        return TRACKBAR_SetThumbLength(infoPtr,wParam,lParam);

    case TBM_SETTIC:
        return TRACKBAR_SetTic(infoPtr,wParam,lParam);

    case TBM_SETTICFREQ:
        return TRACKBAR_SetTicFreq(infoPtr,wParam);

    case TBM_SETTIPSIDE:
        return TRACKBAR_SetTipSide(infoPtr,wParam,lParam);

    case TBM_SETTOOLTIPS:
        return TRACKBAR_SetToolTips(infoPtr,wParam,lParam);

    case WM_CAPTURECHANGED:
        return TRACKBAR_CaptureChanged(infoPtr,wParam,lParam);

    case WM_DESTROY:
        return TRACKBAR_Destroy(infoPtr,wParam,lParam);

    case WM_ENABLE:
        return TRACKBAR_Enable(infoPtr,wParam,lParam);

    case WM_ERASEBKGND:
        return 1;

    case WM_GETDLGCODE:
        return DLGC_WANTARROWS;

    case WM_KEYDOWN:
        return TRACKBAR_KeyDown(infoPtr,wParam,lParam);

    case WM_KEYUP:
        return TRACKBAR_KeyUp(infoPtr,wParam);

    case WM_LBUTTONDOWN:
        return TRACKBAR_LButtonDown(infoPtr,wParam,lParam);

    case WM_LBUTTONUP:
        return TRACKBAR_LButtonUp(infoPtr,wParam,lParam);

    case WM_TIMER:
        return TRACKBAR_Timer(infoPtr,wParam,lParam);

    case WM_MOUSEMOVE:
        return TRACKBAR_MouseMove(infoPtr,wParam,lParam);

    case WM_PAINT:
        return TRACKBAR_Paint(infoPtr,wParam);

    case WM_SETFOCUS:
        return TRACKBAR_SetFocus(infoPtr,wParam,lParam);

    case WM_KILLFOCUS:
        return TRACKBAR_KillFocus(infoPtr,wParam,lParam);

    case WM_STYLECHANGED:
        return TRACKBAR_StyleChanged(infoPtr,wParam,(LPSTYLESTRUCT)lParam);

    case WM_SIZE:
        return TRACKBAR_Size(infoPtr,wParam,lParam);

    case WM_WININICHANGE:
        return TRACKBAR_InitializeThumb(infoPtr);

    case WM_SYSCOLORCHANGE:
        return TRACKBAR_InitColors(infoPtr,TRUE);

    default:
        //if (uMsg >= WM_USER)
        //    ERR (trackbar, "unknown msg %04x wp=%08x lp=%08lx\n",
        //         uMsg, wParam, lParam);
 defproc:
        return defComCtl32ProcA(hwnd,uMsg,wParam,lParam);
    }
    return 0;
}


VOID
TRACKBAR_Register (VOID)
{
  WNDCLASSA wndClass;

  ZeroMemory (&wndClass, sizeof(WNDCLASSA));
  wndClass.style         = CS_GLOBALCLASS;
  wndClass.lpfnWndProc   = (WNDPROC)TRACKBAR_WindowProc;
  wndClass.cbClsExtra    = 0;
  wndClass.cbWndExtra    = sizeof(TRACKBAR_INFO *);
  wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
  wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
  wndClass.lpszClassName = TRACKBAR_CLASSA;

  RegisterClassA(&wndClass);
}


VOID
TRACKBAR_Unregister (VOID)
{
  UnregisterClassA (TRACKBAR_CLASSA, (HINSTANCE)NULL);
}

