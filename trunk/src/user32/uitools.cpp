/* $Id: uitools.cpp,v 1.28 2001-01-11 18:04:19 sandervl Exp $ */
/*
 * User Interface Functions
 *
 * Copyright 1997 Dimitrie O. Paun
 * Copyright 1997 Bertho A. Stultiens
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 * Copyright 1999 Rene Pronk
 *
 * WINE version: 990923
 */

#include "winuser.h"
#include "user32.h"
#include "win32wbase.h"
#include "syscolor.h"

#define DBG_LOCALLOG    DBG_uitools
#include "dbglocal.h"

/* These tables are used in:
 * UITOOLS_DrawDiagEdge()
 * UITOOLS_DrawRectEdge()
 */
static const char LTInnerNormal[] = {
    -1,           -1,                 -1,                 -1,
    -1,           COLOR_BTNHIGHLIGHT, COLOR_BTNHIGHLIGHT, -1,
    -1,           COLOR_3DDKSHADOW,   COLOR_3DDKSHADOW,   -1,
    -1,           -1,                 -1,                 -1
};

static const char LTOuterNormal[] = {
    -1,                 COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    COLOR_BTNHIGHLIGHT, COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    COLOR_3DDKSHADOW,   COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    -1,                 COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1
};

static const char RBInnerNormal[] = {
    -1,           -1,                -1,              -1,
    -1,           COLOR_BTNSHADOW,   COLOR_BTNSHADOW, -1,
    -1,           COLOR_3DLIGHT,     COLOR_3DLIGHT,   -1,
    -1,           -1,                -1,              -1
};

static const char RBOuterNormal[] = {
    -1,              COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    COLOR_BTNSHADOW, COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    COLOR_3DLIGHT,   COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    -1,              COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1
};

static const char LTInnerSoft[] = {
    -1,                  -1,                -1,              -1,
    -1,                  COLOR_3DLIGHT,     COLOR_3DLIGHT,   -1,
    -1,                  COLOR_BTNSHADOW,   COLOR_BTNSHADOW, -1,
    -1,                  -1,                -1,              -1
};

static const char LTOuterSoft[] = {
    -1,              COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    COLOR_3DLIGHT,   COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    COLOR_BTNSHADOW, COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    -1,              COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1
};

#define RBInnerSoft RBInnerNormal   /* These are the same */
#define RBOuterSoft RBOuterNormal

static const char LTRBOuterMono[] = {
    -1,           COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
};

static const char LTRBInnerMono[] = {
    -1, -1,           -1,           -1,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
};

static const char LTRBOuterFlat[] = {
    -1,                COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_WINDOWFRAME, COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_WINDOWFRAME, COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_WINDOWFRAME, COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
};

static const char LTRBInnerFlat[] = {
    -1, -1,              -1,              -1,
    -1, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    -1, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    -1, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
};

/***********************************************************************
 *           UITOOLS_DrawDiagEdge
 *
 * Same as DrawEdge invoked with BF_DIAGONAL
 *
 * 03-Dec-1997: Changed by Bertho Stultiens
 *
 * See also comments with UITOOLS_DrawRectEdge()
 */
static BOOL UITOOLS95_DrawDiagEdge(HDC hdc, LPRECT rc,
                                     UINT uType, UINT uFlags)
{
    POINT Points[4];
    char InnerI, OuterI;
    HPEN InnerPen, OuterPen;
    POINT SavePoint;
    HPEN SavePen;
    int spx, spy;
    int epx, epy;
    int Width = rc->right - rc->left;
    int Height= rc->bottom - rc->top;
    int SmallDiam = Width > Height ? Height : Width;
    BOOL retval = !(   ((uType & BDR_INNER) == BDR_INNER
                       || (uType & BDR_OUTER) == BDR_OUTER)
                      && !(uFlags & (BF_FLAT|BF_MONO)) );
    int add = (LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0)
            + (LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0);

    if (IsRectEmpty(rc)) return retval; //nothing to do

    /* Init some vars */
    OuterPen = InnerPen = (HPEN)GetStockObject(NULL_PEN);
    SavePen = (HPEN)SelectObject(hdc, InnerPen);
    spx = spy = epx = epy = 0; /* Satisfy the compiler... */

    /* Determine the colors of the edges */
    if(uFlags & BF_MONO)
    {
        InnerI = LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)];
        OuterI = LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_FLAT)
    {
        InnerI = LTRBInnerFlat[uType & (BDR_INNER|BDR_OUTER)];
        OuterI = LTRBOuterFlat[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_SOFT)
    {
        if(uFlags & BF_BOTTOM)
        {
            InnerI = RBInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = RBOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        }
        else
        {
            InnerI = LTInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = LTOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        }
    }
    else
    {
        if(uFlags & BF_BOTTOM)
        {
            InnerI = RBInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = RBOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        }
        else
        {
            InnerI = LTInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = LTOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        }
    }

    if(InnerI != -1) InnerPen = GetSysColorPen(InnerI);
    if(OuterI != -1) OuterPen = GetSysColorPen(OuterI);

    MoveToEx(hdc, 0, 0, &SavePoint);

    /* Don't ask me why, but this is what is visible... */
    /* This must be possible to do much simpler, but I fail to */
    /* see the logic in the MS implementation (sigh...). */
    /* So, this might look a bit brute force here (and it is), but */
    /* it gets the job done;) */

    switch(uFlags & BF_RECT)
    {
    case 0:
    case BF_LEFT:
    case BF_BOTTOM:
    case BF_BOTTOMLEFT:
        /* Left bottom endpoint */
        epx = rc->left-1;
        spx = epx + SmallDiam;
        epy = rc->bottom;
        spy = epy - SmallDiam;
        break;

    case BF_TOPLEFT:
    case BF_BOTTOMRIGHT:
        /* Left top endpoint */
        epx = rc->left-1;
        spx = epx + SmallDiam;
        epy = rc->top-1;
        spy = epy + SmallDiam;
        break;

    case BF_TOP:
    case BF_RIGHT:
    case BF_TOPRIGHT:
    case BF_RIGHT|BF_LEFT:
    case BF_RIGHT|BF_LEFT|BF_TOP:
    case BF_BOTTOM|BF_TOP:
    case BF_BOTTOM|BF_TOP|BF_LEFT:
    case BF_BOTTOMRIGHT|BF_LEFT:
    case BF_BOTTOMRIGHT|BF_TOP:
    case BF_RECT:
        /* Right top endpoint */
        spx = rc->left;
        epx = spx + SmallDiam;
        spy = rc->bottom-1;
        epy = spy - SmallDiam;
        break;
    }

    MoveToEx(hdc, spx, spy, NULL);
    SelectObject(hdc, OuterPen);
    LineTo(hdc, epx, epy);

    SelectObject(hdc, InnerPen);

    switch(uFlags & (BF_RECT|BF_DIAGONAL))
    {
    case BF_DIAGONAL_ENDBOTTOMLEFT:
    case (BF_DIAGONAL|BF_BOTTOM):
    case BF_DIAGONAL:
    case (BF_DIAGONAL|BF_LEFT):
        MoveToEx(hdc, spx-1, spy, NULL);
        LineTo(hdc, epx, epy-1);
        Points[0].x = spx-add;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->top;
        Points[2].x = epx+1;
        Points[2].y = epy-1-add;
        Points[3] = Points[2];
        break;

    case BF_DIAGONAL_ENDBOTTOMRIGHT:
        MoveToEx(hdc, spx-1, spy, NULL);
        LineTo(hdc, epx, epy+1);
        Points[0].x = spx-add;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->bottom-1;
        Points[2].x = epx+1;
        Points[2].y = epy+1+add;
        Points[3] = Points[2];
        break;

    case (BF_DIAGONAL|BF_BOTTOM|BF_RIGHT|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_RIGHT|BF_TOP|BF_LEFT):
    case BF_DIAGONAL_ENDTOPRIGHT:
    case (BF_DIAGONAL|BF_RIGHT|BF_TOP|BF_LEFT):
        MoveToEx(hdc, spx+1, spy, NULL);
        LineTo(hdc, epx, epy+1);
        Points[0].x = epx-1;
        Points[0].y = epy+1+add;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top+add;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1;
        Points[3].x = spx+add;
        Points[3].y = spy;
        break;

    case BF_DIAGONAL_ENDTOPLEFT:
        MoveToEx(hdc, spx, spy-1, NULL);
        LineTo(hdc, epx+1, epy);
        Points[0].x = epx+1+add;
        Points[0].y = epy+1;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1-add;
        Points[3].x = spx;
        Points[3].y = spy-add;
        break;

    case (BF_DIAGONAL|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_TOP|BF_LEFT):
        MoveToEx(hdc, spx+1, spy-1, NULL);
        LineTo(hdc, epx, epy);
        Points[0].x = epx-1;
        Points[0].y = epy+1;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1-add;
        Points[3].x = spx+add;
        Points[3].y = spy-add;
        break;

    case (BF_DIAGONAL|BF_RIGHT):
    case (BF_DIAGONAL|BF_RIGHT|BF_LEFT):
    case (BF_DIAGONAL|BF_RIGHT|BF_LEFT|BF_BOTTOM):
        MoveToEx(hdc, spx, spy, NULL);
        LineTo(hdc, epx-1, epy+1);
        Points[0].x = spx;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->top+add;
        Points[2].x = epx-1-add;
        Points[2].y = epy+1+add;
        Points[3] = Points[2];
        break;
    }

    /* Fill the interior if asked */
    if((uFlags & BF_MIDDLE) && retval)
    {
        HBRUSH hbsave;
        HBRUSH hb = GetSysColorBrush(uFlags & BF_MONO ? COLOR_WINDOW
                                         :COLOR_BTNFACE);
        HPEN hpsave;
        HPEN hp = GetSysColorPen(uFlags & BF_MONO ? COLOR_WINDOW
                                     : COLOR_BTNFACE);
        hbsave = (HBRUSH)SelectObject(hdc, hb);
        hpsave = (HPEN)SelectObject(hdc, hp);
        Polygon(hdc, Points, 4);
        SelectObject(hdc, hbsave);
        SelectObject(hdc, hpsave);
    }

    /* Adjust rectangle if asked */
    if(uFlags & BF_ADJUST)
    {
        if(uFlags & BF_LEFT)   rc->left   += add;
        if(uFlags & BF_RIGHT)  rc->right  -= add;
        if(uFlags & BF_TOP)    rc->top    += add;
        if(uFlags & BF_BOTTOM) rc->bottom -= add;
    }

    /* Cleanup */
    SelectObject(hdc, SavePen);
    MoveToEx(hdc, SavePoint.x, SavePoint.y, NULL);

    return retval;
}

