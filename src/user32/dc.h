/* $Id: dc.h,v 1.2 1999-09-21 17:03:29 dengert Exp $ */
/*
 * public dc functions
 *
 *
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DC_H__
#define __DC_H__

extern HDC sendEraseBkgnd (Win32BaseWindow *wnd);
extern void releaseOwnDC (HDC hps);

#endif //__DC_H__
