


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define PASCAL      _pascal
#define CALLBACK    _far _pascal
#define WINAPI      _far _pascal
#define FAR         _far
#define NEAR        _near

#define FALSE       0
#define TRUE        1

#if MSC < 700
#define __export
#endif

#ifndef NULL
#define NULL		    0
#endif

/* Show window */
#define SW_HIDE		    0
#define SW_SHOWNORMAL	    1
#define SW_NORMAL	    1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE	    3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW		    5
#define SW_MINIMIZE	    6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA	    8
#define SW_RESTORE          9

/* Message box */
#define MB_OK		    0x0000
#define MB_OKCANCEL	    0x0001
#define MB_ABORTRETRYIGNORE 0x0002
#define MB_YESNOCANCEL	    0x0003
#define MB_YESNO	    0x0004
#define MB_RETRYCANCEL	    0x0005
#define MB_TYPEMASK	    0x000F

#define MB_ICONHAND	    0x0010
#define MB_ICONQUESTION	    0x0020
#define MB_ICONEXCLAMATION  0x0030
#define MB_ICONASTERISK     0x0040
#define MB_ICONMASK	    0x00F0

#define MB_ICONINFORMATION  MB_ICONASTERISK
#define MB_ICONSTOP         MB_ICONHAND

#define MB_DEFBUTTON1	    0x0000
#define MB_DEFBUTTON2	    0x0100
#define MB_DEFBUTTON3	    0x0200
#define MB_DEFMASK	    0x0F00

#define MB_APPLMODAL	    0x0000
#define MB_SYSTEMMODAL	    0x1000
#define MB_TASKMODAL	    0x2000

#define MB_NOFOCUS	    0x8000


/* Brush */
#define WHITE_BRUSH	    0
#define LTGRAY_BRUSH	    1
#define GRAY_BRUSH	    2
#define DKGRAY_BRUSH	    3
#define BLACK_BRUSH	    4
#define NULL_BRUSH	    5
#define HOLLOW_BRUSH	    NULL_BRUSH


/* Window style */
#define WS_OVERLAPPED	    0x00000000L
#define WS_POPUP	    0x80000000L
#define WS_CHILD	    0x40000000L

#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L

#define WS_VISIBLE	    0x10000000L
#define WS_DISABLED	    0x08000000L

#define WS_MINIMIZE	    0x20000000L
#define WS_MAXIMIZE	    0x01000000L

#define WS_CAPTION	    0x00C00000L     /* WS_BORDER | WS_DLGFRAME	*/
#define WS_BORDER	    0x00800000L
#define WS_DLGFRAME	    0x00400000L
#define WS_VSCROLL	    0x00200000L
#define WS_HSCROLL	    0x00100000L
#define WS_SYSMENU	    0x00080000L
#define WS_THICKFRAME	    0x00040000L
#define WS_MINIMIZEBOX	    0x00020000L
#define WS_MAXIMIZEBOX	    0x00010000L

#define WS_GROUP	    0x00020000L
#define WS_TABSTOP	    0x00010000L

#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW	    (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW	    (WS_CHILD)

#define WS_EX_DLGMODALFRAME  0x00000001L
#define WS_EX_NOPARENTNOTIFY 0x00000004L

#define WS_EX_TOPMOST	     0x00000008L
#define WS_EX_ACCEPTFILES    0x00000010L
#define WS_EX_TRANSPARENT    0x00000020L

#define WS_TILED	    WS_OVERLAPPED
#define WS_ICONIC	    WS_MINIMIZE
#define WS_SIZEBOX	    WS_THICKFRAME
#define WS_TILEDWINDOW	    WS_OVERLAPPEDWINDOW


/* Create window flags */
#define CW_USEDEFAULT	    ((short)0x8000)


/* Window messages */
#define WM_CREATE	    0x0001
#define WM_TIMER	    0x0113
#define WM_DESTROY	    0x0002


/* Misc */
#define HINSTANCE_ERROR     ((HINSTANCE)32)
#define MAKELP(sel, off)    ((void FAR*)MAKELONG((off), (sel)))
#define SELECTOROF(lp)      HIWORD(lp)
#define OFFSETOF(lp)        LOWORD(lp)
#define FIELDOFFSET(type, field)    ((int)(&((type NEAR*)1)->field)-1)
#define LOBYTE(w)	    ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((UINT)(w) >> 8) & 0xFF))
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))


