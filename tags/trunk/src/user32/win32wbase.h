/* $Id: win32wbase.h,v 1.129 2001-10-03 22:34:32 sandervl Exp $ */
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
#include <winuser32.h>
#include <winres.h>
#include <scroll.h>

class Win32BaseWindow;

#define OFFSET_RESERVED           0 //reserved for odin apps (such as Opera)
#define OFFSET_WIN32WNDPTR        4
#define OFFSET_WIN32PM_MAGIC      8
#define OFFSET_WIN32FLAGS         12
#define NROF_WIN32WNDBYTES        16

#define WINDOWFLAG_ACTIVE	  1

#define WIN32PM_MAGIC           0x12345678
#define CheckMagicDword(a)      (a==WIN32PM_MAGIC)

#define GW_HWNDNEXTCHILD          (0x10000 | GW_HWNDNEXT)
#define GW_HWNDFIRSTCHILD         (0x10000 | GW_CHILD)
#define GW_HWNDLASTCHILD          (0x10000 | GW_HWNDLAST)

#ifdef DEBUG
#define RELEASE_WNDOBJ(a)       { a->release(__FUNCTION__, __LINE__); a = NULL; }
#else
#define RELEASE_WNDOBJ(a)       { a->release(); a = NULL; }
#endif

typedef struct {
        USHORT           cb;
        Win32BaseWindow *win32wnd;
        ULONG            win32CreateStruct;      //or dialog create dword
} CUSTOMWNDDATA;

typedef struct tagPROPERTY
{
    struct tagPROPERTY *next;     /* Next property in window list */
    HANDLE              handle;   /* User's data */
    LPSTR               string;   /* Property string (or atom) */
} PROPERTY;

//PostThreadMessage is done through Open32; which means the message id will be translated
//(0xc00 added)
#define OPEN32_MSGDIFF            0xC00
#define WIN32APP_POSTMSG          (0x1000+OPEN32_MSGDIFF)

//PM doesn't allow SetFocus during WM_SETFOCUS message processing; must delay
//this by posting a message
//NOTE Must be smaller than WIN32APP_POSTMSG!
#define WIN32APP_SETFOCUSMSG      (WIN32APP_POSTMSG-1)

#define WIN32MSG_MAGICA           0x12345678
#define WIN32MSG_MAGICW           0x12345679

typedef struct
{
        ULONG           wParam;
        ULONG           lParam;
} POSTMSG_PACKET;

#define BROADCAST_SEND          0
#define BROADCAST_POST          1

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_THICKFRAME)))

#define HAS_THICKFRAME(style,exStyle) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))
#if 0
    (((style) & WS_THICKFRAME) && \
     !((exStyle) & WS_EX_DLGMODALFRAME) && \
     !((style) & WS_CHILD))
#endif

#define HAS_THINFRAME(style) \
    (((style) & WS_BORDER) || !((style) & (WS_CHILD | WS_POPUP)))

#define HAS_BIGFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME))

#define HAS_ANYFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME | WS_BORDER)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME) || \
     !((style) & (WS_CHILD | WS_POPUP)))

#define HAS_3DFRAME(exStyle) \
    ((exStyle & WS_EX_CLIENTEDGE) || (exStyle & WS_EX_STATICEDGE) || (exStyle & WS_EX_WINDOWEDGE))

#define HAS_BORDER(style, exStyle) \
    ((style & WS_BORDER) || HAS_THICKFRAME(style) || HAS_DLGFRAME(style,exStyle))

#define IS_OVERLAPPED(style) \
    !(style & (WS_CHILD | WS_POPUP))

#define HAS_MENU() (!(getStyle() & WS_CHILD) && (GetMenu() != 0))

#define STATE_INIT	         0   //initial state
#define STATE_PRE_WMNCCREATE     1   //before WM_NCCREATE
#define STATE_POST_WMNCCREATE    2   //after WM_NCCREATE
#define STATE_PRE_WMCREATE       3   //before WM_CREATE
#define STATE_POST_WMCREATE      4   //after WM_CREATE
#define STATE_CREATED            5   //after successful return of WinCreateWindow
#define STATE_DESTROYED          6   //DestroyWindow called for window


