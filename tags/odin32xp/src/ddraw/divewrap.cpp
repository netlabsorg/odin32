/* $Id: divewrap.cpp,v 1.1 2003-01-21 11:20:35 sandervl Exp $ */

/*
 * Wrapper for DIVE functions calls
 *
 * Copyright 2003 Sander van Leeuwen <sandervl@innotek.de>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#define INCL_MM_OS2
#include <os2.h>
#include <os2sel.h>
#include <dive.h>


typedef ULONG (* APIENTRY PFNDIVEALLOCIMAGEBUFFER)(HDIVE  a, PULONG b, FOURCC c, ULONG  d, ULONG  e, ULONG  f, PBYTE  g );
typedef ULONG (* APIENTRY PFNDIVEBLITIMAGE)(HDIVE a, ULONG b, ULONG c );
typedef ULONG (* APIENTRY PFNDIVECLOSE)(HDIVE a );
typedef ULONG (* APIENTRY PFNDIVEBEGINIMAGEBUFFERACCESS)(HDIVE  a, ULONG  b, PBYTE *c, PULONG d, PULONG e );
typedef ULONG (* APIENTRY PFNDIVEENDIMAGEBUFFERACCESS)(HDIVE a, ULONG b );
typedef ULONG (* APIENTRY PFNDIVEFREEIMAGEBUFFER)(HDIVE a, ULONG b );
typedef ULONG (* APIENTRY PFNDIVEOPEN)(HDIVE *a, BOOL   b, PVOID  c );
typedef ULONG (* APIENTRY PFNDIVEQUERYCAPS)(PDIVE_CAPS a, ULONG      b );
typedef ULONG (* APIENTRY PFNDIVESETUPBLITTER)(HDIVE a, PSETUP_BLITTER b );


static PFNDIVEALLOCIMAGEBUFFER pfnDiveAllocImageBuffer = NULL;
static PFNDIVEBLITIMAGE pfnDiveBlitImage = NULL;
static PFNDIVECLOSE pfnDiveClose = NULL;
static PFNDIVEBEGINIMAGEBUFFERACCESS pfnDiveBeginImageBufferAccess = NULL;
static PFNDIVEENDIMAGEBUFFERACCESS pfnDiveEndImageBufferAccess = NULL;
static PFNDIVEFREEIMAGEBUFFER pfnDiveFreeImageBuffer = NULL;
static PFNDIVEOPEN pfnDiveOpen = NULL;
static PFNDIVEQUERYCAPS pfnDiveQueryCaps = NULL;
static PFNDIVESETUPBLITTER pfnDiveSetupBlitter = NULL;

static HMODULE hLibDive = 0;
//******************************************************************************
//******************************************************************************
BOOL DiveLoad()
{
    char   szError[CCHMAXPATH];

    if (DosLoadModule(szError, sizeof(szError),
                      "DIVE.DLL", &hLibDive) != NO_ERROR)
    {
        return FALSE;
    }
    // 0000:00000000  Imp  DiveAllocImageBuffer (dive.12)
    if(DosQueryProcAddr(hLibDive, 12, NULL,
                       (PFN*)&pfnDiveAllocImageBuffer) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveBlitImage        (dive.5)
    if(DosQueryProcAddr(hLibDive, 5, NULL,
                       (PFN*)&pfnDiveBlitImage) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveClose            (dive.3)
    if(DosQueryProcAddr(hLibDive, 3, NULL,
                       (PFN*)&pfnDiveClose) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveEndImageBufferAccess (dive.15)
    if(DosQueryProcAddr(hLibDive, 15, NULL,
                       (PFN*)&pfnDiveEndImageBufferAccess) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveFreeImageBuffer  (dive.13)
    if(DosQueryProcAddr(hLibDive, 13, NULL,
                       (PFN*)&pfnDiveFreeImageBuffer) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveOpen             (dive.2)
    if(DosQueryProcAddr(hLibDive, 2, NULL,
                       (PFN*)&pfnDiveOpen) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveQueryCaps        (dive.1)
    if(DosQueryProcAddr(hLibDive, 1, NULL,
                       (PFN*)&pfnDiveQueryCaps) != NO_ERROR)
        return FALSE;

    // 0000:00000000  Imp  DiveSetupBlitter     (dive.4)
    if(DosQueryProcAddr(hLibDive, 4, NULL,
                       (PFN*)&pfnDiveSetupBlitter) != NO_ERROR)
        return FALSE;

    //success!
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void DiveUnload()
{
    if(hLibDive) {
        DosFreeModule(hLibDive);
    }
    pfnDiveAllocImageBuffer = NULL;
    pfnDiveBlitImage = NULL;
    pfnDiveClose = NULL;
    pfnDiveEndImageBufferAccess = NULL;
    pfnDiveFreeImageBuffer = NULL;
    pfnDiveOpen = NULL;
    pfnDiveQueryCaps = NULL;
    pfnDiveSetupBlitter = NULL;
    return;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveOpen ( HDIVE *a, BOOL   b, PVOID  c )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveOpen) ? pfnDiveOpen(a, b, c) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveQueryCaps ( PDIVE_CAPS a, ULONG      b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveQueryCaps) ? pfnDiveQueryCaps(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveSetupBlitter ( HDIVE a, PSETUP_BLITTER b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveSetupBlitter) ? pfnDiveSetupBlitter(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveBlitImage ( HDIVE a, ULONG b, ULONG c )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveBlitImage) ? pfnDiveBlitImage(a, b, c) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
#if 0
ULONG APIENTRY _DiveBlitImageLines ( HDIVE a, ULONG b,
                                     ULONG c, PBYTE d )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveBlitImageLines) ? pfnDiveBlitImageLines(a, b, c, d) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
#endif
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveClose ( HDIVE a )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveClose) ? pfnDiveClose(a) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
#if 0
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveAcquireFrameBuffer ( HDIVE   a, PRECTL  b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveAcquireFrameBuffer) ? pfnDiveAcquireFrameBuffer(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveSwitchBank ( HDIVE a, ULONG b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveSwitchBank) ? pfnDiveSwitchBank(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveDeacquireFrameBuffer ( HDIVE a )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveDeacquireFrameBuffer) ? pfnDiveDeacquireFrameBuffer(a) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveCalcFrameBufferAddress ( HDIVE  a, PRECTL b,
                                             PBYTE *c, PULONG d,
                                             PULONG e )
{
   ULONG yyrc;
   USHORT sel = RestoreOS2FS();

      yyrc = (pfnDiveCalcFrameBufferAddress) ? pfnDiveCalcFrameBufferAddress(a, b, c, d, e) : DIVE_ERR_INVALID_INSTANCE;
      SetFS(sel);

      return yyrc;
}
#endif
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveAllocImageBuffer ( HDIVE  a, PULONG b,
                                       FOURCC c, ULONG  d,
                                       ULONG  e, ULONG  f,
                                       PBYTE  g )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveAllocImageBuffer) ? pfnDiveAllocImageBuffer(a, b, c, d, e, f, g) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveFreeImageBuffer ( HDIVE a, ULONG b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveFreeImageBuffer) ? pfnDiveFreeImageBuffer(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveBeginImageBufferAccess ( HDIVE  a, ULONG  b,
                                             PBYTE *c, PULONG d,
                                             PULONG e )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveBeginImageBufferAccess) ? pfnDiveBeginImageBufferAccess(a, b, c, d, e) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveEndImageBufferAccess ( HDIVE a, ULONG b )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveEndImageBufferAccess) ? pfnDiveEndImageBufferAccess(a, b) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
#if 0
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveSetDestinationPalette ( HDIVE a, ULONG b,
                                            ULONG c, PBYTE d )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveSetDestinationPalette) ? pfnDiveSetDestinationPalette(a, b, c, d) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveSetSourcePalette ( HDIVE a, ULONG b,
                                       ULONG c, PBYTE d )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveSetSourcePalette) ? pfnDiveSetSourcePalette(a, b, c, d) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY _DiveSetTransparentBlitMode ( HDIVE a, ULONG b,
                                             ULONG c, ULONG d )
{
    ULONG yyrc;
    USHORT sel = RestoreOS2FS();

    yyrc = (pfnDiveSetTransparentBlitMode) ? pfnDiveSetTransparentBlitMode(a, b, c, d) : DIVE_ERR_INVALID_INSTANCE;
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
#endif
