/* $Id: read.c,v 1.2 2001-09-05 14:30:46 bird Exp $ */
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
** $Date: 2001-09-05 14:30:46 $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "texusint.h"

#define TX_3DF  0x100
#define TX_PPM  0x101
#define TX_BMP  0x102
#define TX_IFF  0x103
#define TX_PCX  0x104
#define TX_SBI  0x105
#define TX_RGT  0x106
#define TX_UNK  0x200   // TGA is unknown from cookie signature.

int
_txReadHeader( FILE *stream, TxMip *info )
{
    FxU32       cookie;
    int         c;
        int             fformat;                // format of the image file.
        int             status;

    if ( stream == NULL ) {
                txError("Bad file handle.");
                return 0;
    }

    if ((c=getc( stream )) == EOF ) {
                txError("Unexpected end of file");
                return 0;
    }
        cookie = (c << 8);

    if ((c=getc( stream )) == EOF ) {
                txError("Unexpected end of file");
                return 0;
    }
        cookie |= c;

    switch( cookie )
    {
        case ('P' << 8) | '9':  fformat = TX_SBI; break;
        case ('P' << 8) | '6':  fformat = TX_PPM; break;
        case ('3' << 8) | 'd':  fformat = TX_3DF; break;
        case ('3' << 8) | 'D':  fformat = TX_3DF; break;
        case 0x01DA:            // byte swapped RGT file.
        case 0xDA01:            fformat = TX_RGT; break;
        default:                fformat = TX_UNK; break;
    }

    switch (fformat) {
    case TX_SBI: status = _txReadSBIHeader( stream, cookie, info ); break;
    case TX_RGT: status = _txReadRGTHeader( stream, cookie, info ); break;
    case TX_PPM: status = _txReadPPMHeader( stream, cookie, info ); break;
    case TX_3DF: status = _txRead3DFHeader( stream, cookie, info ); break;
    case TX_UNK: status = _txReadTGAHeader( stream, cookie, info ); break;
    }

        /* Not reached. */
        return (status) ? fformat : 0;
}

static FxBool
_txReadData( FILE *stream, int fformat, TxMip *info )
{
        switch (fformat) {
    case TX_SBI:        return _txReadSBIData( stream, info );
    case TX_RGT:        return _txReadRGTData( stream, info );
    case TX_PPM:        return _txReadPPMData( stream, info );
    case TX_3DF:        return _txRead3DFData( stream, info );
        case TX_UNK:    return _txReadTGAData( stream, info );
    }

        /* Not reached. */
        return FXFALSE;
}


FxBool
txMipRead(TxMip *txMip, const char *filename, int prefFormat)
{
  FILE *file;
  FxBool retval;

  file = fopen(filename, "rb");
  if( file == NULL )
    {
      fprintf( stderr,"Error: can't open input file %s\n", filename );
      exit(2);
    }

  retval = txMipReadFromFP( txMip, filename, file, prefFormat );
  fclose(file);
  return retval;
}

FxBool
txMipReadFromFP(TxMip *txMip, const char *debug_filename, FILE *file, int prefFormat)
{
        int                     i, format;
        TxMip           txTrue;
        int                     w, h;

        if ((prefFormat != GR_TEXFMT_ARGB_8888) &&
            (prefFormat != GR_TEXFMT_ANY))
          {
            txPanic("txMipRead: bad preferred format.");
            return FXFALSE;
          }

        if ( (format = _txReadHeader( file, txMip )) == 0 ) {
                fprintf(stderr,"Error: reading info for %s, %s\n",
                        debug_filename, "");
                exit(2);
        }
        if( txVerbose )
          {
            fprintf(stderr,"Loading image file ");

            fprintf (stderr,"%s (%dw x %dh x %d Bpp x %d mips) .. ", debug_filename,
                     txMip->width,txMip->height, GR_TEXFMT_SIZE(txMip->format), txMip->depth);
          }

        /*
         * Allocate memory requested in data[0];
         */

        w = txMip->width;
        h = txMip->height;
        txMip->data[0] = txMalloc(txMip->size);
        for (i=1; i< TX_MAX_LEVEL; i++) {
                if (i >= txMip->depth) {
                        txMip->data[i] = NULL;
                        continue;
                }
                txMip->data[i] = (FxU8*)txMip->data[i-1] +
                        w * h * GR_TEXFMT_SIZE(txMip->format);
                if (w > 1) w >>= 1;
                if (h > 1) h >>= 1;
        }

        if( txVerbose ) {
              fprintf( stderr, "mip-> format: %d width: %d height: %d depth: %d size: %d\n",
        txMip->format, txMip->width, txMip->height, txMip->depth,
        txMip->size );
             fflush( stderr );
        }

        if ( _txReadData( file, format, txMip ) == FXFALSE ) {
                fprintf(stderr, "\nError: reading data for %s\n",debug_filename);
                exit(4);
        }

        if( txVerbose )
          fprintf(stderr," done.\n");
        if (prefFormat == GR_TEXFMT_ANY) return FXTRUE;

        /*
         * Now convert to ARGB8888 if requested.
         */
        txTrue.format = GR_TEXFMT_ARGB_8888;
        txTrue.width  = txMip->width;
        txTrue.height = txMip->height;
        txTrue.depth  = txMip->depth;
        if (txMipAlloc(&txTrue) == FXFALSE) return FXFALSE;

        /*
         * Dequantize from src to dest.
         */
        if( txVerbose )
          {
            fprintf(stderr, "Dequantizing Input from %s to argb8888.\n",
                   Format_Name[txMip->format]);
          }
        txMipDequantize(&txTrue, txMip);

        /*
         * Free the original mipmap, and copy new mipmap into it.
         */
        txFree(txMip->data[0]);
        *txMip = txTrue;

        return FXTRUE;
}


FxBool _txReadSBIHeader( FILE *stream, FxU32 cookie, TxMip *info){ return FXFALSE;}
FxBool _txReadSBIData( FILE *stream, TxMip *info){ return FXFALSE;}