/* Global Memory */
#define GMEM_FIXED	    0x0000
#define GMEM_MOVEABLE	    0x0002
#define GMEM_NOCOMPACT	    0x0010
#define GMEM_NODISCARD	    0x0020
#define GMEM_ZEROINIT	    0x0040
#define GMEM_MODIFY	    0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE	    0x2000
#define GMEM_DDESHARE	    0x2000
#define GMEM_NOTIFY	    0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GHND		    (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR		    (GMEM_FIXED | GMEM_ZEROINIT)
#define GlobalDiscard(h)    GlobalReAlloc(h, 0L, GMEM_MOVEABLE)


/* Open file */
#define OF_READ 	    0x0000
#define OF_WRITE	    0x0001
#define OF_READWRITE	    0x0002
#define OF_SHARE_COMPAT	    0x0000
#define OF_SHARE_EXCLUSIVE  0x0010
#define OF_SHARE_DENY_WRITE 0x0020
#define OF_SHARE_DENY_READ  0x0030
#define OF_SHARE_DENY_NONE  0x0040
#define OF_PARSE	    0x0100
#define OF_DELETE	    0x0200
#define OF_VERIFY	    0x0400      /* Used with OF_REOPEN */
#define OF_SEARCH	    0x0400	/* Used without OF_REOPEN */
#define OF_CANCEL	    0x0800
#define OF_CREATE	    0x1000
#define OF_PROMPT	    0x2000
#define OF_EXIST	    0x4000
#define OF_REOPEN	    0x8000


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef unsigned char   BYTE;
typedef BYTE NEAR *     PBYTE;
typedef BYTE FAR *      LPBYTE;

typedef unsigned short  WORD;
typedef WORD NEAR *     LWORD;
typedef WORD FAR *      LPWORD;

typedef unsigned long   DWORD;
typedef DWORD NEAR *    PDWORD;
typedef DWORD FAR *     LPDWORD;

typedef signed long     LONG;
typedef LONG NEAR *     PLONG;
typedef LONG FAR *      LPLONG;
typedef unsigned long   ULONG;
typedef ULONG NEAR *	PULONG;
typedef ULONG FAR *     LPULONG;
typedef unsigned short  UINT;
typedef UINT NEAR *     PUINT;
typedef UINT FAR *      LPUINT;

typedef char NEAR *     PSTR;
typedef char FAR *      LPSTR;
typedef const char NEAR*PCSTR;
typedef const char FAR *LPCSTR;

typedef void            VOID;
typedef void NEAR *     PVOID;
typedef void FAR *      LPVOID;

typedef int             BOOL;
typedef int NEAR *      PBOOL;
typedef int FAR *       LPBOOL;

typedef UINT            WPARAM;
typedef LONG            LPARAM;
typedef LONG            LRESULT;

typedef UINT            ATOM;

typedef UINT            HANDLE;
typedef UINT            HINSTANCE;
typedef HINSTANCE       HMODULE;
typedef UINT            HGLOBAL;
typedef UINT            HWND;
typedef UINT            HICON;
typedef UINT            HBRUSH;
typedef UINT            HCURSOR;
typedef UINT            HGDIOBJ;
typedef UINT            HMENU;

typedef int             HFILE;

typedef int (CALLBACK*      FARPROC)();
typedef int (NEAR PASCAL*   NEARPROC)();
typedef FARPROC TIMERPROC;
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);


typedef struct tagPOINT
{
    int x;
    int y;
} POINT;
typedef POINT *         PPOINT;
typedef POINT NEAR *    NPPOINT;
typedef POINT FAR *     LPPOINT;


typedef struct tagMSG
{
    HWND	hwnd;
    UINT        message;
    WPARAM	wParam;
    LPARAM	lParam;
    DWORD       time;
    POINT	pt;
} MSG;
typedef MSG *           PMSG;
typedef MSG NEAR *      NPMSG;
typedef MSG FAR *       LPMSG;


typedef struct tagWNDCLASS
{
    UINT        style;
    WNDPROC	lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE	hInstance;
    HICON	hIcon;
    HCURSOR	hCursor;
    HBRUSH	hbrBackground;
    LPCSTR	lpszMenuName;
    LPCSTR	lpszClassName;
} WNDCLASS;
typedef WNDCLASS *      PWNDCLASS;
typedef WNDCLASS NEAR * NPWNDCLASS;
typedef WNDCLASS FAR *  LPWNDCLASS;


