#include <os2win.h>

/*
 * ScrollDC implementation
 * 
 * Ported from Wine (windows\scroll.c)
 * Fixes for clip rectangles & adaption for Odin (SvL)
 *
 * Copyright  David W. Metcalfe, 1993
 *	      Alex Korobka       1995,1996
 *
 *
 */

//******************************************************************************
//TODO: Can be more efficient (update rect/rgn calc.)
//******************************************************************************
BOOL WINAPI ScrollDC( HDC hdc, INT dx, INT dy, const RECT *rc,
                          const RECT *prLClip, HRGN hrgnUpdate,
                          LPRECT rcUpdate )
{
    RECT rect, rClip, rSrc;
    POINT src, dest;

    dprintf(("USER32: ScrollDC %04x %d,%d hrgnUpdate=%04x rcUpdate = %p cliprc = (%d,%d-%d,%d), rc=(%d,%d-%d,%d)",
                   hdc, dx, dy, hrgnUpdate, rcUpdate, 
		   prLClip ? prLClip->left : 0, prLClip ? prLClip->top : 0, prLClip ? prLClip->right : 0, prLClip ? prLClip->bottom : 0,
		   rc ? rc->left : 0, rc ? rc->top : 0, rc ? rc->right : 0, rc ? rc->bottom : 0 ));

    if ( !hdc ) return FALSE;

    /* compute device clipping region (in device coordinates) */

    if ( rc )
	rect = *rc;
    else /* maybe we should just return FALSE? */
    {
        DebugInt3();
	GetClipBox( hdc, &rect );
    }

    LPtoDP( hdc, (LPPOINT)&rect, 2 );

    if (prLClip)
    {
        rClip = *prLClip;
        LPtoDP( hdc, (LPPOINT)&rClip, 2 );
	IntersectRect( &rClip, &rect, &rClip );
    }
    else
        rClip = rect;

    //limit scrolling to DC's clip rectangle
    GetClipBox( hdc, &rSrc );
    IntersectRect( &rClip, &rSrc, &rClip );
    IntersectRect( &rect, &rSrc, &rect );

    POINT ptl[2] = { 0, 0, dx, dy };

    LPtoDP( hdc, ptl, 2);

    dx = (int)(ptl[1].x - ptl[0].x);
    dy = (int)(ptl[1].y - ptl[0].y);

    rSrc = rClip;
    if (prLClip) {
        OffsetRect( &rSrc, -dx, -dy );
        IntersectRect( &rSrc, &rSrc, &rect );
    }
    if (!IsRectEmpty(&rSrc))
    {
            dest.x = (src.x = rSrc.left) + dx;
            dest.y = (src.y = rSrc.top) + dy;

            /* copy bits */
    
            DPtoLP( hdc, (LPPOINT)&rSrc, 2 );
            DPtoLP( hdc, &src, 1 );
            DPtoLP( hdc, &dest, 1 );

            if (!BitBlt( hdc, dest.x, dest.y,
                           rSrc.right - rSrc.left, rSrc.bottom - rSrc.top,
                           hdc, src.x, src.y, SRCCOPY))
            {
                return FALSE;
            }
        }

        /* compute update areas */

        if (hrgnUpdate || rcUpdate)
        {
            HRGN hrgn =
              (hrgnUpdate) ? hrgnUpdate : CreateRectRgn( 0,0,0,0 );
            HRGN hrgn2;

            hrgn2 = CreateRectRgnIndirect( &rect );
            SetRectRgn( hrgn, rClip.left, rClip.top,
                          rClip.right, rClip.bottom );
            CombineRgn( hrgn, hrgn, hrgn2, RGN_AND );
            OffsetRgn( hrgn2, dx, dy );
            CombineRgn( hrgn, hrgn, hrgn2, RGN_DIFF );

            if( rcUpdate )
	    {
		GetRgnBox( hrgn, rcUpdate );

		/* Put the rcUpdate in logical coordinate */
		DPtoLP( hdc, (LPPOINT)rcUpdate, 2 );
	    }
            if (!hrgnUpdate) DeleteObject( hrgn );
            DeleteObject( hrgn2 );

    }

    return TRUE;

}
//******************************************************************************
//******************************************************************************
