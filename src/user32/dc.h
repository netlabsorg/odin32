/* $Id: dc.h,v 1.3 1999-09-27 18:21:25 dengert Exp $ */
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

