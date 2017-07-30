/* $Id: divewrap.h,v 1.5 2003-01-21 11:20:35 sandervl Exp $ */

/*
 * Wrapper for DIVE functions calls
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef __DIVEWRAP_H__
  #define __DIVEWRAP_H__

  #define INCL_MM_OS2
  #include <os2sel.h>
  #include <dive.h>

void DiveUnload();
BOOL DiveLoad();

ULONG APIENTRY _DiveOpen ( HDIVE *a, BOOL   b, PVOID  c );
ULONG APIENTRY _DiveQueryCaps ( PDIVE_CAPS a, ULONG      b );
ULONG APIENTRY _DiveSetupBlitter ( HDIVE a, PSETUP_BLITTER b );
ULONG APIENTRY _DiveBlitImage ( HDIVE a, ULONG b, ULONG c );
ULONG APIENTRY _DiveBlitImageLines ( HDIVE a, ULONG b,
                                     ULONG c, PBYTE d );
ULONG APIENTRY _DiveClose ( HDIVE a );
ULONG APIENTRY _DiveAcquireFrameBuffer ( HDIVE   a, PRECTL  b );
ULONG APIENTRY _DiveSwitchBank ( HDIVE a, ULONG b );
ULONG APIENTRY _DiveDeacquireFrameBuffer ( HDIVE a );
ULONG APIENTRY _DiveCalcFrameBufferAddress ( HDIVE  a, PRECTL b,
                                             PBYTE *c, PULONG d,
                                             PULONG e );
ULONG APIENTRY _DiveAllocImageBuffer ( HDIVE  a, PULONG b,
                                       FOURCC c, ULONG  d,
                                       ULONG  e, ULONG  f,
                                       PBYTE  g );
ULONG APIENTRY _DiveFreeImageBuffer ( HDIVE a, ULONG b );
ULONG APIENTRY _DiveBeginImageBufferAccess ( HDIVE  a, ULONG  b,
                                             PBYTE *c, PULONG d,
                                             PULONG e );
ULONG APIENTRY _DiveEndImageBufferAccess ( HDIVE a, ULONG b );
ULONG APIENTRY _DiveSetDestinationPalette ( HDIVE a, ULONG b,
                                            ULONG c, PBYTE d );
ULONG APIENTRY _DiveSetSourcePalette ( HDIVE a, ULONG b,
                                       ULONG c, PBYTE d );
ULONG APIENTRY _DiveSetTransparentBlitMode ( HDIVE a, ULONG b,
                                             ULONG c, ULONG d );

  #undef  DiveQueryCaps
  #define DiveQueryCaps _DiveQueryCaps

  #undef  DiveOpen
  #define DiveOpen _DiveOpen

  #undef  DiveSetupBlitter
  #define DiveSetupBlitter _DiveSetupBlitter

  #undef  DiveBlitImage
  #define DiveBlitImage _DiveBlitImage

  #undef  DiveBlitImageLines
  #define DiveBlitImageLines _DiveBlitImageLines

  #undef  DiveClose
  #define DiveClose _DiveClose

  #undef  DiveAcquireFrameBuffer
  #define DiveAcquireFrameBuffer _DiveAcquireFrameBuffer

  #undef  DiveSwitchBank
  #define DiveSwitchBank _DiveSwitchBank

  #undef  DiveDeacquireFrameBuffer
  #define DiveDeacquireFrameBuffer _DiveDeacquireFrameBuffer

  #undef  DiveCalcFrameBufferAddress
  #define DiveCalcFrameBufferAddress _DiveCalcFrameBufferAddress

  #undef  DiveAllocImageBuffer
  #define DiveAllocImageBuffer _DiveAllocImageBuffer

  #undef  DiveFreeImageBuffer
  #define DiveFreeImageBuffer _DiveFreeImageBuffer

  #undef  DiveBeginImageBufferAccess
  #define DiveBeginImageBufferAccess _DiveBeginImageBufferAccess

  #undef  DiveEndImageBufferAccess
  #define DiveEndImageBufferAccess _DiveEndImageBufferAccess

  #undef  DiveSetDestinationPalette
  #define DiveSetDestinationPalette _DiveSetDestinationPalette

  #undef  DiveSetSourcePalette
  #define DiveSetSourcePalette _DiveSetSourcePalette

  #undef  DiveSetTransparentBlitMode
  #define DiveSetTransparentBlitMode _DiveSetTransparentBlitMode


#endif