class Win32BaseWindow : public GenericObject, public ChildWindow
{
public:
                Win32BaseWindow();
                Win32BaseWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
                Win32BaseWindow(HWND hwndOS2, ULONG reserved);

virtual        ~Win32BaseWindow();

virtual  ULONG  MsgCreate(HWND hwndOS2);
         ULONG  MsgQuit();
         ULONG  MsgClose();
         ULONG  MsgDestroy();
virtual  ULONG  MsgEnable(BOOL fEnable);
         ULONG  MsgShow(BOOL fShow);
         ULONG  MsgPosChanging(LPARAM lp);
         ULONG  MsgPosChanged(LPARAM lp);
         ULONG  MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd, HWND hwndOS2Win);
         ULONG  MsgChildActivate(BOOL fActivate);
         ULONG  MsgSetFocus(HWND hwnd);
         ULONG  MsgKillFocus(HWND hwnd);
         ULONG  MsgScroll(ULONG msg, ULONG scrollCode, ULONG scrollPos);
         ULONG  MsgButton(MSG *msg);
         ULONG  MsgMouseMove(MSG *msg);
         ULONG  MsgChar(MSG *msg);
         ULONG  MsgPaint(ULONG tmp1, BOOL select = TRUE);
         ULONG  MsgEraseBackGround(HDC hdc);
         ULONG  MsgNCPaint(PRECT pUpdateRect);
         ULONG  MsgFormatFrame(WINDOWPOS *lpWndPos);
         ULONG  DispatchMsgA(MSG *msg);
         ULONG  DispatchMsgW(MSG *msg);

         ULONG  MsgSetText(LPSTR lpsz, LONG cch);
         ULONG  MsgGetTextLength();
         void   MsgGetText(char *wndtext, ULONG textlength);
         VOID   updateWindowStyle(DWORD oldExStyle,DWORD oldStyle);

virtual  LONG   SetWindowLongA(int index, ULONG value, BOOL fUnicode = FALSE);
virtual  ULONG  GetWindowLongA(int index, BOOL fUnicode = FALSE);
virtual  WORD   SetWindowWord(int index, WORD value);
virtual  WORD   GetWindowWord(int index);

         DWORD  getStyle()                      { return dwStyle; };
         DWORD  getOldStyle()                   { return dwOldStyle; };
         void   setStyle(DWORD newstyle)        { dwStyle = newstyle; };
         DWORD  getExStyle()                    { return dwExStyle; };
         void   setExStyle(DWORD newexstyle)    { dwExStyle = newexstyle; };
         ULONG  getInstance()                   { return hInstance; };
         void   setInstance(ULONG newinstance)  { hInstance = newinstance; };
         HWND   getWindowHandle()               { return Win32Hwnd; };
         HWND   getOS2WindowHandle()            { return OS2Hwnd; };
         HWND   getOS2FrameWindowHandle()       { return OS2HwndFrame; };

 Win32WndClass *getWindowClass()                { return windowClass; };

         LONG   getLastHitTestVal()             { return lastHitTestVal; }

         DWORD  getWindowContextHelpId()        { return contextHelpId; };
         void   setWindowContextHelpId(DWORD id){ contextHelpId = id; };

         BOOL   isFrameWindow();
virtual  BOOL   isMDIClient();
virtual  BOOL   isMDIChild();
virtual  BOOL   isDesktopWindow();

         BOOL   fHasParentDC()                  { return fParentDC; };

