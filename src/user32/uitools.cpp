/* $Id: uitools.cpp,v 1.1 1999-06-22 20:00:04 achimha Exp $ */
/*
 * User Interface Functions
 *
 * Copyright 1997 Dimitrie O. Paun
 * Copyright 1997 Bertho A. Stultiens
 * Copyright 1999 Achim Hasenmueller
 */

#include "winuser.h"
#include "user32.h"

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

    if((uFlags & BF_MIDDLE) && retval)
    {
        FillRect(hdc, &InnerRect, GetSysColorBrush(uFlags & BF_MONO ? 
					    COLOR_WINDOW : COLOR_BTNFACE));
    }

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

    /* Adjust rectangle if asked */
    if(uFlags & BF_ADJUST)
    {
        int add = (LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0)
                + (LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0);
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


//**********************************************************************
//          DrawEdge   (USER32.155)
//
BOOL WIN32API DrawEdge(HDC hdc, LPRECT rc, UINT edge, UINT flags)
{

    if(flags & BF_DIAGONAL)
      return UITOOLS95_DrawDiagEdge(hdc, rc, edge, flags);
    else
      return UITOOLS95_DrawRectEdge(hdc, rc, edge, flags);
}