/***********************************************************************
 *           UITOOLS_DrawRectEdge
 *
 * Same as DrawEdge invoked without BF_DIAGONAL
 *
 * 23-Nov-1997: Changed by Bertho Stultiens
 *
 * Well, I started testing this and found out that there are a few things
 * that weren't quite as win95. The following rewrite should reproduce
 * win95 results completely.
 * The colorselection is table-driven to avoid awfull if-statements.
 * The table below show the color settings.
 *
 * Pen selection table for uFlags = 0
 *
 * uType |  LTI  |  LTO  |  RBI  |  RBO
 * ------+-------+-------+-------+-------
 *  0000 |   x   |   x   |   x   |   x
 *  0001 |   x   |  22   |   x   |  21
 *  0010 |   x   |  16   |   x   |  20
 *  0011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  0100 |   x   |  20   |   x   |  16
 *  0101 |  20   |  22   |  16   |  21
 *  0110 |  20   |  16   |  16   |  20
 *  0111 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1000 |   x   |  21   |   x   |  22
 *  1001 |  21   |  22   |  22   |  21
 *  1010 |  21   |  16   |  22   |  20
 *  1011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1100 |   x   |   x   |   x   |   x
 *  1101 |   x   | x (22)|   x   | x (21)
 *  1110 |   x   | x (16)|   x   | x (20)
 *  1111 |   x   |   x   |   x   |   x
 *
 * Pen selection table for uFlags = BF_SOFT
 *
 * uType |  LTI  |  LTO  |  RBI  |  RBO
 * ------+-------+-------+-------+-------
 *  0000 |   x   |   x   |   x   |   x
 *  0001 |   x   |  20   |   x   |  21
 *  0010 |   x   |  21   |   x   |  20
 *  0011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  0100 |   x   |  22   |   x   |  16
 *  0101 |  22   |  20   |  16   |  21
 *  0110 |  22   |  21   |  16   |  20
 *  0111 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1000 |   x   |  16   |   x   |  22
 *  1001 |  16   |  20   |  22   |  21
 *  1010 |  16   |  21   |  22   |  20
 *  1011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1100 |   x   |   x   |   x   |   x
 *  1101 |   x   | x (20)|   x   | x (21)
 *  1110 |   x   | x (21)|   x   | x (20)
 *  1111 |   x   |   x   |   x   |   x
 *
 * x = don't care; (n) = is what win95 actually uses
 * LTI = left Top Inner line
 * LTO = left Top Outer line
 * RBI = Right Bottom Inner line
 * RBO = Right Bottom Outer line
 * 15 = COLOR_BTNFACE
 * 16 = COLOR_BTNSHADOW
 * 20 = COLOR_BTNHIGHLIGHT
 * 21 = COLOR_3DDKSHADOW
 * 22 = COLOR_3DLIGHT
 */


