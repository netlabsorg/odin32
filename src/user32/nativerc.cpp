/*
 * Native resource management (dialogs, bitmaps, ...)
 *
 *
 * Copyright (c) 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include "user32.h"
#include "wndproc.h"
#include "wndclass.h"

//******************************************************************************
// CB: loads OS/2 bitmaps
//******************************************************************************
HBITMAP WINAPI NativeLoadBitmap(HINSTANCE hInstance,LPCTSTR lpBitmapName)
{
  return O32_LoadBitmap(hInstance,lpBitmapName);
}
