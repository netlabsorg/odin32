#ifndef _OS2WIN_H
  #define _OS2WIN_H

#ifdef OS2_INCLUDED
  #include <win32type.h>
#else
  #include <odin.h>

  #include <windef.h>
  #include <winbase.h>
  #include <wingdi.h>
  #include <winuser.h>
  #include <winerror.h>
  #include <commdlg.h>
  #include <ddeml.h>
  #include <mmsystem.h>
#endif

#include <os2sel.h>

#include "misc.h"

#define OPEN32API       _System
#define EXPENTRY_O32    _System
#define APIENTRY_O32    _System

/* WinMain/WinProcs/TimerProcs
 */
typedef int             (* EXPENTRY_O32 FNMAIN_O32 )( HINSTANCE, HINSTANCE, LPSTR, int );
typedef FNMAIN_O32      PFNMAIN_O32;
typedef LRESULT         (* EXPENTRY_O32 WNDPROC_O32 ) ( HWND, UINT, WPARAM, LPARAM );
typedef int             (* EXPENTRY_O32 FARPROC_O32 ) ( );
typedef UINT            (* EXPENTRY_O32 LPOFNHOOKPROC_O32 ) ( HWND, UINT, WPARAM, LPARAM );
typedef BOOL            (* EXPENTRY_O32 DLGPROC_O32 ) ( HWND, UINT, WPARAM, LPARAM );
typedef void            (* EXPENTRY_O32 TIMERPROC_O32)( HWND, UINT, UINT, DWORD );
typedef BOOL            (* EXPENTRY_O32 ABORTPROC_O32)( HDC, int);
typedef UINT            (* APIENTRY_O32 LPPRINTHOOKPROC_O32) (HWND, UINT, WPARAM, LPARAM);
typedef UINT            (* APIENTRY_O32 LPSETUPHOOKPROC_O32) (HWND, UINT, WPARAM, LPARAM);
typedef LRESULT         (* EXPENTRY_O32 HOOKPROC_O32)(int code, WPARAM wParam, LPARAM lParam);
typedef DWORD           (* EXPENTRY_O32 PTHREAD_START_ROUTINE_O32)(PVOID lpThreadParameter);
typedef DWORD           (* EXPENTRY_O32 LPTHREAD_START_ROUTINE_O32)(PVOID lpThreadParameter);

typedef int             (* EXPENTRY_O32 GOBJENUMPROC_O32)(PVOID, LPARAM);
typedef VOID            (* EXPENTRY_O32 LINEDDAPROC_O32)(int, int, LPARAM);
typedef int             (* EXPENTRY_O32 OLDFONTENUMPROC_O32)( PLOGFONTA, LPTEXTMETRICA, DWORD, LPARAM );
typedef int             (* EXPENTRY_O32 FONTENUMPROC_O32)( LPENUMLOGFONTA, LPNEWTEXTMETRICA, DWORD, LPARAM );
typedef BOOL            (* EXPENTRY_O32 WNDENUMPROC_O32)( HWND, LPARAM );
typedef BOOL            (* EXPENTRY_O32 PROPENUMPROC_O32)( HWND, LPCSTR, HANDLE );
typedef UINT            (* EXPENTRY_O32 PFRHOOKPROC_O32) (HWND, UINT, WPARAM, LPARAM);
typedef BOOL            (* EXPENTRY_O32 GRAYSTRINGPROC_O32)(HDC, LPARAM, int);
typedef UINT            (* EXPENTRY_O32 LPCCHOOKPROC_O32) (HWND, UINT, WPARAM, LPARAM);
typedef UINT            (* EXPENTRY_O32 LPCFHOOKPROC_O32) (HWND, UINT, WPARAM, LPARAM);
typedef HDDEDATA        (* EXPENTRY_O32 FNCALLBACK_O32)(UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD );
typedef BOOL            (* EXPENTRY_O32 PROPENUMPROCEX_O32)( HWND, LPCSTR, HANDLE, DWORD );
typedef FNCALLBACK_O32   *PFNCALLBACK_O32;

typedef int (* EXPENTRY_O32 ENHMFENUMPROC_O32)(HDC, LPHANDLETABLE, const PENHMETARECORD, int, LPARAM);
typedef int (* EXPENTRY_O32 MFENUMPROC_O32)(HDC, LPHANDLETABLE, PMETARECORD, int, LPARAM);

#define HKEY_LOCAL_MACHINE_O32      0xFFFFFFEFL
#define HKEY_CURRENT_USER_O32       0xFFFFFFEEL
#define HKEY_USERS_O32              0xFFFFFFEDL
#define HKEY_CLASSES_ROOT_O32       0xFFFFFFECL

#define O32_MakeProcInstance(proc, hinst) (proc)
#define O32_FreeProcInstance(proc) (proc)
#define O32_FreeModule(hLibModule) O32_FreeLibrary((hLibModule))
#define O32_Yield()

#define O32_GetWindowTask(hWnd) ((HANDLE)O32_GetWindowThreadProcessId(hWnd, NULL))


/* Macro'd APIs
 */
#define O32_CreateWindow(lpClassName, lpWindowName, dwStyle, x, y,\
                        nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
        O32_CreateWindowEx(0, lpClassName, lpWindowName, dwStyle, x, y,\
                          nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

/* TBD: This must be removed before release
 */
#ifndef NO_HEAPSIZE_MACRO
#define _O32_HeapSize(handle, flags, pMem) AweHeapSize(handle, flags, pMem)
#endif

#define O32_AnsiToOem     O32_CharToOem
#define O32_OemToAnsi     O32_OemToChar
#define O32_AnsiToOemBuff O32_CharToOemBuff
#define O32_OemToAnsiBuff O32_OemToCharBuff
#define O32_AnsiUpper     O32_CharUpper
#define O32_AnsiUpperBuff O32_CharUpperBuff
#define O32_AnsiLower     O32_CharLower
#define O32_AnsiLowerBuff O32_CharLowerBuff
#define O32_AnsiNext      O32_CharNext
#define O32_AnsiPrev      O32_CharPrev

/* Name Mapped API's
 */
#define _O32_CreateBitmapIndirect            _O32__CreateBitmapIndirect
#define _O32_CreateDialogIndirect            _O32__CreateDialogIndirect
#define _O32_CreateDialogIndirectParam       _O32__CreateDialogIndirectParam
#define _O32_CreateIconIndirect              _O32__CreateIconIndirect
#define _O32_DialogBoxIndirect               _O32__DialogBoxIndirect
#define _O32_DialogBoxIndirectParam          _O32__DialogBoxIndirectParam
#define _O32_LoadMenuIndirect                _O32__LoadMenuIndirect
#define _O32_LoadResource                    _O32__LoadResource

/* API Prototypes
 */
int     OPEN32API _O32_WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int     OPEN32API _O32_AbortDoc( HDC );

BOOL    OPEN32API _O32_AbortPath( HDC );

ATOM    OPEN32API _O32_AddAtom( LPCSTR );

int     OPEN32API _O32_AddFontResource( LPCSTR );

BOOL    OPEN32API _O32_AdjustWindowRect( PRECT, DWORD, BOOL );

BOOL    OPEN32API _O32_AdjustWindowRectEx( PRECT, DWORD, BOOL, DWORD );

BOOL    OPEN32API _O32_AngleArc( HDC, int, int, DWORD, float startAngle, float sweepAngle );

BOOL    OPEN32API _O32_AnimatePalette( HPALETTE, UINT, UINT, const PALETTEENTRY *);

BOOL    OPEN32API _O32_AppendMenu( HMENU, UINT, UINT, LPCSTR );

BOOL    OPEN32API _O32_Arc( HDC, int, int, int, int, int, int, int, int );

BOOL    OPEN32API _O32_ArcTo( HDC, int, int, int, int, int xRad1, int yRad1, int xRad2, int yRad2);

UINT    OPEN32API _O32_ArrangeIconicWindows( HWND );

BOOL    OPEN32API _O32_Beep( DWORD, DWORD );

HDWP    OPEN32API _O32_BeginDeferWindowPos( int );

HDC     OPEN32API _O32_BeginPaint( HWND, PPAINTSTRUCT );

BOOL    OPEN32API _O32_BeginPath( HDC );

BOOL    OPEN32API _O32_BitBlt( HDC , int, int, int, int, HDC , int, int, DWORD );

BOOL    OPEN32API _O32_BringWindowToTop( HWND );

BOOL    OPEN32API _O32_CallMsgFilter( LPMSG, int );

LRESULT OPEN32API _O32_CallNextHookEx( HHOOK, int, WPARAM, LPARAM );

LRESULT OPEN32API _O32_CallWindowProc( WNDPROC_O32, HWND, UINT, WPARAM, LPARAM );

BOOL    OPEN32API _O32_ChangeClipboardChain( HWND, HWND );

LPSTR   OPEN32API _O32_CharLower( LPSTR );

DWORD   OPEN32API _O32_CharLowerBuff( LPSTR, DWORD );

LPSTR   OPEN32API _O32_CharUpper( LPSTR );

DWORD   OPEN32API _O32_CharUpperBuff( LPSTR, DWORD );

BOOL    OPEN32API _O32_CheckDlgButton( HWND, int, UINT );

DWORD   OPEN32API _O32_CheckMenuItem( HMENU, UINT, UINT );

BOOL    OPEN32API _O32_CheckRadioButton( HWND, int, int, int );

HWND    OPEN32API _O32_ChildWindowFromPoint( HWND, POINT );

BOOL    OPEN32API _O32_ChooseColor( LPCHOOSECOLORA );

BOOL    OPEN32API _O32_ChooseFont( LPCHOOSEFONTA );

BOOL    OPEN32API _O32_Chord( HDC , int, int, int, int, int xRadial1, int yRadial1, int xRadial2, int yRadial2);

BOOL    OPEN32API _O32_ClientToScreen( HWND, PPOINT );

BOOL    OPEN32API _O32_ClipCursor( const RECT *);

BOOL    OPEN32API _O32_CloseClipboard( VOID );

HENHMETAFILE OPEN32API _O32_CloseEnhMetaFile( HDC );

BOOL    OPEN32API _O32_CloseFigure( HDC );

BOOL    OPEN32API _O32_CloseHandle( HANDLE );

HMETAFILE OPEN32API _O32_CloseMetaFile( HDC );

BOOL    OPEN32API _O32_CloseWindow( HWND );

int       OPEN32API _O32_CombineRgn( HRGN, HRGN hrgnSrc1, HRGN hrgnSrc2, int );

DWORD   OPEN32API _O32_CommDlgExtendedError( VOID );

LONG    OPEN32API _O32_CompareFileTime( FILETIME *, FILETIME * );

HCURSOR OPEN32API _O32_CopyCursor( HCURSOR );

HENHMETAFILE OPEN32API _O32_CopyEnhMetaFile( HENHMETAFILE, LPCSTR );

BOOL    OPEN32API _O32_CopyFile( LPCSTR, LPCSTR, BOOL );

HICON   OPEN32API _O32_CopyIcon( HICON );

HMETAFILE OPEN32API _O32_CopyMetaFile( HMETAFILE, LPCSTR );

BOOL    OPEN32API _O32_CopyRect( PRECT, const RECT *);

int       OPEN32API _O32_CountClipboardFormats( VOID );

HACCEL  OPEN32API _O32_CreateAcceleratorTable( LPACCEL, int );

HBITMAP OPEN32API _O32_CreateBitmap( int, int, UINT, UINT, const void * );

HBITMAP OPEN32API _O32_CreateBitmapIndirect( const BITMAP * );

HBRUSH  OPEN32API _O32_CreateBrushIndirect( LPLOGBRUSH );

BOOL    OPEN32API _O32_CreateCaret( HWND, HBITMAP, int, int );

HBITMAP OPEN32API _O32_CreateCompatibleBitmap( HDC , int, int );

HDC     OPEN32API _O32_CreateCompatibleDC( HDC );

HCURSOR OPEN32API _O32_CreateCursor( HINSTANCE, int, int, int, int, const VOID *, const VOID *);

HDC     OPEN32API _O32_CreateDC( LPCSTR, LPCSTR, LPCSTR, const DEVMODEA *);

HBRUSH  OPEN32API _O32_CreateDIBPatternBrushPt( const VOID *, UINT );

HBITMAP OPEN32API _O32_CreateDIBitmap( HDC, const BITMAPINFOHEADER *, DWORD, const void *, const BITMAPINFO *, UINT );

HWND    OPEN32API _O32_CreateDialog( HINSTANCE, LPCSTR, HWND, DLGPROC_O32 );

HWND    OPEN32API _O32_CreateDialogParam( HINSTANCE, LPCSTR, HWND, DLGPROC_O32, LPARAM );

HWND    OPEN32API _O32_CreateDialogIndirect( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC_O32 );

HWND    OPEN32API _O32_CreateDialogIndirectParam( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC_O32, LPARAM );

BOOL    OPEN32API _O32_CreateDirectory( LPCSTR, PSECURITY_ATTRIBUTES );

HRGN    OPEN32API _O32_CreateEllipticRgn( int, int, int, int );

HRGN    OPEN32API _O32_CreateEllipticRgnIndirect( const RECT * );

HENHMETAFILE OPEN32API _O32_CreateEnhMetaFile( HDC, LPCSTR, const RECT *, LPCSTR );

HANDLE  OPEN32API _O32_CreateEvent( PSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR );

HANDLE  OPEN32API _O32_CreateFile( LPCSTR, DWORD, DWORD, PSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE );

HFONT   OPEN32API _O32_CreateFont( int, int, int, int, int, DWORD, DWORD, DWORD, DWORD,
                                  DWORD, DWORD, DWORD, DWORD, LPCSTR );

HFONT   OPEN32API _O32_CreateFontIndirect( const  LOGFONTA *);

HBRUSH  OPEN32API _O32_CreateHatchBrush( int, COLORREF );

HDC     OPEN32API _O32_CreateIC( LPCSTR, LPCSTR, LPCSTR, const DEVMODEA *);

HICON   OPEN32API _O32_CreateIcon( HINSTANCE, int, int, BYTE, BYTE, const BYTE *, const BYTE * );

HICON   OPEN32API _O32_CreateIconFromResource( PBYTE, DWORD, BOOL, DWORD );

HICON   OPEN32API _O32_CreateIconIndirect( LPICONINFO );

HMENU   OPEN32API _O32_CreateMenu( void);

HDC     OPEN32API _O32_CreateMetaFile( LPCSTR );

HWND    OPEN32API _O32_CreateMDIWindow( LPSTR, LPSTR, DWORD, int, int, int, int, HWND, HINSTANCE, LPARAM );

HANDLE  OPEN32API _O32_CreateMutex( PSECURITY_ATTRIBUTES, BOOL, LPCSTR );

HPALETTE OPEN32API _O32_CreatePalette( const LOGPALETTE * );

HBRUSH  OPEN32API _O32_CreatePatternBrush( HBITMAP );

HPEN    OPEN32API _O32_CreatePen( int, int, COLORREF );

HPEN    OPEN32API _O32_CreatePenIndirect( const LOGPEN * );

HRGN    OPEN32API _O32_CreatePolyPolygonRgn( const POINT *, const INT *, int, int );

HRGN    OPEN32API _O32_CreatePolygonRgn( const POINT *, int, int );

HMENU   OPEN32API _O32_CreatePopupMenu( VOID );

BOOL    OPEN32API _O32_CreateProcess( LPCSTR, LPCSTR, PSECURITY_ATTRIBUTES, PSECURITY_ATTRIBUTES,
                         BOOL, DWORD, PVOID, LPCSTR, LPSTARTUPINFOA,
                         LPPROCESS_INFORMATION );

HRGN    OPEN32API _O32_CreateRectRgn( int, int, int, int );

HRGN    OPEN32API _O32_CreateRectRgnIndirect( const RECT * );

HRGN    OPEN32API _O32_CreateRoundRectRgn( int, int, int, int, int, int );

HANDLE  OPEN32API _O32_CreateSemaphore( PSECURITY_ATTRIBUTES, LONG, LONG, LPSTR );

HBRUSH  OPEN32API _O32_CreateSolidBrush( COLORREF );

HANDLE  OPEN32API _O32_CreateThread( PSECURITY_ATTRIBUTES, DWORD, PTHREAD_START_ROUTINE_O32, PVOID, DWORD, PDWORD );

HWND    OPEN32API _O32_CreateWindowEx( DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID );

BOOL    OPEN32API _O32_DPtoLP( HDC, PPOINT, int );

BOOL    OPEN32API _O32_DdeAbandonTransaction( DWORD, HCONV, DWORD );

PBYTE   OPEN32API _O32_DdeAccessData(HDDEDATA, PDWORD );

HDDEDATA OPEN32API _O32_DdeAddData( HDDEDATA, PVOID, DWORD, DWORD );

HDDEDATA OPEN32API _O32_DdeClientTransaction( PVOID, DWORD, HCONV,
                          HSZ, UINT, UINT, DWORD, PDWORD );

int       OPEN32API _O32_DdeCmpStringHandles( HSZ hsz1, HSZ hsz2);

HCONV   OPEN32API _O32_DdeConnect( DWORD, HSZ, HSZ, LPCONVCONTEXT );

HCONVLIST OPEN32API _O32_DdeConnectList(DWORD, HSZ, HSZ, HCONVLIST, LPCONVCONTEXT );

HDDEDATA OPEN32API _O32_DdeCreateDataHandle(DWORD, PVOID, DWORD, DWORD, HSZ, UINT, UINT );

HSZ     OPEN32API _O32_DdeCreateStringHandle(DWORD, LPCSTR, int );

BOOL    OPEN32API _O32_DdeDisconnect(HCONV );

BOOL    OPEN32API _O32_DdeDisconnectList( HCONVLIST );

BOOL    OPEN32API _O32_DdeEnableCallback(DWORD, HCONV, UINT );

BOOL    OPEN32API _O32_DdeFreeDataHandle( HDDEDATA );

BOOL    OPEN32API _O32_DdeFreeStringHandle(DWORD, HSZ );

DWORD   OPEN32API _O32_DdeGetData( HDDEDATA, PVOID, DWORD, DWORD );

UINT    OPEN32API _O32_DdeGetLastError(DWORD );

UINT    OPEN32API _O32_DdeInitialize(PDWORD, PFNCALLBACK_O32, DWORD, DWORD );

BOOL    OPEN32API _O32_DdeKeepStringHandle(DWORD, HSZ );

HDDEDATA OPEN32API _O32_DdeNameService( DWORD, HSZ hsz1, HSZ hsz2, UINT );

BOOL    OPEN32API _O32_DdePostAdvise(DWORD, HSZ, HSZ );

UINT    OPEN32API _O32_DdeQueryConvInfo( HCONV, DWORD, LPCONVINFO );

HCONV   OPEN32API _O32_DdeQueryNextServer( HCONVLIST, HCONV );

DWORD   OPEN32API _O32_DdeQueryString(DWORD, HSZ, LPSTR, DWORD, int );

HCONV   OPEN32API _O32_DdeReconnect( HCONV );

BOOL    OPEN32API _O32_DdeSetUserHandle( HCONV, DWORD, DWORD );

BOOL    OPEN32API _O32_DdeUnaccessData( HDDEDATA );

BOOL    OPEN32API _O32_DdeUninitialize(DWORD );

LRESULT OPEN32API _O32_DefDlgProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HDWP    OPEN32API _O32_DeferWindowPos( HDWP, HWND, HWND hWndInsertAfter , int, int, int, int, UINT );

LRESULT OPEN32API _O32_DefFrameProc( HWND, HWND, UINT, WPARAM, LPARAM );

LRESULT OPEN32API _O32_DefMDIChildProc( HWND, UINT, WPARAM, LPARAM );

LRESULT OPEN32API _O32_DefWindowProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

ATOM    OPEN32API _O32_DeleteAtom( ATOM );

VOID    OPEN32API _O32_DeleteCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API _O32_DeleteDC( HDC );

BOOL    OPEN32API _O32_DeleteEnhMetaFile( HENHMETAFILE );

BOOL    OPEN32API _O32_DeleteFile( LPCSTR );

BOOL    OPEN32API _O32_DeleteMenu( HMENU, UINT, UINT );

BOOL    OPEN32API _O32_DeleteMetaFile( HMETAFILE );

BOOL    OPEN32API _O32_DeleteObject( HANDLE );

BOOL    OPEN32API _O32_DestroyAcceleratorTable( HACCEL );

BOOL    OPEN32API _O32_DestroyCaret( VOID );

BOOL    OPEN32API _O32_DestroyCursor( HCURSOR );

BOOL    OPEN32API _O32_DestroyIcon( HICON );

BOOL    OPEN32API _O32_DestroyMenu( HMENU );

BOOL    OPEN32API _O32_DestroyWindow( HWND );

DWORD   OPEN32API _O32_DeviceCapabilities( LPCSTR, LPCSTR, WORD, LPTSTR, const DEVMODEA *);

int       OPEN32API _O32_DialogBox( HINSTANCE, LPCSTR, HWND, DLGPROC_O32 );

int       OPEN32API _O32_DialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC_O32, LPARAM );