static BOOL UITOOLS95_DrawRectEdge(HDC hdc, LPRECT rc,
                                     UINT uType, UINT uFlags)
{
    char LTInnerI, LTOuterI;
    char RBInnerI, RBOuterI;
    HPEN LTInnerPen, LTOuterPen;
    HPEN RBInnerPen, RBOuterPen;
    RECT InnerRect = *rc;
    POINT SavePoint;
    HPEN SavePen;
    int LBpenplus = 0;
    int LTpenplus = 0;
    int RTpenplus = 0;
    int RBpenplus = 0;
    BOOL retval = !(   ((uType & BDR_INNER) == BDR_INNER
                       || (uType & BDR_OUTER) == BDR_OUTER)
                      && !(uFlags & (BF_FLAT|BF_MONO)) );

    if  (IsRectEmpty(rc)) return retval; //nothing to do

    /* Init some vars */
    LTInnerPen = LTOuterPen = RBInnerPen = RBOuterPen = (HPEN)GetStockObject(NULL_PEN);
    SavePen = (HPEN)SelectObject(hdc, LTInnerPen);

    /* Determine the colors of the edges */
    if(uFlags & BF_MONO)
    {
        LTInnerI = RBInnerI = LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = RBOuterI = LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_FLAT)
    {
        LTInnerI = RBInnerI = LTRBInnerFlat[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = RBOuterI = LTRBOuterFlat[uType & (BDR_INNER|BDR_OUTER)];

        /* Bertho Stultiens states above that this function exactly matches win95
         * In win98 BF_FLAT rectangels have an inner border same color as the
         * middle (COLOR_BTNFACE). I believe it's the same for win95 but since
         * I don't know I go with Bertho and just sets it for win98 until proven
         * otherwise.
         *                                          Dennis Björklund, 10 June, 99
         */
        if(LTInnerI != -1 )
            LTInnerI = RBInnerI = COLOR_BTNFACE;
    }
    else if(uFlags & BF_SOFT)
    {
        LTInnerI = LTInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = LTOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        RBInnerI = RBInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
        RBOuterI = RBOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
    }
    else
    {
        LTInnerI = LTInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = LTOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        RBInnerI = RBInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
        RBOuterI = RBOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
    }

    if((uFlags & BF_BOTTOMLEFT) == BF_BOTTOMLEFT)   LBpenplus = 1;
    if((uFlags & BF_TOPRIGHT) == BF_TOPRIGHT)       RTpenplus = 1;
    if((uFlags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT) RBpenplus = 1;
    if((uFlags & BF_TOPLEFT) == BF_TOPLEFT)         LTpenplus = 1;

    if(LTInnerI != -1) LTInnerPen = GetSysColorPen(LTInnerI);
    if(LTOuterI != -1) LTOuterPen = GetSysColorPen(LTOuterI);
    if(RBInnerI != -1) RBInnerPen = GetSysColorPen(RBInnerI);
    if(RBOuterI != -1) RBOuterPen = GetSysColorPen(RBOuterI);

    MoveToEx(hdc, 0, 0, &SavePoint);

    /* Draw the outer edge */
    SelectObject(hdc, LTOuterPen);
    if(uFlags & BF_TOP)
    {
        MoveToEx(hdc, InnerRect.left, InnerRect.top, NULL);
        LineTo(hdc, InnerRect.right, InnerRect.top);
    }
    if(uFlags & BF_LEFT)
    {
        MoveToEx(hdc, InnerRect.left, InnerRect.top, NULL);
        LineTo(hdc, InnerRect.left, InnerRect.bottom);
    }

    SelectObject(hdc, RBOuterPen);
    if(uFlags & BF_BOTTOM)
    {
        MoveToEx(hdc, InnerRect.right-1, InnerRect.bottom-1, NULL);
        LineTo(hdc, InnerRect.left-1, InnerRect.bottom-1);
    }
    if(uFlags & BF_RIGHT)
    {
        MoveToEx(hdc, InnerRect.right-1, InnerRect.bottom-1, NULL);
        LineTo(hdc, InnerRect.right-1, InnerRect.top-1);
    }

    /* Draw the inner edge */
    SelectObject(hdc, LTInnerPen);
    if(uFlags & BF_TOP)
    {
        MoveToEx(hdc, InnerRect.left+LTpenplus, InnerRect.top+1, NULL);
        LineTo(hdc, InnerRect.right-RTpenplus, InnerRect.top+1);
    }
    if(uFlags & BF_LEFT)
    {
        MoveToEx(hdc, InnerRect.left+1, InnerRect.top+LTpenplus, NULL);
        LineTo(hdc, InnerRect.left+1, InnerRect.bottom-LBpenplus);
    }
    SelectObject(hdc, RBInnerPen);
    if(uFlags & BF_BOTTOM)
    {
        MoveToEx(hdc, InnerRect.right-1-RBpenplus, InnerRect.bottom-2, NULL);
        LineTo(hdc, InnerRect.left-1+LBpenplus, InnerRect.bottom-2);
    }
    if(uFlags & BF_RIGHT)
    {
        MoveToEx(hdc, InnerRect.right-2, InnerRect.bottom-1-RBpenplus, NULL);
        LineTo(hdc, InnerRect.right-2, InnerRect.top-1+RTpenplus);
    }

    if( ((uFlags & BF_MIDDLE) && retval) || (uFlags & BF_ADJUST) )
    {
        int add = (LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0)
                + (LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0);

        if(uFlags & BF_LEFT)   InnerRect.left   += add;
        if(uFlags & BF_RIGHT)  InnerRect.right  -= add;
        if(uFlags & BF_TOP)    InnerRect.top    += add;
        if(uFlags & BF_BOTTOM) InnerRect.bottom -= add;

        if((uFlags & BF_MIDDLE) && retval)
        {
            FillRect(hdc, &InnerRect, GetSysColorBrush(uFlags & BF_MONO ?
                                                       COLOR_WINDOW : COLOR_BTNFACE));
        }

        if(uFlags & BF_ADJUST)
            *rc = InnerRect;
    }

    /* Cleanup */
    SelectObject(hdc, SavePen);
    MoveToEx(hdc, SavePoint.x, SavePoint.y, NULL);
    return retval;
}


//**********************************************************************
//          DrawEdge   (USER32.155)
//
BOOL WIN32API DrawEdge(HDC hdc, LPRECT rc, UINT edge, UINT flags)
{

    if (flags & BF_DIAGONAL)
      return UITOOLS95_DrawDiagEdge(hdc, rc, edge, flags);
    else
      return UITOOLS95_DrawRectEdge(hdc, rc, edge, flags);
}

/************************************************************************
 *      UITOOLS_MakeSquareRect
 *
 * Utility to create a square rectangle and returning the width
 */
static int UITOOLS_MakeSquareRect(LPRECT src, LPRECT dst)
{
    int Width  = src->right - src->left;
    int Height = src->bottom - src->top;
    int SmallDiam = Width > Height ? Height : Width;

    *dst = *src;

    /* Make it a square box */
    if(Width < Height)      /* SmallDiam == Width */
    {
        dst->top += (Height-Width)/2;
        dst->bottom = dst->top + SmallDiam;
    }
    else if(Width > Height) /* SmallDiam == Height */
    {
        dst->left += (Width-Height)/2;
        dst->right = dst->left + SmallDiam;
    }

   return SmallDiam;
}


static void UITOOLS_DrawCheckedRect( HDC dc, LPRECT rect )
{
    if(GetSysColor(COLOR_BTNHIGHLIGHT) == RGB(255, 255, 255))
    {
      COLORREF bg;
      HBRUSH hbsave;

      FillRect(dc, rect, GetSysColorBrush(COLOR_BTNFACE));
      bg = SetBkColor(dc, RGB(255, 255, 255));
      hbsave = (HBRUSH)SelectObject(dc,GetPattern55AABrush());
      PatBlt(dc, rect->left, rect->top, rect->right-rect->left, rect->bottom-rect->top, 0x00FA0089);
      SelectObject(dc, hbsave);
      SetBkColor(dc, bg);
    }
    else
    {
        FillRect(dc, rect, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
    }
}

/************************************************************************
 *      UITOOLS_DFC_ButtonPush
 *
 * Draw a push button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */
static BOOL UITOOLS95_DFC_ButtonPush(HDC dc, LPRECT r, UINT uFlags)
{
    UINT edge;
    RECT myr = *r;

    if(uFlags & (DFCS_PUSHED | DFCS_CHECKED | DFCS_FLAT))
        edge = EDGE_SUNKEN;
    else
        edge = EDGE_RAISED;

    if(uFlags & DFCS_CHECKED)
    {
        if(uFlags & DFCS_MONO)
            UITOOLS95_DrawRectEdge(dc, &myr, edge, BF_MONO|BF_RECT|BF_ADJUST);
        else
            UITOOLS95_DrawRectEdge(dc, &myr, edge, (uFlags&DFCS_FLAT)|BF_RECT|BF_SOFT|BF_ADJUST);

        UITOOLS_DrawCheckedRect( dc, &myr );
        }
        else
        {
        if(uFlags & DFCS_MONO)
        {
            UITOOLS95_DrawRectEdge(dc, &myr, edge, BF_MONO|BF_RECT|BF_ADJUST);
            FillRect(dc, &myr, GetSysColorBrush(COLOR_BTNFACE));
        }
        else
        {
            UITOOLS95_DrawRectEdge(dc, r, edge, (uFlags&DFCS_FLAT) | BF_MIDDLE |BF_SOFT| BF_RECT);
        }
    }

    /* Adjust rectangle if asked */
    if(uFlags & DFCS_ADJUSTRECT)
    {
        r->left   += 2;
        r->right  -= 2;
        r->top    += 2;
        r->bottom -= 2;
    }

    return TRUE;
}


/************************************************************************
 *      UITOOLS_DFC_ButtonCheck
 *
 * Draw a check/3state button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */
#define DFC_CHECKPOINTSMAX      6

static BOOL UITOOLS95_DFC_ButtonCheck(HDC dc, LPRECT r, UINT uFlags)
{
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr);
    UINT flags = BF_RECT | BF_ADJUST;

    if(uFlags & DFCS_FLAT) flags |= BF_FLAT;
    else if(uFlags & DFCS_MONO) flags |= BF_MONO;

    UITOOLS95_DrawRectEdge( dc, &myr, EDGE_SUNKEN, flags );

    if(uFlags & (DFCS_INACTIVE|DFCS_PUSHED))
        FillRect(dc, &myr, GetSysColorBrush(COLOR_BTNFACE));
    else if( (uFlags & DFCS_BUTTON3STATE) && (uFlags & DFCS_CHECKED) )
        UITOOLS_DrawCheckedRect( dc, &myr );
    else
    {
        FillRect(dc, &myr, GetSysColorBrush(COLOR_WINDOW));
    }

    if(uFlags & DFCS_CHECKED)
    {
        POINT CheckPoints[DFC_CHECKPOINTSMAX];
        int i;
        HBRUSH hbsave;
        HPEN hpsave;

        /* FIXME: This comes very close to M$'s checkmark, but not */
        /* exactly... When small or large there is a few pixels */
        /* shift. Not bad, but could be better :) */
        UITOOLS_MakeSquareRect(r, &myr);
        CheckPoints[0].x = myr.left + 253*SmallDiam/1000;
        CheckPoints[0].y = myr.top  + 345*SmallDiam/1000;
        CheckPoints[1].x = myr.left + 409*SmallDiam/1000;
        CheckPoints[1].y = CheckPoints[0].y + (CheckPoints[1].x-CheckPoints[0].x);
        CheckPoints[2].x = myr.left + 690*SmallDiam/1000;
        CheckPoints[2].y = CheckPoints[1].y - (CheckPoints[2].x-CheckPoints[1].x);
        CheckPoints[3].x = CheckPoints[2].x;
        CheckPoints[3].y = CheckPoints[2].y + 3*SmallDiam/16;
        CheckPoints[4].x = CheckPoints[1].x;
        CheckPoints[4].y = CheckPoints[1].y + 3*SmallDiam/16;
        CheckPoints[5].x = CheckPoints[0].x;
        CheckPoints[5].y = CheckPoints[0].y + 3*SmallDiam/16;

        i = (uFlags & DFCS_INACTIVE) || (uFlags & 0xff) == DFCS_BUTTON3STATE ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT;
        hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(i));
        hpsave = (HPEN)SelectObject(dc, GetSysColorPen(i));
        Polygon(dc, CheckPoints, DFC_CHECKPOINTSMAX);
        SelectObject(dc, hpsave);
        SelectObject(dc, hbsave);
    }
    return TRUE;
}


/************************************************************************
 *      UITOOLS_DFC_ButtonRadio
 *
 * Draw a radio/radioimage/radiomask button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */
static BOOL UITOOLS95_DFC_ButtonRadio(HDC dc, LPRECT r, UINT uFlags)
{
    RECT myr;
    int i;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr);
    int BorderShrink = SmallDiam / 16;
    HPEN hpsave;
    HBRUSH hbsave;
    int xe, ye;
    int xc, yc;

    if(BorderShrink < 1) BorderShrink = 1;

    if((uFlags & 0xff) == DFCS_BUTTONRADIOIMAGE)
    {
        FillRect(dc, r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }

    xe = myr.left;
    ye = myr.top  + SmallDiam - SmallDiam/2;

    xc = myr.left + SmallDiam - SmallDiam/2;
    yc = myr.top  + SmallDiam - SmallDiam/2;

    /* Define bounding box */
    i = (14*SmallDiam)/16;
    myr.left   = xc - i+i/2;
    myr.right  = xc + i/2;
    myr.top    = yc - i+i/2;
    myr.bottom = yc + i/2;

    if((uFlags & 0xff) == DFCS_BUTTONRADIOMASK)
    {
        hbsave = (HBRUSH)SelectObject(dc, GetStockObject(BLACK_BRUSH));
        Pie(dc, myr.left, myr.top, myr.right, myr.bottom, xe, ye, xe, ye);
        SelectObject(dc, hbsave);
    }
    else
    {
        if(uFlags & (DFCS_FLAT|DFCS_MONO))
        {
            hpsave = (HPEN)SelectObject(dc, GetSysColorPen(COLOR_WINDOWFRAME));
            hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(COLOR_WINDOWFRAME));
            Pie(dc, myr.left, myr.top, myr.right, myr.bottom, xe, ye, xe, ye);
            SelectObject(dc, hbsave);
            SelectObject(dc, hpsave);
        }
        else
        {
            hpsave = (HPEN)SelectObject(dc, GetSysColorPen(COLOR_BTNHIGHLIGHT));
            hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
            Pie(dc, myr.left, myr.top, myr.right, myr.bottom, myr.left-1, myr.bottom, myr.right-1, myr.top);

            SelectObject(dc, GetSysColorPen(COLOR_BTNSHADOW));
            SelectObject(dc, GetSysColorBrush(COLOR_BTNSHADOW));
            Pie(dc, myr.left, myr.top, myr.right, myr.bottom, myr.right+1, myr.top, myr.left+1, myr.bottom);

            myr.left   += BorderShrink;
            myr.right  -= BorderShrink;
            myr.top    += BorderShrink;
            myr.bottom -= BorderShrink;

            SelectObject(dc, GetSysColorPen(COLOR_3DLIGHT));
            SelectObject(dc, GetSysColorBrush(COLOR_3DLIGHT));
            Pie(dc, myr.left, myr.top, myr.right, myr.bottom, myr.left-1, myr.bottom, myr.right-1, myr.top);

            SelectObject(dc, GetSysColorPen(COLOR_3DDKSHADOW));
            SelectObject(dc, GetSysColorBrush(COLOR_3DDKSHADOW));
            Pie(dc, myr.left, myr.top, myr.right, myr.bottom, myr.right+1, myr.top, myr.left+1, myr.bottom);
            SelectObject(dc, hbsave);
            SelectObject(dc, hpsave);
        }

        i = (10*SmallDiam)/16,1;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;
        i= !(uFlags & (DFCS_INACTIVE|DFCS_PUSHED)) ? COLOR_WINDOW : COLOR_BTNFACE;
        hpsave = (HPEN)SelectObject(dc, GetSysColorPen(i));
        hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(i));
        Ellipse(dc,myr.left,myr.top,myr.right,myr.bottom);
        SelectObject(dc, hbsave);
        SelectObject(dc, hpsave);
    }

    if(uFlags & DFCS_CHECKED)
    {
        i = (6*SmallDiam)/16;
        i = i < 1 ? 1 : i;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;

        i = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT;
        hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(i));
        hpsave = (HPEN)SelectObject(dc, GetSysColorPen(i));
        Ellipse(dc,myr.left,myr.top,myr.right,myr.bottom);
        SelectObject(dc, hpsave);
        SelectObject(dc, hbsave);
    }

    /* FIXME: M$ has a polygon in the center at relative points: */
    /* 0.476, 0.476 (times SmallDiam, SmallDiam) */
    /* 0.476, 0.525 */
    /* 0.500, 0.500 */
    /* 0.500, 0.499 */
    /* when the button is unchecked. The reason for it is unknown. The */
    /* color is COLOR_BTNHIGHLIGHT, although the polygon gets painted at */
    /* least 3 times (it looks like a clip-region when you see it happen). */
    /* I do not really see a reason why this should be implemented. If you */
    /* have a good reason, let me know. Maybe this is a quirk in the Marlett */
    /* font. */

    return TRUE;
}

