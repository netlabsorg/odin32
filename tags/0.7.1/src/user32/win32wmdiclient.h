/* $Id: win32wmdiclient.h,v 1.10 2001-09-19 15:39:51 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WMDICLIENT_H__
#define __WIN32WMDICLIENT_H__

#include <win32wbase.h>
#include "mdi.h"

//******************************************************************************
//******************************************************************************
class Win32MDIClientWindow : public Win32BaseWindow
{
public:
                Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32MDIClientWindow();

virtual  BOOL   isMDIClient();

private:
         MDICLIENTINFO mdiClientInfo;
};
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Register();
BOOL MDICLIENT_Unregister();

#endif //__WIN32WMDICLIENT_H__
