/* $Id: divewrap.h,v 1.4 2001-03-18 21:44:44 mike Exp $ */

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

  inline ULONG APIENTRY _DiveQueryCaps ( PDIVE_CAPS a,
                                        ULONG      b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveQueryCaps(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveQueryCaps
  #define DiveQueryCaps _DiveQueryCaps

  inline ULONG APIENTRY _DiveOpen ( HDIVE *a,
                                   BOOL   b,
                                   PVOID  c )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveOpen(a, b, c);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveOpen
  #define DiveOpen _DiveOpen

  inline ULONG APIENTRY _DiveSetupBlitter ( HDIVE          a,
                                           PSETUP_BLITTER b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveSetupBlitter(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveSetupBlitter
  #define DiveSetupBlitter _DiveSetupBlitter

  inline ULONG APIENTRY _DiveBlitImage ( HDIVE a,
                                        ULONG b,
                                        ULONG c )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveBlitImage(a, b, c);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveBlitImage
  #define DiveBlitImage _DiveBlitImage

  inline ULONG APIENTRY _DiveBlitImageLines ( HDIVE a,
                                             ULONG b,
                                             ULONG c,
                                             PBYTE d )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveBlitImageLines(a, b, c, d);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveBlitImageLines
  #define DiveBlitImageLines _DiveBlitImageLines

  inline ULONG APIENTRY _DiveClose ( HDIVE a )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveClose(a);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveClose
  #define DiveClose _DiveClose

  inline ULONG APIENTRY _DiveAcquireFrameBuffer ( HDIVE   a,
                                                 PRECTL  b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveAcquireFrameBuffer(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveAcquireFrameBuffer
  #define DiveAcquireFrameBuffer _DiveAcquireFrameBuffer

  inline ULONG APIENTRY _DiveSwitchBank ( HDIVE a,
                                         ULONG b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveSwitchBank(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveSwitchBank
  #define DiveSwitchBank _DiveSwitchBank

  inline ULONG APIENTRY _DiveDeacquireFrameBuffer ( HDIVE a )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveDeacquireFrameBuffer(a);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveDeacquireFrameBuffer
  #define DiveDeacquireFrameBuffer _DiveDeacquireFrameBuffer

  inline ULONG APIENTRY _DiveCalcFrameBufferAddress ( HDIVE  a,
                                                     PRECTL b,
                                                     PBYTE *c,
                                                     PULONG d,
                                                     PULONG e )
  {
   ULONG yyrc;
   USHORT sel = RestoreOS2FS();

      yyrc = DiveCalcFrameBufferAddress(a, b, c, d, e);
      SetFS(sel);

      return yyrc;
  }
  #undef  DiveCalcFrameBufferAddress
  #define DiveCalcFrameBufferAddress _DiveCalcFrameBufferAddress

  inline ULONG APIENTRY _DiveAllocImageBuffer ( HDIVE  a,
                                               PULONG b,
                                               FOURCC c,
                                               ULONG  d,
                                               ULONG  e,
                                               ULONG  f,
                                               PBYTE  g )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveAllocImageBuffer(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveAllocImageBuffer
  #define DiveAllocImageBuffer _DiveAllocImageBuffer

  inline ULONG APIENTRY _DiveFreeImageBuffer ( HDIVE a,
                                              ULONG b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveFreeImageBuffer(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveFreeImageBuffer
  #define DiveFreeImageBuffer _DiveFreeImageBuffer

  inline ULONG APIENTRY _DiveBeginImageBufferAccess ( HDIVE  a,
                                                     ULONG  b,
                                                     PBYTE *c,
                                                     PULONG d,
                                                     PULONG e )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveBeginImageBufferAccess(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveBeginImageBufferAccess
  #define DiveBeginImageBufferAccess _DiveBeginImageBufferAccess

  inline ULONG APIENTRY _DiveEndImageBufferAccess ( HDIVE a,
                                                   ULONG b )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveEndImageBufferAccess(a, b);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveEndImageBufferAccess
  #define DiveEndImageBufferAccess _DiveEndImageBufferAccess

  inline ULONG APIENTRY _DiveSetDestinationPalette ( HDIVE a,
                                                    ULONG b,
                                                    ULONG c,
                                                    PBYTE d )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveSetDestinationPalette(a, b, c, d);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveSetDestinationPalette
  #define DiveSetDestinationPalette _DiveSetDestinationPalette

  inline ULONG APIENTRY _DiveSetSourcePalette ( HDIVE a,
                                               ULONG b,
                                               ULONG c,
                                               PBYTE d )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveSetSourcePalette(a, b, c, d);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveSetSourcePalette
  #define DiveSetSourcePalette _DiveSetSourcePalette

  inline ULONG APIENTRY _DiveSetTransparentBlitMode ( HDIVE a,
                                                     ULONG b,
                                                     ULONG c,
                                                     ULONG d )
  {
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = DiveSetTransparentBlitMode(a, b, c, d);
    SetFS(sel);

    return yyrc;
  }
  #undef  DiveSetTransparentBlitMode
  #define DiveSetTransparentBlitMode _DiveSetTransparentBlitMode


#endif