typedef struct tagOFSTRUCT
{
    BYTE cBytes;
    BYTE fFixedDisk;
    UINT nErrCode;
    BYTE reserved[4];
    char szPathName[128];
} OFSTRUCT;
typedef OFSTRUCT *      POFSTRUCT;
typedef OFSTRUCT NEAR * NPOFSTRUCT;
typedef OFSTRUCT FAR *  LPOFSTRUCT;




/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
int         WINAPI  GetProfileString(LPCSTR, LPCSTR, LPCSTR, LPSTR, int);
int         WINAPI  MessageBox(HWND, LPCSTR, LPCSTR, UINT);
void        WINAPI  MessageBeep(UINT);
ATOM        WINAPI  RegisterClass(const WNDCLASS FAR*);
BOOL        WINAPI  UnregisterClass(LPCSTR, HINSTANCE);
UINT        WINAPI  SetTimer(HWND, UINT, UINT, TIMERPROC);
BOOL        WINAPI  KillTimer(HWND, UINT);
HINSTANCE   WINAPI  LoadModule(LPCSTR, LPVOID);
BOOL        WINAPI  FreeModule(HINSTANCE);
HINSTANCE   WINAPI  LoadLibrary(LPCSTR);
void        WINAPI  FreeLibrary(HINSTANCE);
UINT        WINAPI  WinExec(LPCSTR, UINT);
HMODULE     WINAPI  GetModuleHandle(LPCSTR);
int         WINAPI  GetModuleUsage(HINSTANCE);
int         WINAPI  GetModuleFileName(HINSTANCE, LPSTR, int);
FARPROC     WINAPI  GetProcAddress(HINSTANCE, LPCSTR);
int         WINAPI  GetInstanceData(HINSTANCE, BYTE NEAR *, int);
HGLOBAL     WINAPI  GetCodeHandle(FARPROC);
BOOL        WINAPI  GetMessage(MSG FAR*, HWND, UINT, UINT);
BOOL        WINAPI  PeekMessage(MSG FAR*, HWND, UINT, UINT, UINT);
BOOL        WINAPI  TranslateMessage(const MSG FAR*);
LONG        WINAPI  DispatchMessage(const MSG FAR*);
BOOL        WINAPI  PostMessage(HWND, UINT, WPARAM, LPARAM);
LRESULT     WINAPI  SendMessage(HWND, UINT, WPARAM, LPARAM);
DWORD       WINAPI  GetVersion(void);
HGLOBAL     WINAPI  GlobalAlloc(UINT, DWORD);
HGLOBAL     WINAPI  GlobalReAlloc(HGLOBAL, DWORD, UINT);
HGLOBAL     WINAPI  GlobalFree(HGLOBAL);
DWORD       WINAPI  GlobalDosAlloc(DWORD);
UINT        WINAPI  GlobalDosFree(UINT);
char FAR*   WINAPI  GlobalLock(HGLOBAL);
BOOL        WINAPI  GlobalUnlock(HGLOBAL);
DWORD       WINAPI  GlobalSize(HGLOBAL);
DWORD       WINAPI  GlobalHandle(UINT);
HFILE       WINAPI  OpenFile(LPCSTR, OFSTRUCT FAR*, UINT);
BOOL        WINAPI  DestroyWindow(HWND);
HGDIOBJ     WINAPI  GetStockObject(int);
HWND        WINAPI  CreateWindow(LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void FAR*);
void        WINAPI  PostQuitMessage(int);
LRESULT     WINAPI  DefWindowProc(HWND, UINT, WPARAM, LPARAM);
UINT        WINAPI  AllocSelector(UINT);
UINT        WINAPI  FreeSelector(UINT);
UINT        WINAPI  AllocDStoCSAlias(UINT);
UINT        WINAPI  PrestoChangoSelector(UINT sourceSel, UINT destSel);
DWORD       WINAPI  GetSelectorBase(UINT);
UINT        WINAPI  SetSelectorBase(UINT, DWORD);
DWORD       WINAPI  GetSelectorLimit(UINT);
UINT        WINAPI  SetSelectorLimit(UINT, DWORD);
UINT        WINAPI  _lread(HFILE, void _huge*, UINT);
UINT        WINAPI  _lwrite(HFILE, const void _huge*, UINT);
HFILE       WINAPI  _lopen(LPCSTR, int);
HFILE       WINAPI  _lcreat(LPCSTR, int);
HFILE       WINAPI  _lclose(HFILE);
LONG        WINAPI  _llseek(HFILE, LONG, int);
DWORD       WINAPI  GetTickCount(void);
void        WINAPI  Yield(void);