BOOL    OPEN32API _O32_DialogBoxIndirect( HINSTANCE, LPDLGTEMPLATEA, HWND, DLGPROC_O32 );

BOOL    OPEN32API _O32_DialogBoxIndirectParam( HINSTANCE, LPDLGTEMPLATEA, HWND, DLGPROC_O32, LPARAM );

LONG    OPEN32API _O32_DispatchMessage( const MSG * );

int       OPEN32API _O32_DlgDirList( HWND, LPSTR, int, int, UINT );

int       OPEN32API _O32_DlgDirListComboBox( HWND, LPSTR, int, int, UINT );

BOOL    OPEN32API _O32_DlgDirSelectEx( HWND, LPSTR, int, int );

BOOL    OPEN32API _O32_DlgDirSelectComboBoxEx( HWND, LPSTR, int, int );

BOOL    OPEN32API _O32_DllEntryPoint( HINSTANCE, DWORD, LPVOID );

BOOL    OPEN32API _O32_DosDateTimeToFileTime( WORD, WORD, LPFILETIME );

VOID    OPEN32API _O32_DragAcceptFiles( HWND, BOOL );

VOID    OPEN32API _O32_DragFinish( HDROP );

UINT    OPEN32API _O32_DragQueryFile( HDROP, int, LPSTR, int );

BOOL    OPEN32API _O32_DragQueryPoint( HDROP, PPOINT );

BOOL    OPEN32API _O32_DrawFocusRect( HDC, const RECT * );

BOOL    OPEN32API _O32_DrawIcon( HDC, int, int, HICON );

BOOL    OPEN32API _O32_DrawMenuBar( HWND );

int       OPEN32API _O32_DrawText( HDC, LPCSTR, int, PRECT, UINT );

BOOL    OPEN32API _O32_DuplicateHandle( HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD, BOOL, DWORD );

BOOL    OPEN32API _O32_Ellipse( HDC, int, int, int, int );

BOOL    OPEN32API _O32_EmptyClipboard( void);

BOOL    OPEN32API _O32_EnableMenuItem( HMENU, UINT, UINT );

BOOL    OPEN32API _O32_EnableScrollBar( HWND, UINT, UINT );

BOOL    OPEN32API _O32_EnableWindow( HWND, BOOL );

BOOL    OPEN32API _O32_EndDeferWindowPos( HDWP );

BOOL    OPEN32API _O32_EndDialog( HWND, int );

int       OPEN32API _O32_EndDoc( HDC );

int       OPEN32API _O32_EndPage( HDC );

BOOL    OPEN32API _O32_EndPath( HDC );

BOOL    OPEN32API _O32_EndPaint( HWND, const PAINTSTRUCT *);

VOID    OPEN32API _O32_EnterCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API _O32_EnumChildWindows( HWND, WNDENUMPROC_O32, LPARAM );

UINT    OPEN32API _O32_EnumClipboardFormats( UINT );

BOOL    OPEN32API _O32_EnumEnhMetaFile( HDC , HENHMETAFILE, ENHMFENUMPROC_O32, PVOID, const RECT * );

int       OPEN32API _O32_EnumFonts( HDC , LPCSTR, OLDFONTENUMPROC_O32, LPARAM );

int       OPEN32API _O32_EnumFontFamilies( HDC , LPCSTR, FONTENUMPROC_O32, LPARAM );

BOOL    OPEN32API _O32_EnumMetaFile( HDC , HMETAFILE, MFENUMPROC_O32, LPARAM );

int       OPEN32API _O32_EnumObjects( HDC , int, GOBJENUMPROC_O32, LPARAM );

BOOL    OPEN32API _O32_EnumPrinters( DWORD, LPTSTR, DWORD, LPBYTE, DWORD, LPDWORD, LPDWORD);

int       OPEN32API _O32_EnumProps( HWND, PROPENUMPROC_O32 );

int       OPEN32API _O32_EnumPropsEx( HWND, PROPENUMPROCEX_O32, LPARAM );

BOOL    OPEN32API _O32_EnumThreadWindows( DWORD, WNDENUMPROC_O32, LPARAM );

BOOL    OPEN32API _O32_EnumWindows( WNDENUMPROC_O32, LPARAM );

BOOL    OPEN32API _O32_EqualRect( const RECT *, const RECT * );

BOOL    OPEN32API _O32_EqualRgn( HRGN, HRGN );

int       OPEN32API _O32_Escape( HDC, int, int, LPCSTR, PVOID );

int       OPEN32API _O32_ExcludeClipRect( HDC, int, int, int, int );

BOOL    OPEN32API _O32_ExcludeUpdateRgn( HDC, HWND );

VOID    OPEN32API _O32_ExitProcess( UINT );

VOID    OPEN32API _O32_ExitThread( DWORD );

BOOL    OPEN32API _O32_ExitWindows( DWORD, UINT );

BOOL    OPEN32API _O32_ExitWindowsEx( UINT, DWORD );

HPEN    OPEN32API _O32_ExtCreatePen( DWORD, DWORD, const LOGBRUSH *, DWORD, const DWORD * );

HRGN    OPEN32API _O32_ExtCreateRegion( const XFORM *, DWORD, const RGNDATA * );

BOOL    OPEN32API _O32_ExtFloodFill( HDC, int, int, COLORREF, UINT );

int       OPEN32API _O32_ExtSelectClipRgn( HDC, HRGN, int );

BOOL    OPEN32API _O32_ExtTextOut( HDC, int, int, UINT, const RECT *, LPCSTR, UINT, const int * );

VOID    OPEN32API _O32_FatalAppExit( UINT, LPCSTR );

VOID    OPEN32API _O32_FatalExit( UINT );

BOOL    OPEN32API _O32_FileTimeToDosDateTime( const FILETIME *, LPWORD, LPWORD );

BOOL    OPEN32API _O32_FileTimeToLocalFileTime( const FILETIME *, FILETIME * );

BOOL    OPEN32API _O32_FileTimeToSystemTime( const FILETIME *, LPSYSTEMTIME );

BOOL    OPEN32API _O32_FillPath( HDC );

int     OPEN32API _O32_FillRect( HDC, const RECT *, HBRUSH );

BOOL    OPEN32API _O32_FillRgn( HDC, HRGN, HBRUSH );

ATOM    OPEN32API _O32_FindAtom( LPCSTR );

BOOL    OPEN32API _O32_FindClose( HANDLE );

HANDLE  OPEN32API _O32_FindFirstFile( LPCSTR, LPWIN32_FIND_DATAA );

BOOL    OPEN32API _O32_FindNextFile( HANDLE, LPWIN32_FIND_DATAA );

HRSRC   OPEN32API _O32_FindResource( HINSTANCE, LPCSTR, LPCSTR );

HWND    OPEN32API _O32_FindText( LPFINDREPLACEA );

HWND    OPEN32API _O32_FindWindow( LPCSTR lpClassName , LPCSTR );

BOOL    OPEN32API _O32_FlashWindow( HWND, BOOL );

BOOL    OPEN32API _O32_FlattenPath( HDC );

BOOL    OPEN32API _O32_FlushFileBuffers( HANDLE );

int       OPEN32API _O32_FrameRect( HDC, const RECT *, HBRUSH );

BOOL    OPEN32API _O32_FrameRgn( HDC, HRGN, HBRUSH, int, int );

BOOL    OPEN32API _O32_FreeDDElParam( UINT, LONG );

BOOL    OPEN32API _O32_FreeLibrary( HINSTANCE );

BOOL    OPEN32API _O32_GdiComment(HDC hdc, UINT cbSize, CONST BYTE * lpData);

UINT    OPEN32API _O32_GetACP( VOID );

HWND    OPEN32API _O32_GetActiveWindow( void);

int       OPEN32API _O32_GetArcDirection( HDC );

BOOL    OPEN32API _O32_GetAspectRatioFilterEx( HDC, PSIZE );

UINT    OPEN32API _O32_GetAtomName( ATOM, LPSTR, int );

LONG    OPEN32API _O32_GetBitmapBits( HBITMAP, LONG, PVOID );

BOOL    OPEN32API _O32_GetBitmapDimensionEx( HBITMAP, PSIZE );

COLORREF OPEN32API _O32_GetBkColor( HDC );

int       OPEN32API _O32_GetBkMode( HDC );

UINT    OPEN32API _O32_GetBoundsRect( HDC, PRECT, UINT );

BOOL    OPEN32API _O32_GetBrushOrgEx( HDC, PPOINT );

HWND    OPEN32API _O32_GetCapture( void);

UINT    OPEN32API _O32_GetCaretBlinkTime( VOID );

BOOL    OPEN32API _O32_GetCaretPos( PPOINT );

BOOL    OPEN32API _O32_GetCharABCWidths( HDC, UINT, UINT, LPABC );

BOOL    OPEN32API _O32_GetCharWidth( HDC, UINT, UINT, PINT );

BOOL    OPEN32API _O32_GetClassInfo( HINSTANCE hInstance , LPCSTR, WNDCLASSA * );

DWORD   OPEN32API _O32_GetClassLong( HWND, int );

int       OPEN32API _O32_GetClassName( HWND, LPSTR, int );

WORD    OPEN32API _O32_GetClassWord( HWND, int );

BOOL    OPEN32API _O32_GetClientRect( HWND, PRECT );

HANDLE  OPEN32API _O32_GetClipboardData( UINT );

int       OPEN32API _O32_GetClipboardFormatName( UINT, LPSTR, int );

HWND    OPEN32API _O32_GetClipboardOwner( void);

HWND    OPEN32API _O32_GetClipboardViewer( VOID );

int       OPEN32API _O32_GetClipBox( HDC, PRECT );

BOOL    OPEN32API _O32_GetClipCursor( PRECT );

int       OPEN32API _O32_GetClipRgn( HDC, HRGN );

LPSTR   OPEN32API _O32_GetCommandLine( VOID );

int       OPEN32API _O32_GetCurrentDirectory( DWORD, LPSTR );

HGDIOBJ OPEN32API _O32_GetCurrentObject( HDC, DWORD );

BOOL    OPEN32API _O32_GetCurrentPositionEx( HDC, PPOINT );

HANDLE  OPEN32API _O32_GetCurrentProcess( VOID );

DWORD   OPEN32API _O32_GetCurrentProcessId( VOID );

HANDLE  OPEN32API _O32_GetCurrentThread( VOID );

DWORD   OPEN32API _O32_GetCurrentThreadId( VOID );

DWORD   OPEN32API _O32_GetCurrentTime( VOID );

HCURSOR OPEN32API _O32_GetCursor( VOID );

BOOL    OPEN32API _O32_GetCursorPos( PPOINT );

HDC     OPEN32API _O32_GetDC( HWND );

HDC     OPEN32API _O32_GetDCEx( HWND, HRGN, DWORD );

BOOL    OPEN32API _O32_GetDCOrgEx( HDC, PPOINT );

int       OPEN32API _O32_GetDIBits( HDC, HBITMAP, UINT, UINT, void *, PBITMAPINFO, UINT );

int       OPEN32API _O32_GetDeviceCaps( HDC, int );

LONG    OPEN32API _O32_GetDialogBaseUnits( void);

BOOL    OPEN32API _O32_GetDiskFreeSpace( LPCSTR, PDWORD, PDWORD, PDWORD, PDWORD );

int       OPEN32API _O32_GetDlgCtrlID( HWND );

HWND    OPEN32API _O32_GetDlgItem( HWND, int );

UINT    OPEN32API _O32_GetDlgItemInt( HWND, int, PBOOL, BOOL );

UINT    OPEN32API _O32_GetDlgItemText( HWND, int, LPSTR, int );

UINT    OPEN32API _O32_GetDoubleClickTime( VOID );

UINT    OPEN32API _O32_GetDriveType( LPCSTR );

HENHMETAFILE OPEN32API _O32_GetEnhMetaFile( LPCSTR );

UINT    OPEN32API _O32_GetEnhMetaFileBits( HENHMETAFILE, UINT, PBYTE );

UINT    OPEN32API _O32_GetEnhMetaFileHeader( HENHMETAFILE, UINT, LPENHMETAHEADER );

UINT    OPEN32API _O32_GetEnhMetaFilePaletteEntries( HENHMETAFILE, UINT, LPPALETTEENTRY );

PVOID   OPEN32API _O32_GetEnvironmentStrings( VOID );

