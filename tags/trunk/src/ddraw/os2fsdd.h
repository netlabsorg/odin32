/*
 * Few routines for full-screen DirectDraw on OS/2
 *
 * Copyright 2000 Michal Necasek
 * Copyright 2000 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OS2FSDD__
#define __OS2FSDD__

#ifdef __cplusplus
extern "C" {
#endif

ULONG APIENTRY Gre32Entry3(ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry3 (ULONG a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry3(a,b,c);
    SetFS(sel);

    return yyrc;
}

ULONG APIENTRY Gre32Entry5(ULONG,ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry5 (ULONG a, ULONG b, ULONG c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry5(a,b,c,d,e);
    SetFS(sel);

    return yyrc;
}

#define NGreDeath 0x000040B7L
#define GreDeath(a) (BOOL) _Gre32Entry3((HDC)(a),0L,NGreDeath)
#define NGreResurrection 0x000040B8L
#define GreResurrection(a,b,c) (BOOL) _Gre32Entry5((HDC)(a),(LONG)(b),(c),0L,NGreResurrection)


#ifdef __cplusplus
}
#endif

ULONG SetSVGAMode (ULONG     ulHorRes,
                   ULONG     ulVerRes,
                   ULONG     ulColors,
                   ULONG     ulVerRefr,
                   PULONG    pulModeInd,
                   PVOID     pCLUTData);

ULONG LoadPMIService (VOID);

ULONG SetUpModeTable (VOID);

ULONG SetSVGAPalette (PVOID pal);

VOID RestorePM(VOID);

VOID KillPM(VOID);

ULONG FreeModeTable (VOID);

BOOL ModeInSVGAModeTable(ULONG resX,ULONG resY, ULONG bpp);

#endif /* __OS2FSDD__ */
