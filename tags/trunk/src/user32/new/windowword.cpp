/* $Id: windowword.cpp,v 1.5 2000-01-01 14:57:36 cbratschi Exp $ */

/*
 * Win32 Get/SetWindowLong/Word user32 API functions for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>

#include <win32wbase.h>

//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongA(HWND hwnd, int nIndex, LONG  lNewLong)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
      return window->SetWindowLongA(nIndex,lNewLong);
    }
    else
    {
      if (nIndex == GWL_WNDPROC && !Win32BaseWindow::GetWindowFromOS2Handle(hwnd))
      {
        dprintf(("Start subclassing OS/2 window"));

        window = new Win32BaseWindow(hwnd,(PVOID)lNewLong);

        if(window == NULL)
        {
          dprintf(("Win32BaseWindow creation failed!!"));
          return 0;
        }

        if(GetLastError() != 0)
        {
          dprintf(("Win32BaseWindow error found!!"));
          delete window;
          return 0;
        }

        return (LONG)window->getOldWndProc();
      }
      else dprintf(("SetWindowLongA; window %x not found!", hwnd));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongW(HWND hwnd, int nIndex, LONG  lNewLong)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
      return window->SetWindowLongA(nIndex,lNewLong, TRUE);
    }
    else
    {
      if (nIndex == GWL_WNDPROC && !Win32BaseWindow::GetWindowFromOS2Handle(hwnd))
      {
        dprintf(("Start subclassing OS/2 window"));

        window = new Win32BaseWindow(hwnd,(PVOID)lNewLong);

        if(window == NULL)
        {
          dprintf(("Win32BaseWindow creation failed!!"));
          return 0;
        }

        if(GetLastError() != 0)
        {
          dprintf(("Win32BaseWindow error found!!"));
          delete window;
          return 0;
        }

        return (LONG)window->getOldWndProc();
      }
      else dprintf(("SetWindowLongW; window %x not found!", hwnd));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetWindowWord(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowWord(nIndex);
    }
    else
    {
        dprintf(("GetWindowWord; window %x not found!", hwnd));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetWindowWord(HWND hwnd, int nIndex, WORD arg3)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->SetWindowWord(nIndex, arg3);
    }
    else
    {
        dprintf(("SetWindowWord; window %x not found!", hwnd));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongA(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowLongA(nIndex);
    }
    else
    {
        dprintf(("GetWindowLongA; window %x (%d) not found!", hwnd, nIndex));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongW(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowLongA(nIndex, TRUE);
    }
    else
    {
        dprintf(("GetWindowLongW; window %x not found!", hwnd));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
