/* $Id: CALLBACK.H,v 1.3 1999-06-10 17:09:03 phaller Exp $ */

/*
 * GDI32 support code for Line DDA callbacks
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __CALLBACK_H__
#define __CALLBACK_H__

//Why the @#%@#$ didn't IBM use _stdcall??

class LineDDAProcCallback
{
public:
        LineDDAProcCallback(LINEDDAPROC pUserCallback, LPARAM lpData);
       ~LineDDAProcCallback();

        LINEDDAPROC_O32 GetOS2Callback();
private:

 LPARAM         lpUserData;
 LINEDDAPROC    pCallback;
 friend static VOID OPEN32API Callback(int X, int Y, LPARAM lpData);
};

#endif