DWORD   OPEN32API _O32_GetEnvironmentVariable(LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API _O32_GetExitCodeProcess( HANDLE, LPDWORD );

BOOL    OPEN32API _O32_GetExitCodeThread( HANDLE, LPDWORD );

DWORD   OPEN32API _O32_GetFileAttributes( LPSTR );

DWORD   OPEN32API _O32_GetFileInformationByHandle( HANDLE, BY_HANDLE_FILE_INFORMATION * );

DWORD   OPEN32API _O32_GetFileSize( HANDLE, PDWORD );

BOOL    OPEN32API _O32_GetFileTime( HANDLE, FILETIME *, FILETIME *, FILETIME * );

short     OPEN32API _O32_GetFileTitle( LPCSTR, LPSTR, WORD );

DWORD   OPEN32API _O32_GetFileType( HANDLE );

HWND    OPEN32API _O32_GetFocus( VOID );

HWND    OPEN32API _O32_GetForegroundWindow( VOID );

DWORD   OPEN32API _O32_GetFullPathName( LPCSTR, DWORD, LPSTR, LPSTR * );

int       OPEN32API _O32_GetGraphicsMode(HDC);

BOOL    OPEN32API _O32_GetIconInfo( HICON, LPICONINFO );

DWORD   OPEN32API _O32_GetKerningPairs( HDC, DWORD, LPKERNINGPAIR );

int       OPEN32API _O32_GetKeyboardType( int );

int       OPEN32API _O32_GetKeyNameText( LPARAM, LPSTR, int );

USHORT  OPEN32API _O32_GetAsyncKeyState(INT nVirtKey);
SHORT   OPEN32API _O32_GetKeyState( int );

HWND    OPEN32API _O32_GetLastActivePopup( HWND );

DWORD   OPEN32API _O32_GetLastError( VOID );

VOID    OPEN32API _O32_GetLocalTime( LPSYSTEMTIME );

DWORD   OPEN32API _O32_GetLogicalDriveStrings( DWORD, LPSTR );

DWORD   OPEN32API _O32_GetLogicalDrives( VOID );

int       OPEN32API _O32_GetMapMode( HDC);

HMENU   OPEN32API _O32_GetMenu( HWND );

LONG    OPEN32API _O32_GetMenuCheckMarkDimensions(void);

int       OPEN32API _O32_GetMenuItemCount( HMENU );

UINT    OPEN32API _O32_GetMenuItemID( HMENU, int );

UINT    OPEN32API _O32_GetMenuState( HMENU, UINT, UINT );

int       OPEN32API _O32_GetMenuString( HMENU, UINT, LPSTR, int, UINT );

BOOL    OPEN32API _O32_GetMessage( LPMSG, HWND hwnd , UINT, UINT );

LONG    OPEN32API _O32_GetMessageExtraInfo( VOID );

DWORD   OPEN32API _O32_GetMessagePos( VOID );

LONG    OPEN32API _O32_GetMessageTime( VOID );

HMETAFILE OPEN32API _O32_GetMetaFile( LPCSTR );

UINT    OPEN32API _O32_GetMetaFileBitsEx( HMETAFILE, UINT, LPVOID );

BOOL    OPEN32API _O32_GetMiterLimit( HDC, float *);

DWORD   OPEN32API _O32_GetModuleFileName( HINSTANCE, LPSTR, DWORD );

HMODULE OPEN32API _O32_GetModuleHandle( LPCSTR );

COLORREF OPEN32API _O32_GetNearestColor( HDC, COLORREF );

UINT    OPEN32API _O32_GetNearestPaletteIndex( HPALETTE, COLORREF );

HWND    OPEN32API _O32_GetNextDlgGroupItem( HWND, HWND, BOOL );

HWND    OPEN32API _O32_GetNextDlgTabItem( HWND, HWND, BOOL );

HWND    OPEN32API _O32_GetNextWindow( HWND, UINT );

int       OPEN32API _O32_GetOEMCP( VOID );

int       OPEN32API _O32_GetObject( HGDIOBJ, int, void * );

DWORD   OPEN32API _O32_GetObjectType( HGDIOBJ );

HWND    OPEN32API _O32_GetOpenClipboardWindow( VOID );

BOOL    OPEN32API _O32_GetOpenFileName( LPOPENFILENAMEA );

UINT    OPEN32API _O32_GetOutlineTextMetrics( HDC, UINT, LPOUTLINETEXTMETRICA );

BOOL    OPEN32API _O32_GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL );

UINT    OPEN32API _O32_GetPaletteEntries( HPALETTE, UINT, UINT, PPALETTEENTRY );

HWND    OPEN32API _O32_GetParent( HWND );

UINT    OPEN32API _O32_GetPath( HDC, PPOINT, PBYTE, int );

COLORREF OPEN32API _O32_GetPixel( HDC, int, int );

int       OPEN32API _O32_GetPolyFillMode( HDC);

DWORD   OPEN32API _O32_GetPriorityClass( HANDLE );

int       OPEN32API _O32_GetPriorityClipboardFormat( PUINT, int );

UINT    OPEN32API _O32_GetPrivateProfileInt( LPCSTR, LPCSTR, INT, LPCSTR );

DWORD   OPEN32API _O32_GetPrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR );

FARPROC OPEN32API _O32_GetProcAddress( HMODULE, LPCSTR );

UINT    OPEN32API _O32_GetProfileInt( LPCSTR, LPCSTR, INT );

DWORD   OPEN32API _O32_GetProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD );

HANDLE  OPEN32API _O32_GetProp( HWND, LPCSTR );

DWORD   OPEN32API _O32_GetQueueStatus( UINT );

int       OPEN32API _O32_GetROP2( HDC );

BOOL    OPEN32API _O32_GetRasterizerCaps( LPRASTERIZER_STATUS, UINT );

int       OPEN32API _O32_GetRgnBox( HRGN, PRECT );

DWORD   OPEN32API _O32_GetRegionData( HRGN, DWORD, PRGNDATA );

BOOL    OPEN32API _O32_GetSaveFileName( LPOPENFILENAMEA );

int       OPEN32API _O32_GetScrollPos( HWND, int );

BOOL    OPEN32API _O32_GetScrollRange( HWND, int, int *, int * );

HANDLE  OPEN32API _O32_GetStdHandle( DWORD );

HGDIOBJ OPEN32API _O32_GetStockObject( int );

int       OPEN32API _O32_GetStretchBltMode( HDC );

HMENU   OPEN32API _O32_GetSubMenu( HWND, int );

DWORD   OPEN32API _O32_GetSysColor( int );

UINT    OPEN32API _O32_GetSystemDirectory( LPSTR, UINT );

HMENU   OPEN32API _O32_GetSystemMenu( HWND, BOOL );

int       OPEN32API _O32_GetSystemMetrics( int );

UINT    OPEN32API _O32_GetSystemPaletteEntries( HDC, UINT, UINT, PPALETTEENTRY );

void      OPEN32API _O32_GetSystemTime( LPSYSTEMTIME );

DWORD   OPEN32API _O32_GetTabbedTextExtent( HDC, LPCSTR, int, int, int * );

UINT    OPEN32API _O32_GetTempFileName( LPCSTR, LPCSTR, UINT, LPSTR );

DWORD   OPEN32API _O32_GetTempPath( DWORD, LPSTR );

UINT    OPEN32API _O32_GetTextAlign( HDC);

int       OPEN32API _O32_GetTextCharacterExtra( HDC);

COLORREF OPEN32API _O32_GetTextColor( HDC );

BOOL    OPEN32API _O32_GetTextExtentPoint( HDC, LPCSTR, int, PSIZE );

BOOL    OPEN32API _O32_GetTextExtentPoint32( HDC, LPCSTR, int, PSIZE );

int       OPEN32API _O32_GetTextFace( HDC, int, LPSTR );

BOOL    OPEN32API _O32_GetTextMetrics( HDC, LPTEXTMETRICA );

DWORD   OPEN32API _O32_GetThreadPriority( HANDLE );

DWORD   OPEN32API _O32_GetTickCount( VOID );

DWORD   OPEN32API _O32_GetTimeZoneInformation( LPTIME_ZONE_INFORMATION );

HWND    OPEN32API _O32_GetTopWindow( HWND );

BOOL    OPEN32API _O32_GetUpdateRect( HWND, PRECT, BOOL );

int       OPEN32API _O32_GetUpdateRgn( HWND, HRGN, BOOL );

BOOL    OPEN32API _O32_GetViewportExtEx( HDC, PSIZE );

BOOL    OPEN32API _O32_GetViewportOrgEx( HDC, PPOINT );

DWORD   OPEN32API _O32_GetVolumeInformation( LPCSTR, LPSTR, DWORD, PDWORD,
                       PDWORD, PDWORD, LPSTR, DWORD );

UINT    OPEN32API _O32_GetWinMetaFileBits( HENHMETAFILE, UINT, PBYTE, int, HDC);

HWND    OPEN32API _O32_GetWindow( HWND, UINT );

HDC     OPEN32API _O32_GetWindowDC( HWND );

BOOL    OPEN32API _O32_GetWindowExtEx( HDC, PSIZE );

LONG    OPEN32API _O32_GetWindowLong( HWND, int );

BOOL    OPEN32API _O32_GetWindowOrgEx( HDC, PPOINT );

BOOL    OPEN32API _O32_GetWindowPlacement( HWND, LPWINDOWPLACEMENT );

BOOL    OPEN32API _O32_GetWindowRect( HWND, PRECT );

UINT    OPEN32API _O32_GetWindowsDirectory( LPSTR, UINT );

int       OPEN32API _O32_GetWindowText( HWND, LPSTR, int );

int       OPEN32API _O32_GetWindowTextLength( HWND );

DWORD   OPEN32API _O32_GetWindowThreadProcessId(HWND, PDWORD );

WORD    OPEN32API _O32_GetWindowWord( HWND, int );

BOOL    OPEN32API _O32_GetWorldTransform( HDC, LPXFORM );

ATOM    OPEN32API _O32_GlobalAddAtom( LPCSTR );

HGLOBAL OPEN32API _O32_GlobalAlloc( UINT, DWORD );

ATOM    OPEN32API _O32_GlobalDeleteAtom( ATOM );

HGLOBAL OPEN32API _O32_GlobalDiscard( HGLOBAL );

ATOM    OPEN32API _O32_GlobalFindAtom( LPCSTR );

UINT    OPEN32API _O32_GlobalFlags( HGLOBAL );

HGLOBAL OPEN32API _O32_GlobalFree( HGLOBAL );

UINT    OPEN32API _O32_GlobalGetAtomName( ATOM, LPSTR, int );

HGLOBAL OPEN32API _O32_GlobalHandle( PVOID memPtr );

PVOID   OPEN32API _O32_GlobalLock( HGLOBAL hglbMem );

VOID    OPEN32API _O32_GlobalMemoryStatus( LPMEMORYSTATUS );

HGLOBAL OPEN32API _O32_GlobalReAlloc( HGLOBAL, DWORD, UINT );

DWORD   OPEN32API _O32_GlobalSize( HGLOBAL );

BOOL    OPEN32API _O32_GlobalUnlock( HGLOBAL hglbMem );

PVOID   OPEN32API _O32_HeapAlloc( HANDLE, DWORD, DWORD );

HANDLE  OPEN32API _O32_HeapCreate( DWORD, DWORD, DWORD );

BOOL    OPEN32API _O32_HeapDestroy( HANDLE );

BOOL    OPEN32API _O32_HeapFree( HANDLE, DWORD, PVOID );

PVOID   OPEN32API _O32_HeapReAlloc( HANDLE, DWORD, PVOID, DWORD );

DWORD   OPEN32API _O32_HeapSize( HANDLE, DWORD, PVOID );

BOOL    OPEN32API _O32_HideCaret( HWND );

BOOL    OPEN32API _O32_HiliteMenuItem( HWND, HMENU, UINT, UINT );

BOOL    OPEN32API _O32_InflateRect( PRECT, int, int );

BOOL    OPEN32API _O32_InSendMessage( VOID );

BOOL    OPEN32API _O32_InitAtomTable( DWORD );

VOID    OPEN32API _O32_InitializeCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API _O32_InsertMenu( HMENU, UINT, UINT, UINT, LPCSTR );

LONG    OPEN32API _O32_InterlockedDecrement( PLONG );

LONG    OPEN32API _O32_InterlockedExchange( PLONG, LONG );

LONG    OPEN32API _O32_InterlockedIncrement( PLONG );

int       OPEN32API _O32_IntersectClipRect( HDC, int, int, int, int );

BOOL    OPEN32API _O32_IntersectRect( PRECT, const RECT * rect1, const RECT * rect2);

BOOL    OPEN32API _O32_InvalidateRect( HWND, const RECT *, BOOL );

BOOL    OPEN32API _O32_InvalidateRgn( HWND, HRGN, BOOL );

BOOL    OPEN32API _O32_InvertRect( HDC, const RECT *);

BOOL    OPEN32API _O32_InvertRgn( HDC, HRGN );

BOOL    OPEN32API _O32_IsBadCodePtr( FARPROC );

BOOL    OPEN32API _O32_IsBadReadPtr( const VOID *, UINT );

BOOL    OPEN32API _O32_IsBadStringPtr( LPCSTR, UINT );

BOOL    OPEN32API _O32_IsBadWritePtr( PVOID, UINT );

BOOL    OPEN32API _O32_IsChild( HWND, HWND );

BOOL    OPEN32API _O32_IsClipboardFormatAvailable( UINT );

BOOL    OPEN32API _O32_IsDBCSLeadByte( BYTE );

BOOL    OPEN32API _O32_IsDialogMessage( HWND, LPMSG );

BOOL    OPEN32API _O32_IsDlgButtonChecked( HWND, int );

BOOL    OPEN32API _O32_IsIconic( HWND );

BOOL    OPEN32API _O32_IsMenu( HMENU );

BOOL    OPEN32API _O32_IsRectEmpty( const RECT *);

BOOL    OPEN32API _O32_IsWindow( HWND );

BOOL    OPEN32API _O32_IsWindowEnabled( HWND );

BOOL    OPEN32API _O32_IsWindowVisible( HWND );

BOOL    OPEN32API _O32_IsZoomed( HWND );

BOOL    OPEN32API _O32_KillTimer(HWND, UINT );

BOOL    OPEN32API _O32_LPtoDP( HDC, PPOINT, int );

VOID    OPEN32API _O32_LeaveCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API _O32_LineDDA( int, int, int, int, LINEDDAPROC_O32, LPARAM );

 BOOL   OPEN32API _O32_LineTo( HDC, int, int );

HACCEL  OPEN32API _O32_LoadAccelerators( HINSTANCE, LPCSTR );

HBITMAP OPEN32API _O32_LoadBitmap( HINSTANCE, LPCSTR );

HCURSOR OPEN32API _O32_LoadCursor( HINSTANCE, LPCSTR );

HICON   OPEN32API _O32_LoadIcon( HINSTANCE, LPCSTR );

HINSTANCE OPEN32API _O32_LoadLibrary( LPCSTR );

HMENU   OPEN32API _O32_LoadMenu( HINSTANCE, LPCSTR );

HMENU   OPEN32API _O32_LoadMenuIndirect( const MENUITEMTEMPLATEHEADER * );

DWORD   OPEN32API _O32_LoadModule( LPCSTR, PVOID );

HGLOBAL OPEN32API _O32_LoadResource( HINSTANCE, HRSRC );

PVOID   OPEN32API _O32_LockResource( HGLOBAL );

int       OPEN32API _O32_LoadString( HINSTANCE, UINT, LPSTR, int );

HLOCAL  OPEN32API _O32_LocalAlloc( UINT, UINT );

HLOCAL  OPEN32API _O32_LocalDiscard( HLOCAL );

UINT    OPEN32API _O32_LocalFlags( HLOCAL );

BOOL    OPEN32API _O32_LocalFileTimeToFileTime( const FILETIME *, FILETIME * );

HLOCAL  OPEN32API _O32_LocalFree( HLOCAL );

HLOCAL  OPEN32API _O32_LocalHandle( PVOID lpvMem );

BOOL    OPEN32API _O32_LocalUnlock( HLOCAL hlocal);

HLOCAL  OPEN32API _O32_LocalReAlloc( HLOCAL, UINT, UINT );

UINT    OPEN32API _O32_LocalSize( HLOCAL );

PVOID   OPEN32API _O32_LocalLock( HLOCAL hlocal );

BOOL    OPEN32API _O32_LockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    OPEN32API _O32_LockWindowUpdate( HWND );

BOOL    OPEN32API _O32_MapDialogRect( HWND, PRECT );

UINT    OPEN32API _O32_MapVirtualKey( UINT, UINT );

int       OPEN32API _O32_MapWindowPoints( HWND, HWND, PPOINT, int );

BOOL    OPEN32API _O32_MaskBlt( HDC, int, int, int, int, HDC  hdcSrc, int, int, HBITMAP, int, int, DWORD );

int       OPEN32API _O32_MessageBox( HWND hwnd , LPCSTR, LPCSTR, UINT );

BOOL    OPEN32API _O32_MessageBeep( UINT );

DWORD   OPEN32API _O32_MsgWaitForMultipleObjects( DWORD, LPHANDLE, BOOL, DWORD, DWORD );

BOOL    OPEN32API _O32_ModifyMenu( HMENU, UINT, UINT, UINT, LPCSTR );

BOOL    OPEN32API _O32_ModifyWorldTransform( HDC, LPXFORM, DWORD );

BOOL    OPEN32API _O32_MoveFile( LPCSTR, LPCSTR );

BOOL    OPEN32API _O32_MoveToEx( HDC, int, int, PPOINT );

BOOL    OPEN32API _O32_MoveWindow( HWND, int, int, int, int, BOOL );

int       OPEN32API _O32_MulDiv(int, int, int );

int       OPEN32API _O32_OffsetClipRgn( HDC, int, int );

BOOL    OPEN32API _O32_OffsetRect( PRECT, int, int );

int       OPEN32API _O32_OffsetRgn( HRGN, int, int );

BOOL    OPEN32API _O32_OffsetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API _O32_OffsetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API _O32_OpenClipboard( HWND );

HANDLE  OPEN32API _O32_OpenEvent( DWORD, BOOL, LPCSTR );

HFILE   OPEN32API _O32_OpenFile( LPCSTR, LPOFSTRUCT, UINT );

HANDLE  OPEN32API _O32_OpenMutex( DWORD, BOOL, LPCSTR );

HANDLE  OPEN32API _O32_OpenProcess( DWORD, BOOL, DWORD );

HANDLE  OPEN32API _O32_OpenSemaphore( DWORD, BOOL, LPCSTR );

VOID    OPEN32API _O32_OutputDebugString( LPCSTR );

LONG    OPEN32API _O32_PackDDElParam(UINT, UINT, UINT );

BOOL    OPEN32API _O32_PaintRgn( HDC, HRGN );

BOOL    OPEN32API _O32_PatBlt( HDC, int, int, int, int, DWORD );

HRGN    OPEN32API _O32_PathToRegion( HDC);

