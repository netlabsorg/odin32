/* $Id: oslibstyle.h,v 1.1 1999-07-14 08:35:35 sandervl Exp $ */
/*
 * Win32 Window styles
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBSTYLE_H__
#define __OSLIBSTYLE_H__

/* Window Styles */
#define WINWS_OVERLAPPED    0x00000000L
#define WINWS_POPUP         0x80000000L
#define WINWS_CHILD         0x40000000L
#define WINWS_MINIMIZE      0x20000000L
#define WINWS_VISIBLE       0x10000000L
#define WINWS_DISABLED      0x08000000L
#define WINWS_CLIPSIBLINGS  0x04000000L
#define WINWS_CLIPCHILDREN  0x02000000L
#define WINWS_MAXIMIZE      0x01000000L
#define WINWS_CAPTION       0x00C00000L
#define WINWS_BORDER        0x00800000L
#define WINWS_DLGFRAME      0x00400000L
#define WINWS_VSCROLL       0x00200000L
#define WINWS_HSCROLL       0x00100000L
#define WINWS_SYSMENU       0x00080000L
#define WINWS_THICKFRAME    0x00040000L
#define WINWS_GROUP         0x00020000L
#define WINWS_TABSTOP       0x00010000L
#define WINWS_MINIMIZEBOX   0x00020000L
#define WINWS_MAXIMIZEBOX   0x00010000L

/* Window extended styles */
#define WINWS_EX_DLGMODALFRAME    0x00000001L
#define WINWS_EX_DRAGDETECT       0x00000002L
#define WINWS_EX_NOPARENTNOTIFY   0x00000004L
#define WINWS_EX_TOPMOST          0x00000008L
#define WINWS_EX_ACCEPTFILES      0x00000010L
#define WINWS_EX_TRANSPARENT      0x00000020L

#endif //__OSLIBSTYLE_H__
