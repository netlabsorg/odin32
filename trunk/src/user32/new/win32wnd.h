/* $Id: win32wnd.h,v 1.28 1999-08-28 19:32:47 sandervl Exp $ */
/*
 * Win32 Window Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WND_H__
#define __WIN32WND_H__

#ifdef __cplusplus

#include <win32class.h>
#include <gen_object.h>
#include <win32wndchild.h>
#include <winres.h>

class Win32Window;

#define OFFSET_WIN32WNDPTR        0
#define OFFSET_WIN32PM_MAGIC      4
#define OFFSET_WIN32PM_SHAREDMEM  8
#define NROF_WIN32WNDBYTES        12

#define WIN32PM_MAGIC           0x12345678
#define CheckMagicDword(a)      (a==WIN32PM_MAGIC)

#define WIN32APP_USERMSGBASE		0x1000

typedef struct {
        USHORT          cb;
        Win32Window    *win32wnd;
        ULONG           win32CreateStruct;      //or dialog create dword
} CUSTOMWNDDATA;

typedef struct
{
        ULONG           Msg;
        ULONG           wParam;
        ULONG           lParam;
} POSTMSG_PACKET;

#define WM_WIN32_POSTMESSAGEA   0x4000
#define WM_WIN32_POSTMESSAGEW   0x4001

class Win32Window : public GenericObject, private ChildWindow
{
public:
        DWORD   magic;

                Win32Window(DWORD objType);
                Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32Window();

virtual  ULONG  MsgCreate(HWND hwndOS2, ULONG initParam);
         ULONG  MsgQuit();
         ULONG  MsgClose();
         ULONG  MsgDestroy();
         ULONG  MsgEnable(BOOL fEnable);
         ULONG  MsgShow(BOOL fShow);
         ULONG  MsgPosChanging(LPARAM lp);
         ULONG  MsgPosChanged(LPARAM lp);
         ULONG  MsgMove(ULONG x, ULONG y);
         ULONG  MsgHitTest(ULONG x, ULONG y);
         ULONG  MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize);
         ULONG  MsgActivate(BOOL fActivate, HWND hwnd);
         ULONG  MsgSetFocus(HWND hwnd);
         ULONG  MsgKillFocus(HWND hwnd);
         ULONG  MsgCommand(ULONG cmd, ULONG Id, HWND hwnd);
         ULONG  MsgSysCommand(ULONG win32sc, ULONG x, ULONG y);
         ULONG  MsgChar(ULONG cmd, ULONG repeatcnt, ULONG scancode, ULONG vkey, ULONG keyflags);
         ULONG  MsgButton(ULONG msg, ULONG ncx, ULONG ncy, ULONG clx, ULONG cly);
         ULONG  MsgMouseMove(ULONG keystate, ULONG x, ULONG y);
         ULONG  MsgPaint(ULONG tmp1, ULONG tmp2);
         ULONG  MsgEraseBackGround(HDC hdc);
         ULONG  MsgSetText(LPSTR lpsz, LONG cch);
         ULONG  MsgGetTextLength();
         char  *MsgGetText();

virtual  LONG   SetWindowLongA(int index, ULONG value);
virtual  ULONG  GetWindowLongA(int index);
virtual  WORD   SetWindowWord(int index, WORD value);
virtual  WORD   GetWindowWord(int index);

         DWORD  getStyle()                      { return dwStyle; };
         DWORD  getExStyle()                    { return dwExStyle; };
         HWND   getWindowHandle()               { return Win32Hwnd; };
         HWND   getOS2WindowHandle()            { return OS2Hwnd; };
         HWND   getOS2FrameWindowHandle()       { return OS2HwndFrame; };
 Win32WndClass *getWindowClass()                { return windowClass; };

         BOOL   isFrameWindow()                 { return OS2Hwnd != OS2HwndFrame; };
   Win32Window *getParent()                     { return (Win32Window *)ChildWindow::GetParent(); };
         void   setParent(Win32Window *pwindow) { ChildWindow::SetParent((ChildWindow *)pwindow); };
       WNDPROC  getWindowProc()                 { return win32wndproc; };
         void   setWindowProc(WNDPROC newproc)  { win32wndproc = newproc; };
        DWORD   getWindowId()                   { return windowId; };
         void   setWindowId(DWORD id)           { windowId = id; };
         ULONG  getWindowHeight()               { return rectClient.bottom - rectClient.top; };
         BOOL   isChild();
         PRECT  getClientRect()                 { return &rectClient; };
         PRECT  getWindowRect()                 { return &rectWindow; };
void   setClientRect(LONG left, LONG top, LONG right, LONG bottom)
{ rectClient.left  = left;  rectClient.top    = top;
  rectClient.right = right; rectClient.bottom = bottom;
};
void   setWindowRect(LONG left, LONG top, LONG right, LONG bottom)
{ rectWindow.left  = left;  rectWindow.top    = top;
  rectWindow.right = right; rectWindow.bottom = bottom;
};
         void   setWindowRect(PRECT rect)       { rectWindow = *rect; };

         DWORD  getFlags()                      { return flags; };
         void   setFlags(DWORD newflags)        { flags = newflags; };

         HACCEL GetAccelTable()                     { return (HACCEL) acceltableResource; };
         BOOL   SetAccelTable(ULONG hAccel);

         HMENU  GetMenu()                           { return (HMENU) menuResource; };
         BOOL   SetMenu(ULONG hMenu);

         BOOL   SetIcon(HICON hIcon);
         HICON  GetIcon()                           { return (HICON) iconResource; };

         BOOL   ShowWindow(ULONG nCmdShow);
         BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags);
         BOOL   DestroyWindow();
         HWND   SetActiveWindow();
         HWND   GetParent();
         HWND   SetParent(HWND hwndNewParent);
         BOOL   IsChild(HWND hwndParent);
         HWND   GetTopWindow();
         BOOL   UpdateWindow();
         BOOL   IsIconic();
         HWND   GetWindow(UINT uCmd);
         BOOL   EnableWindow(BOOL fEnable);
         BOOL   CloseWindow();
  static HWND   GetActiveWindow();
         BOOL   IsWindow();
         BOOL   IsWindowEnabled();
         BOOL   IsWindowVisible();
         BOOL   IsUnicode()  { return isUnicode; };

         BOOL   GetWindowRect(PRECT pRect);
         int    GetWindowTextLengthA();
         int    GetWindowTextA(LPSTR lpsz, int cch);
         BOOL   SetWindowText(LPSTR lpsz);
          BOOL  hasWindowName(LPSTR wndname, BOOL fUnicode = 0);

       LRESULT  SendMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcA(UINT msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcW(UINT msg, WPARAM wParam, LPARAM lParam);

         void   NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

Win32WndClass  *getClass()  { return windowClass; };

    static HWND FindWindowEx(HWND hwndParent, HWND hwndChildAfter, LPSTR lpszClass, LPSTR lpszWindow,
                             BOOL fUnicode = 0);

static   HWND  Win32ToOS2Handle(HWND hwnd)
{
 Win32Window *window = GetWindowFromHandle(hwnd);

  if(window) {
        return window->getOS2WindowHandle();
  }
  else  return hwnd;    //OS/2 window handle
}

static   HWND  OS2ToWin32Handle(HWND hwnd)
{
 Win32Window *window = GetWindowFromOS2Handle(hwnd);

  if(window) {
        return window->getWindowHandle();
  }
  else  return hwnd;    //OS/2 window handle
}

static Win32Window *GetWindowFromHandle(HWND hwnd);
static Win32Window *GetWindowFromOS2Handle(HWND hwnd);

protected:
       LRESULT  SendInternalMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendInternalMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
        void    Init();

        HWND    OS2Hwnd;
        HWND    OS2HwndFrame;
        HWND    OS2HwndMenu;
        HWND    Win32Hwnd;
        BOOL    isUnicode;

        int     posx, posy, width, height;

        // values normally contained in the standard window words
        ULONG   dwExStyle;              //GWL_EXSTYLE
        ULONG   dwStyle;                //GWL_STYLE
      WNDPROC   win32wndproc;           //GWL_WNDPROC
        ULONG   hInstance;              //GWL_HINSTANCE
//Moved in ChildWindow class
/////   Win32Window *parent;                    //GWL_HWNDPARENT
        ULONG   windowId;               //GWL_ID
        ULONG   userData;               //GWL_USERDATA

         HWND   hwndLinkAfter;
        DWORD   flags;
        DWORD   lastHitTestVal;         //Last value returned by WM_NCHITTEST handler

        BOOL    isIcon;
        BOOL    fCreated;
        BOOL    fFirstShow;

   Win32Window *owner;

 Win32Resource *acceltableResource;
 Win32Resource *menuResource;
 Win32Resource *iconResource;

        char   *windowNameA;
        WCHAR  *windowNameW;
        ULONG   wndNameLength; //including 0 terminator

        ULONG  *userWindowLong;
        ULONG   nrUserWindowLong;

        RECT    rectWindow;
        RECT    rectClient;

Win32WndClass  *windowClass;

static GenericObject *windows;

private:
#ifndef OS2_INCLUDED
        BOOL  CreateWindowExA(CREATESTRUCTA *lpCreateStruct, ATOM classAtom);

        void  GetMinMaxInfo(POINT *maxSize, POINT *maxPos, POINT *minTrack, POINT *maxTrack );

        LONG  SendNCCalcSize(BOOL calcValidRect,
                             RECT *newWindowRect, RECT *oldWindowRect,
                             RECT *oldClientRect, WINDOWPOS *winpos,
                             RECT *newClientRect );

     LRESULT  SendInternalMessage(ULONG msg, WPARAM wParam, LPARAM lParam)
     {
        if(isUnicode)
                return SendInternalMessageW(msg, wParam, lParam);
        else    return SendInternalMessageA(msg, wParam, lParam);
     }
#endif
};


#define BUTTON_LEFTDOWN         0
#define BUTTON_LEFTUP           1
#define BUTTON_LEFTDBLCLICK     2
#define BUTTON_RIGHTUP          3
#define BUTTON_RIGHTDOWN        4
#define BUTTON_RIGHTDBLCLICK    5
#define BUTTON_MIDDLEUP         6
#define BUTTON_MIDDLEDOWN       7
#define BUTTON_MIDDLEDBLCLICK   8

#define WMMOVE_LBUTTON          1
#define WMMOVE_MBUTTON          2
#define WMMOVE_RBUTTON          4
#define WMMOVE_CTRL             8
#define WMMOVE_SHIFT            16


#define CMD_MENU                1
#define CMD_CONTROL             2
#define CMD_ACCELERATOR         3

#define WIN32SC_SIZE         0xf000
#define WIN32SC_MOVE         0xf010
#define WIN32SC_MINIMIZE     0xf020
#define WIN32SC_MAXIMIZE     0xf030
#define WIN32SC_NEXTWINDOW   0xf040
#define WIN32SC_PREVWINDOW   0xf050
#define WIN32SC_CLOSE        0xf060
#define WIN32SC_VSCROLL      0xf070
#define WIN32SC_HSCROLL      0xf080
#define WIN32SC_MOUSEMENU    0xf090
#define WIN32SC_KEYMENU      0xf100
#define WIN32SC_ARRANGE      0xf110
#define WIN32SC_RESTORE      0xf120
#define WIN32SC_TASKLIST     0xf130
#define WIN32SC_SCREENSAVE   0xf140
#define WIN32SC_HOTKEY       0xf150

#define KEY_ALTDOWN             1
#define KEY_PREVDOWN            2
#define KEY_UP                  4
#define KEY_DEADKEY             8

#endif //__cplusplus

#endif //__WIN32WND_H__