/***********************************************************************
 *           UITOOLS_DrawFrameButton
 */
static BOOL UITOOLS95_DrawFrameButton(HDC hdc, LPRECT rc, UINT uState)
{
    switch(uState & 0xff)
    {
    case DFCS_BUTTONPUSH:
        return UITOOLS95_DFC_ButtonPush(hdc, rc, uState);

    case DFCS_BUTTONCHECK:
    case DFCS_BUTTON3STATE:
        return UITOOLS95_DFC_ButtonCheck(hdc, rc, uState);

    case DFCS_BUTTONRADIOIMAGE:
    case DFCS_BUTTONRADIOMASK:
    case DFCS_BUTTONRADIO:
        return UITOOLS95_DFC_ButtonRadio(hdc, rc, uState);

//    default:
//        WARN("Invalid button state=0x%04x\n", uState);
    }

    return FALSE;
}

/***********************************************************************
 *           UITOOLS_DrawFrameCaption
 *
 * Draw caption buttons (win95), coming from DrawFrameControl()
 */

static BOOL UITOOLS95_DrawFrameCaption(HDC dc, LPRECT r, UINT uFlags)
{
    POINT Line1[10];
    POINT Line2[10];
    int Line1N;
    int Line2N;
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr)-2;
    int i;
    HBRUSH hbsave;
    HPEN hpsave;
    HFONT hfsave, hf;
    int xc = (myr.left+myr.right)/2;
    int yc = (myr.top+myr.bottom)/2;
    int edge, move;
    char str[2] = "?";
    UINT alignsave;
    int bksave;
    COLORREF clrsave;
    SIZE size;

    UITOOLS95_DFC_ButtonPush(dc, r, uFlags & 0xff00);

    switch(uFlags & 0xff)
    {
    case DFCS_CAPTIONCLOSE:
        edge = 328*SmallDiam/1000;
        move = 95*SmallDiam/1000;
        Line1[0].x = Line2[0].x = Line1[1].x = Line2[1].x = xc - edge;
        Line1[2].y = Line2[5].y = Line1[1].y = Line2[4].y = yc - edge;
        Line1[3].x = Line2[3].x = Line1[4].x = Line2[4].x = xc + edge;
        Line1[5].y = Line2[2].y = Line1[4].y = Line2[1].y = yc + edge;
        Line1[2].x = Line2[2].x = Line1[1].x + move;
        Line1[0].y = Line2[3].y = Line1[1].y + move;
        Line1[5].x = Line2[5].x = Line1[4].x - move;
        Line1[3].y = Line2[0].y = Line1[4].y - move;
        Line1N = 6;
        Line2N = 6;
        break;

    case DFCS_CAPTIONHELP:
        /* This one breaks the flow */
        /* FIXME: We need the Marlett font in order to get this right. */

        hf = CreateFontA(-SmallDiam, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, "System");
        alignsave = SetTextAlign(dc, TA_TOP|TA_LEFT);
        bksave = SetBkMode(dc, TRANSPARENT);
        clrsave = GetTextColor(dc);
        hfsave = (HFONT)SelectObject(dc, hf);
        GetTextExtentPoint32A(dc, str, 1, &size);

        if(uFlags & DFCS_INACTIVE)
        {
            SetTextColor(dc, GetSysColor(COLOR_BTNHIGHLIGHT));
            TextOutA(dc, xc-size.cx/2+1, yc-size.cy/2+1, str, 1);
        }
        SetTextColor(dc, GetSysColor(uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_BTNTEXT));
        TextOutA(dc, xc-size.cx/2, yc-size.cy/2, str, 1);

        SelectObject(dc, hfsave);
        SetTextColor(dc, clrsave);
        SetBkMode(dc, bksave);
        SetTextAlign(dc, alignsave);
        DeleteObject(hf);
        return TRUE;

    case DFCS_CAPTIONMIN:
        Line1[0].x = Line1[3].x = myr.left   +  96*SmallDiam/750+2;
        Line1[1].x = Line1[2].x = Line1[0].x + 372*SmallDiam/750;
        Line1[0].y = Line1[1].y = myr.top    + 563*SmallDiam/750+1;
        Line1[2].y = Line1[3].y = Line1[0].y +  92*SmallDiam/750;
        Line1N = 4;
        Line2N = 0;
        break;

    case DFCS_CAPTIONMAX:
        edge = 47*SmallDiam/750;
        Line1[0].x = Line1[5].x = myr.left +  57*SmallDiam/750+3;
        Line1[0].y = Line1[1].y = myr.top  + 143*SmallDiam/750+1;
        Line1[1].x = Line1[2].x = Line1[0].x + 562*SmallDiam/750;
        Line1[5].y = Line1[4].y = Line1[0].y +  93*SmallDiam/750;
        Line1[2].y = Line1[3].y = Line1[0].y + 513*SmallDiam/750;
        Line1[3].x = Line1[4].x = Line1[1].x -  edge;

        Line2[0].x = Line2[5].x = Line1[0].x;
        Line2[3].x = Line2[4].x = Line1[1].x;
        Line2[1].x = Line2[2].x = Line1[0].x + edge;
        Line2[0].y = Line2[1].y = Line1[0].y;
        Line2[4].y = Line2[5].y = Line1[2].y;
        Line2[2].y = Line2[3].y = Line1[2].y - edge;
        Line1N = 6;
        Line2N = 6;
        break;

    case DFCS_CAPTIONRESTORE:
        /* FIXME: this one looks bad at small sizes < 15x15 :( */
        edge = 47*SmallDiam/750;
        move = 420*SmallDiam/750;
        Line1[0].x = Line1[9].x = myr.left + 198*SmallDiam/750+2;
        Line1[0].y = Line1[1].y = myr.top  + 169*SmallDiam/750+1;
        Line1[6].y = Line1[7].y = Line1[0].y + 93*SmallDiam/750;
        Line1[7].x = Line1[8].x = Line1[0].x + edge;
        Line1[1].x = Line1[2].x = Line1[0].x + move;
        Line1[5].x = Line1[6].x = Line1[1].x - edge;
        Line1[9].y = Line1[8].y = Line1[0].y + 187*SmallDiam/750;
        Line1[2].y = Line1[3].y = Line1[0].y + 327*SmallDiam/750;
        Line1[4].y = Line1[5].y = Line1[2].y - edge;
        Line1[3].x = Line1[4].x = Line1[2].x - 140*SmallDiam/750;

        Line2[1].x = Line2[2].x = Line1[3].x;
        Line2[7].x = Line2[8].x = Line2[1].x - edge;
        Line2[0].x = Line2[9].x = Line2[3].x = Line2[4].x = Line2[1].x - move;
        Line2[5].x = Line2[6].x = Line2[0].x + edge;
        Line2[0].y = Line2[1].y = Line1[9].y;
        Line2[4].y = Line2[5].y = Line2[8].y = Line2[9].y = Line2[0].y + 93*SmallDiam/750;
        Line2[2].y = Line2[3].y = Line2[0].y + 327*SmallDiam/750;
        Line2[6].y = Line2[7].y = Line2[2].y - edge;
        Line1N = 10;
        Line2N = 10;
        break;

    default:
//        WARN("Invalid caption; flags=0x%04x\n", uFlags);
        return FALSE;
    }

    /* Here the drawing takes place */
    if(uFlags & DFCS_INACTIVE)
    {
        /* If we have an inactive button, then you see a shadow */
        hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
        hpsave = (HPEN)SelectObject(dc, GetSysColorPen(COLOR_BTNHIGHLIGHT));
        Polygon(dc, Line1, Line1N);
        if(Line2N > 0)
            Polygon(dc, Line2, Line2N);
        SelectObject(dc, hpsave);
        SelectObject(dc, hbsave);
    }

    /* Correct for the shadow shift */
    for(i = 0; i < Line1N; i++)
    {
        Line1[i].x--;
        Line1[i].y--;
    }
    for(i = 0; i < Line2N; i++)
    {
        Line2[i].x--;
        Line2[i].y--;
    }

    /* Make the final picture */
    i = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_BTNTEXT;
    hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(i));
    hpsave = (HPEN)SelectObject(dc, GetSysColorPen(i));

    Polygon(dc, Line1, Line1N);
    if(Line2N > 0)
        Polygon(dc, Line2, Line2N);
    SelectObject(dc, hpsave);
    SelectObject(dc, hbsave);

    return TRUE;
}


