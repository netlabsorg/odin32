/* $Id: os2mewrap.h,v 1.1 1999-12-29 10:42:23 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2MEWRAP_H__
#define __OS2MEWRAP_H__

#include <os2me.h>

#if (__IBMC__ > 300)
// VA3.6: inline is a C++ keyword, must be translated to _inline or _Inline or __inline in C code
#define inline _inline
#endif


#ifdef INCL_MCIOS2

inline ULONG _mciSendCommand(USHORT a, USHORT b, ULONG c, PVOID d, USHORT e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = mciSendCommand(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  mciSendCommand
#define mciSendCommand _mciSendCommand
      
inline ULONG _mciSendString(PSZ a, PSZ b, USHORT c, HWND d, USHORT e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = mciSendString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  mciSendString
#define mciSendString _mciSendString
      
inline ULONG _mciGetErrorString(ULONG a, PSZ b, USHORT c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = mciGetErrorString(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  mciGetErrorString
#define mciGetErrorString _mciGetErrorString

/*      
      ULONG APIENTRY mciMakeGroup     (PUSHORT pusDeviceGroupID,
                                       USHORT  usDeviceCount,
                                       PUSHORT pausDeviceList,
                                       ULONG   ulFlags,
                                       ULONG   ulMMTime);
      
      
      ULONG APIENTRY mciDeleteGroup   (USHORT   usGroupID);
      
      
      
      BOOL APIENTRY mciSetSysValue (USHORT iSysValue,
                                    PVOID pValue);
      
      BOOL  APIENTRY mciQuerySysValue (USHORT iSysValue,
                                       PVOID pValue);
      
      ULONG  APIENTRY mciGetDeviceID  (PSZ  pszName);

ULONG APIENTRY mdmDriverNotify (USHORT  usDeviceID,
                                HWND    hwnd,
                                USHORT  usMsgType,
                                USHORT  usUserParm,
                                ULONG   ulMsgParm);

*/
#endif // INCL_MCIOS2

#endif //__OS2MEWRAP_H__
