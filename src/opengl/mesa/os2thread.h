/* $Id: os2thread.h,v 1.1 2000-05-23 20:34:54 jeroen Exp $ */
typedef ULONG HMTX;

extern ULONG _System OS2CreateMutexSem(CHAR*,HMTX*,ULONG,ULONG);
extern ULONG _System OS2RequestMutexSem(HMTX,ULONG);
extern ULONG _System OS2ReleaseMutexSem(HMTX);