/************************************************************************
 *      UITOOLS_DrawFrameScroll
 *
 * Draw a scroll-bar control coming from DrawFrameControl()
 */
static BOOL UITOOLS95_DrawFrameScroll(HDC dc, LPRECT r, UINT uFlags)
{
    POINT Line[4];
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr) - 2;
    int i;
    HBRUSH hbsave, hb, hb2;
    HPEN hpsave, hp, hp2;
    int tri = 290*SmallDiam/1000 -1;
    int d46, d93;

    /*
     * This fixes a problem with really tiny "scroll" buttons. In particular
     * with the updown control.
     * Making sure that the arrow is as least 3 pixels wide (or high).
     */
    if (tri == 0)
      tri = 1;

    switch(uFlags & 0xff)
    {
    case DFCS_SCROLLCOMBOBOX:
    case DFCS_SCROLLDOWN:
        Line[2].x = myr.left + 470*SmallDiam/1000 + 2;
        Line[2].y = myr.top  + 687*SmallDiam/1000 + 1;
        Line[0].x = Line[2].x - tri;
        Line[1].x = Line[2].x + tri;
        Line[0].y = Line[1].y = Line[2].y - tri;
        break;

    case DFCS_SCROLLUP:
        Line[2].x = myr.left + 470*SmallDiam/1000 + 2;
        Line[2].y = myr.bottom - 687*SmallDiam/1000 + 1;
        Line[0].x = Line[2].x - tri;
        Line[1].x = Line[2].x + tri;
        Line[0].y = Line[1].y = Line[2].y + tri;
        break;

    case DFCS_SCROLLLEFT:
        Line[2].x = myr.right - 687*SmallDiam/1000 + 1;
        Line[2].y = myr.top  + 470*SmallDiam/1000 + 2;
        Line[0].y = Line[2].y - tri;
        Line[1].y = Line[2].y + tri;
        Line[0].x = Line[1].x = Line[2].x + tri;
        break;

    case DFCS_SCROLLRIGHT:
        Line[2].x = myr.left + 687*SmallDiam/1000 + 1;
        Line[2].y = myr.top  + 470*SmallDiam/1000 + 2;
        Line[0].y = Line[2].y - tri;
        Line[1].y = Line[2].y + tri;
        Line[0].x = Line[1].x = Line[2].x - tri;
        break;

    case DFCS_SCROLLSIZEGRIP:
        /* This one breaks the flow... */
        UITOOLS95_DrawRectEdge(dc, r, EDGE_BUMP, BF_MIDDLE | ((uFlags&(DFCS_MONO|DFCS_FLAT)) ? BF_MONO : 0));
        hpsave = (HPEN)SelectObject(dc, GetStockObject(NULL_PEN));
        hbsave = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
        if(uFlags & (DFCS_MONO|DFCS_FLAT))
        {
            hp = hp2 = GetSysColorPen(COLOR_WINDOWFRAME);
            hb = hb2 = GetSysColorBrush(COLOR_WINDOWFRAME);
        }
        else
        {
            hp  = GetSysColorPen(COLOR_BTNHIGHLIGHT);
            hp2 = GetSysColorPen(COLOR_BTNSHADOW);
            hb  = GetSysColorBrush(COLOR_BTNHIGHLIGHT);
            hb2 = GetSysColorBrush(COLOR_BTNSHADOW);
        }
        Line[0].x = Line[1].x = r->right-1;
        Line[2].y = Line[3].y = r->bottom-1;
        d46 = 46*SmallDiam/750;
        d93 = 93*SmallDiam/750;

        i = 586*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject(dc, hb);
        SelectObject(dc, hp);
        Polygon(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject(dc, hb2);
        SelectObject(dc, hp2);
        Polygon(dc, Line, 4);

        i = 398*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject(dc, hb);
        SelectObject(dc, hp);
        Polygon(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject(dc, hb2);
        SelectObject(dc, hp2);
        Polygon(dc, Line, 4);

        i = 210*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject(dc, hb);
        SelectObject(dc, hp);
        Polygon(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject(dc, hb2);
        SelectObject(dc, hp2);
        Polygon(dc, Line, 4);

        SelectObject(dc, hpsave);
        SelectObject(dc, hbsave);
        return TRUE;

    default:
//        WARN("Invalid scroll; flags=0x%04x\n", uFlags);
        return FALSE;
    }

    /* Here do the real scroll-bar controls end up */
    if( ! (uFlags & (0xff00 & ~DFCS_ADJUSTRECT)) )
      /* UITOOLS95_DFC_ButtonPush always uses BF_SOFT which we don't */
      /* want for the normal scroll-arrow button. */
      UITOOLS95_DrawRectEdge( dc, r, EDGE_RAISED, (uFlags&DFCS_ADJUSTRECT) | BF_MIDDLE | BF_RECT);
    else
      UITOOLS95_DFC_ButtonPush(dc, r, (uFlags & 0xff00) );

    if(uFlags & DFCS_INACTIVE)
    {
        hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
        hpsave = (HPEN)SelectObject(dc, GetSysColorPen(COLOR_BTNHIGHLIGHT));
        Polygon(dc, Line, 3);
        SelectObject(dc, hpsave);
        SelectObject(dc, hbsave);
    }

    if( (uFlags & DFCS_INACTIVE) || !(uFlags & DFCS_PUSHED) )
      for(i = 0; i < 3; i++)
      {
        Line[i].x--;
        Line[i].y--;
      }

    i = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_BTNTEXT;
    hbsave = (HBRUSH)SelectObject(dc, GetSysColorBrush(i));
    hpsave = (HPEN)SelectObject(dc, GetSysColorPen(i));
    Polygon(dc, Line, 3);
    SelectObject(dc, hpsave);
    SelectObject(dc, hbsave);

    return TRUE;
}

/************************************************************************
 *      UITOOLS_DrawFrameMenu
 *
 * Draw a menu control coming from DrawFrameControl()
 */
static BOOL UITOOLS95_DrawFrameMenu(HDC dc, LPRECT r, UINT uFlags)
{
    POINT Points[6];
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr);
    int i;
    HBRUSH hbsave;
    HPEN hpsave;
    int xe, ye;
    int xc, yc;
    BOOL retval = TRUE;

    /* Using black and white seems to be utterly wrong, but win95 doesn't */
    /* use anything else. I think I tried all sys-colors to change things */
    /* without luck. It seems as if this behavior is inherited from the */
    /* win31 DFC() implementation... (you remember, B/W menus). */

    FillRect(dc, r, (HBRUSH)GetStockObject(WHITE_BRUSH));

    hbsave = (HBRUSH)SelectObject(dc, GetStockObject(BLACK_BRUSH));
    hpsave = (HPEN)SelectObject(dc, GetStockObject(BLACK_PEN));

    switch(uFlags & 0xff)
    {
    case DFCS_MENUARROW:
        i = 187*SmallDiam/750;
        Points[2].x = myr.left + 468*SmallDiam/750;
        Points[2].y = myr.top  + 352*SmallDiam/750+1;
        Points[0].y = Points[2].y - i;
        Points[1].y = Points[2].y + i;
        Points[0].x = Points[1].x = Points[2].x - i;
        Polygon(dc, Points, 3);
        break;

    case DFCS_MENUBULLET:
        xe = myr.left;
        ye = myr.top  + SmallDiam - SmallDiam/2;
        xc = myr.left + SmallDiam - SmallDiam/2;
        yc = myr.top  + SmallDiam - SmallDiam/2;
        i = 234*SmallDiam/750;
        i = i < 1 ? 1 : i;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;
        Pie(dc, myr.left, myr.top, myr.right, myr.bottom, xe, ye, xe, ye);
        break;

    case DFCS_MENUCHECK:
        Points[0].x = myr.left + 253*SmallDiam/1000;
        Points[0].y = myr.top  + 445*SmallDiam/1000;
        Points[1].x = myr.left + 409*SmallDiam/1000;
        Points[1].y = Points[0].y + (Points[1].x-Points[0].x);
        Points[2].x = myr.left + 690*SmallDiam/1000;
        Points[2].y = Points[1].y - (Points[2].x-Points[1].x);
        Points[3].x = Points[2].x;
        Points[3].y = Points[2].y + 3*SmallDiam/16;
        Points[4].x = Points[1].x;
        Points[4].y = Points[1].y + 3*SmallDiam/16;
        Points[5].x = Points[0].x;
        Points[5].y = Points[0].y + 3*SmallDiam/16;
        Polygon(dc, Points, 6);
        break;

    default:
//        WARN("Invalid menu; flags=0x%04x\n", uFlags);
        retval = FALSE;
        break;
    }

    SelectObject(dc, hpsave);
    SelectObject(dc, hbsave);
    return retval;
}

/**********************************************************************
 *          DrawFrameControl  (USER32.158)
 */
BOOL WIN32API DrawFrameControl(HDC hdc, LPRECT rc, UINT uType,
                               UINT uState )
{
    /* Win95 doesn't support drawing in other mapping modes */
    if(GetMapMode(hdc) != MM_TEXT)
        return FALSE;

    switch(uType)
    {
    case DFC_BUTTON:
      return UITOOLS95_DrawFrameButton(hdc, rc, uState);
    case DFC_CAPTION:
      return UITOOLS95_DrawFrameCaption(hdc, rc, uState);
    case DFC_MENU:
      return UITOOLS95_DrawFrameMenu(hdc, rc, uState);
    case DFC_SCROLL:
      return UITOOLS95_DrawFrameScroll(hdc, rc, uState);
//    default:
//      WARN("(%x,%p,%d,%x), bad type!\n",
//         hdc,rc,uType,uState );
    }
    return FALSE;
}
/******************************************************************************
 *
 * This function is used by Paint_DrawState which is inturn used by both
 * DrawStateA and DrawStateW. The code is directly borrowed from Wine.
 *
 ******************************************************************************/

static BOOL Paint_DrawStateJam(HDC hdc, UINT opcode,
                               DRAWSTATEPROC func, LPARAM lp, WPARAM wp,
                               LPRECT rc, UINT dtflags, BOOL unicode)
{
    HDC memdc;
    HBITMAP hbmsave;
    BOOL retval;
    INT cx = rc->right - rc->left;
    INT cy = rc->bottom - rc->top;

    switch(opcode)
    {
    case DST_TEXT:
    case DST_PREFIXTEXT:
        if(unicode)
            return DrawTextW(hdc, (LPWSTR)lp, (INT)wp, rc, dtflags);
        else
            return DrawTextA(hdc, (LPSTR)lp, (INT)wp, rc, dtflags);

    case DST_ICON:
        return DrawIcon(hdc, rc->left, rc->top, (HICON)lp);

    case DST_BITMAP:
        memdc = CreateCompatibleDC(hdc);
        if(!memdc) return FALSE;
        hbmsave = (HBITMAP)SelectObject(memdc, (HBITMAP)lp);
        if(!hbmsave)
        {
            DeleteDC(memdc);
            return FALSE;
        }
        retval = BitBlt(hdc, rc->left, rc->top, cx, cy, memdc, 0, 0, SRCCOPY);
        SelectObject(memdc, hbmsave);
        DeleteDC(memdc);
        return retval;

    case DST_COMPLEX:
        if(func)
            return func(hdc, lp, wp, cx, cy);
        else
            return FALSE;
    }
    return FALSE;
}


/******************************************************************************
 *
 * This function is used by both DrawStateA and DrawStateW. The code is directly
 * borrowed from Wine
 *
 ******************************************************************************/
BOOL Paint_DrawState (HDC hdc, HBRUSH hbr, DRAWSTATEPROC func, LPARAM lp, WPARAM wp,
                      INT x, INT y, INT cx, INT cy, UINT flags, BOOL unicode)
{
    HBITMAP hbm, hbmsave;
    HFONT hfsave;
    HBRUSH hbsave;
    HDC memdc;
    RECT rc;
    UINT dtflags = DT_NOCLIP;
    COLORREF fg, bg;
    UINT opcode = flags & 0xf;
    INT len = wp;
    BOOL retval, tmp;

    if((opcode == DST_TEXT || opcode == DST_PREFIXTEXT) && !len)    /* The string is '\0' terminated */
    {
        if(unicode)
            len = lstrlenW((LPWSTR)lp);
        else
            len = lstrlenA((LPSTR)lp);
   }

    /* Find out what size the image has if not given by caller */
    if(!cx || !cy)
    {
        SIZE s;
        BITMAP bmp;

        switch(opcode)
        {
        case DST_TEXT:
        case DST_PREFIXTEXT:
            if(unicode)
                retval = GetTextExtentPoint32W(hdc, (LPWSTR)lp, len, &s);
            else
                retval = GetTextExtentPoint32A(hdc, (LPSTR)lp, len, &s);
            if(!retval) return FALSE;
            break;

        case DST_ICON:
            /* Just assume the icon has the size given by GetSystemMetrics. This should be
               valid since both CreateIcon and LoadIcon can't create icons with different
               sizes. I wouldn't how else to get the size of the icon. RP
            */
            s.cx = GetSystemMetrics (SM_CXICON);
            s.cy = GetSystemMetrics (SM_CYICON);
            break;

        case DST_BITMAP:
            retval = GetObjectA ((HBITMAP) lp, sizeof (BITMAP), &bmp);
            if(retval == 0) return FALSE;
            s.cx = bmp.bmWidth;
            s.cy = bmp.bmHeight;
            break;

        case DST_COMPLEX: /* cx and cy must be set in this mode */
            return FALSE;
        }

        if(!cx) cx = s.cx;
        if(!cy) cy = s.cy;
    }

    rc.left   = x;
    rc.top    = y;
    rc.right  = x + cx;
    rc.bottom = y + cy;

    if(flags & DSS_RIGHT)    /* This one is not documented in the win32.hlp file */
        dtflags |= DT_RIGHT;
    if(opcode == DST_TEXT)
        dtflags |= DT_NOPREFIX;

    /* For DSS_NORMAL we just jam in the image and return */
    if((flags & 0x7ff0) == DSS_NORMAL)
    {
        return Paint_DrawStateJam(hdc, opcode, func, lp, len, &rc, dtflags, unicode);
    }

    /* For all other states we need to convert the image to B/W in a local bitmap */
    /* before it is displayed */
    fg = SetTextColor(hdc, RGB(0, 0, 0));
    bg = SetBkColor(hdc, RGB(255, 255, 255));
    hbm = (HBITMAP)NULL; hbmsave = (HBITMAP)NULL;
    memdc = (HDC)NULL; hbsave = (HBRUSH)NULL;
    retval = FALSE; /* assume failure */

    /* From here on we must use "goto cleanup" when something goes wrong */
    hbm     = CreateBitmap(cx, cy, 1, 1, NULL);
    if(!hbm) goto cleanup;
    memdc   = CreateCompatibleDC(hdc);
    if(!memdc) goto cleanup;
    hbmsave = (HBITMAP)SelectObject(memdc, hbm);
    if(!hbmsave) goto cleanup;
    rc.left = rc.top = 0;
    rc.right = cx;
    rc.bottom = cy;
    if(!FillRect(memdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH))) goto cleanup;
    SetBkColor(memdc, RGB(255, 255, 255));
    SetTextColor(memdc, RGB(0, 0, 0));
    hfsave  = (HFONT)SelectObject(memdc, GetCurrentObject(hdc, OBJ_FONT));
    if(!hfsave && (opcode == DST_TEXT || opcode == DST_PREFIXTEXT)) goto cleanup;
    tmp = Paint_DrawStateJam(memdc, opcode, func, lp, len, &rc, dtflags, unicode);
    if(hfsave) SelectObject(memdc, hfsave);
    if(!tmp) goto cleanup;

    /* These states cause the image to be dithered */
    if(flags & (DSS_UNION|DSS_DISABLED))
    {
        hbsave = (HBRUSH)SelectObject(memdc,GetPattern55AABrush());
        if(!hbsave) goto cleanup;
        tmp = PatBlt(memdc, 0, 0, cx, cy, 0x00FA0089);
        if(hbsave) SelectObject(memdc, hbsave);
        if(!tmp) goto cleanup;
    }

    hbsave = (HBRUSH)SelectObject(hdc, hbr ? hbr : GetStockObject(WHITE_BRUSH));
    if(!hbsave) goto cleanup;

    if(!BitBlt(hdc, x, y, cx, cy, memdc, 0, 0, 0x00B8074A)) goto cleanup;

    /* DSS_DEFAULT makes the image boldface */
    if(flags & DSS_DEFAULT)
    {
        if(!BitBlt(hdc, x+1, y, cx, cy, memdc, 0, 0, 0x00B8074A)) goto cleanup;
    }

    retval = TRUE; /* We succeeded */

cleanup:
    SetTextColor(hdc, fg);
    SetBkColor(hdc, bg);

    if(hbsave)  SelectObject(hdc, hbsave);
    if(hbmsave) SelectObject(memdc, hbmsave);
    if(hbm)     DeleteObject(hbm);
    if(memdc)   DeleteDC(memdc);

    return retval;
}



