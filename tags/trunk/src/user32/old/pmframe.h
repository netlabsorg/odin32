/* $Id: pmframe.h,v 1.1 2001-05-11 08:35:20 sandervl Exp $ */
/*
 * Win32 Frame Managment Code for OS/2
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 */
#ifndef __PMFRAME_H__
#define __PMFRAME_H__

PVOID FrameSubclassFrameWindow(Win32BaseWindow *win32wnd);
VOID  FrameUpdateClient(Win32BaseWindow *win32wnd);

#define HTERROR_W             (-2)
#define HTTRANSPARENT_W       (-1)
#define HTNOWHERE_W           0
#define HTCLIENT_W            1
#define HTCAPTION_W           2
#define HTSYSMENU_W           3
#define HTSIZE_W              4
#define HTMENU_W              5
#define HTHSCROLL_W           6
#define HTVSCROLL_W           7
#define HTMINBUTTON_W         8
#define HTMAXBUTTON_W         9
#define HTLEFT_W              10
#define HTRIGHT_W             11
#define HTTOP_W               12
#define HTTOPLEFT_W           13
#define HTTOPRIGHT_W          14
#define HTBOTTOM_W            15
#define HTBOTTOMLEFT_W        16
#define HTBOTTOMRIGHT_W       17
#define HTBORDER_W            18
#define HTGROWBOX_W           HTSIZE_W
#define HTREDUCE_W            HTMINBUTTON_W
#define HTZOOM_W              HTMAXBUTTON_W
#define HTOBJECT_W            19
#define HTCLOSE_W             20
#define HTHELP_W              21
#define HTSIZEFIRST_W         HTLEFT_W
#define HTSIZELAST_W          HTBOTTOMRIGHT_W

#define TFOS_LEFT                    0x0001
#define TFOS_TOP                     0x0002
#define TFOS_RIGHT                   0x0004
#define TFOS_BOTTOM                  0x0008
/* TF_MOVE = TF_LEFT | TF_TOP | TF_RIGHT | TF_BOTTOM */
#define TFOS_MOVE                    0x000F

VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags);

#define WS_MINIMIZE_W      0x20000000L
#define WS_MAXIMIZE_W      0x01000000L

#endif