BOOL    OPEN32API _O32_PeekMessage( LPMSG, HWND, UINT, UINT, UINT );

BOOL    OPEN32API _O32_Pie( HDC, int, int, int, int, int, int, int, int );

BOOL    OPEN32API _O32_PlayEnhMetaFile( HDC, HENHMETAFILE, const RECT * );

BOOL    OPEN32API _O32_PlayMetaFile( HDC, HMETAFILE );

BOOL    OPEN32API _O32_PlayMetaFileRecord( HDC, LPHANDLETABLE, PMETARECORD, int );

BOOL    OPEN32API _O32_PolyBezier( HDC, const POINT *, int );

BOOL    OPEN32API _O32_PolyBezierTo( HDC, const POINT *, DWORD );

BOOL    OPEN32API _O32_PolyDraw( HDC, const POINT *, const BYTE *, int );

BOOL    OPEN32API _O32_Polygon( HDC, const POINT *, int );

BOOL    OPEN32API _O32_Polyline( HDC, const POINT *, int );

BOOL    OPEN32API _O32_PolylineTo( HDC, const POINT *, DWORD );

BOOL    OPEN32API _O32_PolyPolygon( HDC, const POINT *, const int *, int );

BOOL    OPEN32API _O32_PolyPolyline( HDC, const POINT *, const DWORD *, DWORD );

VOID    OPEN32API _O32_PostQuitMessage( int );

BOOL    OPEN32API _O32_PostMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

BOOL    OPEN32API _O32_PostThreadMessage( DWORD, UINT, WPARAM, LPARAM );

BOOL    OPEN32API _O32_PtInRect( const RECT *, POINT );

BOOL    OPEN32API _O32_PtInRegion( HRGN, int, int );

BOOL    OPEN32API _O32_PtVisible( HDC, int, int );

BOOL    OPEN32API _O32_PrintDlg( LPPRINTDLGA );

BOOL    OPEN32API _O32_PulseEvent ( HANDLE );

BOOL    OPEN32API _O32_ReadFile( HANDLE, PVOID, DWORD, PDWORD, LPOVERLAPPED );

UINT    OPEN32API _O32_RealizePalette( HDC);

BOOL    OPEN32API _O32_RectInRegion( HRGN, const RECT * );

BOOL    OPEN32API _O32_RectVisible( HDC, const RECT *);

BOOL    OPEN32API _O32_Rectangle( HDC, int, int, int, int );

BOOL    OPEN32API _O32_RedrawWindow( HWND, const RECT *, HRGN, DWORD );

LONG    OPEN32API _O32_RegCloseKey( HKEY );

LONG    OPEN32API _O32_RegCreateKey( HKEY, LPCSTR, PHKEY );

LONG    OPEN32API _O32_RegCreateKeyEx( HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, PDWORD );

LONG    OPEN32API _O32_RegDeleteKey( HKEY, LPCSTR );

LONG    OPEN32API _O32_RegDeleteValue( HKEY, LPSTR );

LONG    OPEN32API _O32_RegEnumKey( HKEY, DWORD, LPSTR, DWORD );

LONG    OPEN32API _O32_RegEnumKeyEx( HKEY, DWORD, LPSTR, PDWORD, PDWORD, LPSTR, PDWORD, FILETIME * );