/*****************************************************************************
 * Name      : BOOL WIN32API DrawStateA
 * Purpose   : Draws a bitmap, icon, text or complex object in one of the following
 *             states: normal, disabled, union or mono
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               window station associated with the calling process.
 *             If the function fails, the return value is NULL. This can occur
 *               if the calling process is not an application written for Windows
 *               NT. To get extended error information, call GetLastError.
 * Remark    : Disabled doesn't look like the Windows implementation, but it does
 *             look disabled.
 * Status    : PARTIALLY IMPLEMENTED
 *
 * Author    : Rene Pronk [Sun, 1999/07/25 21:27]
 *****************************************************************************/

BOOL WIN32API DrawStateA(HDC hdc, HBRUSH hbc, DRAWSTATEPROC lpOutputFunc, LPARAM lData,
                         WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
{
  dprintf(("USER32:DrawStateA (%08xh,%08xh,%08xh,%08xh,%08xh,%d,%d,%d,%d,%08x).\n",
         hdc, hbc, lpOutputFunc, lData, wData, x, y, cx, cy, fuFlags));

  return Paint_DrawState (hdc, hbc, lpOutputFunc, lData, wData, x, y ,cx, cy, fuFlags, FALSE);
}



/*****************************************************************************
 * Name      : BOOL WIN32API DrawStateW
 * Purpose   : Draws a bitmap, icon, text or complex object in one of the following
 *             states: normal, disabled, union or mono
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               window station associated with the calling process.
 *             If the function fails, the return value is NULL. This can occur
 *               if the calling process is not an application written for Windows
 *               NT. To get extended error information, call GetLastError.
 * Remark    : Disabled doesn't look like the Windows implementation, but it does
 *             look disabled.
 * Status    : PARTIALLY IMPLEMENTED
 *
 * Author    : Rene Pronk [Sun, 1999/07/25 21:27]
 *****************************************************************************/

BOOL WIN32API DrawStateW(HDC hdc, HBRUSH hbc, DRAWSTATEPROC lpOutputFunc, LPARAM lData,
                         WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
{
  dprintf(("USER32:DrawStateW (%08xh,%08xh,%08xh,%08xh,%08xh,%d,%d,%d,%d,%08x).\n",
         hdc, hbc, lpOutputFunc, lData, wData, x, y, cx, cy, fuFlags));

  return Paint_DrawState (hdc, hbc, lpOutputFunc, lData, wData, x, y ,cx, cy, fuFlags, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawFocusRect( HDC hdc, const RECT *lpRect)
{
 BOOL rc;

    rc = O32_DrawFocusRect(hdc, lpRect);
    dprintf(("USER32:  DrawFocusRect %x %x returned %d", hdc, lpRect, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawIcon( HDC hDC, int X, int Y, HICON  hIcon)
{
  ICONINFO ii;
  HDC hMemDC;
  BITMAP bmp;
  HBITMAP oldBmp;
  COLORREF oldFg,oldBg;

  dprintf(("USER32:  DrawIcon\n"));

  if (!hDC || !hIcon)
  {
    SetLastError(ERROR_INVALID_PARAMETER);

    return FALSE;
  }

  if (!GetIconInfo(hIcon,&ii)) return FALSE;
  oldFg = SetTextColor(hDC,RGB(0,0,0));
  oldBg = SetBkColor(hDC,RGB(255,255,255));
  hMemDC = CreateCompatibleDC(hDC);
  oldBmp = SelectObject(hMemDC,ii.hbmMask);
  if (ii.hbmColor)
  {
    GetObjectA(ii.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);
    BitBlt(hDC,X,Y,bmp.bmWidth,bmp.bmHeight,hMemDC,0,0,SRCAND);
    SelectObject(hMemDC,ii.hbmColor);
    BitBlt(hDC,X,Y,bmp.bmWidth,bmp.bmHeight,hMemDC,0,0,SRCINVERT);
  } else
  {
    GetObjectA(ii.hbmMask,sizeof(BITMAP),(LPVOID)&bmp);
    BitBlt(hDC,X,Y,bmp.bmWidth,bmp.bmHeight/2,hMemDC,0,0,SRCAND);
    BitBlt(hDC,X,Y,bmp.bmWidth,bmp.bmHeight/2,hMemDC,0,bmp.bmHeight/2,SRCINVERT);
  }
  SelectObject(hMemDC,oldBmp);
  DeleteObject(hMemDC);
  if (ii.hbmColor) DeleteObject(ii.hbmColor);
  if (ii.hbmMask) DeleteObject(ii.hbmMask);
  SetTextColor(hDC,oldFg);
  SetBkColor(hDC,oldBg);

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawIconEx(HDC hdc, int xLeft, int yTop, HICON hIcon,
                         int cxWidth, int cyWidth, UINT istepIfAniCur,
                         HBRUSH hbrFlickerFreeDraw, UINT diFlags)
{
  ICONINFO ii;
  HDC hMemDC;
  BITMAP bmp;
  HBITMAP oldBmp;
  COLORREF oldFg,oldBg;
  INT oldStretchMode;

  dprintf(("USER32:  DrawIconEx"));

  //CB: istepIfAniCur, DI_COMPAT ignored

  if (!hdc || !hIcon)
  {
    SetLastError(ERROR_INVALID_PARAMETER);

    return FALSE;
  }

  if (!GetIconInfo(hIcon,&ii)) return FALSE;
  if (ii.hbmColor)
    GetObjectA(ii.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);
  else
    GetObjectA(ii.hbmMask,sizeof(BITMAP),(LPVOID)&bmp);

  /* Calculate the size of the destination image.  */
  if (diFlags & DI_DEFAULTSIZE)
  {
    cxWidth = GetSystemMetrics(SM_CXICON);
    cyWidth = GetSystemMetrics(SM_CYICON);
  } else if (cxWidth == 0)
  {
    cxWidth = bmp.bmWidth;
  } else if (cyWidth == 0)
  {
    cyWidth = ii.hbmColor ? bmp.bmHeight:bmp.bmHeight/2;
  }

  if (hbrFlickerFreeDraw)
  {
    HDC hBmpDC = CreateCompatibleDC(hdc);
    HBITMAP oldMemBmp, memBmp = CreateCompatibleBitmap(hdc,cxWidth,cyWidth);
    RECT rect;

    oldMemBmp = SelectObject(hBmpDC,memBmp);
    rect.left = 0;
    rect.top = 0;
    rect.right = cxWidth;
    rect.bottom = cyWidth;
    FillRect(hBmpDC,&rect,hbrFlickerFreeDraw);
    oldFg = SetTextColor(hBmpDC,RGB(0,0,0));
    oldBg = SetBkColor(hBmpDC,RGB(255,255,255));
    hMemDC = CreateCompatibleDC(hdc);
    oldBmp = SelectObject(hMemDC,ii.hbmMask);
    oldStretchMode = SetStretchBltMode(hBmpDC,STRETCH_DELETESCANS);
    if (ii.hbmColor)
    {
      if (diFlags & DI_MASK)
        StretchBlt(hBmpDC,0,0,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);
      if (diFlags & DI_IMAGE)
      {
        SelectObject(hMemDC,ii.hbmColor);
        StretchBlt(hBmpDC,0,0,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCINVERT);
      }
    } else if (diFlags & DI_MASK)
    {
      StretchBlt(hBmpDC,0,0,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight/2,SRCAND);
      StretchBlt(hBmpDC,0,0,cxWidth,cyWidth,hMemDC,0,bmp.bmHeight/2,bmp.bmWidth,bmp.bmHeight/2,SRCINVERT);
    }
    BitBlt(hdc,xLeft,yTop,cxWidth,cyWidth,hBmpDC,0,0,SRCCOPY);
    SelectObject(hMemDC,oldBmp);
    DeleteObject(hMemDC);
    SetTextColor(hdc,oldFg);
    SetBkColor(hdc,oldBg);
    SetStretchBltMode(hBmpDC,oldStretchMode);
    SelectObject(hBmpDC,oldMemBmp);
    DeleteObject(hBmpDC);
    DeleteObject(memBmp);
  } else
  {
    oldFg = SetTextColor(hdc,RGB(0,0,0));
    oldBg = SetBkColor(hdc,RGB(255,255,255));
    hMemDC = CreateCompatibleDC(hdc);
    oldBmp = SelectObject(hMemDC,ii.hbmMask);
    oldStretchMode = SetStretchBltMode(hdc,STRETCH_DELETESCANS);
    if (ii.hbmColor)
    {
      if (diFlags & DI_MASK)
        StretchBlt(hdc,xLeft,yTop,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);
      if (diFlags & DI_IMAGE)
      {
        SelectObject(hMemDC,ii.hbmColor);
        StretchBlt(hdc,xLeft,yTop,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCINVERT);
      }
    } else if (diFlags & DI_MASK)
    {
      StretchBlt(hdc,xLeft,yTop,cxWidth,cyWidth,hMemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);
      StretchBlt(hdc,xLeft,yTop,cxWidth,cyWidth,hMemDC,0,bmp.bmHeight/2,bmp.bmWidth,bmp.bmHeight/2,SRCINVERT);
    }
    SelectObject(hMemDC,oldBmp);
    DeleteObject(hMemDC);
    SetTextColor(hdc,oldFg);
    SetBkColor(hdc,oldBg);
    SetStretchBltMode(hdc,oldStretchMode);
  }
  if (ii.hbmColor) DeleteObject(ii.hbmColor);
  if (ii.hbmMask) DeleteObject(ii.hbmMask);

  return TRUE;
}
/*****************************************************************************
 * Name      : BOOL WIN32API DrawAnimatedRects
 * Purpose   : The DrawAnimatedRects function draws a wire-frame rectangle
 *             and animates it to indicate the opening of an icon or the
 *             minimizing or maximizing of a window.
 * Parameters: HWND hwnd             handle of clipping window
 *             int idAni             type of animation
 *             CONST RECT * lprcFrom address of rectangle coordinates (minimized)
 *             CONST RECT * lprcTo   address of rectangle coordinates (restored)
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DrawAnimatedRects(HWND hwnd,
                                   int  idAni,
                                   CONST RECT *lprcFrom,
                                   CONST RECT *lprcTo)
{
  dprintf(("USER32:DrawAnimatedRects (%08xh,%u,%08xh,%08x) not implemented.\n",
         hwnd,
         idAni,
         lprcFrom,
         lprcTo));

  return (TRUE);
}
//******************************************************************************
//******************************************************************************