Win32BaseWindow *getParent();
         void   setParent(Win32BaseWindow *pwindow) { setParentOfChild((ChildWindow *)pwindow); };
       WNDPROC  getWindowProc()                 { return win32wndproc; };
         void   setWindowProc(WNDPROC newproc)  { win32wndproc = newproc; };
        DWORD   getWindowId()                   { return dwIDMenu; };
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
                rect->right  -= rect->left;
                rect->bottom -= rect->top;
                rect->left = rect->top = 0;
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

         DWORD  getCBExtra()                    { return cbExtra; };
         PVOID  getExtraPtr()                   { return pExtra; };

         HMENU  GetMenu()                           { return dwIDMenu; };
         VOID   SetMenu(HMENU newMenu)              { dwIDMenu = newMenu; };
         void   SetSysMenu(HMENU hSystemMenu)       { hSysMenu = hSystemMenu; };
         HMENU  GetSysMenu()                        { return hSysMenu; }

         HICON  IconForWindow(WPARAM fType);

         void   SetWindowRegion(HRGN hRegion)       { hWindowRegion = hRegion; };
         HRGN   GetWindowRegion()                   { return hWindowRegion; };

         //Save old clip region for CS_OWNDC windows (in BeginPaint)
         HRGN   GetClipRegion()                   { return hClipRegion; };
         void   SetClipRegion(HRGN hRegion)       { hClipRegion = hRegion; };

         BOOL   ShowWindow(ULONG nCmdShow);
         BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags);
         BOOL   SetWindowPlacement(WINDOWPLACEMENT *winpos);
         BOOL   GetWindowPlacement(LPWINDOWPLACEMENT winpos);
         BOOL   ScrollWindow(int dx, int dy);
virtual  BOOL   DestroyWindow();
         HWND   SetActiveWindow();
         BOOL   DeactivateChildWindow();
         HWND   GetParent();
         HWND   SetParent(HWND hwndNewParent);

         BOOL   IsChild(HWND hwndParent);

         HWND   GetTopWindow();
         HWND   GetTopParent();

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
         BOOL   CanReceiveSizeMsgs()          { return state >= STATE_PRE_WMCREATE; };
         BOOL   IsParentChanging()            { return fParentChange; };
         BOOL   IsWindowCreated()             { return state >= STATE_PRE_WMNCCREATE; }
         BOOL   IsWindowDestroyed()           { return state >= STATE_DESTROYED; };
         BOOL   IsWindowIconic();
         //Window procedure type
         BOOL   IsWindowUnicode();
         BOOL   IsMixMaxStateChanging()       { return fMinMaxChange; };

         void   SetVisibleRegionChanged(BOOL changed) { fVisibleRegionChanged = changed; };
         BOOL   IsVisibleRegionChanged()              { return fVisibleRegionChanged; };
         BOOL   setVisibleRgnNotifyProc(VISRGN_NOTIFY_PROC lpNotifyProc, DWORD dwUserData)
         {
             lpVisRgnNotifyProc  = lpNotifyProc;
             dwVisRgnNotifyParam = dwUserData;
             return TRUE;
         }
         void   callVisibleRgnNotifyProc(BOOL fDrawingAllowed) 
         {
             if(lpVisRgnNotifyProc) {
                 lpVisRgnNotifyProc(getWindowHandle(), fDrawingAllowed, dwVisRgnNotifyParam);
             }
         }
         int    GetWindowTextLength(BOOL fUnicode);
         int    GetWindowTextLengthA() { return GetWindowTextLength(FALSE); };
         int    GetWindowTextLengthW() { return GetWindowTextLength(TRUE);  };

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

           HWND FindWindowById(int id);

    static HWND FindWindowEx(HWND hwndParent, HWND hwndChildAfter, ATOM atom, LPSTR lpszWindow);

           BOOL EnumChildWindows(WNDENUMPROC lpfn, LPARAM lParam);
           BOOL EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam);
           BOOL EnumWindows(WNDENUMPROC lpfn, LPARAM lParam);


	 BOOL   isComingToTop()			{ return fComingToTop; };
	 void   setComingToTop(BOOL fTop) 	{ fComingToTop = fTop; };
         BOOL   isInTasklist()                  { return fTaskList; };

         //window property methods
         HANDLE getProp(LPCSTR str);
	 BOOL   setProp(LPCSTR str, HANDLE handle);
	 HANDLE removeProp(LPCSTR str);
	 INT    enumPropsExA(PROPENUMPROCEXA func, LPARAM lParam);
	 INT    enumPropsExW(PROPENUMPROCEXW func, LPARAM lParam);

