/* $Id: winos2def.h,v 1.4 1999-05-31 22:37:22 phaller Exp $ */

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

#ifndef PSZ
#  define PSZ char *
#endif


extern "C" {
   BOOL APIENTRY_OS2 WinPostMsg(HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2);

   APIRET APIENTRY_OS2 DosSleep(ULONG msec);

}

#endif //__WINOS2DEF_H__
