/* $Id: winos2def.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

/*
 *
 * OS/2 declarations when including os2win.h
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __WINOS2DEF_H__
#define __WINOS2DEF_H__

#define	APIENTRY_OS2	_System

typedef unsigned long 	HMODULE;
typedef unsigned long   HINSTANCE;
//typedef unsigned long 	ULONG;
typedef char * 		PSZ;
typedef ULONG		MPARAM;
typedef ULONG		APIRET;

extern "C" {
   BOOL APIENTRY_OS2 WinPostMsg(HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2);

   APIRET APIENTRY_OS2 DosSleep(ULONG msec);

}

#endif //__WINOS2DEF_H__