#ifdef DEBUG
         LONG addRef();
         LONG release(char *function = __FUNCTION__, int line = __LINE__ );
#endif

//Locates window in linked list and increases reference count (if found)
//Window object must be unreferenced after usage
static Win32BaseWindow *GetWindowFromHandle(HWND hwnd);
static Win32BaseWindow *GetWindowFromOS2Handle(HWND hwnd);
static Win32BaseWindow *GetWindowFromOS2FrameHandle(HWND hwnd);

    static void DestroyAll();

protected:
#ifndef OS2_INCLUDED
        BOOL  CreateWindowExA(CREATESTRUCTA *lpCreateStruct, ATOM classAtom);
#endif
       LRESULT  SendInternalMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendInternalMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
        void    Init();

        void    NotifyFrameChanged(WINDOWPOS *wpos, RECT *oldClientRect);

        //called in destructor to remove all (if any) window properties
        void    removeWindowProps();
    PROPERTY   *findWindowProperty(LPCSTR str);

        HWND    OS2Hwnd, OS2HwndFrame;
        HMENU   hSysMenu;
        HWND    Win32Hwnd;

        int     posx, posy, width, height;

        // values normally contained in the standard window words
        ULONG   dwExStyle;              //GWL_EXSTYLE
        ULONG   dwStyle;                //GWL_STYLE
        ULONG   dwOldStyle;             //Used to determine which frame control buttons to change (OS/2 mode)
      WNDPROC   win32wndproc;           //GWL_WNDPROC
        ULONG   hInstance;              //GWL_HINSTANCE
//Moved in ChildWindow class
/////   Win32BaseWindow *parent;                    //GWL_HWNDPARENT
        ULONG   dwIDMenu;               //GWL_ID
        ULONG   userData;               //GWL_USERDATA

        ULONG   cbExtra;
        PVOID   pExtra;

         HWND   hwndLinkAfter;
        DWORD   flags;
        DWORD   contextHelpId;
        DWORD   hotkey;
        LONG    lastHitTestVal;         //Last value returned by WM_NCHITTEST handler

        HWND    OS2HwndModalDialog;

        ULONG    fFirstShow:1,
                 fIsDialog:1,
                 fIsModalDialog:1,
                 fIsModalDialogOwner:1,
                 fInternalMsg:1,         //Used to distinguish between messages
                                         //sent by PM and those sent by apps
                 fParentChange:1,
                 fDestroyWindowCalled:1, //DestroyWindow was called for this window
                 fTaskList:1,            //should be listed in PM tasklist or not
                 fXDefault:1,
                 fCXDefault:1,
                 fParentDC:1,
	         fComingToTop:1,
                 isUnicode:1,
                 fMinMaxChange:1,        //set when switching between min/max/restored state
                 fVisibleRegionChanged:1, //set when visible region has changed -> erase background must be sent during next BeginPaint
                 fEraseBkgndFlag:1;

        ULONG   state;
        HRGN    hWindowRegion;
        HRGN    hClipRegion;

        DWORD   dwThreadId;             //id of thread that created this window
        DWORD   dwProcessId;            //id of process that created this window

   Win32BaseWindow *owner;
        HICON   hIcon,hIconSm;

        char   *windowNameA;
        WCHAR  *windowNameW;

        char   *userWindowBytes;
        ULONG   nrUserWindowBytes;

        RECT    rectWindow; //relative to parent
        RECT    rectClient;  //relative to frame
WINDOWPLACEMENT windowpos;

    PROPERTY   *propertyList;

VISRGN_NOTIFY_PROC lpVisRgnNotifyProc;
        DWORD   dwVisRgnNotifyParam;
       
        HANDLE  hTaskList; //PM specific (switchentry handle)

CREATESTRUCTA  *tmpcs; //temporary pointer to CREATESTRUCT used in CreateWindowEx
        ULONG   sw;    //set in CreateWindowExA, used in MsgCreate method

