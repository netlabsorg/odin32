/* $Id: winos2def.h,v 1.6 1999-06-26 18:24:23 sandervl Exp $ */

/*
 *
 * OS/2 declarations when including os2win.h
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __WINOS2DEF_H__
#define __WINOS2DEF_H__

#define APIENTRY_OS2    _System

/*
typedef unsigned long   HMODULE;
typedef unsigned long   HINSTANCE;
//typedef unsigned long         ULONG;
typedef char *          PSZ;
typedef ULONG           MPARAM;
typedef ULONG           APIRET;
*/

#ifndef MPARAM
#  define MPARAM void*
#endif

#ifndef APIRET
#  define APIRET ULONG
#endif

typedef char *PSZ;
typedef const char *PCSZ;


extern "C" {
   BOOL APIENTRY_OS2 WinPostMsg(HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2);

   APIRET APIENTRY_OS2 DosSleep(ULONG msec);

}

inline BOOL _WinPostMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPostMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  WinPostMsg
#define WinPostMsg _WinPostMsg

inline ULONG _DosSleep(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSleep(a);
    SetFS(sel);

    return yyrc;
}

#undef  DosSleep
#define DosSleep _DosSleep


   typedef struct _COUNTRYCODE   /* ctryc */
   {
      ULONG       country;
      ULONG       codepage;
   } COUNTRYCODE;
   typedef COUNTRYCODE *PCOUNTRYCODE;

   typedef struct _COUNTRYINFO   /* ctryi */
   {
      ULONG       country;
      ULONG       codepage;
      ULONG       fsDateFmt;
      CHAR        szCurrency[5];
      CHAR        szThousandsSeparator[2];
      CHAR        szDecimal[2];
      CHAR        szDateSeparator[2];
      CHAR        szTimeSeparator[2];
      UCHAR       fsCurrencyFmt;
      UCHAR       cDecimalPlace;
      UCHAR       fsTimeFmt;
      USHORT      abReserved1[2];
      CHAR        szDataSeparator[2];
      USHORT      abReserved2[5];
   } COUNTRYINFO, *PCOUNTRYINFO;

extern "C" {
   APIRET APIENTRY_OS2  DosQueryCtryInfo(ULONG cb,
                                         PCOUNTRYCODE pcc,
                                         PCOUNTRYINFO pci,
                                         PULONG pcbActual);
}

inline ULONG _DosQueryCtryInfo(ULONG a, PCOUNTRYCODE b, PCOUNTRYINFO c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCtryInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  DosQueryCtryInfo
#define DosQueryCtryInfo _DosQueryCtryInfo



#endif //__WINOS2DEF_H__
