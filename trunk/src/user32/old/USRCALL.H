/* $Id: USRCALL.H,v 1.1 1999-09-15 23:04:25 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 callback functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __USRCALL_H__
#define __USRCALL_H__

//Why the @#%@#$ didn't IBM use _stdcall??

class EnumWindowCallback
{
public:
	EnumWindowCallback(WNDENUMPROC pUserCallback, LPARAM lpData);
       ~EnumWindowCallback();

	WNDENUMPROC_O32 GetOS2Callback();
private:

 LPARAM 	lpUserData;
 WNDENUMPROC    pCallback;
 friend static BOOL EXPENTRY_O32 EnumWndProc(HWND hwnd, LPARAM lpParam);
};

#endif
