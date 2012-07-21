#ifndef __SURFACEHLP_H__
#define __SURFACEHLP_H__


extern FOURCC  SupportedFourCCs[];

extern BYTE DefaultPalette[];
extern WORD wDefaultPalete16[256];
extern DWORD dwDefaultPalete24[256];
extern BOOL fPalInit;

//#define PERFTEST
#ifdef PERFTEST
extern LARGE_INTEGER liStart;
extern LARGE_INTEGER liEnd;
extern ULONG average;
#endif

#ifdef DEBUG
void _dump_DDBLTFX(DWORD flagmask);
void _dump_DDBLTFAST(DWORD flagmask);
void _dump_DDBLT(DWORD flagmask);
void _dump_DDSCAPS(DWORD flagmask);
void _dump_DDSCAPS2(DWORD flagmask);
void _dump_DDSD(DWORD flagmask);
void _dump_DDCOLORKEY(DWORD flagmask);
void _dump_DDPIXELFORMAT(DWORD flagmask);
void _dump_pixelformat(LPDDPIXELFORMAT pf);

#else

#define _dump_DDBLTFX(flagmask)
#define _dump_DDBLTFAST(flagmask)
#define _dump_DDBLT(flagmask)
#define _dump_DDSCAPS(flagmask)
#define _dump_DDSCAPS2(flagmask)
#define _dump_DDSD(flagmask)
#define _dump_DDCOLORKEY(flagmask)
#define _dump_DDPIXELFORMAT(flagmask)
#define _dump_pixelformat(pf)

#endif


#endif //__SURFACEHLP_H__