SCROLLBAR_INFO *vertScrollInfo;
SCROLLBAR_INFO *horzScrollInfo;

Win32WndClass  *windowClass;

static GenericObject *windows;
static CRITICAL_SECTION  critsect;

private:
#ifndef OS2_INCLUDED
        void  GetMinMaxInfo(POINT *maxSize, POINT *maxPos, POINT *minTrack, POINT *maxTrack );
        UINT  MinMaximize(UINT cmd, LPRECT lpRect);
        LONG  HandleWindowPosChanging(WINDOWPOS *winpos);
        LONG  HandleNCActivate(WPARAM wParam);
        VOID  TrackMinMaxHelpBox(WORD wParam);
        VOID  TrackCloseButton(WORD wParam);
        VOID  TrackScrollBar(WPARAM wParam,POINT pt);
        LONG  HandleNCLButtonDown(WPARAM wParam,LPARAM lParam);
        LONG  HandleNCLButtonDblClk(WPARAM wParam,LPARAM lParam);
        LONG  HandleNCRButtonUp(WPARAM wParam,LPARAM lParam);
        VOID  AdjustRectOuter(LPRECT rect,BOOL menu);
        VOID  AdjustRectInner(LPRECT rect);
        LONG  HandleNCCalcSize(BOOL calcValidRects,RECT *winRect);
        VOID  DrawFrame(HDC hdc,RECT *rect,BOOL dlgFrame,BOOL active);
public:
        VOID  GetInsideRect(RECT *rect);
        LONG  HandleNCHitTest(POINT pt);
        BOOL  GetSysPopupPos(RECT* rect);
private:
        BOOL  DrawSysButton(HDC hdc,RECT *rect);
        BOOL  DrawGrayButton(HDC hdc,int x,int y);
        VOID  DrawCloseButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed);
        VOID  DrawMaxButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed);
        VOID  DrawMinButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed);
        VOID  DrawContextHelpButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed);
        VOID  DrawCaption(HDC hdc,RECT *rect,BOOL active);
        VOID  DoNCPaint(HRGN clip,BOOL suppress_menupaint);
        LONG  HandleNCPaint(HRGN clip);
        LONG  HandleSysCommand(WPARAM wParam, POINT *pt32);

        LONG  SendNCCalcSize(BOOL calcValidRect,
                             RECT *newWindowRect, RECT *oldWindowRect,
                             RECT *oldClientRect, WINDOWPOS *winpos,
                             RECT *newClientRect );

#else
friend BOOL  OS2ToWinMsgTranslate(void *pThdb, QMSG *os2Msg, MSG *winMsg, BOOL isUnicode, BOOL fTranslateExtraMsgs);
#endif

public:
         void SetFakeOpen32()    { WinSetDAXData (OS2Hwnd, &fakeWinBase); }
         void RemoveFakeOpen32() { WinSetDAXData (OS2Hwnd, NULL); }

         fakeOpen32WinBaseClass fakeWinBase;

         VOID  AdjustMaximizedRect(LPRECT rect);
         VOID  AdjustTrackInfo(PPOINT minTrackSize,PPOINT maxTrackSize);

         //Temporary hack for CS_CLASSDC style (do the same as for CS_OWNDC)
#ifndef OS2_INCLUDED
         BOOL   isOwnDC() { return (windowClass && windowClass->getStyle() & (CS_OWNDC|CS_CLASSDC)); }
#else
         BOOL   isOwnDC() { return (windowClass && windowClass->getStyle() & (CS_OWNDC_W|CS_CLASSDC_W)); }
#endif

         HDC    getOwnDC() { return ownDC; }
         void   setOwnDC(HDC hdc) { ownDC = hdc; }
protected:
         HDC    ownDC;

public:
         VOID   setEraseBkgnd (BOOL erase)      { fEraseBkgndFlag = erase; }
         BOOL   needsEraseBkgnd()               { return fEraseBkgndFlag; }
};

#endif //__cplusplus

#endif //__WIN32WNDBASE_H__
