/* $Id: dc.h,v 1.4 1999-09-28 08:00:56 dengert Exp $ */
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

extern BOOL isYup (struct _DCData *pHps);
extern HDC sendEraseBkgnd (Win32BaseWindow *wnd);
extern void releaseOwnDC (HDC hps);

#endif //__DC_H__

