/* $Id: win32wbase.h,v 1.14 2000-01-02 19:30:45 cbratschi Exp $ */
/*
 * Win32 Window Base Class for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WNDBASE_H__
#define __WIN32WNDBASE_H__

#ifdef __cplusplus

#include <win32class.h>
#include "open32wbase.h"
#include <gen_object.h>
#include <win32wndchild.h>
#include <winres.h>
#include <winconst.h>
#include <scroll.h>

class Win32BaseWindow;

#define OFFSET_WIN32WNDPTR        0
#define OFFSET_WIN32PM_MAGIC      4
#define NROF_WIN32WNDBYTES        8

#define WIN32PM_MAGIC           0x12345678
#define CheckMagicDword(a)      (a==WIN32PM_MAGIC)

typedef struct {
        USHORT           cb;
        Win32BaseWindow *win32wnd;
        ULONG            win32CreateStruct;      //or dialog create dword
} CUSTOMWNDDATA;

#define WIN32APP_USERMSGBASE      0x1000
#define WIN32APP_POSTMSG          0x1000
#define WIN32MSG_MAGICA           0x12345678
#define WIN32MSG_MAGICW           0x12345679

typedef struct
{
        ULONG           Msg;
        ULONG           wParam;
        ULONG           lParam;
} POSTMSG_PACKET;

#define BROADCAST_SEND          0
#define BROADCAST_POST          1

class Win32BaseWindow : public GenericObject, public ChildWindow
{
public:
        DWORD   magic;

                Win32BaseWindow(DWORD objType);
                Win32BaseWindow(HWND os2Handle,VOID* win32WndProc);
                Win32BaseWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32BaseWindow();

virtual  ULONG  MsgCreate(HWND hwndFrame, HWND hwndClient);
         ULONG  MsgQuit();
         ULONG  MsgClose();
         ULONG  MsgDestroy();
virtual  ULONG  MsgEnable(BOOL fEnable);
         ULONG  MsgShow(BOOL fShow);
         ULONG  MsgPosChanging(LPARAM lp);
         ULONG  MsgPosChanged(LPARAM lp);
virtual  ULONG  MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd);
         ULONG  MsgSetFocus(HWND hwnd);
         ULONG  MsgKillFocus(HWND hwnd);
         ULONG  MsgScroll(ULONG msg, ULONG scrollCode, ULONG scrollPos);
         ULONG  MsgButton(MSG *msg);
         ULONG  MsgMouseMove(MSG *msg);
         ULONG  MsgPaint(ULONG tmp1, BOOL select = TRUE);
         ULONG  MsgEraseBackGround(HDC hdc);
         ULONG  MsgInitMenu(MSG *msg);
         ULONG  MsgHitTest(MSG *msg);
         ULONG  MsgNCPaint();
         ULONG  DispatchMsgA(MSG *msg);
         ULONG  DispatchMsgW(MSG *msg);

         ULONG  MsgSetText(LPSTR lpsz, LONG cch);
         ULONG  MsgGetTextLength();
         char  *MsgGetText();
         ULONG  MsgContextMenu(ULONG x,ULONG y);
         VOID   updateWindowStyle(DWORD oldExStyle,DWORD oldStyle);

virtual  LONG   SetWindowLongA(int index, ULONG value, BOOL fUnicode = FALSE);
virtual  ULONG  GetWindowLongA(int index, BOOL fUnicode = FALSE);
virtual  WORD   SetWindowWord(int index, WORD value);
virtual  WORD   GetWindowWord(int index);

         DWORD  getStyle()                      { return dwStyle; };
         void   setStyle(DWORD newstyle)        { dwStyle = newstyle; };
         DWORD  getExStyle()                    { return dwExStyle; };
         void   setExStyle(DWORD newexstyle)    { dwExStyle = newexstyle; };
         ULONG  getInstance()                   { return hInstance; };
         void   setInstance(ULONG newinstance)  { hInstance = newinstance; };
         HWND   getWindowHandle()               { return Win32Hwnd; };
         HWND   getOS2WindowHandle()            { return OS2Hwnd; };
         HWND   getOS2FrameWindowHandle()       { return OS2HwndFrame; };
 Win32WndClass *getWindowClass()                { return windowClass; };

         BOOL   getIgnoreHitTest()              { return fIgnoreHitTest; }
         VOID   setIgnoreHitTest(BOOL ignore)   { fIgnoreHitTest = ignore; }

         DWORD  getWindowContextHelpId()        { return contextHelpId; };
         void   setWindowContextHelpId(DWORD id){ contextHelpId = id; };

         BOOL   isFrameWindow();
virtual  BOOL   isMDIClient();
virtual  BOOL   isMDIChild();

         BOOL   fHasParentDC()                  { return fParentDC; };

Win32BaseWindow *getParent();
         void   setParent(Win32BaseWindow *pwindow) { ChildWindow::SetParent((ChildWindow *)pwindow); };
       WNDPROC  getWindowProc()                 { return win32wndproc; };
         void   setWindowProc(WNDPROC newproc)  { win32wndproc = newproc; };
        DWORD   getWindowId()                   { return windowId; };
         void   setWindowId(DWORD id);
         ULONG  getWindowHeight()               { return rectWindow.bottom - rectWindow.top; };
         ULONG  getWindowWidth()                { return rectWindow.right - rectWindow.left; };
         ULONG  getClientHeight()               { return rectClient.bottom - rectClient.top; };
         ULONG  getClientWidth()                { return rectClient.right - rectClient.left; };
         BOOL   isChild();
         PRECT  getClientRectPtr()              { return &rectClient; };
         void   getClientRect(PRECT rect)
         {
                *rect = rectClient;
                rectClient.right  -= rectClient.left;
                rectClient.bottom -= rectClient.top;
                rectClient.left = rectClient.top = 0;
         }
         void   setClientRect(PRECT rect)       { rectClient = *rect; };
         PRECT  getWindowRect()                 { return &rectWindow; };
         void   setClientRect(LONG left, LONG top, LONG right, LONG bottom)
         {
                rectClient.left  = left;  rectClient.top    = top;
                rectClient.right = right; rectClient.bottom = bottom;
         };
         void   setWindowRect(LONG left, LONG top, LONG right, LONG bottom)
         {
                rectWindow.left  = left;  rectWindow.top    = top;
                rectWindow.right = right; rectWindow.bottom = bottom;
         };
         void   setWindowRect(PRECT rect)       { rectWindow = *rect; };
         DWORD  getFlags()                      { return flags; };
         void   setFlags(DWORD newflags)        { flags = newflags; };

         HMENU  GetMenu()                           { return (HMENU) OS2HwndMenu; };
         BOOL   SetMenu(ULONG hMenu);

         BOOL   SetIcon(HICON hIcon);
         HICON  GetIcon()                           { return (HICON) iconResource; };

         BOOL   ShowWindow(ULONG nCmdShow);
         BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags);
         BOOL   SetWindowPlacement(WINDOWPLACEMENT *winpos);
         BOOL   DestroyWindow();
         HWND   SetActiveWindow();
         HWND   GetParent();
         HWND   SetParent(HWND hwndNewParent);
         BOOL   IsChild(HWND hwndParent);
         HWND   GetTopWindow();
Win32BaseWindow *GetTopParent();
         BOOL   UpdateWindow();
         BOOL   IsIconic();
         HWND   GetWindow(UINT uCmd);
 virtual BOOL   EnableWindow(BOOL fEnable);
         BOOL   CloseWindow();
  static HWND   GetActiveWindow();
         //Window handle has already been verified, so just return true
         BOOL   IsWindow()                    { return TRUE; };
         BOOL   IsDialog()                    { return fIsDialog; };
         BOOL   IsModalDialog()               { return fIsModalDialog; };
         BOOL   IsModalDialogOwner()          { return fIsModalDialogOwner; };
         VOID   setModalDialogOwner(BOOL fMDO)    { fIsModalDialogOwner = fMDO; };
         VOID   setOS2HwndModalDialog(HWND aHwnd) { OS2HwndModalDialog = aHwnd; };
         HWND   getOS2HwndModalDialog()       { return OS2HwndModalDialog; };
         BOOL   CanReceiveSizeMsgs()          { return !fNoSizeMsg; };
         BOOL   IsWindowCreated()             { return fCreated; }
         BOOL   IsWindowDestroyed()           { return fIsDestroyed; };
         BOOL   IsWindowEnabled();
         BOOL   IsWindowVisible();
         //Created with CreateWindowExA or ExW
//         BOOL   IsUnicode()                   { return isUnicode; };
         //Window procedure type
         BOOL   IsWindowUnicode();

         BOOL   GetWindowRect(PRECT pRect);
         int    GetWindowTextLength();
         int    GetWindowTextA(LPSTR lpsz, int cch);
         int    GetWindowTextW(LPWSTR lpsz, int cch);
         BOOL   SetWindowTextA(LPSTR lpsz);
         BOOL   SetWindowTextW(LPWSTR lpsz);
         BOOL   hasWindowName(LPSTR wndname, BOOL fUnicode = 0);
         CHAR  *getWindowNamePtrA();
         WCHAR *getWindowNamePtrW();
         VOID   freeWindowNamePtr(PVOID namePtr);
         CHAR  *getWindowNameA()              { return windowNameA; }; //only for MDI windows!
         WCHAR *getWindowNameW()              { return windowNameW; }; //only for MDI windows!
Win32WndClass  *getClass()  { return windowClass; };
Win32BaseWindow *getOwner()                         { return owner; };
        void    setOwner(Win32BaseWindow *newOwner) { owner = newOwner; };

 SCROLLBAR_INFO *getScrollInfo(int nBar);
       HWND      getVertScrollHandle()               { return hwndVertScroll; };
       VOID      setVertScrollHandle(HWND newHandle) { hwndVertScroll = newHandle; }
       HWND      getHorzScrollHandle()               { return hwndHorzScroll; };
       VOID      setHorzScrollHandle(HWND newHandle) { hwndHorzScroll = newHandle; }
       VOID      subclassScrollBars(BOOL subHorz,BOOL subVert);
       BOOL      showScrollBars(BOOL changeHorz,BOOL changeVert,BOOL fShow);

       LRESULT  SendMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
static LRESULT  BroadcastMessageA(int type, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT  BroadcastMessageW(int type, UINT msg, WPARAM wParam, LPARAM lParam);
       void     CallWindowHookProc(ULONG hooktype, ULONG Msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode = FALSE);

       LRESULT  DefWindowProcA(UINT Msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcW(UINT msg, WPARAM wParam, LPARAM lParam);

       LRESULT  DefWndControlColor(UINT ctlType, HDC hdc);
       LRESULT  DefWndPrint(HDC hdc,ULONG uFlags);

         void   NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

Win32BaseWindow *FindWindowById(int id);

    static HWND FindWindowEx(HWND hwndParent, HWND hwndChildAfter, LPSTR lpszClass, LPSTR lpszWindow,
                             BOOL fUnicode = 0);

           BOOL EnumChildWindows(WNDENUMPROC lpfn, LPARAM lParam);
           BOOL EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam);
           BOOL EnumWindows(WNDENUMPROC lpfn, LPARAM lParam);

         HWND   getNextDlgTabItem(HWND hwndCtrl, BOOL fPrevious);
         HWND   getNextDlgGroupItem(HWND hwndCtrl, BOOL fPrevious);

    static HWND Win32ToOS2Handle(HWND hwnd);
    static HWND Win32ToOS2FrameHandle(HWND hwnd);
    static HWND OS2ToWin32Handle(HWND hwnd);

static Win32BaseWindow *GetWindowFromHandle(HWND hwnd);
static Win32BaseWindow *GetWindowFromOS2Handle(HWND hwnd);
static Win32BaseWindow *GetWindowFromOS2FrameHandle(HWND hwnd);

    static void DestroyAll();

       PVOID getOldFrameProc() { return pOldFrameProc; };
       VOID  setOldFrameProc(PVOID aOldFrameProc) { pOldFrameProc = aOldFrameProc; };
       ULONG getBorderWidth() { return borderWidth; };
       ULONG getBorderHeight() { return borderHeight; };

       PVOID getOldWndProc() { return pOldWndProc; }
       VOID  setOldWndProc(PVOID aOldWndProc) { pOldWndProc = aOldWndProc; }
       BOOL  isSubclassedOS2Wnd() { return fIsSubclassedOS2Wnd; };

protected:
#ifndef OS2_INCLUDED
        BOOL  CreateWindowExA(CREATESTRUCTA *lpCreateStruct, ATOM classAtom);
#endif
       LRESULT  SendInternalMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendInternalMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
        void    Init();
        void    setExtendedKey(ULONG virtualkey, ULONG *lParam);

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
/////   Win32BaseWindow *parent;                    //GWL_HWNDPARENT
        ULONG   windowId;               //GWL_ID
        ULONG   userData;               //GWL_USERDATA

         HWND   hwndLinkAfter;
        DWORD   flags;
        DWORD   contextHelpId;
        LONG    lastHitTestVal;         //Last value returned by WM_NCHITTEST handler
        BOOL    fIgnoreHitTest;         //Use WinWindowFromPoint during WM_HITTEST

        BOOL    isIcon;
        BOOL    fFirstShow;
        BOOL    fIsDialog;
        BOOL    fIsModalDialog;
        BOOL    fIsModalDialogOwner;
        HWND    OS2HwndModalDialog;
        BOOL    fInternalMsg;           //Used to distinguish between messages
                                        //sent by PM and those sent by apps
        BOOL    fNoSizeMsg;
        BOOL    fIsDestroyed;
        BOOL    fDestroyWindowCalled;   //DestroyWindow was called for this window
        BOOL    fCreated;
        BOOL    fTaskList;              //should be listed in PM tasklist or not
        BOOL    fParentDC;

        DWORD   dwThreadId;             //id of thread that created this window
        DWORD   dwProcessId;            //id of process that created this window
        PVOID   pOldFrameProc;
        ULONG   borderWidth;
        ULONG   borderHeight;

        PVOID   pOldWndProc;
        BOOL    fIsSubclassedOS2Wnd;  //subclassed OS/2 window: Netscape plug-in/scrollbar

   Win32BaseWindow *owner;
     Win32Resource *iconResource;

        char   *windowNameA;
        WCHAR  *windowNameW;
        ULONG   wndNameLength; //including 0 terminator

        ULONG  *userWindowLong;
        ULONG   nrUserWindowLong;

        RECT    rectWindow; //relative to screen
        RECT    rectClient;  //relative to parent

CREATESTRUCTA  *tmpcs; //temporary pointer to CREATESTRUCT used in CreateWindowEx
        ULONG   sw;    //set in CreateWindowExA, used in MsgCreate method

SCROLLBAR_INFO *vertScrollInfo;
SCROLLBAR_INFO *horzScrollInfo;
        HWND    hwndHorzScroll; //os/2 handle
        HWND    hwndVertScroll; //os/2 handle

Win32WndClass  *windowClass;

static GenericObject *windows;

private:
#ifndef OS2_INCLUDED
        void  GetMinMaxInfo(POINT *maxSize, POINT *maxPos, POINT *minTrack, POINT *maxTrack );
        LONG  HandleWindowPosChanging(WINDOWPOS *winpos);
        LONG  HandleNCActivate(WPARAM wParam);
        VOID  TrackMinMaxBox(WORD wParam);
        VOID  TrackCloseButton(WORD wParam);
        LONG  HandleNCLButtonDown(WPARAM wParam,LPARAM lParam);
        LONG  HandleNCLButtonUp(WPARAM wParam,LPARAM lParam);
        LONG  HandleNCLButtonDblClk(WPARAM wParam,LPARAM lParam);
        BOOL  WindowNeedsWMBorder();
        VOID  AdjustRectOuter(LPRECT rect,BOOL menu);
        VOID  AdjustRectInner(LPRECT rect);
        LONG  HandleNCCalcSize(RECT *winRect);
        LONG  HandleNCHitTest(POINT pt);
        VOID  GetInsideRect(RECT *rect);
        VOID  DrawFrame(HDC hdc,RECT *rect,BOOL dlgFrame,BOOL active);
        BOOL  DrawSysButton(HDC hdc,BOOL down);
        BOOL  DrawGrayButton(HDC hdc,int x,int y);
        VOID  DrawCloseButton(HDC hdc,BOOL down,BOOL bGrayed);
        VOID  DrawMaxButton(HDC hdc,BOOL down,BOOL bGrayed);
        VOID  DrawMinButton(HDC hdc,BOOL down,BOOL bGrayed);
        VOID  DrawCaption(HDC hdc,RECT *rect,BOOL active);
        VOID  DoNCPaint(HRGN clip,BOOL suppress_menupaint);
        LONG  HandleNCPaint(HRGN clip);
        LONG  HandleSysCommand(WPARAM wParam, POINT *pt32);

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
#else
friend BOOL  OS2ToWinMsgTranslate(void *pThdb, QMSG *os2Msg, MSG *winMsg, BOOL isUnicode, BOOL fTranslateExtraMsgs);
#endif

public:
         void SetFakeOpen32()    { WinSetDAXData (OS2Hwnd, &fakeWinBase); }
         void RemoveFakeOpen32() { WinSetDAXData (OS2Hwnd, NULL); }

         fakeOpen32WinBaseClass fakeWinBase;

         BOOL   isOwnDC() { return (windowClass && windowClass->getStyle() & CS_OWNDC_W); }
         HDC    getOwnDC() { return ownDC; }
         void   setOwnDC(HDC hdc) { ownDC = hdc; }
protected:
         HDC    ownDC;

         ULONG  EraseBkgndFlag:1,
                PSEraseFlag:1,
                SuppressEraseFlag:1,
                filler:29;
public:
         VOID   setEraseBkgnd (BOOL erase, BOOL PSErase = FALSE)
                    { EraseBkgndFlag = erase; PSEraseFlag = PSErase; }
         VOID   setSuppressErase (BOOL erase = FALSE)
                    { SuppressEraseFlag = erase; }
         BOOL   isEraseBkgnd()   { return EraseBkgndFlag; }
         BOOL   isPSErase()      { return EraseBkgndFlag | PSEraseFlag; }
         BOOL   isSuppressErase() { return SuppressEraseFlag; }
};

#endif //__cplusplus

#endif //__WIN32WNDBASE_H__
