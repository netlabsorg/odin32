/* $Id: mesadive.h,v 1.1 2000-03-01 18:49:32 jeroen Exp $ */

BOOL  _System DiveInit( PWMC, HWND );
void  _System DiveFree( PWMC );
void  _System DiveWriteBackbuffer( PWMC , int, int, BYTE, BYTE, BYTE );
void  _System DiveWriteFrontbuffer( PWMC , int, int, BYTE, BYTE, BYTE );
ULONG _System DiveBlitSetup(PWMC,BOOL);
void  _System DiveFlush( PWMC );
void  _System DiveGlobalTerminate(void);
void  _System DiveGlobalInitialize(void);
BOOL  _System DiveDirectAccess(void);
ULONG _System DiveQueryDepth(void);
ULONG _System DiveQueryNativePixelFormat(void);
void  _System DiveDefineRegion(PWMC,HWND);
void  _System DiveResizeBuffers(GLint width,GLint height);
