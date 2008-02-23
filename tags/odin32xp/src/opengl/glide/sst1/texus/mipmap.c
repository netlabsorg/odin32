/* $Id: mipmap.c,v 1.2 2001-09-05 14:31:10 bird Exp $ */

/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com).
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.
**
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.
**
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Revision: 1.2 $
** $Date: 2001-09-05 14:31:10 $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "texusint.h"

#define B0(x)   ((x>>24)&0xFF)
#define B1(x)   ((x>>16)&0xFF)
#define B2(x)   ((x>>8)&0xFF)
#define B3(x)   ((x>>0)&0xFF)

static void
_txImgHalve(long *outdata, int width, int height, long *indata)
{
    unsigned int i,j,k;
    unsigned int w,h, *p,sum,*q;

    if ((outdata == NULL)       ||
        (width   <= 0   )       ||
        (height  <= 0   )       ||
        (width & (width-1)) ||
        (height & (height-1)) ||
        ((width == 1) && (height==1))) return;

    w = width>>1;
    h = height>>1;
    p = (unsigned int *) outdata;
    q = (unsigned int *) indata;

    if ((w == 0) || (h == 0)) {
        // Input and output are a single span each (row or column)
        for (j=0; j<w; j++) {
                sum = B0(q[0]) + B0(q[1]); sum = (sum + 1) >> 1; k = sum;
                sum = B1(q[0]) + B1(q[1]); sum = (sum + 1) >> 1; k = (k << 8) + sum;
                sum = B2(q[0]) + B2(q[1]); sum = (sum + 1) >> 1; k = (k << 8) + sum;
                sum = B3(q[0]) + B3(q[1]); sum = (sum + 1) >> 1; k = (k << 8) + sum;
                *p++ = k;
                q += 2;
        }
        return;
    }

    for (i=0; i<h; i++) {
    for (j=0; j<w; j++) {
        sum = B0(q[0]) + B0(q[1]) + B0(q[width]) + B0(q[width+1]);
        sum = (sum + 2) >> 2;   // add 2 to round, then divide by 4
        k = sum;
        sum = B1(q[0]) + B1(q[1]) + B1(q[width]) + B1(q[width+1]);
        sum = (sum + 2) >> 2;   // add 2 to round, then divide by 4
        k = (k<<8) + sum;
        sum = B2(q[0]) + B2(q[1]) + B2(q[width]) + B2(q[width+1]);
        sum = (sum + 2) >> 2;   // add 2 to round, then divide by 4
        k = (k<<8) + sum;
        sum = B3(q[0]) + B3(q[1]) + B3(q[width]) + B3(q[width+1]);
        sum = (sum + 2) >> 2;   // add 2 to round, then divide by 4
        k = (k<<8) + sum;
        *p++ = k;
        q += 2;
    }
    q += width;
    }
}

void
txMipMipmap(TxMip *txMip)
{
    int         i, w, h;

    w = txMip->width;
    h = txMip->height;

    if( txVerbose )
      {
        printf("Mipmapping: .."); fflush(stdout);
        printf(" %dx%d", w, h);
      }
    for (i=1; i< txMip->depth; i++) {
        _txImgHalve (txMip->data[i], w, h, txMip->data[i-1]);
        if (w > 1) w >>= 1;
        if (h > 1) h >>= 1;
        if( txVerbose )
          {
            printf(" %dx%d", w, h); fflush(stdout);
          }
    }
    if( txVerbose )
      {
        printf(".\n");
      }
}
