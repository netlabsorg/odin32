/* $Id: pmframe.h,v 1.1 1999-10-08 16:13:08 cbratschi Exp $ */
/*
 * Win32 Frame Managment Code for OS/2
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 */
#ifndef __PMFRAME_H__
#define __PMFRAME_H__

PVOID FrameSubclassFrameWindow(Win32BaseWindow *win32wnd);
VOID  FrameSetBorderSize(Win32BaseWindow *win32wnd);

#endif