LONG    OPEN32API _O32_RegEnumValue( HKEY, DWORD, LPSTR, PDWORD, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    OPEN32API _O32_RegOpenKey( HKEY, LPCSTR, PHKEY );

LONG    OPEN32API _O32_RegOpenKeyEx( HKEY, LPCSTR, DWORD, REGSAM, PHKEY );

LONG    OPEN32API _O32_RegQueryInfoKey( HKEY, LPSTR, PDWORD, PDWORD, PDWORD, PDWORD,
                                      PDWORD, PDWORD, PDWORD, PDWORD, PDWORD, FILETIME * );

LONG    OPEN32API _O32_RegQueryValue( HKEY, LPCSTR, LPSTR, PLONG );

LONG    OPEN32API _O32_RegQueryValueEx( HKEY, LPCSTR, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    OPEN32API _O32_RegSetValue( HKEY, LPCSTR, DWORD, LPCSTR, DWORD );

LONG    OPEN32API _O32_RegSetValueEx( HKEY, LPCSTR, DWORD, DWORD, const BYTE *, DWORD );

WORD    OPEN32API _O32_RegisterClass( const WNDCLASSA *);

UINT    OPEN32API _O32_RegisterClipboardFormat( LPCSTR );

UINT    OPEN32API _O32_RegisterWindowMessage( LPCSTR );

BOOL    OPEN32API _O32_ReleaseCapture( void );

int       OPEN32API _O32_ReleaseDC( HWND, HDC );

BOOL    OPEN32API _O32_ReleaseMutex( HANDLE );

BOOL    OPEN32API _O32_ReleaseSemaphore( HANDLE, LONG, PLONG );

BOOL    OPEN32API _O32_RemoveDirectory( LPCSTR );

BOOL    OPEN32API _O32_RemoveFontResource( LPCSTR );

BOOL    OPEN32API _O32_RemoveMenu( HMENU, UINT, UINT );

HANDLE  OPEN32API _O32_RemoveProp( HWND, LPCSTR );

HWND    OPEN32API _O32_ReplaceText( LPFINDREPLACEA );

BOOL    OPEN32API _O32_ReplyMessage( LRESULT );

BOOL    OPEN32API _O32_ResetDC( HDC, const DEVMODEA * );

BOOL    OPEN32API _O32_ResetEvent( HANDLE );

BOOL    OPEN32API _O32_ResizePalette( HPALETTE, UINT );

BOOL    OPEN32API _O32_RestoreDC( HDC, int );

DWORD   OPEN32API _O32_ResumeThread( HANDLE );

LONG    OPEN32API _O32_ReuseDDElParam( LONG, UINT, UINT, UINT, UINT );

BOOL    OPEN32API _O32_RoundRect( HDC, int, int, int, int, int, int );

int       OPEN32API _O32_SaveDC( HDC );

BOOL    OPEN32API _O32_ScaleViewportExtEx( HDC, int, int, int, int, PSIZE );

BOOL    OPEN32API _O32_ScaleWindowExtEx( HDC, int, int, int, int, PSIZE );

BOOL    OPEN32API _O32_ScreenToClient( HWND, PPOINT );

BOOL    OPEN32API _O32_ScrollDC( HDC, int, int, const RECT *, const RECT *, HRGN, PRECT );

BOOL    OPEN32API _O32_ScrollWindow( HWND, int, int, const RECT *, const RECT * );

BOOL    OPEN32API _O32_ScrollWindowEx( HWND, int, int, const RECT *, const RECT *, HRGN, PRECT, UINT );

DWORD   OPEN32API _O32_SearchPath( LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPSTR *);

int       OPEN32API _O32_SelectClipRgn( HDC, HRGN );

HGDIOBJ OPEN32API _O32_SelectObject( HDC, HGDIOBJ );

HPALETTE OPEN32API _O32_SelectPalette( HDC, HPALETTE, BOOL );

LONG    OPEN32API _O32_SendDlgItemMessage( HWND, int, UINT, WPARAM, LPARAM );

LRESULT OPEN32API _O32_SendMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HWND    OPEN32API _O32_SetActiveWindow( HWND );

int       OPEN32API _O32_SetArcDirection( HDC, int );

LONG    OPEN32API _O32_SetBitmapBits( HBITMAP, DWORD, const VOID * );

BOOL    OPEN32API _O32_SetBitmapDimensionEx( HBITMAP, int, int, PSIZE );

COLORREF OPEN32API _O32_SetBkColor( HDC, COLORREF );

int       OPEN32API _O32_SetBkMode( HDC, int );

LONG    OPEN32API _O32_SetBoundsRect( HDC, const RECT *, UINT );

BOOL    OPEN32API _O32_SetBrushOrgEx( HDC, int, int, PPOINT );

HWND    OPEN32API _O32_SetCapture( HWND );

BOOL    OPEN32API _O32_SetCaretBlinkTime( UINT );

BOOL    OPEN32API _O32_SetCaretPos( int, int );

DWORD   OPEN32API _O32_SetClassLong( HWND, int, LONG );

WORD    OPEN32API _O32_SetClassWord( HWND, int, WORD );

HANDLE  OPEN32API _O32_SetClipboardData( UINT, HANDLE );

HWND    OPEN32API _O32_SetClipboardViewer( HWND );

BOOL    OPEN32API _O32_SetCurrentDirectory(  LPSTR );

HCURSOR OPEN32API _O32_SetCursor( HCURSOR );

BOOL    OPEN32API _O32_SetCursorPos( int, int );

int       OPEN32API _O32_SetDIBits( HDC, HBITMAP, UINT, UINT, const VOID *, const BITMAPINFO *, UINT );

int       OPEN32API _O32_SetDIBitsToDevice( HDC, int, int, int, int, int, int, UINT, UINT, PVOID, PBITMAPINFO, UINT );

BOOL    OPEN32API _O32_SetDlgItemInt( HWND, int, UINT, BOOL );

BOOL    OPEN32API _O32_SetDlgItemText( HWND, int, LPCSTR );

BOOL    OPEN32API _O32_SetDoubleClickTime( UINT );

BOOL    OPEN32API _O32_SetEndOfFile( HANDLE );

HENHMETAFILE OPEN32API _O32_SetEnhMetaFileBits( UINT, const BYTE * );

BOOL    OPEN32API _O32_SetEnvironmentVariable( LPCSTR, LPCSTR );

BOOL    OPEN32API _O32_SetEvent( HANDLE );

BOOL    OPEN32API _O32_SetFileAttributes( LPCSTR, DWORD );

DWORD   OPEN32API _O32_SetFilePointer( HANDLE, LONG, PLONG, DWORD );

BOOL    OPEN32API _O32_SetFileTime( HANDLE, const FILETIME *, const FILETIME *, const FILETIME * );

HWND    OPEN32API _O32_SetFocus( HWND );

BOOL    OPEN32API _O32_SetForegroundWindow( HWND );

int       OPEN32API _O32_SetGraphicsMode(HDC, int );

UINT    OPEN32API _O32_SetHandleCount( UINT );

VOID    OPEN32API _O32_SetLastError( DWORD );

BOOL    OPEN32API _O32_SetLocalTime( const SYSTEMTIME * );

int       OPEN32API _O32_SetMapMode( HDC, int );

DWORD   OPEN32API _O32_SetMapperFlags( HDC, DWORD );

BOOL    OPEN32API _O32_SetMenu( HWND, HMENU );

BOOL    OPEN32API _O32_SetMenuItemBitmaps( HMENU, UINT, UINT, HBITMAP, HBITMAP );

HMETAFILE OPEN32API _O32_SetMetaFileBitsEx( UINT, PBYTE );

BOOL    OPEN32API _O32_SetMiterLimit( HDC, float limit, float * );

UINT    OPEN32API _O32_SetPaletteEntries( HPALETTE, UINT, UINT, const PALETTEENTRY *);

HWND    OPEN32API _O32_SetParent( HWND, HWND );

COLORREF
          OPEN32API _O32_SetPixel( HDC, int, int, COLORREF );

int       OPEN32API _O32_SetPolyFillMode( HDC, int );

BOOL    OPEN32API _O32_SetPriorityClass( HANDLE, DWORD );

BOOL    OPEN32API _O32_SetProp( HWND, LPCSTR, HANDLE );

BOOL    OPEN32API _O32_SetRect( PRECT, int, int, int, int );

BOOL    OPEN32API _O32_SetRectEmpty( PRECT );

BOOL    OPEN32API _O32_SetRectRgn( HRGN, int, int, int, int );

int       OPEN32API _O32_SetROP2( HDC, int );

int       OPEN32API _O32_SetScrollPos( HWND, int, int, BOOL );

BOOL    OPEN32API _O32_SetScrollRange( HWND, int, int, int, BOOL );

BOOL    OPEN32API _O32_SetStdHandle( DWORD, HANDLE );

int       OPEN32API _O32_SetStretchBltMode( HDC, int );

UINT    OPEN32API _O32_SetSysColors( int, const int *, const COLORREF * );

BOOL    OPEN32API _O32_SetSystemTime( const SYSTEMTIME * );

BOOL    OPEN32API _O32_SetTimeZoneInformation( const TIME_ZONE_INFORMATION * );

UINT    OPEN32API _O32_SetTextAlign( HDC, UINT );

int       OPEN32API _O32_SetTextCharacterExtra( HDC, int );

COLORREF OPEN32API _O32_SetTextColor( HDC, COLORREF );

BOOL    OPEN32API _O32_SetTextJustification( HDC, int, int );

BOOL    OPEN32API _O32_SetThreadPriority( HANDLE, int );

UINT    OPEN32API _O32_SetTimer( HWND, UINT, UINT, TIMERPROC_O32 );

BOOL    OPEN32API _O32_SetViewportExtEx( HDC, int, int, PSIZE );

BOOL    OPEN32API _O32_SetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API _O32_SetVolumeLabel( LPCSTR, LPCSTR );

BOOL    OPEN32API _O32_SetWindowExtEx( HDC, int, int, PSIZE );

LONG    OPEN32API _O32_SetWindowLong( HWND, int, LONG );

BOOL    OPEN32API _O32_SetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API _O32_SetWindowPlacement( HWND, const WINDOWPLACEMENT * );

BOOL    OPEN32API _O32_SetWindowPos( HWND, HWND, int, int, int, int, UINT );

HHOOK   OPEN32API _O32_SetWindowsHookEx( int, HOOKPROC_O32, HINSTANCE, DWORD );

BOOL    OPEN32API _O32_SetWindowText( HWND, LPCSTR );

WORD    OPEN32API _O32_SetWindowWord( HWND, int, WORD );

HENHMETAFILE OPEN32API _O32_SetWinMetaFileBits( UINT, const BYTE *, HDC, const METAFILEPICT * );

BOOL    OPEN32API _O32_SetWorldTransform( HDC, LPXFORM );

BOOL    OPEN32API _O32_ShowCaret( HWND );

int       OPEN32API _O32_ShowCursor( BOOL );

BOOL    OPEN32API _O32_ShowOwnedPopups( HWND, BOOL );

BOOL    OPEN32API _O32_ShowScrollBar( HWND, int, BOOL );

BOOL    OPEN32API _O32_ShowWindow( HWND, int );

DWORD   OPEN32API _O32_SizeofResource( HINSTANCE, HRSRC );

VOID    OPEN32API _O32_Sleep( DWORD );

int       OPEN32API _O32_StartDoc( HDC, LPDOCINFOA );

int       OPEN32API _O32_StartPage( HDC );

int       OPEN32API _O32_StretchDIBits( HDC, int, int, int, int, int, int, int, int, void *, PBITMAPINFO, UINT, DWORD );

BOOL    OPEN32API _O32_StretchBlt( HDC, int, int, int, int, HDC, int, int, int, int,  DWORD );

BOOL    OPEN32API _O32_StrokeAndFillPath( HDC );

BOOL    OPEN32API _O32_StrokePath( HDC );

BOOL    OPEN32API _O32_SubtractRect( PRECT, const RECT *, const RECT *);

DWORD   OPEN32API _O32_SuspendThread( HANDLE );

BOOL    OPEN32API _O32_SwapMouseButton( BOOL );

UINT    OPEN32API _O32_SystemParametersInfo( UINT, UINT, PVOID, UINT );

BOOL    OPEN32API _O32_SystemTimeToFileTime( const SYSTEMTIME *, FILETIME * );

BOOL    OPEN32API _O32_SystemTimeToTzSpecificLocalTime( LPTIME_ZONE_INFORMATION, LPSYSTEMTIME, LPSYSTEMTIME );

LONG    OPEN32API _O32_TabbedTextOut( HDC, int, int, LPCSTR, int, int, int *, int );

BOOL    OPEN32API _O32_TerminateProcess( HANDLE, UINT );

BOOL    OPEN32API _O32_TerminateThread( HANDLE, DWORD );

BOOL    OPEN32API _O32_TextOut( HDC, int, int, LPCSTR, int );

DWORD   OPEN32API _O32_TlsAlloc( VOID );

BOOL    OPEN32API _O32_TlsFree( DWORD );

PVOID   OPEN32API _O32_TlsGetValue( DWORD );

BOOL    OPEN32API _O32_TlsSetValue( DWORD, PVOID );

BOOL    OPEN32API _O32_TrackPopupMenu( HMENU, UINT, int, int, int, HWND, const RECT * );

int       OPEN32API _O32_TranslateAccelerator( HWND, HACCEL, LPMSG );

BOOL    OPEN32API _O32_TranslateMDISysAccel( HWND, LPMSG );

BOOL    OPEN32API _O32_TranslateMessage( const MSG * );

BOOL    OPEN32API _O32_UnhookWindowsHookEx( HHOOK );

BOOL    OPEN32API _O32_UnionRect( PRECT, const RECT *, const RECT * );

BOOL    OPEN32API _O32_UnlockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    OPEN32API _O32_UnpackDDElParam(UINT, LONG, PUINT, PUINT );

BOOL    OPEN32API _O32_UnrealizeObject( HGDIOBJ );

BOOL    OPEN32API _O32_UnregisterClass( LPCSTR, HINSTANCE );

BOOL    OPEN32API _O32_UpdateWindow( HWND );

BOOL    OPEN32API _O32_ValidateRect( HWND, const RECT *);

BOOL    OPEN32API _O32_ValidateRgn( HWND, HRGN );

SHORT   OPEN32API _O32_VkKeyScan( char keyScan);

DWORD   OPEN32API _O32_WaitForMultipleObjects( DWORD, const HANDLE *, BOOL, DWORD );

DWORD   OPEN32API _O32_WaitForSingleObject( HANDLE, DWORD );

DWORD   OPEN32API _O32_WaitForInputIdle(HANDLE hProcess, DWORD dwTimeOut);

BOOL    OPEN32API _O32_WaitMessage( void);

BOOL    OPEN32API _O32_WidenPath( HDC);

UINT    OPEN32API _O32_WinExec( LPCSTR, UINT );

BOOL    OPEN32API _O32_WinHelp( HWND, LPCSTR, UINT, DWORD );

HWND    OPEN32API _O32_WindowFromPoint( POINT );

HWND    OPEN32API _O32_WindowFromDC( HDC );

BOOL    OPEN32API _O32_WriteFile( HANDLE, const VOID *, DWORD, PDWORD, LPOVERLAPPED );

BOOL    OPEN32API _O32_WritePrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPCSTR );

BOOL    OPEN32API _O32_WriteProfileString( LPCSTR, LPCSTR, LPCSTR );

VOID    OPEN32API _O32_ZeroMemory( PVOID, DWORD );

HFILE   OPEN32API _O32__lclose( HFILE );

HFILE   OPEN32API _O32__lcreat( LPCSTR, int );

HFILE   OPEN32API _O32__lopen( LPCSTR, int );

UINT    OPEN32API _O32__lread( HFILE, PVOID, UINT );

LONG    OPEN32API _O32__llseek( HFILE, LONG, int );

UINT    OPEN32API _O32__lwrite( HFILE, const VOID *, UINT );

DWORD   OPEN32API _O32_timeGetSystemTime( LPMMTIME, UINT );

DWORD   OPEN32API _O32_timeGetTime( VOID );

int     OPEN32API _O32_wsprintf( LPSTR, LPCSTR, ... );
int     OPEN32API _O32_wvsprintf( LPSTR, LPCSTR, const VOID *);

LPSTR   OPEN32API _O32_CharNext( LPCSTR );

LPSTR   OPEN32API _O32_CharPrev( LPCSTR, LPCSTR );

HWND    OPEN32API _O32_GetDesktopWindow( VOID );

BOOL    OPEN32API _O32_CharToOem( LPCSTR, LPSTR );

BOOL    OPEN32API _O32_OemToChar( LPCSTR, LPSTR );

BOOL    OPEN32API _O32_CharToOemBuff( LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API _O32_OemToCharBuff( LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API _O32_IsBadHugeReadPtr( const void *, UINT );

BOOL    OPEN32API _O32_IsBadHugeWritePtr( PVOID, UINT );

BOOL    OPEN32API _O32_FloodFill(HDC, int, int, COLORREF );

BOOL    OPEN32API _O32_IsCharAlpha( WCHAR );

BOOL    OPEN32API _O32_IsCharAlphaNumeric( WCHAR );

BOOL    OPEN32API _O32_IsCharLower( WCHAR );

BOOL    OPEN32API _O32_IsCharUpper( WCHAR );

LPSTR   OPEN32API _O32_lstrcat( LPSTR, LPCSTR );

int       OPEN32API _O32_lstrcmp( LPCSTR, LPCSTR );

int       OPEN32API _O32_lstrcmpi( LPCSTR, LPCSTR );

LPSTR   OPEN32API _O32_lstrcpy( LPSTR, LPCSTR );

int       OPEN32API _O32_lstrlen( LPCSTR );

/* -----------------------------------------------------------------*/


int       OPEN32API _O32_WinCallWinMain(int, char *[], FNMAIN_O32, int);

/* The following functions are used for translating data between the native
 * Presentation Manager format and the Developer API Extensions equivalents.
 */
typedef enum { WINX2PM, PM2WINX} XLATEDIR;

/* Use the GDI object type to specify the type of handle passing in
 */
BOOL    OPEN32API _O32_WinTranslateDevicePoints( HDC, HWND, PPOINT, INT, XLATEDIR );
BOOL    OPEN32API _O32_WinTranslateDeviceRects ( HDC, HWND, PRECT,  INT, XLATEDIR );
ULONG   OPEN32API _O32_WinTranslateGraphicsObjectHandle( HGDIOBJ, XLATEDIR, ULONG );
ULONG   OPEN32API _O32_WinTranslateMnemonicString( LPCSTR, LPSTR *, ULONG, XLATEDIR );
DWORD   OPEN32API _O32_WinQueryTranslateMode( VOID );
BOOL    OPEN32API _O32_WinSetTranslateMode( DWORD );

inline int O32_WinMain(HINSTANCE a, HINSTANCE b, LPSTR c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinMain(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_AbortDoc(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AbortDoc(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AbortPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AbortPath(a);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_AddAtom(LPCSTR a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AddAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_AddFontResource(LPCSTR a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AddFontResource(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AdjustWindowRect(PRECT a, DWORD b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AdjustWindowRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AdjustWindowRectEx(PRECT a, DWORD b, BOOL c, DWORD d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AdjustWindowRectEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AngleArc(HDC a, int b, int c, DWORD d, float e, float f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AngleArc(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AnimatePalette(HPALETTE a, UINT b, UINT c, CONST PALETTEENTRY *d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AnimatePalette(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_AppendMenu(HMENU a, UINT b, UINT c, LPCSTR d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_AppendMenu(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Arc(HDC a, int b, int c, int d, int e, int f, int g, int h, int i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Arc(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ArcTo(HDC a, int b, int c, int d, int e, int f, int g, int h, int i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ArcTo(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_ArrangeIconicWindows(HWND a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ArrangeIconicWindows(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Beep(DWORD a, DWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Beep(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HDWP O32_BeginDeferWindowPos(int a)
{
 HDWP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_BeginDeferWindowPos(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_BeginPaint(HWND a, PPAINTSTRUCT b)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_BeginPaint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_BeginPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_BeginPath(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_BitBlt(HDC a, int b, int c, int d, int e, HDC f, int g, int h, DWORD i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_BitBlt(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_BringWindowToTop(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_BringWindowToTop(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CallMsgFilter(LPMSG a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CallMsgFilter(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_CallNextHookEx(HHOOK a, int b, WPARAM c, LPARAM d)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CallNextHookEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_CallWindowProc(WNDPROC_O32 a, HWND b, UINT c, WPARAM d, LPARAM e)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CallWindowProc(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ChangeClipboardChain(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ChangeClipboardChain(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_CharLower(LPSTR a)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharLower(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_CharLowerBuff(LPSTR a, DWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharLowerBuff(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_CharUpper(LPSTR a)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharUpper(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_CharUpperBuff(LPSTR a, DWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharUpperBuff(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CheckDlgButton(HWND a, int b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CheckDlgButton(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_CheckMenuItem(HMENU a, UINT b, UINT c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CheckMenuItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CheckRadioButton(HWND a, int b, int c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CheckRadioButton(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_ChildWindowFromPoint(HWND a, POINT b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ChildWindowFromPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ChooseColor(LPCHOOSECOLORA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ChooseColor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ChooseFont(LPCHOOSEFONTA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ChooseFont(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Chord(HDC a, int b, int c, int d, int e, int f, int g, int h, int i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Chord(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ClientToScreen(HWND a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ClientToScreen(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ClipCursor(CONST RECT *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ClipCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CloseClipboard()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseClipboard();
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_CloseEnhMetaFile(HDC a)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseEnhMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CloseFigure(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseFigure(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CloseHandle(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline HMETAFILE O32_CloseMetaFile(HDC a)
{
 HMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CloseWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CloseWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_CombineRgn(HRGN a, HRGN b, HRGN c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CombineRgn(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_CommDlgExtendedError()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CommDlgExtendedError();
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_CompareFileTime(FILETIME *a, FILETIME *b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CompareFileTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HCURSOR O32_CopyCursor(HCURSOR a)
{
 HCURSOR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_CopyEnhMetaFile(HENHMETAFILE a, LPCSTR b)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyEnhMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CopyFile(LPCSTR a, LPCSTR b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyFile(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HICON O32_CopyIcon(HICON a)
{
 HICON yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyIcon(a);
    SetFS(sel);

    return yyrc;
} 

inline HMETAFILE O32_CopyMetaFile(HMETAFILE a, LPCSTR b)
{
 HMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CopyRect(PRECT a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CopyRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_CountClipboardFormats()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CountClipboardFormats();
    SetFS(sel);

    return yyrc;
} 

inline HACCEL O32_CreateAcceleratorTable(LPACCEL a, int b)
{
 HACCEL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateAcceleratorTable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP O32_CreateBitmap(int a, int b, UINT c, UINT d, CONST void *e)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateBitmap(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP O32_CreateBitmapIndirect(CONST BITMAP *a)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateBitmapIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HBRUSH O32_CreateBrushIndirect(LPLOGBRUSH a)
{
 HBRUSH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateBrushIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CreateCaret(HWND a, HBITMAP b, int c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateCaret(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP O32_CreateCompatibleBitmap(HDC a, int b, int c)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateCompatibleBitmap(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_CreateCompatibleDC(HDC a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateCompatibleDC(a);
    SetFS(sel);

    return yyrc;
} 

inline HCURSOR O32_CreateCursor(HINSTANCE a, int b, int c, int d, int e, CONST VOID *f, CONST VOID *g)
{
 HCURSOR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateCursor(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_CreateDC(LPCSTR a, LPCSTR b, LPCSTR c, CONST DEVMODEA *d)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDC(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HBRUSH O32_CreateDIBPatternBrushPt(CONST VOID *a, UINT b)
{
 HBRUSH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDIBPatternBrushPt(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP O32_CreateDIBitmap(HDC a, CONST BITMAPINFOHEADER *b, DWORD c, CONST void *d, CONST BITMAPINFO *e, UINT f)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDIBitmap(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateDialog(HINSTANCE a, LPCSTR b, HWND c, DLGPROC_O32 d)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDialog(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateDialogParam(HINSTANCE a, LPCSTR b, HWND c, DLGPROC_O32 d, LPARAM e)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDialogParam(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateDialogIndirect(HINSTANCE a, CONST DLGTEMPLATE *b, HWND c, DLGPROC_O32 d)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDialogIndirect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateDialogIndirectParam(HINSTANCE a, CONST DLGTEMPLATE *b, HWND c, DLGPROC_O32 d, LPARAM e)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDialogIndirectParam(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CreateDirectory(LPCSTR a, PSECURITY_ATTRIBUTES b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateDirectory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreateEllipticRgn(int a, int b, int c, int d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateEllipticRgn(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreateEllipticRgnIndirect(CONST RECT *a)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateEllipticRgnIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_CreateEnhMetaFile(HDC a, LPCSTR b, CONST RECT *c, LPCSTR d)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateEnhMetaFile(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_CreateEvent(PSECURITY_ATTRIBUTES a, BOOL b, BOOL c, LPCSTR d)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateEvent(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_CreateFile(LPCSTR a, DWORD b, DWORD c, PSECURITY_ATTRIBUTES d, DWORD e, DWORD f, HANDLE g)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateFile(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline HFONT O32_CreateFont(int a, int b, int c, int d, int e, DWORD f, DWORD g, DWORD h, DWORD i, DWORD j, DWORD k, DWORD l, DWORD m, LPCSTR n)
{
 HFONT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateFont(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    SetFS(sel);

    return yyrc;
} 

inline HFONT O32_CreateFontIndirect(CONST LOGFONTA *a)
{
 HFONT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateFontIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HBRUSH O32_CreateHatchBrush(int a, COLORREF b)
{
 HBRUSH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateHatchBrush(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_CreateIC(LPCSTR a, LPCSTR b, LPCSTR c, CONST DEVMODEA *d)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateIC(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HICON O32_CreateIcon(HINSTANCE a, int b, int c, BYTE d, BYTE e, CONST BYTE *f, CONST BYTE *g)
{
 HICON yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateIcon(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline HICON O32_CreateIconFromResource(PBYTE a, DWORD b, BOOL c, DWORD d)
{
 HICON yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateIconFromResource(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HICON O32_CreateIconIndirect(LPICONINFO a)
{
 HICON yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateIconIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_CreateMenu()
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateMenu();
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_CreateMetaFile(LPCSTR a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateMDIWindow(LPSTR a, LPSTR b, DWORD c, int d, int e, int f, int g, HWND h, HINSTANCE i, LPARAM j)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateMDIWindow(a, b, c, d, e, f, g, h, i, j);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_CreateMutex(PSECURITY_ATTRIBUTES a, BOOL b, LPCSTR c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateMutex(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HPALETTE O32_CreatePalette(CONST LOGPALETTE *a)
{
 HPALETTE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePalette(a);
    SetFS(sel);

    return yyrc;
} 

inline HBRUSH O32_CreatePatternBrush(HBITMAP a)
{
 HBRUSH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePatternBrush(a);
    SetFS(sel);

    return yyrc;
} 

inline HPEN O32_CreatePen(int a, int b, COLORREF c)
{
 HPEN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePen(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HPEN O32_CreatePenIndirect(CONST LOGPEN *a)
{
 HPEN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePenIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreatePolyPolygonRgn(CONST POINT *a, CONST INT *b, int c, int d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePolyPolygonRgn(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreatePolygonRgn(CONST POINT *a, int b, int c)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePolygonRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_CreatePopupMenu()
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreatePopupMenu();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CreateProcess(LPCSTR a, LPCSTR b, PSECURITY_ATTRIBUTES c, PSECURITY_ATTRIBUTES d, BOOL e, DWORD f, PVOID g, LPCSTR h, LPSTARTUPINFOA i, LPPROCESS_INFORMATION j)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateProcess(a, b, c, d, e, f, g, h, i, j);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreateRectRgn(int a, int b, int c, int d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateRectRgn(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreateRectRgnIndirect(CONST RECT *a)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateRectRgnIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_CreateRoundRectRgn(int a, int b, int c, int d, int e, int f)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateRoundRectRgn(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_CreateSemaphore(PSECURITY_ATTRIBUTES a, LONG b, LONG c, LPSTR d)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateSemaphore(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HBRUSH O32_CreateSolidBrush(COLORREF a)
{
 HBRUSH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateSolidBrush(a);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_CreateThread(PSECURITY_ATTRIBUTES a, DWORD b, PTHREAD_START_ROUTINE_O32 c, PVOID d, DWORD e, PDWORD f)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateThread(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_CreateWindowEx(DWORD a, LPCSTR b, LPCSTR c, DWORD d, int e, int f, int g, int h, HWND i, HMENU j, HINSTANCE k, PVOID l)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateWindowEx(a, b, c, d, e, f, g, h, i, j, k, l);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DPtoLP(HDC a, PPOINT b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DPtoLP(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeAbandonTransaction(DWORD a, HCONV b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeAbandonTransaction(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline PBYTE O32_DdeAccessData(HDDEDATA a, PDWORD b)
{
 PBYTE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeAccessData(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HDDEDATA O32_DdeAddData(HDDEDATA a, PVOID b, DWORD c, DWORD d)
{
 HDDEDATA yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeAddData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HDDEDATA O32_DdeClientTransaction(PVOID a, DWORD b, HCONV c, HSZ d, UINT e, UINT f, DWORD g, PDWORD h)
{
 HDDEDATA yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeClientTransaction(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DdeCmpStringHandles(HSZ a, HSZ b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeCmpStringHandles(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HCONV O32_DdeConnect(DWORD a, HSZ b, HSZ c, LPCONVCONTEXT d)
{
 HCONV yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeConnect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HCONVLIST O32_DdeConnectList(DWORD a, HSZ b, HSZ c, HCONVLIST d, LPCONVCONTEXT e)
{
 HCONVLIST yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeConnectList(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HDDEDATA O32_DdeCreateDataHandle(DWORD a, PVOID b, DWORD c, DWORD d, HSZ e, UINT f, UINT g)
{
 HDDEDATA yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeCreateDataHandle(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline HSZ O32_DdeCreateStringHandle(DWORD a, LPCSTR b, int c)
{
 HSZ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeCreateStringHandle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeDisconnect(HCONV a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeDisconnect(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeDisconnectList(HCONVLIST a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeDisconnectList(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeEnableCallback(DWORD a, HCONV b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeEnableCallback(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeFreeDataHandle(HDDEDATA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeFreeDataHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeFreeStringHandle(DWORD a, HSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeFreeStringHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_DdeGetData(HDDEDATA a, PVOID b, DWORD c, DWORD d)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeGetData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_DdeGetLastError(DWORD a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeGetLastError(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_DdeInitialize(PDWORD a, PFNCALLBACK_O32 b, DWORD c, DWORD d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeInitialize(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeKeepStringHandle(DWORD a, HSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeKeepStringHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HDDEDATA O32_DdeNameService(DWORD a, HSZ b, HSZ c, UINT d)
{
 HDDEDATA yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeNameService(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdePostAdvise(DWORD a, HSZ b, HSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdePostAdvise(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_DdeQueryConvInfo(HCONV a, DWORD b, LPCONVINFO c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeQueryConvInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HCONV O32_DdeQueryNextServer(HCONVLIST a, HCONV b)
{
 HCONV yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeQueryNextServer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_DdeQueryString(DWORD a, HSZ b, LPSTR c, DWORD d, int e)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeQueryString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HCONV O32_DdeReconnect(HCONV a)
{
 HCONV yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeReconnect(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeSetUserHandle(HCONV a, DWORD b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeSetUserHandle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeUnaccessData(HDDEDATA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeUnaccessData(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DdeUninitialize(DWORD a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DdeUninitialize(a);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_DefDlgProc(HWND a, UINT b, WPARAM c, LPARAM d)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DefDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HDWP O32_DeferWindowPos(HDWP a, HWND b, HWND c, int d, int e, int f, int g, UINT h)
{
 HDWP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeferWindowPos(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_DefFrameProc(HWND a, HWND b, UINT c, WPARAM d, LPARAM e)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DefFrameProc(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_DefMDIChildProc(HWND a, UINT b, WPARAM c, LPARAM d)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DefMDIChildProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_DefWindowProc(HWND a, UINT b, WPARAM c, LPARAM d)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DefWindowProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_DeleteAtom(ATOM a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_DeleteCriticalSection(CRITICAL_SECTION *a)
{
 USHORT sel = RestoreOS2FS();

    _O32_DeleteCriticalSection(a);
    SetFS(sel);
} 

inline BOOL O32_DeleteDC(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteDC(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DeleteEnhMetaFile(HENHMETAFILE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteEnhMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DeleteFile(LPCSTR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DeleteMenu(HMENU a, UINT b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteMenu(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DeleteMetaFile(HMETAFILE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DeleteObject(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteObject(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyAcceleratorTable(HACCEL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyAcceleratorTable(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyCaret()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyCaret();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyCursor(HCURSOR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyIcon(HICON a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyIcon(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyMenu(HMENU a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyMenu(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DestroyWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DestroyWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_DeviceCapabilities(LPCSTR a, LPCSTR b, WORD c, LPTSTR d, CONST DEVMODEA *e)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeviceCapabilities(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DialogBox(HINSTANCE a, LPCSTR b, HWND c, DLGPROC_O32 d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DialogBox(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DialogBoxParam(HINSTANCE a, LPCSTR b, HWND c, DLGPROC_O32 d, LPARAM e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DialogBoxParam(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DialogBoxIndirect(HINSTANCE a, LPDLGTEMPLATEA b, HWND c, DLGPROC_O32 d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DialogBoxIndirect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DialogBoxIndirectParam(HINSTANCE a, LPDLGTEMPLATEA b, HWND c, DLGPROC_O32 d, LPARAM e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DialogBoxIndirectParam(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_DispatchMessage(CONST MSG *a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DispatchMessage(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DlgDirList(HWND a, LPSTR b, int c, int d, UINT e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DlgDirList(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DlgDirListComboBox(HWND a, LPSTR b, int c, int d, UINT e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DlgDirListComboBox(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DlgDirSelectEx(HWND a, LPSTR b, int c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DlgDirSelectEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DlgDirSelectComboBoxEx(HWND a, LPSTR b, int c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DlgDirSelectComboBoxEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DllEntryPoint(HINSTANCE a, DWORD b, LPVOID c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DllEntryPoint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DosDateTimeToFileTime(WORD a, WORD b, LPFILETIME c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DosDateTimeToFileTime(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_DragAcceptFiles(HWND a, BOOL b)
{
 USHORT sel = RestoreOS2FS();

    _O32_DragAcceptFiles(a, b);
    SetFS(sel);
} 

inline VOID O32_DragFinish(HDROP a)
{
 USHORT sel = RestoreOS2FS();

    _O32_DragFinish(a);
    SetFS(sel);
} 

inline UINT O32_DragQueryFile(HDROP a, int b, LPSTR c, int d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DragQueryFile(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DragQueryPoint(HDROP a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DragQueryPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DrawFocusRect(HDC a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DrawFocusRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DrawIcon(HDC a, int b, int c, HICON d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DrawIcon(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DrawMenuBar(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DrawMenuBar(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_DrawText(HDC a, LPCSTR b, int c, PRECT d, UINT e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DrawText(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_DuplicateHandle(HANDLE a, HANDLE b, HANDLE c, LPHANDLE d, DWORD e, BOOL f, DWORD g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DuplicateHandle(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Ellipse(HDC a, int b, int c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Ellipse(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EmptyClipboard()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EmptyClipboard();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnableMenuItem(HMENU a, UINT b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnableMenuItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnableScrollBar(HWND a, UINT b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnableScrollBar(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnableWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnableWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EndDeferWindowPos(HDWP a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndDeferWindowPos(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EndDialog(HWND a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndDialog(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EndDoc(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndDoc(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EndPage(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndPage(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EndPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndPath(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EndPaint(HWND a, CONST PAINTSTRUCT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndPaint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_EnterCriticalSection(CRITICAL_SECTION *a)
{
 USHORT sel = RestoreOS2FS();

    _O32_EnterCriticalSection(a);
    SetFS(sel);
} 

inline BOOL O32_EnumChildWindows(HWND a, WNDENUMPROC_O32 b, LPARAM c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumChildWindows(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_EnumClipboardFormats(UINT a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumClipboardFormats(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnumEnhMetaFile(HDC a, HENHMETAFILE b, ENHMFENUMPROC_O32 c, PVOID d, CONST RECT *e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumEnhMetaFile(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EnumFonts(HDC a, LPCSTR b, OLDFONTENUMPROC_O32 c, LPARAM d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumFonts(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EnumFontFamilies(HDC a, LPCSTR b, FONTENUMPROC_O32 c, LPARAM d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumFontFamilies(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnumMetaFile(HDC a, HMETAFILE b, MFENUMPROC_O32 c, LPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumMetaFile(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EnumObjects(HDC a, int b, GOBJENUMPROC_O32 c, LPARAM d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumObjects(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnumPrinters(DWORD a, LPTSTR b, DWORD c, LPBYTE d, DWORD e, LPDWORD f, LPDWORD g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumPrinters(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EnumProps(HWND a, PROPENUMPROC_O32 b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumProps(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_EnumPropsEx(HWND a, PROPENUMPROCEX_O32 b, LPARAM c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumPropsEx(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnumThreadWindows(DWORD a, WNDENUMPROC_O32 b, LPARAM c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumThreadWindows(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EnumWindows(WNDENUMPROC_O32 a, LPARAM b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EnumWindows(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EqualRect(CONST RECT *a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EqualRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_EqualRgn(HRGN a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EqualRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_Escape(HDC a, int b, int c, LPCSTR d, PVOID e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Escape(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_ExcludeClipRect(HDC a, int b, int c, int d, int e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExcludeClipRect(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ExcludeUpdateRgn(HDC a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExcludeUpdateRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_ExitProcess(UINT a)
{
 USHORT sel = RestoreOS2FS();

    _O32_ExitProcess(a);
    SetFS(sel);
} 

inline VOID O32_ExitThread(DWORD a)
{
 USHORT sel = RestoreOS2FS();

    _O32_ExitThread(a);
    SetFS(sel);
} 

inline BOOL O32_ExitWindows(DWORD a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExitWindows(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ExitWindowsEx(UINT a, DWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExitWindowsEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPEN O32_ExtCreatePen(DWORD a, DWORD b, CONST LOGBRUSH *c, DWORD d, CONST DWORD *e)
{
 HPEN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtCreatePen(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_ExtCreateRegion(CONST XFORM *a, DWORD b, CONST RGNDATA *c)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtCreateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ExtFloodFill(HDC a, int b, int c, COLORREF d, UINT e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtFloodFill(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_ExtSelectClipRgn(HDC a, HRGN b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtSelectClipRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ExtTextOut(HDC a, int b, int c, UINT d, CONST RECT *e, LPCSTR f, UINT g, CONST int *h)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtTextOut(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_FatalAppExit(UINT a, LPCSTR b)
{
 USHORT sel = RestoreOS2FS();

    _O32_FatalAppExit(a, b);
    SetFS(sel);
} 

inline VOID O32_FatalExit(UINT a)
{
 USHORT sel = RestoreOS2FS();

    _O32_FatalExit(a);
    SetFS(sel);
} 

inline BOOL O32_FileTimeToDosDateTime(CONST FILETIME *a, LPWORD b, LPWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FileTimeToDosDateTime(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FileTimeToLocalFileTime(CONST FILETIME *a, FILETIME *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FileTimeToLocalFileTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FileTimeToSystemTime(CONST FILETIME *a, LPSYSTEMTIME b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FileTimeToSystemTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FillPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FillPath(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_FillRect(HDC a, CONST RECT *b, HBRUSH c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FillRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FillRgn(HDC a, HRGN b, HBRUSH c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FillRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_FindAtom(LPCSTR a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FindClose(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindClose(a);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_FindFirstFile(LPCSTR a, LPWIN32_FIND_DATAA b)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindFirstFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FindNextFile(HANDLE a, LPWIN32_FIND_DATAA b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindNextFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HRSRC O32_FindResource(HINSTANCE a, LPCSTR b, LPCSTR c)
{
 HRSRC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindResource(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_FindText(LPFINDREPLACEA a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindText(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_FindWindow(LPCSTR a, LPCSTR b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FindWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FlashWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FlashWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FlattenPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FlattenPath(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FlushFileBuffers(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FlushFileBuffers(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_FrameRect(HDC a, CONST RECT *b, HBRUSH c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FrameRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FrameRgn(HDC a, HRGN b, HBRUSH c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FrameRgn(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FreeDDElParam(UINT a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FreeDDElParam(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FreeLibrary(HINSTANCE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FreeLibrary(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GdiComment(HDC hdc, UINT cbSize, CONST BYTE * lpData)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GdiComment(hdc, cbSize, lpData);
    SetFS(sel);

    return yyrc;
}

inline UINT O32_GetACP()
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetACP();
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetActiveWindow()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetActiveWindow();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetArcDirection(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetArcDirection(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetAspectRatioFilterEx(HDC a, PSIZE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetAspectRatioFilterEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetAtomName(ATOM a, LPSTR b, int c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetAtomName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetBitmapBits(HBITMAP a, LONG b, PVOID c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBitmapBits(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetBitmapDimensionEx(HBITMAP a, PSIZE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBitmapDimensionEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_GetBkColor(HDC a)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBkColor(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetBkMode(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBkMode(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetBoundsRect(HDC a, PRECT b, UINT c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBoundsRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetBrushOrgEx(HDC a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetBrushOrgEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetCapture()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCapture();
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetCaretBlinkTime()
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCaretBlinkTime();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetCaretPos(PPOINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCaretPos(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetCharABCWidths(HDC a, UINT b, UINT c, LPABC d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCharABCWidths(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetCharWidth(HDC a, UINT b, UINT c, PINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCharWidth(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetClassInfo(HINSTANCE a, LPCSTR b, WNDCLASSA *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClassInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetClassLong(HWND a, int b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClassLong(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetClassName(HWND a, LPSTR b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClassName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline WORD O32_GetClassWord(HWND a, int b)
{
 WORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClassWord(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetClientRect(HWND a, PRECT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClientRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_GetClipboardData(UINT a)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipboardData(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetClipboardFormatName(UINT a, LPSTR b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipboardFormatName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetClipboardOwner()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipboardOwner();
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetClipboardViewer()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipboardViewer();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetClipBox(HDC a, PRECT b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetClipCursor(PRECT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetClipRgn(HDC a, HRGN b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_GetCommandLine()
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCommandLine();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetCurrentDirectory(DWORD a, LPSTR b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentDirectory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HGDIOBJ O32_GetCurrentObject(HDC a, DWORD b)
{
 HGDIOBJ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentObject(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetCurrentPositionEx(HDC a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentPositionEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_GetCurrentProcess()
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentProcess();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetCurrentProcessId()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentProcessId();
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_GetCurrentThread()
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentThread();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetCurrentThreadId()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentThreadId();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetCurrentTime()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCurrentTime();
    SetFS(sel);

    return yyrc;
} 

inline HCURSOR O32_GetCursor()
{
 HCURSOR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCursor();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetCursorPos(PPOINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetCursorPos(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_GetDC(HWND a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDC(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_GetDCEx(HWND a, HRGN b, DWORD c)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDCEx(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetDCOrgEx(HDC a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDCOrgEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetDIBits(HDC a, HBITMAP b, UINT c, UINT d, void *e, PBITMAPINFO f, UINT g)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDIBits(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetDeviceCaps(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDeviceCaps(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetDialogBaseUnits()
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDialogBaseUnits();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetDiskFreeSpace(LPCSTR a, PDWORD b, PDWORD c, PDWORD d, PDWORD e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDiskFreeSpace(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetDlgCtrlID(HWND a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDlgCtrlID(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetDlgItem(HWND a, int b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDlgItem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetDlgItemInt(HWND a, int b, PBOOL c, BOOL d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDlgItemInt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetDlgItemText(HWND a, int b, LPSTR c, int d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDlgItemText(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetDoubleClickTime()
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDoubleClickTime();
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetDriveType(LPCSTR a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDriveType(a);
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_GetEnhMetaFile(LPCSTR a)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnhMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetEnhMetaFileBits(HENHMETAFILE a, UINT b, PBYTE c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnhMetaFileBits(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetEnhMetaFileHeader(HENHMETAFILE a, UINT b, LPENHMETAHEADER c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnhMetaFileHeader(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetEnhMetaFilePaletteEntries(HENHMETAFILE a, UINT b, LPPALETTEENTRY c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnhMetaFilePaletteEntries(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_GetEnvironmentStrings()
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnvironmentStrings();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetEnvironmentVariable(LPCSTR a, LPSTR b, DWORD c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetEnvironmentVariable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetExitCodeProcess(HANDLE a, LPDWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetExitCodeProcess(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetExitCodeThread(HANDLE a, LPDWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetExitCodeThread(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetFileAttributes(LPSTR a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileAttributes(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetFileInformationByHandle(HANDLE a, BY_HANDLE_FILE_INFORMATION *b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileInformationByHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetFileSize(HANDLE a, PDWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileSize(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetFileTime(HANDLE a, FILETIME *b, FILETIME *c, FILETIME *d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileTime(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline short O32_GetFileTitle(LPCSTR a, LPSTR b, WORD c)
{
 short yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileTitle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetFileType(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFileType(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetFocus()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFocus();
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetForegroundWindow()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetForegroundWindow();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetFullPathName(LPCSTR a, DWORD b, LPSTR c, LPSTR *d)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetFullPathName(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetGraphicsMode(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetGraphicsMode(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetIconInfo(HICON a, LPICONINFO b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetIconInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetKerningPairs(HDC a, DWORD b, LPKERNINGPAIR c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKerningPairs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetKeyboardType(int a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKeyboardType(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetKeyNameText(LPARAM a, LPSTR b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKeyNameText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline SHORT O32_GetAsyncKeyState(int a)
{
 SHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetAsyncKeyState(a);
    SetFS(sel);

    return yyrc;
} 

inline SHORT O32_GetKeyState(int a)
{
 SHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKeyState(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetLastActivePopup(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetLastActivePopup(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetLastError()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetLastError();
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_GetLocalTime(LPSYSTEMTIME a)
{
 USHORT sel = RestoreOS2FS();

    _O32_GetLocalTime(a);
    SetFS(sel);
} 

inline DWORD O32_GetLogicalDriveStrings(DWORD a, LPSTR b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetLogicalDriveStrings(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetLogicalDrives()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetLogicalDrives();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetMapMode(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMapMode(a);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_GetMenu(HWND a)
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenu(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetMenuCheckMarkDimensions()
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenuCheckMarkDimensions();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetMenuItemCount(HMENU a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenuItemCount(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetMenuItemID(HMENU a, int b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenuItemID(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetMenuState(HMENU a, UINT b, UINT c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenuState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetMenuString(HMENU a, UINT b, LPSTR c, int d, UINT e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMenuString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetMessage(LPMSG a, HWND b, UINT c, UINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMessage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetMessageExtraInfo()
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMessageExtraInfo();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetMessagePos()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMessagePos();
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetMessageTime()
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMessageTime();
    SetFS(sel);

    return yyrc;
} 

inline HMETAFILE O32_GetMetaFile(LPCSTR a)
{
 HMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetMetaFileBitsEx(HMETAFILE a, UINT b, LPVOID c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMetaFileBitsEx(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetMiterLimit(HDC a, float *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetMiterLimit(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetModuleFileName(HINSTANCE a, LPSTR b, DWORD c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetModuleFileName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HMODULE O32_GetModuleHandle(LPCSTR a)
{
 HMODULE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetModuleHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_GetNearestColor(HDC a, COLORREF b)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetNearestColor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetNearestPaletteIndex(HPALETTE a, COLORREF b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetNearestPaletteIndex(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetNextDlgGroupItem(HWND a, HWND b, BOOL c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetNextDlgGroupItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetNextDlgTabItem(HWND a, HWND b, BOOL c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetNextDlgTabItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetNextWindow(HWND a, UINT b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetNextWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetOEMCP()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetOEMCP();
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetObject(HGDIOBJ a, int b, void *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetObjectType(HGDIOBJ a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetObjectType(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetOpenClipboardWindow()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetOpenClipboardWindow();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetOpenFileName(LPOPENFILENAMEA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetOpenFileName(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetOutlineTextMetrics(HDC a, UINT b, LPOUTLINETEXTMETRICA c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetOutlineTextMetrics(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetOverlappedResult(HANDLE a, LPOVERLAPPED b, LPDWORD c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetOverlappedResult(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetPaletteEntries(HPALETTE a, UINT b, UINT c, PPALETTEENTRY d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPaletteEntries(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetParent(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetParent(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetPath(HDC a, PPOINT b, PBYTE c, int d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPath(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_GetPixel(HDC a, int b, int c)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPixel(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetPolyFillMode(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPolyFillMode(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetPriorityClass(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPriorityClass(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetPriorityClipboardFormat(PUINT a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPriorityClipboardFormat(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetPrivateProfileInt(LPCSTR a, LPCSTR b, INT c, LPCSTR d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPrivateProfileInt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetPrivateProfileString(LPCSTR a, LPCSTR b, LPCSTR c, LPSTR d, DWORD e, LPCSTR f)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetPrivateProfileString(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline FARPROC O32_GetProcAddress(HMODULE a, LPCSTR b)
{
 FARPROC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetProcAddress(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetProfileInt(LPCSTR a, LPCSTR b, INT c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetProfileInt(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetProfileString(LPCSTR a, LPCSTR b, LPCSTR c, LPSTR d, DWORD e)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetProfileString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_GetProp(HWND a, LPCSTR b)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetProp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetQueueStatus(UINT a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetQueueStatus(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetROP2(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetROP2(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetRasterizerCaps(LPRASTERIZER_STATUS a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetRasterizerCaps(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetRgnBox(HRGN a, PRECT b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetRgnBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetRegionData(HRGN a, DWORD b, PRGNDATA c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetRegionData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetSaveFileName(LPOPENFILENAMEA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSaveFileName(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetScrollPos(HWND a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetScrollPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetScrollRange(HWND a, int b, int *c, int *d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetScrollRange(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_GetStdHandle(DWORD a)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetStdHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline HGDIOBJ O32_GetStockObject(int a)
{
 HGDIOBJ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetStockObject(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetStretchBltMode(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetStretchBltMode(a);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_GetSubMenu(HWND a, int b)
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSubMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetSysColor(int a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSysColor(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetSystemDirectory(LPSTR a, UINT b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSystemDirectory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_GetSystemMenu(HWND a, BOOL b)
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSystemMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetSystemMetrics(int a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSystemMetrics(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetSystemPaletteEntries(HDC a, UINT b, UINT c, PPALETTEENTRY d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetSystemPaletteEntries(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline void O32_GetSystemTime(LPSYSTEMTIME a)
{
 USHORT sel = RestoreOS2FS();

    _O32_GetSystemTime(a);
    SetFS(sel);
} 

inline DWORD O32_GetTabbedTextExtent(HDC a, LPCSTR b, int c, int d, int *e)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTabbedTextExtent(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetTempFileName(LPCSTR a, LPCSTR b, UINT c, LPSTR d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTempFileName(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetTempPath(DWORD a, LPSTR b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTempPath(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetTextAlign(HDC a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextAlign(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetTextCharacterExtra(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextCharacterExtra(a);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_GetTextColor(HDC a)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextColor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetTextExtentPoint(HDC a, LPCSTR b, int c, PSIZE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextExtentPoint(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetTextExtentPoint32(HDC a, LPCSTR b, int c, PSIZE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextExtentPoint32(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetTextFace(HDC a, int b, LPSTR c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextFace(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetTextMetrics(HDC a, LPTEXTMETRICA b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTextMetrics(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetThreadPriority(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetThreadPriority(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetTickCount()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTickCount();
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetTimeZoneInformation(LPTIME_ZONE_INFORMATION a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTimeZoneInformation(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetTopWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetTopWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetUpdateRect(HWND a, PRECT b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetUpdateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetUpdateRgn(HWND a, HRGN b, BOOL c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetUpdateRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetViewportExtEx(HDC a, PSIZE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetViewportExtEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetViewportOrgEx(HDC a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetViewportOrgEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetVolumeInformation(LPCSTR a, LPSTR b, DWORD c, PDWORD d, PDWORD e, PDWORD f, LPSTR g, DWORD h)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetVolumeInformation(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetWinMetaFileBits(HENHMETAFILE a, UINT b, PBYTE c, int d, HDC e)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWinMetaFileBits(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetWindow(HWND a, UINT b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HDC O32_GetWindowDC(HWND a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowDC(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetWindowExtEx(HDC a, PSIZE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowExtEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_GetWindowLong(HWND a, int b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowLong(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetWindowOrgEx(HDC a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowOrgEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetWindowPlacement(HWND a, LPWINDOWPLACEMENT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowPlacement(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetWindowRect(HWND a, PRECT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GetWindowsDirectory(LPSTR a, UINT b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowsDirectory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetWindowText(HWND a, LPSTR b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_GetWindowTextLength(HWND a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowTextLength(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GetWindowThreadProcessId(HWND a, PDWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowThreadProcessId(a, b);
    SetFS(sel);

    return yyrc;
} 

inline WORD O32_GetWindowWord(HWND a, int b)
{
 WORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWindowWord(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GetWorldTransform(HDC a, LPXFORM b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetWorldTransform(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_GlobalAddAtom(LPCSTR a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalAddAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline HGLOBAL O32_GlobalAlloc(UINT a, DWORD b)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalAlloc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_GlobalDeleteAtom(ATOM a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalDeleteAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline HGLOBAL O32_GlobalDiscard(HGLOBAL a)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalDiscard(a);
    SetFS(sel);

    return yyrc;
} 

inline ATOM O32_GlobalFindAtom(LPCSTR a)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalFindAtom(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GlobalFlags(HGLOBAL a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalFlags(a);
    SetFS(sel);

    return yyrc;
} 

inline HGLOBAL O32_GlobalFree(HGLOBAL a)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalFree(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_GlobalGetAtomName(ATOM a, LPSTR b, int c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalGetAtomName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HGLOBAL O32_GlobalHandle(PVOID a)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_GlobalLock(HGLOBAL a)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalLock(a);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_GlobalMemoryStatus(LPMEMORYSTATUS a)
{
 USHORT sel = RestoreOS2FS();

    _O32_GlobalMemoryStatus(a);
    SetFS(sel);
} 

inline HGLOBAL O32_GlobalReAlloc(HGLOBAL a, DWORD b, UINT c)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalReAlloc(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_GlobalSize(HGLOBAL a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalSize(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_GlobalUnlock(HGLOBAL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GlobalUnlock(a);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_HeapAlloc(HANDLE a, DWORD b, DWORD c)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapAlloc(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_HeapCreate(DWORD a, DWORD b, DWORD c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapCreate(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_HeapDestroy(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapDestroy(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_HeapFree(HANDLE a, DWORD b, PVOID c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapFree(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_HeapReAlloc(HANDLE a, DWORD b, PVOID c, DWORD d)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapReAlloc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_HeapSize(HANDLE a, DWORD b, PVOID c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HeapSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_HideCaret(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HideCaret(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_HiliteMenuItem(HWND a, HMENU b, UINT c, UINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_HiliteMenuItem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InflateRect(PRECT a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InflateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InSendMessage()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InSendMessage();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InitAtomTable(DWORD a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InitAtomTable(a);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_InitializeCriticalSection(CRITICAL_SECTION *a)
{
 USHORT sel = RestoreOS2FS();

    _O32_InitializeCriticalSection(a);
    SetFS(sel);
} 

inline BOOL O32_InsertMenu(HMENU a, UINT b, UINT c, UINT d, LPCSTR e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InsertMenu(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_InterlockedDecrement(PLONG a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InterlockedDecrement(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_InterlockedExchange(PLONG a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InterlockedExchange(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_InterlockedIncrement(PLONG a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InterlockedIncrement(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_IntersectClipRect(HDC a, int b, int c, int d, int e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IntersectClipRect(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IntersectRect(PRECT a, CONST RECT *b, CONST RECT *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IntersectRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InvalidateRect(HWND a, CONST RECT *b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InvalidateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InvalidateRgn(HWND a, HRGN b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InvalidateRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InvertRect(HDC a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InvertRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_InvertRgn(HDC a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_InvertRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadCodePtr(FARPROC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadCodePtr(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadReadPtr(CONST VOID *a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadReadPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadStringPtr(LPCSTR a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadStringPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadWritePtr(PVOID a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadWritePtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsChild(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsChild(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsClipboardFormatAvailable(UINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsClipboardFormatAvailable(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsDBCSLeadByte(BYTE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsDBCSLeadByte(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsDialogMessage(HWND a, LPMSG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsDialogMessage(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsDlgButtonChecked(HWND a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsDlgButtonChecked(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsIconic(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsIconic(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsMenu(HMENU a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsMenu(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsRectEmpty(CONST RECT *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsRectEmpty(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsWindowEnabled(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsWindowEnabled(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsWindowVisible(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsWindowVisible(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsZoomed(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsZoomed(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_KillTimer(HWND a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_KillTimer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LPtoDP(HDC a, PPOINT b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LPtoDP(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_LeaveCriticalSection(CRITICAL_SECTION *a)
{
 USHORT sel = RestoreOS2FS();

    _O32_LeaveCriticalSection(a);
    SetFS(sel);
} 

inline BOOL O32_LineDDA(int a, int b, int c, int d, LINEDDAPROC_O32 e, LPARAM f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LineDDA(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LineTo(HDC a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LineTo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HACCEL O32_LoadAccelerators(HINSTANCE a, LPCSTR b)
{
 HACCEL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadAccelerators(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP O32_LoadBitmap(HINSTANCE a, LPCSTR b)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadBitmap(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HCURSOR O32_LoadCursor(HINSTANCE a, LPCSTR b)
{
 HCURSOR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadCursor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HICON O32_LoadIcon(HINSTANCE a, LPCSTR b)
{
 HICON yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadIcon(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HINSTANCE O32_LoadLibrary(LPCSTR a)
{
 HINSTANCE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadLibrary(a);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_LoadMenu(HINSTANCE a, LPCSTR b)
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HMENU O32_LoadMenuIndirect(CONST MENUITEMTEMPLATEHEADER *a)
{
 HMENU yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadMenuIndirect(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_LoadModule(LPCSTR a, PVOID b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadModule(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HGLOBAL O32_LoadResource(HINSTANCE a, HRSRC b)
{
 HGLOBAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadResource(a, b);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_LockResource(HGLOBAL a)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LockResource(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_LoadString(HINSTANCE a, UINT b, LPSTR c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LoadString(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HLOCAL O32_LocalAlloc(UINT a, UINT b)
{
 HLOCAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalAlloc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HLOCAL O32_LocalDiscard(HLOCAL a)
{
 HLOCAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalDiscard(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_LocalFlags(HLOCAL a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalFlags(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LocalFileTimeToFileTime(CONST FILETIME *a, FILETIME *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalFileTimeToFileTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HLOCAL O32_LocalFree(HLOCAL a)
{
 HLOCAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalFree(a);
    SetFS(sel);

    return yyrc;
} 

inline HLOCAL O32_LocalHandle(PVOID a)
{
 HLOCAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LocalUnlock(HLOCAL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalUnlock(a);
    SetFS(sel);

    return yyrc;
} 

inline HLOCAL O32_LocalReAlloc(HLOCAL a, UINT b, UINT c)
{
 HLOCAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalReAlloc(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_LocalSize(HLOCAL a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalSize(a);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_LocalLock(HLOCAL a)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LocalLock(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LockFile(HANDLE a, DWORD b, DWORD c, DWORD d, DWORD e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LockFile(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_LockWindowUpdate(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LockWindowUpdate(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MapDialogRect(HWND a, PRECT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MapDialogRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_MapVirtualKey(UINT a, UINT b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MapVirtualKey(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_MapWindowPoints(HWND a, HWND b, PPOINT c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MapWindowPoints(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MaskBlt(HDC a, int b, int c, int d, int e, HDC f, int g, int h, HBITMAP i, int j, int k, DWORD l)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MaskBlt(a, b, c, d, e, f, g, h, i, j, k, l);
    SetFS(sel);

    return yyrc;
} 

inline int O32_MessageBox(HWND a, LPCSTR b, LPCSTR c, UINT d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MessageBox(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MessageBeep(UINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MessageBeep(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_MsgWaitForMultipleObjects(DWORD a, LPHANDLE b, BOOL c, DWORD d, DWORD e)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MsgWaitForMultipleObjects(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ModifyMenu(HMENU a, UINT b, UINT c, UINT d, LPCSTR e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ModifyMenu(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ModifyWorldTransform(HDC a, LPXFORM b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ModifyWorldTransform(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MoveFile(LPCSTR a, LPCSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MoveFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MoveToEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MoveToEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_MoveWindow(HWND a, int b, int c, int d, int e, BOOL f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MoveWindow(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline int O32_MulDiv(int a, int b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_MulDiv(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_OffsetClipRgn(HDC a, int b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetClipRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OffsetRect(PRECT a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline int O32_OffsetRgn(HRGN a, int b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OffsetViewportOrgEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetViewportOrgEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OffsetWindowOrgEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetWindowOrgEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OpenClipboard(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenClipboard(a);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_OpenEvent(DWORD a, BOOL b, LPCSTR c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenEvent(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HFILE O32_OpenFile(LPCSTR a, LPOFSTRUCT b, UINT c)
{
 HFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenFile(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_OpenMutex(DWORD a, BOOL b, LPCSTR c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenMutex(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_OpenProcess(DWORD a, BOOL b, DWORD c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenProcess(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_OpenSemaphore(DWORD a, BOOL b, LPCSTR c)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OpenSemaphore(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_OutputDebugString(LPCSTR a)
{
 USHORT sel = RestoreOS2FS();

    _O32_OutputDebugString(a);
    SetFS(sel);
} 

inline LONG O32_PackDDElParam(UINT a, UINT b, UINT c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PackDDElParam(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PaintRgn(HDC a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PaintRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PatBlt(HDC a, int b, int c, int d, int e, DWORD f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PatBlt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HRGN O32_PathToRegion(HDC a)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PathToRegion(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PeekMessage(LPMSG a, HWND b, UINT c, UINT d, UINT e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PeekMessage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Pie(HDC a, int b, int c, int d, int e, int f, int g, int h, int i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Pie(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PlayEnhMetaFile(HDC a, HENHMETAFILE b, CONST RECT *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PlayEnhMetaFile(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PlayMetaFile(HDC a, HMETAFILE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PlayMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PlayMetaFileRecord(HDC a, LPHANDLETABLE b, PMETARECORD c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PlayMetaFileRecord(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolyBezier(HDC a, CONST POINT *b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolyBezier(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolyBezierTo(HDC a, CONST POINT *b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolyBezierTo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolyDraw(HDC a, CONST POINT *b, CONST BYTE *c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolyDraw(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Polygon(HDC a, CONST POINT *b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Polygon(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Polyline(HDC a, CONST POINT *b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Polyline(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolylineTo(HDC a, CONST POINT *b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolylineTo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolyPolygon(HDC a, CONST POINT *b, CONST int *c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolyPolygon(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PolyPolyline(HDC a, CONST POINT *b, CONST DWORD *c, DWORD d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PolyPolyline(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_PostQuitMessage(int a)
{
 USHORT sel = RestoreOS2FS();

    _O32_PostQuitMessage(a);
    SetFS(sel);
} 

inline BOOL O32_PostMessage(HWND a, UINT b, WPARAM c, LPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PostMessage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PostThreadMessage(DWORD a, UINT b, WPARAM c, LPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PostThreadMessage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PtInRect(CONST RECT *a, POINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PtInRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PtInRegion(HRGN a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PtInRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PtVisible(HDC a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PtVisible(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PrintDlg(LPPRINTDLGA a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PrintDlg(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_PulseEvent(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PulseEvent(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ReadFile(HANDLE a, PVOID b, DWORD c, PDWORD d, LPOVERLAPPED e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReadFile(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_RealizePalette(HDC a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RealizePalette(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RectInRegion(HRGN a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RectInRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RectVisible(HDC a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RectVisible(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_Rectangle(HDC a, int b, int c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_Rectangle(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RedrawWindow(HWND a, CONST RECT *b, HRGN c, DWORD d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RedrawWindow(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegCloseKey(HKEY a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegCloseKey(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegCreateKey(HKEY a, LPCSTR b, PHKEY c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegCreateKey(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegCreateKeyEx(HKEY a, LPCSTR b, DWORD c, LPSTR d, DWORD e, REGSAM f, LPSECURITY_ATTRIBUTES g, PHKEY h, PDWORD i)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegCreateKeyEx(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegDeleteKey(HKEY a, LPCSTR b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegDeleteKey(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegDeleteValue(HKEY a, LPSTR b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegDeleteValue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegEnumKey(HKEY a, DWORD b, LPSTR c, DWORD d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegEnumKey(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegEnumKeyEx(HKEY a, DWORD b, LPSTR c, PDWORD d, PDWORD e, LPSTR f, PDWORD g, FILETIME *h)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegEnumKeyEx(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegEnumValue(HKEY a, DWORD b, LPSTR c, PDWORD d, PDWORD e, PDWORD f, LPBYTE g, PDWORD h)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegEnumValue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegOpenKey(HKEY a, LPCSTR b, PHKEY c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegOpenKey(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegOpenKeyEx(HKEY a, LPCSTR b, DWORD c, REGSAM d, PHKEY e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegOpenKeyEx(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegQueryInfoKey(HKEY a, LPSTR b, PDWORD c, PDWORD d, PDWORD e, PDWORD f, PDWORD g, PDWORD h, PDWORD i, PDWORD j, PDWORD k, FILETIME *l)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegQueryInfoKey(a, b, c, d, e, f, g, h, i, j, k, l);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegQueryValue(HKEY a, LPCSTR b, LPSTR c, PLONG d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegQueryValue(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegQueryValueEx(HKEY a, LPCSTR b, PDWORD c, PDWORD d, LPBYTE e, PDWORD f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegQueryValueEx(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegSetValue(HKEY a, LPCSTR b, DWORD c, LPCSTR d, DWORD e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegSetValue(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_RegSetValueEx(HKEY a, LPCSTR b, DWORD c, DWORD d, CONST BYTE *e, DWORD f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegSetValueEx(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline WORD O32_RegisterClass(CONST WNDCLASSA *a)
{
 WORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegisterClass(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_RegisterClipboardFormat(LPCSTR a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegisterClipboardFormat(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_RegisterWindowMessage(LPCSTR a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RegisterWindowMessage(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ReleaseCapture()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReleaseCapture();
    SetFS(sel);

    return yyrc;
} 

inline int O32_ReleaseDC(HWND a, HDC b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReleaseDC(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ReleaseMutex(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReleaseMutex(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ReleaseSemaphore(HANDLE a, LONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReleaseSemaphore(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RemoveDirectory(LPCSTR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RemoveDirectory(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RemoveFontResource(LPCSTR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RemoveFontResource(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RemoveMenu(HMENU a, UINT b, UINT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RemoveMenu(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_RemoveProp(HWND a, LPCSTR b)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RemoveProp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_ReplaceText(LPFINDREPLACEA a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReplaceText(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ReplyMessage(LRESULT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReplyMessage(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ResetDC(HDC a, CONST DEVMODEA *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ResetDC(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ResetEvent(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ResetEvent(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ResizePalette(HPALETTE a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ResizePalette(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RestoreDC(HDC a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RestoreDC(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_ResumeThread(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ResumeThread(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_ReuseDDElParam(LONG a, UINT b, UINT c, UINT d, UINT e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReuseDDElParam(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_RoundRect(HDC a, int b, int c, int d, int e, int f, int g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_RoundRect(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SaveDC(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SaveDC(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScaleViewportExtEx(HDC a, int b, int c, int d, int e, PSIZE f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScaleViewportExtEx(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScaleWindowExtEx(HDC a, int b, int c, int d, int e, PSIZE f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScaleWindowExtEx(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScreenToClient(HWND a, PPOINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScreenToClient(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScrollDC(HDC a, int b, int c, CONST RECT *d, CONST RECT *e, HRGN f, PRECT g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScrollDC(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScrollWindow(HWND a, int b, int c, CONST RECT *d, CONST RECT *e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScrollWindow(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ScrollWindowEx(HWND a, int b, int c, CONST RECT *d, CONST RECT *e, HRGN f, PRECT g, UINT h)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ScrollWindowEx(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SearchPath(LPCSTR a, LPCSTR b, LPCSTR c, DWORD d, LPSTR e, LPSTR *f)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SearchPath(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SelectClipRgn(HDC a, HRGN b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SelectClipRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HGDIOBJ O32_SelectObject(HDC a, HGDIOBJ b)
{
 HGDIOBJ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SelectObject(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPALETTE O32_SelectPalette(HDC a, HPALETTE b, BOOL c)
{
 HPALETTE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SelectPalette(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_SendDlgItemMessage(HWND a, int b, UINT c, WPARAM d, LPARAM e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SendDlgItemMessage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LRESULT O32_SendMessage(HWND a, UINT b, WPARAM c, LPARAM d)
{
 LRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SendMessage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_SetActiveWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetActiveWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetArcDirection(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetArcDirection(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_SetBitmapBits(HBITMAP a, DWORD b, CONST VOID *c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBitmapBits(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetBitmapDimensionEx(HBITMAP a, int b, int c, PSIZE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBitmapDimensionEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_SetBkColor(HDC a, COLORREF b)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBkColor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetBkMode(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBkMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_SetBoundsRect(HDC a, CONST RECT *b, UINT c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBoundsRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetBrushOrgEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetBrushOrgEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_SetCapture(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCapture(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetCaretBlinkTime(UINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCaretBlinkTime(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetCaretPos(int a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCaretPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SetClassLong(HWND a, int b, LONG c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetClassLong(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline WORD O32_SetClassWord(HWND a, int b, WORD c)
{
 WORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetClassWord(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HANDLE O32_SetClipboardData(UINT a, HANDLE b)
{
 HANDLE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetClipboardData(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_SetClipboardViewer(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetClipboardViewer(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetCurrentDirectory(LPSTR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCurrentDirectory(a);
    SetFS(sel);

    return yyrc;
} 

inline HCURSOR O32_SetCursor(HCURSOR a)
{
 HCURSOR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetCursorPos(int a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetCursorPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetDIBits(HDC a, HBITMAP b, UINT c, UINT d, CONST VOID *e, CONST BITMAPINFO *f, UINT g)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetDIBits(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetDIBitsToDevice(HDC a, int b, int c, int d, int e, int f, int g, UINT h, UINT i, PVOID j, PBITMAPINFO k, UINT l)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetDIBitsToDevice(a, b, c, d, e, f, g, h, i, j, k, l);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetDlgItemInt(HWND a, int b, UINT c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetDlgItemInt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetDlgItemText(HWND a, int b, LPCSTR c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetDlgItemText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetDoubleClickTime(UINT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetDoubleClickTime(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetEndOfFile(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetEndOfFile(a);
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_SetEnhMetaFileBits(UINT a, CONST BYTE *b)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetEnhMetaFileBits(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetEnvironmentVariable(LPCSTR a, LPCSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetEnvironmentVariable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetEvent(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetEvent(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetFileAttributes(LPCSTR a, DWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetFileAttributes(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SetFilePointer(HANDLE a, LONG b, PLONG c, DWORD d)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetFilePointer(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetFileTime(HANDLE a, CONST FILETIME *b, CONST FILETIME *c, CONST FILETIME *d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetFileTime(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_SetFocus(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetFocus(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetForegroundWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetForegroundWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetGraphicsMode(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetGraphicsMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SetHandleCount(UINT a)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetHandleCount(a);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_SetLastError(DWORD a)
{
 USHORT sel = RestoreOS2FS();

    _O32_SetLastError(a);
    SetFS(sel);
} 

inline BOOL O32_SetLocalTime(CONST SYSTEMTIME *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetLocalTime(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetMapMode(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMapMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SetMapperFlags(HDC a, DWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMapperFlags(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetMenu(HWND a, HMENU b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetMenuItemBitmaps(HMENU a, UINT b, UINT c, HBITMAP d, HBITMAP e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMenuItemBitmaps(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HMETAFILE O32_SetMetaFileBitsEx(UINT a, PBYTE b)
{
 HMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMetaFileBitsEx(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetMiterLimit(HDC a, float b, float *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetMiterLimit(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SetPaletteEntries(HPALETTE a, UINT b, UINT c, CONST PALETTEENTRY *d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetPaletteEntries(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_SetParent(HWND a, HWND b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetParent(a, b);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_SetPixel(HDC a, int b, int c, COLORREF d)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetPixel(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetPolyFillMode(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetPolyFillMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetPriorityClass(HANDLE a, DWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetPriorityClass(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetProp(HWND a, LPCSTR b, HANDLE c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetProp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetRect(PRECT a, int b, int c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetRect(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetRectEmpty(PRECT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetRectEmpty(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetRectRgn(HRGN a, int b, int c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetRectRgn(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetROP2(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetROP2(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetScrollPos(HWND a, int b, int c, BOOL d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetScrollPos(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetScrollRange(HWND a, int b, int c, int d, BOOL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetScrollRange(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetStdHandle(DWORD a, HANDLE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetStdHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetStretchBltMode(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetStretchBltMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SetSysColors(int a, CONST int *b, CONST COLORREF *c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetSysColors(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetSystemTime(CONST SYSTEMTIME *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetSystemTime(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetTimeZoneInformation(CONST TIME_ZONE_INFORMATION *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTimeZoneInformation(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SetTextAlign(HDC a, UINT b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTextAlign(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_SetTextCharacterExtra(HDC a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTextCharacterExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

inline COLORREF O32_SetTextColor(HDC a, COLORREF b)
{
 COLORREF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTextColor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetTextJustification(HDC a, int b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTextJustification(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetThreadPriority(HANDLE a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetThreadPriority(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SetTimer(HWND a, UINT b, UINT c, TIMERPROC_O32 d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetTimer(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetViewportExtEx(HDC a, int b, int c, PSIZE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetViewportExtEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetViewportOrgEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetViewportOrgEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetVolumeLabel(LPCSTR a, LPCSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetVolumeLabel(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWindowExtEx(HDC a, int b, int c, PSIZE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowExtEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_SetWindowLong(HWND a, int b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowLong(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWindowOrgEx(HDC a, int b, int c, PPOINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowOrgEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWindowPlacement(HWND a, CONST WINDOWPLACEMENT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowPlacement(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWindowPos(HWND a, HWND b, int c, int d, int e, int f, UINT g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowPos(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline HHOOK O32_SetWindowsHookEx(int a, HOOKPROC_O32 b, HINSTANCE c, DWORD d)
{
 HHOOK yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowsHookEx(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWindowText(HWND a, LPCSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowText(a, b);
    SetFS(sel);

    return yyrc;
} 

inline WORD O32_SetWindowWord(HWND a, int b, WORD c)
{
 WORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWindowWord(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HENHMETAFILE O32_SetWinMetaFileBits(UINT a, CONST BYTE *b, HDC c, CONST METAFILEPICT *d)
{
 HENHMETAFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWinMetaFileBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SetWorldTransform(HDC a, LPXFORM b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetWorldTransform(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ShowCaret(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ShowCaret(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_ShowCursor(BOOL a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ShowCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ShowOwnedPopups(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ShowOwnedPopups(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ShowScrollBar(HWND a, int b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ShowScrollBar(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ShowWindow(HWND a, int b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ShowWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SizeofResource(HINSTANCE a, HRSRC b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SizeofResource(a, b);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_Sleep(DWORD a)
{
 USHORT sel = RestoreOS2FS();

    _O32_Sleep(a);
    SetFS(sel);
} 

inline int O32_StartDoc(HDC a, LPDOCINFOA b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StartDoc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_StartPage(HDC a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StartPage(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_StretchDIBits(HDC a, int b, int c, int d, int e, int f, int g, int h, int i, void *j, PBITMAPINFO k, UINT l, DWORD m)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StretchDIBits(a, b, c, d, e, f, g, h, i, j, k, l, m);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_StretchBlt(HDC a, int b, int c, int d, int e, HDC f, int g, int h, int i, int j, DWORD k)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StretchBlt(a, b, c, d, e, f, g, h, i, j, k);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_StrokeAndFillPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StrokeAndFillPath(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_StrokePath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_StrokePath(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SubtractRect(PRECT a, CONST RECT *b, CONST RECT *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SubtractRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_SuspendThread(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SuspendThread(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SwapMouseButton(BOOL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SwapMouseButton(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_SystemParametersInfo(UINT a, UINT b, PVOID c, UINT d)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SystemParametersInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SystemTimeToFileTime(CONST SYSTEMTIME *a, FILETIME *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SystemTimeToFileTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION a, LPSYSTEMTIME b, LPSYSTEMTIME c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SystemTimeToTzSpecificLocalTime(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32_TabbedTextOut(HDC a, int b, int c, LPCSTR d, int e, int f, int *g, int h)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TabbedTextOut(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TerminateProcess(HANDLE a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TerminateProcess(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TerminateThread(HANDLE a, DWORD b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TerminateThread(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TextOut(HDC a, int b, int c, LPCSTR d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TextOut(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_TlsAlloc()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TlsAlloc();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TlsFree(DWORD a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TlsFree(a);
    SetFS(sel);

    return yyrc;
} 

inline PVOID O32_TlsGetValue(DWORD a)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TlsGetValue(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TlsSetValue(DWORD a, PVOID b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TlsSetValue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TrackPopupMenu(HMENU a, UINT b, int c, int d, int e, HWND f, CONST RECT *g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TrackPopupMenu(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline int O32_TranslateAccelerator(HWND a, HACCEL b, LPMSG c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TranslateAccelerator(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TranslateMDISysAccel(HWND a, LPMSG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TranslateMDISysAccel(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_TranslateMessage(CONST MSG *a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_TranslateMessage(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnhookWindowsHookEx(HHOOK a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnhookWindowsHookEx(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnionRect(PRECT a, CONST RECT *b, CONST RECT *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnionRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnlockFile(HANDLE a, DWORD b, DWORD c, DWORD d, DWORD e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnlockFile(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnpackDDElParam(UINT a, LONG b, PUINT c, PUINT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnpackDDElParam(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnrealizeObject(HGDIOBJ a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnrealizeObject(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UnregisterClass(LPCSTR a, HINSTANCE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UnregisterClass(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_UpdateWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_UpdateWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ValidateRect(HWND a, CONST RECT *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ValidateRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_ValidateRgn(HWND a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ValidateRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SHORT O32_VkKeyScan(char a)
{
 SHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_VkKeyScan(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_WaitForMultipleObjects(DWORD a, CONST HANDLE *b, BOOL c, DWORD d)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WaitForMultipleObjects(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_WaitForSingleObject(HANDLE a, DWORD b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WaitForSingleObject(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WaitMessage()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WaitMessage();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WidenPath(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WidenPath(a);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32_WinExec(LPCSTR a, UINT b)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinExec(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WinHelp(HWND a, LPCSTR b, UINT c, DWORD d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinHelp(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_WindowFromPoint(POINT a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WindowFromPoint(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_WindowFromDC(HDC a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WindowFromDC(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WriteFile(HANDLE a, CONST VOID *b, DWORD c, PDWORD d, LPOVERLAPPED e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WriteFile(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WritePrivateProfileString(LPCSTR a, LPCSTR b, LPCSTR c, LPCSTR d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WritePrivateProfileString(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WriteProfileString(LPCSTR a, LPCSTR b, LPCSTR c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WriteProfileString(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline VOID O32_ZeroMemory(PVOID a, DWORD b)
{
 USHORT sel = RestoreOS2FS();

    _O32_ZeroMemory(a, b);
    SetFS(sel);
} 

inline HFILE O32__lclose(HFILE a)
{
 HFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__lclose(a);
    SetFS(sel);

    return yyrc;
} 

inline HFILE O32__lcreat(LPCSTR a, int b)
{
 HFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__lcreat(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HFILE O32__lopen(LPCSTR a, int b)
{
 HFILE yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__lopen(a, b);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32__lread(HFILE a, PVOID b, UINT c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__lread(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG O32__llseek(HFILE a, LONG b, int c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__llseek(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline UINT O32__lwrite(HFILE a, CONST VOID *b, UINT c)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32__lwrite(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_timeGetSystemTime(LPMMTIME a, UINT b)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_timeGetSystemTime(a, b);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_timeGetTime()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_timeGetTime();
    SetFS(sel);

    return yyrc;
} 

inline int O32_wvsprintf(LPSTR a, LPCSTR b, CONST VOID *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_wvsprintf(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_CharNext(LPCSTR a)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharNext(a);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_CharPrev(LPCSTR a, LPCSTR b)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharPrev(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND O32_GetDesktopWindow()
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetDesktopWindow();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CharToOem(LPCSTR a, LPSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharToOem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OemToChar(LPCSTR a, LPSTR b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OemToChar(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_CharToOemBuff(LPCSTR a, LPSTR b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CharToOemBuff(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_OemToCharBuff(LPCSTR a, LPSTR b, DWORD c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OemToCharBuff(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadHugeReadPtr(CONST void *a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadHugeReadPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsBadHugeWritePtr(PVOID a, UINT b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsBadHugeWritePtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_FloodFill(HDC a, int b, int c, COLORREF d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_FloodFill(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsCharAlpha(WCHAR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsCharAlpha(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsCharAlphaNumeric(WCHAR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsCharAlphaNumeric(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsCharLower(WCHAR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsCharLower(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_IsCharUpper(WCHAR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IsCharUpper(a);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_lstrcat(LPSTR a, LPCSTR b)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_lstrcat(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_lstrcmp(LPCSTR a, LPCSTR b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_lstrcmp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_lstrcmpi(LPCSTR a, LPCSTR b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_lstrcmpi(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LPSTR O32_lstrcpy(LPSTR a, LPCSTR b)
{
 LPSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_lstrcpy(a, b);
    SetFS(sel);

    return yyrc;
} 

inline int O32_lstrlen(LPCSTR a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_lstrlen(a);
    SetFS(sel);

    return yyrc;
} 

inline int O32_WinCallWinMain(int a, char **b, FNMAIN_O32 c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinCallWinMain(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WinTranslateDevicePoints(HDC a, HWND b, PPOINT c, INT d, XLATEDIR e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinTranslateDevicePoints(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WinTranslateDeviceRects(HDC a, HWND b, PRECT c, INT d, XLATEDIR e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinTranslateDeviceRects(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG O32_WinTranslateGraphicsObjectHandle(HGDIOBJ a, XLATEDIR b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinTranslateGraphicsObjectHandle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG O32_WinTranslateMnemonicString(LPCSTR a, LPSTR *b, ULONG c, XLATEDIR d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinTranslateMnemonicString(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_WinQueryTranslateMode()
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinQueryTranslateMode();
    SetFS(sel);

    return yyrc;
} 

inline BOOL O32_WinSetTranslateMode(DWORD a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WinSetTranslateMode(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD O32_WaitForInputIdle(HANDLE hProcess, DWORD dwTimeOut)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_WaitForInputIdle(hProcess, dwTimeOut);
    SetFS(sel);

    return yyrc;
}

#endif    /* ifdef _OS2WIN_H */
