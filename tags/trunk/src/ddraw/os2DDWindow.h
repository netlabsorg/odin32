/* $Id: os2DDWindow.h,v 1.2 1999-12-21 01:28:19 hugh Exp $ */

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

