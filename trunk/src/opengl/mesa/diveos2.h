typedef ULONG HDIVE;

#define DIVE_ERR_ACQUIRE_FAILED                          0x0000100b

#define DIVE_BUFFER_SCREEN                               0x00000000
#define DIVE_BUFFER_GRAPHICS_PLANE                       0x00000001
#define DIVE_BUFFER_ALTERNATE_PLANE                      0x00000002

typedef struct _DIVE_CAPS {
   ULONG  ulStructLen;
   ULONG  ulPlaneCount;
   BOOL   fScreenDirect;
   BOOL   fBankSwitched;
   ULONG  ulDepth;
   ULONG  ulHorizontalResolution;
   ULONG  ulVerticalResolution;
   ULONG  ulScanLineBytes;
   FOURCC fccColorEncoding;
   ULONG  ulApertureSize;
   ULONG  ulInputFormats;
   ULONG  ulOutputFormats;
   ULONG  ulFormatLength;
   PVOID  pFormatData;
  }DIVE_CAPS;

typedef struct _SETUP_BLITTER {
   ULONG  ulStructLen;
   ULONG  fInvert;
   FOURCC fccSrcColorFormat;
   ULONG  ulSrcWidth;
   ULONG  ulSrcHeight;
   ULONG  ulSrcPosX;
   ULONG  ulSrcPosY;
   ULONG  ulDitherType;
   FOURCC fccDstColorFormat;
   ULONG  ulDstWidth;
   ULONG  ulDstHeight;
   LONG   lDstPosX;
   LONG   lDstPosY;
   LONG   lScreenPosX;
   LONG   lScreenPosY;
   ULONG  ulNumDstRects;
   PRECTL pVisDstRects;
  }SETUP_BLITTER;

ULONG _System DiveFreeImageBuffer(HDIVE,ULONG);
ULONG _System DiveOpen(HDIVE *,BOOL,PVOID);
ULONG _System DiveClose(HDIVE);
ULONG _System DiveAllocImageBuffer(HDIVE,PULONG,FOURCC,ULONG,ULONG,ULONG,PBYTE*);
ULONG _System DiveSetSourcePalette(HDIVE,ULONG,ULONG,PBYTE);
ULONG _System DiveAcquireFrameBuffer(HDIVE,PRECTL);
ULONG _System DiveBlitImage(HDIVE,ULONG,ULONG);
ULONG _System DiveDeacquireFrameBuffer(HDIVE);
ULONG _System DiveQueryCaps(DIVE_CAPS *,ULONG);
ULONG _System DiveBeginImageBufferAccess(HDIVE,ULONG,PBYTE*,PULONG,PULONG);
ULONG _System DiveEndImageBufferAccess(HDIVE,ULONG);
ULONG _System DiveSetupBlitter(HDIVE,SETUP_BLITTER *);
