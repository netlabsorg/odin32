/* $Id: os2DDWindow.h,v 1.3 2001-03-18 21:44:45 mike Exp $ */

/*
 * Windwo subclass definitions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef OS2DDWindow
  #define OS2DDWindow
  HWND hwndFrame;
  BOOL OS2DDSubclassWindow(HWND hwndClient);
#endif

