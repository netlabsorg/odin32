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

#define OPEN32API       SYSTEM
#define EXPENTRY_O32    SYSTEM
#define APIENTRY_O32    SYSTEM

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

typedef int (* EXPENTRY_O32 ENHMFENUMPROC_O32)(HDC, LPHANDLETABLE, const LPENHMETARECORD, int, LPARAM);
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
#define O32_HeapSize(handle, flags, pMem) AweHeapSize(handle, flags, pMem)
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
#define O32_CreateDialogIndirect            O32__CreateDialogIndirect
#define O32_CreateDialogIndirectParam       O32__CreateDialogIndirectParam
#define O32_CreateIconIndirect              O32__CreateIconIndirect
#define O32_DialogBoxIndirect               O32__DialogBoxIndirect
#define O32_DialogBoxIndirectParam          O32__DialogBoxIndirectParam
#define O32_LoadMenuIndirect                O32__LoadMenuIndirect
#define O32_LoadResource                    O32__LoadResource

/* API Prototypes
 */
int     OPEN32API O32_WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int     OPEN32API O32_AbortDoc( HDC );

BOOL    OPEN32API O32_AbortPath( HDC );

ATOM    OPEN32API O32_AddAtom( LPCSTR );

int     OPEN32API O32_AddFontResource( LPCSTR );

BOOL    OPEN32API O32_AdjustWindowRect( PRECT, DWORD, BOOL );

BOOL    OPEN32API O32_AdjustWindowRectEx( PRECT, DWORD, BOOL, DWORD );

BOOL    OPEN32API O32_AngleArc( HDC, int, int, DWORD, float startAngle, float sweepAngle );

BOOL    OPEN32API O32_AnimatePalette( HPALETTE, UINT, UINT, const PALETTEENTRY *);

BOOL    OPEN32API O32_AppendMenu( HMENU, UINT, UINT, LPCSTR );

BOOL    OPEN32API O32_Arc( HDC, int, int, int, int, int, int, int, int );

BOOL    OPEN32API O32_ArcTo( HDC, int, int, int, int, int xRad1, int yRad1, int xRad2, int yRad2);

UINT    OPEN32API O32_ArrangeIconicWindows( HWND );

BOOL    OPEN32API O32_Beep( DWORD, DWORD );

HDWP    OPEN32API O32_BeginDeferWindowPos( int );

HDC     OPEN32API O32_BeginPaint( HWND, PPAINTSTRUCT );

BOOL    OPEN32API O32_BeginPath( HDC );

BOOL    OPEN32API O32_BitBlt( HDC , int, int, int, int, HDC , int, int, DWORD );

BOOL    OPEN32API O32_BringWindowToTop( HWND );

BOOL    OPEN32API O32_CallMsgFilter( LPMSG, int );

LRESULT OPEN32API O32_CallNextHookEx( HHOOK, int, WPARAM, LPARAM );

LRESULT OPEN32API O32_CallWindowProc( WNDPROC_O32, HWND, UINT, WPARAM, LPARAM );

BOOL    OPEN32API O32_ChangeClipboardChain( HWND, HWND );

LPSTR   OPEN32API O32_CharLower( LPSTR );

DWORD   OPEN32API O32_CharLowerBuff( LPSTR, DWORD );

LPSTR   OPEN32API O32_CharUpper( LPSTR );

DWORD   OPEN32API O32_CharUpperBuff( LPSTR, DWORD );

BOOL    OPEN32API O32_CheckDlgButton( HWND, int, UINT );

DWORD   OPEN32API O32_CheckMenuItem( HMENU, UINT, UINT );

BOOL    OPEN32API O32_CheckRadioButton( HWND, int, int, int );

HWND    OPEN32API O32_ChildWindowFromPoint( HWND, POINT );

BOOL    OPEN32API O32_ChooseColor( LPCHOOSECOLORA );

BOOL    OPEN32API O32_ChooseFont( LPCHOOSEFONTA );

BOOL    OPEN32API O32_Chord( HDC , int, int, int, int, int xRadial1, int yRadial1, int xRadial2, int yRadial2);

BOOL    OPEN32API O32_ClientToScreen( HWND, PPOINT );

BOOL    OPEN32API O32_ClipCursor( const RECT *);

BOOL    OPEN32API O32_CloseClipboard( VOID );

HENHMETAFILE OPEN32API O32_CloseEnhMetaFile( HDC );

BOOL    OPEN32API O32_CloseFigure( HDC );

BOOL    OPEN32API O32_CloseHandle( HANDLE );

HMETAFILE OPEN32API O32_CloseMetaFile( HDC );

BOOL    OPEN32API O32_CloseWindow( HWND );

int       OPEN32API O32_CombineRgn( HRGN, HRGN hrgnSrc1, HRGN hrgnSrc2, int );

DWORD   OPEN32API O32_CommDlgExtendedError( VOID );

LONG    OPEN32API O32_CompareFileTime( FILETIME *, FILETIME * );

HCURSOR OPEN32API O32_CopyCursor( HCURSOR );

HENHMETAFILE OPEN32API O32_CopyEnhMetaFile( HENHMETAFILE, LPCSTR );

BOOL    OPEN32API O32_CopyFile( LPCSTR, LPCSTR, BOOL );

HICON   OPEN32API O32_CopyIcon( HICON );

HMETAFILE OPEN32API O32_CopyMetaFile( HMETAFILE, LPCSTR );

BOOL    OPEN32API O32_CopyRect( PRECT, const RECT *);

int       OPEN32API O32_CountClipboardFormats( VOID );

HACCEL  OPEN32API O32_CreateAcceleratorTable( LPACCEL, int );

HBITMAP OPEN32API O32_CreateBitmap( int, int, UINT, UINT, const void * );

HBITMAP OPEN32API O32_CreateBitmapIndirect( const BITMAP * );

HBRUSH  OPEN32API O32_CreateBrushIndirect( LPLOGBRUSH );

BOOL    OPEN32API O32_CreateCaret( HWND, HBITMAP, int, int );

HBITMAP OPEN32API O32_CreateCompatibleBitmap( HDC , int, int );

HDC     OPEN32API O32_CreateCompatibleDC( HDC );

HCURSOR OPEN32API O32_CreateCursor( HINSTANCE, int, int, int, int, const VOID *, const VOID *);

HDC     OPEN32API O32_CreateDC( LPCSTR, LPCSTR, LPCSTR, const DEVMODEA *);

HBRUSH  OPEN32API O32_CreateDIBPatternBrushPt( const VOID *, UINT );

HBITMAP OPEN32API O32_CreateDIBitmap( HDC, const BITMAPINFOHEADER *, DWORD, const void *, const BITMAPINFO *, UINT );

HWND    OPEN32API O32_CreateDialog( HINSTANCE, LPCSTR, HWND, DLGPROC_O32 );

HWND    OPEN32API O32_CreateDialogParam( HINSTANCE, LPCSTR, HWND, DLGPROC_O32, LPARAM );

HWND    OPEN32API O32_CreateDialogIndirect( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC_O32 );

HWND    OPEN32API O32_CreateDialogIndirectParam( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC_O32, LPARAM );

BOOL    OPEN32API O32_CreateDirectory( LPCSTR, PSECURITY_ATTRIBUTES );

HRGN    OPEN32API O32_CreateEllipticRgn( int, int, int, int );

HRGN    OPEN32API O32_CreateEllipticRgnIndirect( const RECT * );

HENHMETAFILE OPEN32API O32_CreateEnhMetaFile( HDC, LPCSTR, const RECT *, LPCSTR );

HANDLE  OPEN32API O32_CreateEvent( PSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR );

HANDLE  OPEN32API O32_CreateFile( LPCSTR, DWORD, DWORD, PSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE );

HFONT   OPEN32API O32_CreateFont( int, int, int, int, int, DWORD, DWORD, DWORD, DWORD,
                                  DWORD, DWORD, DWORD, DWORD, LPCSTR );

HFONT   OPEN32API O32_CreateFontIndirect( const  LOGFONTA *);

HBRUSH  OPEN32API O32_CreateHatchBrush( int, COLORREF );

HDC     OPEN32API O32_CreateIC( LPCSTR, LPCSTR, LPCSTR, const DEVMODEA *);

HICON   OPEN32API O32_CreateIcon( HINSTANCE, int, int, BYTE, BYTE, const BYTE *, const BYTE * );

HICON   OPEN32API O32_CreateIconFromResource( PBYTE, DWORD, BOOL, DWORD );

HICON   OPEN32API O32_CreateIconIndirect( LPICONINFO );

HMENU   OPEN32API O32_CreateMenu( void);

HDC     OPEN32API O32_CreateMetaFile( LPCSTR );

HWND    OPEN32API O32_CreateMDIWindow( LPSTR, LPSTR, DWORD, int, int, int, int, HWND, HINSTANCE, LPARAM );

HANDLE  OPEN32API O32_CreateMutex( PSECURITY_ATTRIBUTES, BOOL, LPCSTR );

HPALETTE OPEN32API O32_CreatePalette( const LOGPALETTE * );

HBRUSH  OPEN32API O32_CreatePatternBrush( HBITMAP );

HPEN    OPEN32API O32_CreatePen( int, int, COLORREF );

HPEN    OPEN32API O32_CreatePenIndirect( const LOGPEN * );

HRGN    OPEN32API O32_CreatePolyPolygonRgn( const POINT *, const INT *, int, int );

HRGN    OPEN32API O32_CreatePolygonRgn( const POINT *, int, int );

HMENU   OPEN32API O32_CreatePopupMenu( VOID );

BOOL    OPEN32API O32_CreateProcess( LPCSTR, LPCSTR, PSECURITY_ATTRIBUTES, PSECURITY_ATTRIBUTES,
                         BOOL, DWORD, PVOID, LPCSTR, LPSTARTUPINFOA,
                         LPPROCESS_INFORMATION );

HRGN    OPEN32API O32_CreateRectRgn( int, int, int, int );

HRGN    OPEN32API O32_CreateRectRgnIndirect( const RECT * );

HRGN    OPEN32API O32_CreateRoundRectRgn( int, int, int, int, int, int );

HANDLE  OPEN32API O32_CreateSemaphore( PSECURITY_ATTRIBUTES, LONG, LONG, LPSTR );

HBRUSH  OPEN32API O32_CreateSolidBrush( COLORREF );

HANDLE  OPEN32API O32_CreateThread( PSECURITY_ATTRIBUTES, DWORD, PTHREAD_START_ROUTINE_O32, PVOID, DWORD, PDWORD );

HWND    OPEN32API O32_CreateWindowEx( DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID );

BOOL    OPEN32API O32_DPtoLP( HDC, PPOINT, int );

BOOL    OPEN32API O32_DdeAbandonTransaction( DWORD, HCONV, DWORD );

PBYTE   OPEN32API O32_DdeAccessData(HDDEDATA, PDWORD );

HDDEDATA OPEN32API O32_DdeAddData( HDDEDATA, PVOID, DWORD, DWORD );

HDDEDATA OPEN32API O32_DdeClientTransaction( PVOID, DWORD, HCONV,
                          HSZ, UINT, UINT, DWORD, PDWORD );

int       OPEN32API O32_DdeCmpStringHandles( HSZ hsz1, HSZ hsz2);

HCONV   OPEN32API O32_DdeConnect( DWORD, HSZ, HSZ, LPCONVCONTEXT );

HCONVLIST OPEN32API O32_DdeConnectList(DWORD, HSZ, HSZ, HCONVLIST, LPCONVCONTEXT );

HDDEDATA OPEN32API O32_DdeCreateDataHandle(DWORD, PVOID, DWORD, DWORD, HSZ, UINT, UINT );

HSZ     OPEN32API O32_DdeCreateStringHandle(DWORD, LPCSTR, int );

BOOL    OPEN32API O32_DdeDisconnect(HCONV );

BOOL    OPEN32API O32_DdeDisconnectList( HCONVLIST );

BOOL    OPEN32API O32_DdeEnableCallback(DWORD, HCONV, UINT );

BOOL    OPEN32API O32_DdeFreeDataHandle( HDDEDATA );

BOOL    OPEN32API O32_DdeFreeStringHandle(DWORD, HSZ );

DWORD   OPEN32API O32_DdeGetData( HDDEDATA, PVOID, DWORD, DWORD );

UINT    OPEN32API O32_DdeGetLastError(DWORD );

UINT    OPEN32API O32_DdeInitialize(PDWORD, PFNCALLBACK_O32, DWORD, DWORD );

BOOL    OPEN32API O32_DdeKeepStringHandle(DWORD, HSZ );

HDDEDATA OPEN32API O32_DdeNameService( DWORD, HSZ hsz1, HSZ hsz2, UINT );

BOOL    OPEN32API O32_DdePostAdvise(DWORD, HSZ, HSZ );

UINT    OPEN32API O32_DdeQueryConvInfo( HCONV, DWORD, LPCONVINFO );

HCONV   OPEN32API O32_DdeQueryNextServer( HCONVLIST, HCONV );

DWORD   OPEN32API O32_DdeQueryString(DWORD, HSZ, LPSTR, DWORD, int );

HCONV   OPEN32API O32_DdeReconnect( HCONV );

BOOL    OPEN32API O32_DdeSetUserHandle( HCONV, DWORD, DWORD );

BOOL    OPEN32API O32_DdeUnaccessData( HDDEDATA );

BOOL    OPEN32API O32_DdeUninitialize(DWORD );

LRESULT OPEN32API O32_DefDlgProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HDWP    OPEN32API O32_DeferWindowPos( HDWP, HWND, HWND hWndInsertAfter , int, int, int, int, UINT );

LRESULT OPEN32API O32_DefFrameProc( HWND, HWND, UINT, WPARAM, LPARAM );

LRESULT OPEN32API O32_DefMDIChildProc( HWND, UINT, WPARAM, LPARAM );

LRESULT OPEN32API O32_DefWindowProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

ATOM    OPEN32API O32_DeleteAtom( ATOM );

VOID    OPEN32API O32_DeleteCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API O32_DeleteDC( HDC );

BOOL    OPEN32API O32_DeleteEnhMetaFile( HENHMETAFILE );

BOOL    OPEN32API O32_DeleteFile( LPCSTR );

BOOL    OPEN32API O32_DeleteMenu( HMENU, UINT, UINT );

BOOL    OPEN32API O32_DeleteMetaFile( HMETAFILE );

BOOL    OPEN32API O32_DeleteObject( HANDLE );

BOOL    OPEN32API O32_DestroyAcceleratorTable( HACCEL );

BOOL    OPEN32API O32_DestroyCaret( VOID );

BOOL    OPEN32API O32_DestroyCursor( HCURSOR );

BOOL    OPEN32API O32_DestroyIcon( HICON );

BOOL    OPEN32API O32_DestroyMenu( HMENU );

BOOL    OPEN32API O32_DestroyWindow( HWND );

DWORD   OPEN32API O32_DeviceCapabilities( LPCSTR, LPCSTR, WORD, LPTSTR, const DEVMODEA *);

int       OPEN32API O32_DialogBox( HINSTANCE, LPCSTR, HWND, DLGPROC_O32 );

int       OPEN32API O32_DialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC_O32, LPARAM );

BOOL    OPEN32API O32_DialogBoxIndirect( HINSTANCE, LPDLGTEMPLATEA, HWND, DLGPROC_O32 );

BOOL    OPEN32API O32_DialogBoxIndirectParam( HINSTANCE, LPDLGTEMPLATEA, HWND, DLGPROC_O32, LPARAM );

LONG    OPEN32API O32_DispatchMessage( const MSG * );

int       OPEN32API O32_DlgDirList( HWND, LPSTR, int, int, UINT );

int       OPEN32API O32_DlgDirListComboBox( HWND, LPSTR, int, int, UINT );

BOOL    OPEN32API O32_DlgDirSelectEx( HWND, LPSTR, int, int );

BOOL    OPEN32API O32_DlgDirSelectComboBoxEx( HWND, LPSTR, int, int );

BOOL    OPEN32API O32_DllEntryPoint( HINSTANCE, DWORD, LPVOID );

BOOL    OPEN32API O32_DosDateTimeToFileTime( WORD, WORD, LPFILETIME );

VOID    OPEN32API O32_DragAcceptFiles( HWND, BOOL );

VOID    OPEN32API O32_DragFinish( HDROP );

UINT    OPEN32API O32_DragQueryFile( HDROP, int, LPSTR, int );

BOOL    OPEN32API O32_DragQueryPoint( HDROP, PPOINT );

BOOL    OPEN32API O32_DrawFocusRect( HDC, const RECT * );

BOOL    OPEN32API O32_DrawIcon( HDC, int, int, HICON );

BOOL    OPEN32API O32_DrawMenuBar( HWND );

int       OPEN32API O32_DrawText( HDC, LPCSTR, int, PRECT, UINT );

BOOL    OPEN32API O32_DuplicateHandle( HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD, BOOL, DWORD );

BOOL    OPEN32API O32_Ellipse( HDC, int, int, int, int );

BOOL    OPEN32API O32_EmptyClipboard( void);

BOOL    OPEN32API O32_EnableMenuItem( HMENU, UINT, UINT );

BOOL    OPEN32API O32_EnableScrollBar( HWND, UINT, UINT );

BOOL    OPEN32API O32_EnableWindow( HWND, BOOL );

BOOL    OPEN32API O32_EndDeferWindowPos( HDWP );

BOOL    OPEN32API O32_EndDialog( HWND, int );

int       OPEN32API O32_EndDoc( HDC );

int       OPEN32API O32_EndPage( HDC );

BOOL    OPEN32API O32_EndPath( HDC );

BOOL    OPEN32API O32_EndPaint( HWND, const PAINTSTRUCT *);

VOID    OPEN32API O32_EnterCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API O32_EnumChildWindows( HWND, WNDENUMPROC_O32, LPARAM );

UINT    OPEN32API O32_EnumClipboardFormats( UINT );

BOOL    OPEN32API O32_EnumEnhMetaFile( HDC , HENHMETAFILE, ENHMFENUMPROC_O32, PVOID, const RECT * );

int       OPEN32API O32_EnumFonts( HDC , LPCSTR, OLDFONTENUMPROC_O32, LPARAM );

int       OPEN32API O32_EnumFontFamilies( HDC , LPCSTR, FONTENUMPROC_O32, LPARAM );

BOOL    OPEN32API O32_EnumMetaFile( HDC , HMETAFILE, MFENUMPROC_O32, LPARAM );

int       OPEN32API O32_EnumObjects( HDC , int, GOBJENUMPROC_O32, LPARAM );

BOOL    OPEN32API O32_EnumPrinters( DWORD, LPTSTR, DWORD, LPBYTE, DWORD, LPDWORD, LPDWORD);

int       OPEN32API O32_EnumProps( HWND, PROPENUMPROC_O32 );

int       OPEN32API O32_EnumPropsEx( HWND, PROPENUMPROCEX_O32, LPARAM );

BOOL    OPEN32API O32_EnumThreadWindows( DWORD, WNDENUMPROC_O32, LPARAM );

BOOL    OPEN32API O32_EnumWindows( WNDENUMPROC_O32, LPARAM );

BOOL    OPEN32API O32_EqualRect( const RECT *, const RECT * );

BOOL    OPEN32API O32_EqualRgn( HRGN, HRGN );

int       OPEN32API O32_Escape( HDC, int, int, LPCSTR, PVOID );

int       OPEN32API O32_ExcludeClipRect( HDC, int, int, int, int );

BOOL    OPEN32API O32_ExcludeUpdateRgn( HDC, HWND );

VOID    OPEN32API O32_ExitProcess( UINT );

VOID    OPEN32API O32_ExitThread( DWORD );

BOOL    OPEN32API O32_ExitWindows( DWORD, UINT );

BOOL    OPEN32API O32_ExitWindowsEx( UINT, DWORD );

HPEN    OPEN32API O32_ExtCreatePen( DWORD, DWORD, const LOGBRUSH *, DWORD, const DWORD * );

HRGN    OPEN32API O32_ExtCreateRegion( const XFORM *, DWORD, const RGNDATA * );

BOOL    OPEN32API O32_ExtFloodFill( HDC, int, int, COLORREF, UINT );

int       OPEN32API O32_ExtSelectClipRgn( HDC, HRGN, int );

BOOL    OPEN32API O32_ExtTextOut( HDC, int, int, UINT, const RECT *, LPCSTR, UINT, const int * );

VOID    OPEN32API O32_FatalAppExit( UINT, LPCSTR );

VOID    OPEN32API O32_FatalExit( UINT );

BOOL    OPEN32API O32_FileTimeToDosDateTime( const FILETIME *, LPWORD, LPWORD );

BOOL    OPEN32API O32_FileTimeToLocalFileTime( const FILETIME *, FILETIME * );

BOOL    OPEN32API O32_FileTimeToSystemTime( const FILETIME *, LPSYSTEMTIME );

BOOL    OPEN32API O32_FillPath( HDC );

int     OPEN32API O32_FillRect( HDC, const RECT *, HBRUSH );

BOOL    OPEN32API O32_FillRgn( HDC, HRGN, HBRUSH );

ATOM    OPEN32API O32_FindAtom( LPCSTR );

BOOL    OPEN32API O32_FindClose( HANDLE );

HANDLE  OPEN32API O32_FindFirstFile( LPCSTR, LPWIN32_FIND_DATAA );

BOOL    OPEN32API O32_FindNextFile( HANDLE, LPWIN32_FIND_DATAA );

HRSRC   OPEN32API O32_FindResource( HINSTANCE, LPCSTR, LPCSTR );

HWND    OPEN32API O32_FindText( LPFINDREPLACEA );

HWND    OPEN32API O32_FindWindow( LPCSTR lpClassName , LPCSTR );

BOOL    OPEN32API O32_FlashWindow( HWND, BOOL );

BOOL    OPEN32API O32_FlattenPath( HDC );

BOOL    OPEN32API O32_FlushFileBuffers( HANDLE );

int       OPEN32API O32_FrameRect( HDC, const RECT *, HBRUSH );

BOOL    OPEN32API O32_FrameRgn( HDC, HRGN, HBRUSH, int, int );

BOOL    OPEN32API O32_FreeDDElParam( UINT, LONG );

BOOL    OPEN32API O32_FreeLibrary( HINSTANCE );

BOOL    OPEN32API O32_GdiComment(HDC hdc, UINT cbSize, CONST BYTE * lpData);

UINT    OPEN32API O32_GetACP( VOID );

HWND    OPEN32API O32_GetActiveWindow( void);

int       OPEN32API O32_GetArcDirection( HDC );

BOOL    OPEN32API O32_GetAspectRatioFilterEx( HDC, PSIZE );

UINT    OPEN32API O32_GetAtomName( ATOM, LPSTR, int );

LONG    OPEN32API O32_GetBitmapBits( HBITMAP, LONG, PVOID );

BOOL    OPEN32API O32_GetBitmapDimensionEx( HBITMAP, PSIZE );

COLORREF OPEN32API O32_GetBkColor( HDC );

int       OPEN32API O32_GetBkMode( HDC );

UINT    OPEN32API O32_GetBoundsRect( HDC, PRECT, UINT );

BOOL    OPEN32API O32_GetBrushOrgEx( HDC, PPOINT );

HWND    OPEN32API O32_GetCapture( void);

UINT    OPEN32API O32_GetCaretBlinkTime( VOID );

BOOL    OPEN32API O32_GetCaretPos( PPOINT );

BOOL    OPEN32API O32_GetCharABCWidths( HDC, UINT, UINT, LPABC );

BOOL    OPEN32API O32_GetCharWidth( HDC, UINT, UINT, PINT );

BOOL    OPEN32API O32_GetClassInfo( HINSTANCE hInstance , LPCSTR, WNDCLASSA * );

DWORD   OPEN32API O32_GetClassLong( HWND, int );

int       OPEN32API O32_GetClassName( HWND, LPSTR, int );

WORD    OPEN32API O32_GetClassWord( HWND, int );

BOOL    OPEN32API O32_GetClientRect( HWND, PRECT );

HANDLE  OPEN32API O32_GetClipboardData( UINT );

int       OPEN32API O32_GetClipboardFormatName( UINT, LPSTR, int );

HWND    OPEN32API O32_GetClipboardOwner( void);

HWND    OPEN32API O32_GetClipboardViewer( VOID );

int       OPEN32API O32_GetClipBox( HDC, PRECT );

BOOL    OPEN32API O32_GetClipCursor( PRECT );

int       OPEN32API O32_GetClipRgn( HDC, HRGN );

LPSTR   OPEN32API O32_GetCommandLine( VOID );

int       OPEN32API O32_GetCurrentDirectory( DWORD, LPSTR );

HGDIOBJ OPEN32API O32_GetCurrentObject( HDC, DWORD );

BOOL    OPEN32API O32_GetCurrentPositionEx( HDC, PPOINT );

HANDLE  OPEN32API O32_GetCurrentProcess( VOID );

DWORD   OPEN32API O32_GetCurrentProcessId( VOID );

HANDLE  OPEN32API O32_GetCurrentThread( VOID );

DWORD   OPEN32API O32_GetCurrentThreadId( VOID );

DWORD   OPEN32API O32_GetCurrentTime( VOID );

HCURSOR OPEN32API O32_GetCursor( VOID );

BOOL    OPEN32API O32_GetCursorPos( PPOINT );

HDC     OPEN32API O32_GetDC( HWND );

HDC     OPEN32API O32_GetDCEx( HWND, HRGN, DWORD );

BOOL    OPEN32API O32_GetDCOrgEx( HDC, PPOINT );

int       OPEN32API O32_GetDIBits( HDC, HBITMAP, UINT, UINT, void *, PBITMAPINFO, UINT );

int       OPEN32API O32_GetDeviceCaps( HDC, int );

LONG    OPEN32API O32_GetDialogBaseUnits( void);

BOOL    OPEN32API O32_GetDiskFreeSpace( LPCSTR, PDWORD, PDWORD, PDWORD, PDWORD );

int       OPEN32API O32_GetDlgCtrlID( HWND );

HWND    OPEN32API O32_GetDlgItem( HWND, int );

UINT    OPEN32API O32_GetDlgItemInt( HWND, int, PBOOL, BOOL );

UINT    OPEN32API O32_GetDlgItemText( HWND, int, LPSTR, int );

UINT    OPEN32API O32_GetDoubleClickTime( VOID );

UINT    OPEN32API O32_GetDriveType( LPCSTR );

HENHMETAFILE OPEN32API O32_GetEnhMetaFile( LPCSTR );

UINT    OPEN32API O32_GetEnhMetaFileBits( HENHMETAFILE, UINT, PBYTE );

UINT    OPEN32API O32_GetEnhMetaFileHeader( HENHMETAFILE, UINT, LPENHMETAHEADER );

UINT    OPEN32API O32_GetEnhMetaFilePaletteEntries( HENHMETAFILE, UINT, LPPALETTEENTRY );

PVOID   OPEN32API O32_GetEnvironmentStrings( VOID );

DWORD   OPEN32API O32_GetEnvironmentVariable(LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API O32_GetExitCodeProcess( HANDLE, LPDWORD );

BOOL    OPEN32API O32_GetExitCodeThread( HANDLE, LPDWORD );

DWORD   OPEN32API O32_GetFileAttributes( LPSTR );

DWORD   OPEN32API O32_GetFileInformationByHandle( HANDLE, BY_HANDLE_FILE_INFORMATION * );

DWORD   OPEN32API O32_GetFileSize( HANDLE, PDWORD );

BOOL    OPEN32API O32_GetFileTime( HANDLE, FILETIME *, FILETIME *, FILETIME * );

short     OPEN32API O32_GetFileTitle( LPCSTR, LPSTR, WORD );

DWORD   OPEN32API O32_GetFileType( HANDLE );

HWND    OPEN32API O32_GetFocus( VOID );

HWND    OPEN32API O32_GetForegroundWindow( VOID );

DWORD   OPEN32API O32_GetFullPathName( LPCSTR, DWORD, LPSTR, LPSTR * );

int       OPEN32API O32_GetGraphicsMode(HDC);

BOOL    OPEN32API O32_GetIconInfo( HICON, LPICONINFO );

DWORD   OPEN32API O32_GetKerningPairs( HDC, DWORD, LPKERNINGPAIR );

int       OPEN32API O32_GetKeyboardType( int );

int       OPEN32API O32_GetKeyNameText( LPARAM, LPSTR, int );

USHORT  OPEN32API O32_GetAsyncKeyState(INT nVirtKey);
SHORT   OPEN32API O32_GetKeyState( int );

HWND    OPEN32API O32_GetLastActivePopup( HWND );

DWORD   OPEN32API O32_GetLastError( VOID );

VOID    OPEN32API O32_GetLocalTime( LPSYSTEMTIME );

DWORD   OPEN32API O32_GetLogicalDriveStrings( DWORD, LPSTR );

DWORD   OPEN32API O32_GetLogicalDrives( VOID );

int       OPEN32API O32_GetMapMode( HDC);

HMENU   OPEN32API O32_GetMenu( HWND );

LONG    OPEN32API O32_GetMenuCheckMarkDimensions(void);

int       OPEN32API O32_GetMenuItemCount( HMENU );

UINT    OPEN32API O32_GetMenuItemID( HMENU, int );

UINT    OPEN32API O32_GetMenuState( HMENU, UINT, UINT );

int       OPEN32API O32_GetMenuString( HMENU, UINT, LPSTR, int, UINT );

BOOL    OPEN32API O32_GetMessage( LPMSG, HWND hwnd , UINT, UINT );

LONG    OPEN32API O32_GetMessageExtraInfo( VOID );

DWORD   OPEN32API O32_GetMessagePos( VOID );

LONG    OPEN32API O32_GetMessageTime( VOID );

HMETAFILE OPEN32API O32_GetMetaFile( LPCSTR );

UINT    OPEN32API O32_GetMetaFileBitsEx( HMETAFILE, UINT, LPVOID );

BOOL    OPEN32API O32_GetMiterLimit( HDC, float *);

DWORD   OPEN32API O32_GetModuleFileName( HINSTANCE, LPSTR, DWORD );

HMODULE OPEN32API O32_GetModuleHandle( LPCSTR );

COLORREF OPEN32API O32_GetNearestColor( HDC, COLORREF );

UINT    OPEN32API O32_GetNearestPaletteIndex( HPALETTE, COLORREF );

HWND    OPEN32API O32_GetNextDlgGroupItem( HWND, HWND, BOOL );

HWND    OPEN32API O32_GetNextDlgTabItem( HWND, HWND, BOOL );

HWND    OPEN32API O32_GetNextWindow( HWND, UINT );

int       OPEN32API O32_GetOEMCP( VOID );

int       OPEN32API O32_GetObject( HGDIOBJ, int, void * );

DWORD   OPEN32API O32_GetObjectType( HGDIOBJ );

HWND    OPEN32API O32_GetOpenClipboardWindow( VOID );

BOOL    OPEN32API O32_GetOpenFileName( LPOPENFILENAMEA );

UINT    OPEN32API O32_GetOutlineTextMetrics( HDC, UINT, LPOUTLINETEXTMETRICA );

BOOL    OPEN32API O32_GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL );

UINT    OPEN32API O32_GetPaletteEntries( HPALETTE, UINT, UINT, PPALETTEENTRY );

HWND    OPEN32API O32_GetParent( HWND );

UINT    OPEN32API O32_GetPath( HDC, PPOINT, PBYTE, int );

COLORREF OPEN32API O32_GetPixel( HDC, int, int );

int       OPEN32API O32_GetPolyFillMode( HDC);

DWORD   OPEN32API O32_GetPriorityClass( HANDLE );

int       OPEN32API O32_GetPriorityClipboardFormat( PUINT, int );

UINT    OPEN32API O32_GetPrivateProfileInt( LPCSTR, LPCSTR, INT, LPCSTR );

DWORD   OPEN32API O32_GetPrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR );

FARPROC OPEN32API O32_GetProcAddress( HMODULE, LPCSTR );

UINT    OPEN32API O32_GetProfileInt( LPCSTR, LPCSTR, INT );

DWORD   OPEN32API O32_GetProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD );

HANDLE  OPEN32API O32_GetProp( HWND, LPCSTR );

DWORD   OPEN32API O32_GetQueueStatus( UINT );

int       OPEN32API O32_GetROP2( HDC );

BOOL    OPEN32API O32_GetRasterizerCaps( LPRASTERIZER_STATUS, UINT );

int       OPEN32API O32_GetRgnBox( HRGN, PRECT );

DWORD   OPEN32API O32_GetRegionData( HRGN, DWORD, PRGNDATA );

BOOL    OPEN32API O32_GetSaveFileName( LPOPENFILENAMEA );

int       OPEN32API O32_GetScrollPos( HWND, int );

BOOL    OPEN32API O32_GetScrollRange( HWND, int, int *, int * );

HANDLE  OPEN32API O32_GetStdHandle( DWORD );

HGDIOBJ OPEN32API O32_GetStockObject( int );

int       OPEN32API O32_GetStretchBltMode( HDC );

HMENU   OPEN32API O32_GetSubMenu( HWND, int );

DWORD   OPEN32API O32_GetSysColor( int );

UINT    OPEN32API O32_GetSystemDirectory( LPSTR, UINT );

HMENU   OPEN32API O32_GetSystemMenu( HWND, BOOL );

int       OPEN32API O32_GetSystemMetrics( int );

UINT    OPEN32API O32_GetSystemPaletteEntries( HDC, UINT, UINT, PPALETTEENTRY );

void      OPEN32API O32_GetSystemTime( LPSYSTEMTIME );

DWORD   OPEN32API O32_GetTabbedTextExtent( HDC, LPCSTR, int, int, int * );

UINT    OPEN32API O32_GetTempFileName( LPCSTR, LPCSTR, UINT, LPSTR );

DWORD   OPEN32API O32_GetTempPath( DWORD, LPSTR );

UINT    OPEN32API O32_GetTextAlign( HDC);

int       OPEN32API O32_GetTextCharacterExtra( HDC);

COLORREF OPEN32API O32_GetTextColor( HDC );

BOOL    OPEN32API O32_GetTextExtentPoint( HDC, LPCSTR, int, PSIZE );

BOOL    OPEN32API O32_GetTextExtentPoint32( HDC, LPCSTR, int, PSIZE );

int       OPEN32API O32_GetTextFace( HDC, int, LPSTR );

BOOL    OPEN32API O32_GetTextMetrics( HDC, LPTEXTMETRICA );

DWORD   OPEN32API O32_GetThreadPriority( HANDLE );

DWORD   OPEN32API O32_GetTickCount( VOID );

DWORD   OPEN32API O32_GetTimeZoneInformation( LPTIME_ZONE_INFORMATION );

HWND    OPEN32API O32_GetTopWindow( HWND );

BOOL    OPEN32API O32_GetUpdateRect( HWND, PRECT, BOOL );

int       OPEN32API O32_GetUpdateRgn( HWND, HRGN, BOOL );

BOOL    OPEN32API O32_GetViewportExtEx( HDC, PSIZE );

BOOL    OPEN32API O32_GetViewportOrgEx( HDC, PPOINT );

DWORD   OPEN32API O32_GetVolumeInformation( LPCSTR, LPSTR, DWORD, PDWORD,
                       PDWORD, PDWORD, LPSTR, DWORD );

UINT    OPEN32API O32_GetWinMetaFileBits( HENHMETAFILE, UINT, PBYTE, int, HDC);

HWND    OPEN32API O32_GetWindow( HWND, UINT );

HDC     OPEN32API O32_GetWindowDC( HWND );

BOOL    OPEN32API O32_GetWindowExtEx( HDC, PSIZE );

LONG    OPEN32API O32_GetWindowLong( HWND, int );

BOOL    OPEN32API O32_GetWindowOrgEx( HDC, PPOINT );

BOOL    OPEN32API O32_GetWindowPlacement( HWND, LPWINDOWPLACEMENT );

BOOL    OPEN32API O32_GetWindowRect( HWND, PRECT );

UINT    OPEN32API O32_GetWindowsDirectory( LPSTR, UINT );

int       OPEN32API O32_GetWindowText( HWND, LPSTR, int );

int       OPEN32API O32_GetWindowTextLength( HWND );

DWORD   OPEN32API O32_GetWindowThreadProcessId(HWND, PDWORD );

WORD    OPEN32API O32_GetWindowWord( HWND, int );

BOOL    OPEN32API O32_GetWorldTransform( HDC, LPXFORM );

ATOM    OPEN32API O32_GlobalAddAtom( LPCSTR );

HGLOBAL OPEN32API O32_GlobalAlloc( UINT, DWORD );

ATOM    OPEN32API O32_GlobalDeleteAtom( ATOM );

HGLOBAL OPEN32API O32_GlobalDiscard( HGLOBAL );

ATOM    OPEN32API O32_GlobalFindAtom( LPCSTR );

UINT    OPEN32API O32_GlobalFlags( HGLOBAL );

HGLOBAL OPEN32API O32_GlobalFree( HGLOBAL );

UINT    OPEN32API O32_GlobalGetAtomName( ATOM, LPSTR, int );

HGLOBAL OPEN32API O32_GlobalHandle( PVOID memPtr );

PVOID   OPEN32API O32_GlobalLock( HGLOBAL hglbMem );

VOID    OPEN32API O32_GlobalMemoryStatus( LPMEMORYSTATUS );

HGLOBAL OPEN32API O32_GlobalReAlloc( HGLOBAL, DWORD, UINT );

DWORD   OPEN32API O32_GlobalSize( HGLOBAL );

BOOL    OPEN32API O32_GlobalUnlock( HGLOBAL hglbMem );

PVOID   OPEN32API O32_HeapAlloc( HANDLE, DWORD, DWORD );

HANDLE  OPEN32API O32_HeapCreate( DWORD, DWORD, DWORD );

BOOL    OPEN32API O32_HeapDestroy( HANDLE );

BOOL    OPEN32API O32_HeapFree( HANDLE, DWORD, PVOID );

PVOID   OPEN32API O32_HeapReAlloc( HANDLE, DWORD, PVOID, DWORD );

DWORD   OPEN32API O32_HeapSize( HANDLE, DWORD, PVOID );

BOOL    OPEN32API O32_HideCaret( HWND );

BOOL    OPEN32API O32_HiliteMenuItem( HWND, HMENU, UINT, UINT );

BOOL    OPEN32API O32_InflateRect( PRECT, int, int );

BOOL    OPEN32API O32_InSendMessage( VOID );

BOOL    OPEN32API O32_InitAtomTable( DWORD );

VOID    OPEN32API O32_InitializeCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API O32_InsertMenu( HMENU, UINT, UINT, UINT, LPCSTR );

LONG    OPEN32API O32_InterlockedDecrement( PLONG );

LONG    OPEN32API O32_InterlockedExchange( PLONG, LONG );

LONG    OPEN32API O32_InterlockedIncrement( PLONG );

int       OPEN32API O32_IntersectClipRect( HDC, int, int, int, int );

BOOL    OPEN32API O32_IntersectRect( PRECT, const RECT * rect1, const RECT * rect2);

BOOL    OPEN32API O32_InvalidateRect( HWND, const RECT *, BOOL );

BOOL    OPEN32API O32_InvalidateRgn( HWND, HRGN, BOOL );

BOOL    OPEN32API O32_InvertRect( HDC, const RECT *);

BOOL    OPEN32API O32_InvertRgn( HDC, HRGN );

BOOL    OPEN32API O32_IsBadCodePtr( FARPROC );

BOOL    OPEN32API O32_IsBadReadPtr( const VOID *, UINT );

BOOL    OPEN32API O32_IsBadStringPtr( LPCSTR, UINT );

BOOL    OPEN32API O32_IsBadWritePtr( PVOID, UINT );

BOOL    OPEN32API O32_IsChild( HWND, HWND );

BOOL    OPEN32API O32_IsClipboardFormatAvailable( UINT );

BOOL    OPEN32API O32_IsDBCSLeadByte( BYTE );

BOOL    OPEN32API O32_IsDialogMessage( HWND, LPMSG );

BOOL    OPEN32API O32_IsDlgButtonChecked( HWND, int );

BOOL    OPEN32API O32_IsIconic( HWND );

BOOL    OPEN32API O32_IsMenu( HMENU );

BOOL    OPEN32API O32_IsRectEmpty( const RECT *);

BOOL    OPEN32API O32_IsWindow( HWND );

BOOL    OPEN32API O32_IsWindowEnabled( HWND );

BOOL    OPEN32API O32_IsWindowVisible( HWND );

BOOL    OPEN32API O32_IsZoomed( HWND );

BOOL    OPEN32API O32_KillTimer(HWND, UINT );

BOOL    OPEN32API O32_LPtoDP( HDC, PPOINT, int );

VOID    OPEN32API O32_LeaveCriticalSection( CRITICAL_SECTION * );

BOOL    OPEN32API O32_LineDDA( int, int, int, int, LINEDDAPROC_O32, LPARAM );

 BOOL   OPEN32API O32_LineTo( HDC, int, int );

HACCEL  OPEN32API O32_LoadAccelerators( HINSTANCE, LPCSTR );

HBITMAP OPEN32API O32_LoadBitmap( HINSTANCE, LPCSTR );

HCURSOR OPEN32API O32_LoadCursor( HINSTANCE, LPCSTR );

HICON   OPEN32API O32_LoadIcon( HINSTANCE, LPCSTR );

HINSTANCE OPEN32API O32_LoadLibrary( LPCSTR );

HMENU   OPEN32API O32_LoadMenu( HINSTANCE, LPCSTR );

HMENU   OPEN32API O32_LoadMenuIndirect( const MENUITEMTEMPLATEHEADER * );

DWORD   OPEN32API O32_LoadModule( LPCSTR, PVOID );

HGLOBAL OPEN32API O32_LoadResource( HINSTANCE, HRSRC );

PVOID   OPEN32API O32_LockResource( HGLOBAL );

int       OPEN32API O32_LoadString( HINSTANCE, UINT, LPSTR, int );

HLOCAL  OPEN32API O32_LocalAlloc( UINT, UINT );

HLOCAL  OPEN32API O32_LocalDiscard( HLOCAL );

UINT    OPEN32API O32_LocalFlags( HLOCAL );

BOOL    OPEN32API O32_LocalFileTimeToFileTime( const FILETIME *, FILETIME * );

HLOCAL  OPEN32API O32_LocalFree( HLOCAL );

HLOCAL  OPEN32API O32_LocalHandle( PVOID lpvMem );

BOOL    OPEN32API O32_LocalUnlock( HLOCAL hlocal);

HLOCAL  OPEN32API O32_LocalReAlloc( HLOCAL, UINT, UINT );

UINT    OPEN32API O32_LocalSize( HLOCAL );

PVOID   OPEN32API O32_LocalLock( HLOCAL hlocal );

BOOL    OPEN32API O32_LockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    OPEN32API O32_LockWindowUpdate( HWND );

BOOL    OPEN32API O32_MapDialogRect( HWND, PRECT );

UINT    OPEN32API O32_MapVirtualKey( UINT, UINT );

int       OPEN32API O32_MapWindowPoints( HWND, HWND, PPOINT, int );

BOOL    OPEN32API O32_MaskBlt( HDC, int, int, int, int, HDC  hdcSrc, int, int, HBITMAP, int, int, DWORD );

int       OPEN32API O32_MessageBox( HWND hwnd , LPCSTR, LPCSTR, UINT );

BOOL    OPEN32API O32_MessageBeep( UINT );

DWORD   OPEN32API O32_MsgWaitForMultipleObjects( DWORD, LPHANDLE, BOOL, DWORD, DWORD );

BOOL    OPEN32API O32_ModifyMenu( HMENU, UINT, UINT, UINT, LPCSTR );

BOOL    OPEN32API O32_ModifyWorldTransform( HDC, LPXFORM, DWORD );

BOOL    OPEN32API O32_MoveFile( LPCSTR, LPCSTR );

BOOL    OPEN32API O32_MoveToEx( HDC, int, int, PPOINT );

BOOL    OPEN32API O32_MoveWindow( HWND, int, int, int, int, BOOL );

int       OPEN32API O32_MulDiv(int, int, int );

int       OPEN32API O32_OffsetClipRgn( HDC, int, int );

BOOL    OPEN32API O32_OffsetRect( PRECT, int, int );

int       OPEN32API O32_OffsetRgn( HRGN, int, int );

BOOL    OPEN32API O32_OffsetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API O32_OffsetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API O32_OpenClipboard( HWND );

HANDLE  OPEN32API O32_OpenEvent( DWORD, BOOL, LPCSTR );

HFILE   OPEN32API O32_OpenFile( LPCSTR, LPOFSTRUCT, UINT );

HANDLE  OPEN32API O32_OpenMutex( DWORD, BOOL, LPCSTR );

HANDLE  OPEN32API O32_OpenProcess( DWORD, BOOL, DWORD );

HANDLE  OPEN32API O32_OpenSemaphore( DWORD, BOOL, LPCSTR );

VOID    OPEN32API O32_OutputDebugString( LPCSTR );

LONG    OPEN32API O32_PackDDElParam(UINT, UINT, UINT );

BOOL    OPEN32API O32_PaintRgn( HDC, HRGN );

BOOL    OPEN32API O32_PatBlt( HDC, int, int, int, int, DWORD );

HRGN    OPEN32API O32_PathToRegion( HDC);

BOOL    OPEN32API O32_PeekMessage( LPMSG, HWND, UINT, UINT, UINT );

BOOL    OPEN32API O32_Pie( HDC, int, int, int, int, int, int, int, int );

BOOL    OPEN32API O32_PlayEnhMetaFile( HDC, HENHMETAFILE, const RECT * );

BOOL    OPEN32API O32_PlayMetaFile( HDC, HMETAFILE );

BOOL    OPEN32API O32_PlayMetaFileRecord( HDC, LPHANDLETABLE, PMETARECORD, int );

BOOL    OPEN32API O32_PolyBezier( HDC, const POINT *, int );

BOOL    OPEN32API O32_PolyBezierTo( HDC, const POINT *, DWORD );

BOOL    OPEN32API O32_PolyDraw( HDC, const POINT *, const BYTE *, int );

BOOL    OPEN32API O32_Polygon( HDC, const POINT *, int );

BOOL    OPEN32API O32_Polyline( HDC, const POINT *, int );

BOOL    OPEN32API O32_PolylineTo( HDC, const POINT *, DWORD );

BOOL    OPEN32API O32_PolyPolygon( HDC, const POINT *, const int *, int );

BOOL    OPEN32API O32_PolyPolyline( HDC, const POINT *, const DWORD *, DWORD );

VOID    OPEN32API O32_PostQuitMessage( int );

BOOL    OPEN32API O32_PostMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

BOOL    OPEN32API O32_PostThreadMessage( DWORD, UINT, WPARAM, LPARAM );

BOOL    OPEN32API O32_PtInRect( const RECT *, POINT );

BOOL    OPEN32API O32_PtInRegion( HRGN, int, int );

BOOL    OPEN32API O32_PtVisible( HDC, int, int );

BOOL    OPEN32API O32_PrintDlg( LPPRINTDLGA );

BOOL    OPEN32API O32_PulseEvent ( HANDLE );

BOOL    OPEN32API O32_ReadFile( HANDLE, PVOID, DWORD, PDWORD, LPOVERLAPPED );

UINT    OPEN32API O32_RealizePalette( HDC);

BOOL    OPEN32API O32_RectInRegion( HRGN, const RECT * );

BOOL    OPEN32API O32_RectVisible( HDC, const RECT *);

BOOL    OPEN32API O32_Rectangle( HDC, int, int, int, int );

BOOL    OPEN32API O32_RedrawWindow( HWND, const RECT *, HRGN, DWORD );

LONG    OPEN32API O32_RegCloseKey( HKEY );

LONG    OPEN32API O32_RegCreateKey( HKEY, LPCSTR, PHKEY );

LONG    OPEN32API O32_RegCreateKeyEx( HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, PDWORD );

LONG    OPEN32API O32_RegDeleteKey( HKEY, LPCSTR );

LONG    OPEN32API O32_RegDeleteValue( HKEY, LPSTR );

LONG    OPEN32API O32_RegEnumKey( HKEY, DWORD, LPSTR, DWORD );

LONG    OPEN32API O32_RegEnumKeyEx( HKEY, DWORD, LPSTR, PDWORD, PDWORD, LPSTR, PDWORD, FILETIME * );

LONG    OPEN32API O32_RegEnumValue( HKEY, DWORD, LPSTR, PDWORD, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    OPEN32API O32_RegOpenKey( HKEY, LPCSTR, PHKEY );

LONG    OPEN32API O32_RegOpenKeyEx( HKEY, LPCSTR, DWORD, REGSAM, PHKEY );

LONG    OPEN32API O32_RegQueryInfoKey( HKEY, LPSTR, PDWORD, PDWORD, PDWORD, PDWORD,
                                      PDWORD, PDWORD, PDWORD, PDWORD, PDWORD, FILETIME * );

LONG    OPEN32API O32_RegQueryValue( HKEY, LPCSTR, LPSTR, PLONG );

LONG    OPEN32API O32_RegQueryValueEx( HKEY, LPCSTR, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    OPEN32API O32_RegSetValue( HKEY, LPCSTR, DWORD, LPCSTR, DWORD );

LONG    OPEN32API O32_RegSetValueEx( HKEY, LPCSTR, DWORD, DWORD, const BYTE *, DWORD );

WORD    OPEN32API O32_RegisterClass( const WNDCLASSA *);

UINT    OPEN32API O32_RegisterClipboardFormat( LPCSTR );

UINT    OPEN32API O32_RegisterWindowMessage( LPCSTR );

BOOL    OPEN32API O32_ReleaseCapture( void );

int       OPEN32API O32_ReleaseDC( HWND, HDC );

BOOL    OPEN32API O32_ReleaseMutex( HANDLE );

BOOL    OPEN32API O32_ReleaseSemaphore( HANDLE, LONG, PLONG );

BOOL    OPEN32API O32_RemoveDirectory( LPCSTR );

BOOL    OPEN32API O32_RemoveFontResource( LPCSTR );

BOOL    OPEN32API O32_RemoveMenu( HMENU, UINT, UINT );

HANDLE  OPEN32API O32_RemoveProp( HWND, LPCSTR );

HWND    OPEN32API O32_ReplaceText( LPFINDREPLACEA );

BOOL    OPEN32API O32_ReplyMessage( LRESULT );

BOOL    OPEN32API O32_ResetDC( HDC, const DEVMODEA * );

BOOL    OPEN32API O32_ResetEvent( HANDLE );

BOOL    OPEN32API O32_ResizePalette( HPALETTE, UINT );

BOOL    OPEN32API O32_RestoreDC( HDC, int );

DWORD   OPEN32API O32_ResumeThread( HANDLE );

LONG    OPEN32API O32_ReuseDDElParam( LONG, UINT, UINT, UINT, UINT );

BOOL    OPEN32API O32_RoundRect( HDC, int, int, int, int, int, int );

int       OPEN32API O32_SaveDC( HDC );

BOOL    OPEN32API O32_ScaleViewportExtEx( HDC, int, int, int, int, PSIZE );

BOOL    OPEN32API O32_ScaleWindowExtEx( HDC, int, int, int, int, PSIZE );

BOOL    OPEN32API O32_ScreenToClient( HWND, PPOINT );

BOOL    OPEN32API O32_ScrollDC( HDC, int, int, const RECT *, const RECT *, HRGN, PRECT );

BOOL    OPEN32API O32_ScrollWindow( HWND, int, int, const RECT *, const RECT * );

BOOL    OPEN32API O32_ScrollWindowEx( HWND, int, int, const RECT *, const RECT *, HRGN, PRECT, UINT );

DWORD   OPEN32API O32_SearchPath( LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPSTR *);

int       OPEN32API O32_SelectClipRgn( HDC, HRGN );

HGDIOBJ OPEN32API O32_SelectObject( HDC, HGDIOBJ );

HPALETTE OPEN32API O32_SelectPalette( HDC, HPALETTE, BOOL );

LONG    OPEN32API O32_SendDlgItemMessage( HWND, int, UINT, WPARAM, LPARAM );

LRESULT OPEN32API O32_SendMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HWND    OPEN32API O32_SetActiveWindow( HWND );

int       OPEN32API O32_SetArcDirection( HDC, int );

LONG    OPEN32API O32_SetBitmapBits( HBITMAP, DWORD, const VOID * );

BOOL    OPEN32API O32_SetBitmapDimensionEx( HBITMAP, int, int, PSIZE );

COLORREF OPEN32API O32_SetBkColor( HDC, COLORREF );

int       OPEN32API O32_SetBkMode( HDC, int );

LONG    OPEN32API O32_SetBoundsRect( HDC, const RECT *, UINT );

BOOL    OPEN32API O32_SetBrushOrgEx( HDC, int, int, PPOINT );

HWND    OPEN32API O32_SetCapture( HWND );

BOOL    OPEN32API O32_SetCaretBlinkTime( UINT );

BOOL    OPEN32API O32_SetCaretPos( int, int );

DWORD   OPEN32API O32_SetClassLong( HWND, int, LONG );

WORD    OPEN32API O32_SetClassWord( HWND, int, WORD );

HANDLE  OPEN32API O32_SetClipboardData( UINT, HANDLE );

HWND    OPEN32API O32_SetClipboardViewer( HWND );

BOOL    OPEN32API O32_SetCurrentDirectory(  LPSTR );

HCURSOR OPEN32API O32_SetCursor( HCURSOR );

BOOL    OPEN32API O32_SetCursorPos( int, int );

int       OPEN32API O32_SetDIBits( HDC, HBITMAP, UINT, UINT, const VOID *, const BITMAPINFO *, UINT );

int       OPEN32API O32_SetDIBitsToDevice( HDC, int, int, int, int, int, int, UINT, UINT, PVOID, PBITMAPINFO, UINT );

BOOL    OPEN32API O32_SetDlgItemInt( HWND, int, UINT, BOOL );

BOOL    OPEN32API O32_SetDlgItemText( HWND, int, LPCSTR );

BOOL    OPEN32API O32_SetDoubleClickTime( UINT );

BOOL    OPEN32API O32_SetEndOfFile( HANDLE );

HENHMETAFILE OPEN32API O32_SetEnhMetaFileBits( UINT, const BYTE * );

BOOL    OPEN32API O32_SetEnvironmentVariable( LPCSTR, LPCSTR );

BOOL    OPEN32API O32_SetEvent( HANDLE );

BOOL    OPEN32API O32_SetFileAttributes( LPCSTR, DWORD );

DWORD   OPEN32API O32_SetFilePointer( HANDLE, LONG, PLONG, DWORD );

BOOL    OPEN32API O32_SetFileTime( HANDLE, const FILETIME *, const FILETIME *, const FILETIME * );

HWND    OPEN32API O32_SetFocus( HWND );

BOOL    OPEN32API O32_SetForegroundWindow( HWND );

int       OPEN32API O32_SetGraphicsMode(HDC, int );

UINT    OPEN32API O32_SetHandleCount( UINT );

VOID    OPEN32API O32_SetLastError( DWORD );

BOOL    OPEN32API O32_SetLocalTime( const SYSTEMTIME * );

int       OPEN32API O32_SetMapMode( HDC, int );

DWORD   OPEN32API O32_SetMapperFlags( HDC, DWORD );

BOOL    OPEN32API O32_SetMenu( HWND, HMENU );

BOOL    OPEN32API O32_SetMenuItemBitmaps( HMENU, UINT, UINT, HBITMAP, HBITMAP );

HMETAFILE OPEN32API O32_SetMetaFileBitsEx( UINT, PBYTE );

BOOL    OPEN32API O32_SetMiterLimit( HDC, float limit, float * );

UINT    OPEN32API O32_SetPaletteEntries( HPALETTE, UINT, UINT, const PALETTEENTRY *);

HWND    OPEN32API O32_SetParent( HWND, HWND );

COLORREF
          OPEN32API O32_SetPixel( HDC, int, int, COLORREF );

int       OPEN32API O32_SetPolyFillMode( HDC, int );

BOOL    OPEN32API O32_SetPriorityClass( HANDLE, DWORD );

BOOL    OPEN32API O32_SetProp( HWND, LPCSTR, HANDLE );

BOOL    OPEN32API O32_SetRect( PRECT, int, int, int, int );

BOOL    OPEN32API O32_SetRectEmpty( PRECT );

BOOL    OPEN32API O32_SetRectRgn( HRGN, int, int, int, int );

int       OPEN32API O32_SetROP2( HDC, int );

int       OPEN32API O32_SetScrollPos( HWND, int, int, BOOL );

BOOL    OPEN32API O32_SetScrollRange( HWND, int, int, int, BOOL );

BOOL    OPEN32API O32_SetStdHandle( DWORD, HANDLE );

int       OPEN32API O32_SetStretchBltMode( HDC, int );

UINT    OPEN32API O32_SetSysColors( int, const int *, const COLORREF * );

BOOL    OPEN32API O32_SetSystemTime( const SYSTEMTIME * );

BOOL    OPEN32API O32_SetTimeZoneInformation( const TIME_ZONE_INFORMATION * );

UINT    OPEN32API O32_SetTextAlign( HDC, UINT );

int       OPEN32API O32_SetTextCharacterExtra( HDC, int );

COLORREF OPEN32API O32_SetTextColor( HDC, COLORREF );

BOOL    OPEN32API O32_SetTextJustification( HDC, int, int );

BOOL    OPEN32API O32_SetThreadPriority( HANDLE, int );

UINT    OPEN32API O32_SetTimer( HWND, UINT, UINT, TIMERPROC_O32 );

BOOL    OPEN32API O32_SetViewportExtEx( HDC, int, int, PSIZE );

BOOL    OPEN32API O32_SetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API O32_SetVolumeLabel( LPCSTR, LPCSTR );

BOOL    OPEN32API O32_SetWindowExtEx( HDC, int, int, PSIZE );

LONG    OPEN32API O32_SetWindowLong( HWND, int, LONG );

BOOL    OPEN32API O32_SetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    OPEN32API O32_SetWindowPlacement( HWND, const WINDOWPLACEMENT * );

BOOL    OPEN32API O32_SetWindowPos( HWND, HWND, int, int, int, int, UINT );

HHOOK   OPEN32API O32_SetWindowsHookEx( int, HOOKPROC_O32, HINSTANCE, DWORD );

BOOL    OPEN32API O32_SetWindowText( HWND, LPCSTR );

WORD    OPEN32API O32_SetWindowWord( HWND, int, WORD );

HENHMETAFILE OPEN32API O32_SetWinMetaFileBits( UINT, const BYTE *, HDC, const METAFILEPICT * );

BOOL    OPEN32API O32_SetWorldTransform( HDC, LPXFORM );

BOOL    OPEN32API O32_ShowCaret( HWND );

int       OPEN32API O32_ShowCursor( BOOL );

BOOL    OPEN32API O32_ShowOwnedPopups( HWND, BOOL );

BOOL    OPEN32API O32_ShowScrollBar( HWND, int, BOOL );

BOOL    OPEN32API O32_ShowWindow( HWND, int );

DWORD   OPEN32API O32_SizeofResource( HINSTANCE, HRSRC );

VOID    OPEN32API O32_Sleep( DWORD );

int       OPEN32API O32_StartDoc( HDC, LPDOCINFOA );

int       OPEN32API O32_StartPage( HDC );

int       OPEN32API O32_StretchDIBits( HDC, int, int, int, int, int, int, int, int, void *, PBITMAPINFO, UINT, DWORD );

BOOL    OPEN32API O32_StretchBlt( HDC, int, int, int, int, HDC, int, int, int, int,  DWORD );

BOOL    OPEN32API O32_StrokeAndFillPath( HDC );

BOOL    OPEN32API O32_StrokePath( HDC );

BOOL    OPEN32API O32_SubtractRect( PRECT, const RECT *, const RECT *);

DWORD   OPEN32API O32_SuspendThread( HANDLE );

BOOL    OPEN32API O32_SwapMouseButton( BOOL );

UINT    OPEN32API O32_SystemParametersInfo( UINT, UINT, PVOID, UINT );

BOOL    OPEN32API O32_SystemTimeToFileTime( const SYSTEMTIME *, FILETIME * );

BOOL    OPEN32API O32_SystemTimeToTzSpecificLocalTime( LPTIME_ZONE_INFORMATION, LPSYSTEMTIME, LPSYSTEMTIME );

LONG    OPEN32API O32_TabbedTextOut( HDC, int, int, LPCSTR, int, int, int *, int );

BOOL    OPEN32API O32_TerminateProcess( HANDLE, UINT );

BOOL    OPEN32API O32_TerminateThread( HANDLE, DWORD );

BOOL    OPEN32API O32_TextOut( HDC, int, int, LPCSTR, int );

DWORD   OPEN32API O32_TlsAlloc( VOID );

BOOL    OPEN32API O32_TlsFree( DWORD );

PVOID   OPEN32API O32_TlsGetValue( DWORD );

BOOL    OPEN32API O32_TlsSetValue( DWORD, PVOID );

BOOL    OPEN32API O32_TrackPopupMenu( HMENU, UINT, int, int, int, HWND, const RECT * );

int       OPEN32API O32_TranslateAccelerator( HWND, HACCEL, LPMSG );

BOOL    OPEN32API O32_TranslateMDISysAccel( HWND, LPMSG );

BOOL    OPEN32API O32_TranslateMessage( const MSG * );

BOOL    OPEN32API O32_UnhookWindowsHookEx( HHOOK );

BOOL    OPEN32API O32_UnionRect( PRECT, const RECT *, const RECT * );

BOOL    OPEN32API O32_UnlockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    OPEN32API O32_UnpackDDElParam(UINT, LONG, PUINT, PUINT );

BOOL    OPEN32API O32_UnrealizeObject( HGDIOBJ );

BOOL    OPEN32API O32_UnregisterClass( LPCSTR, HINSTANCE );

BOOL    OPEN32API O32_UpdateWindow( HWND );

BOOL    OPEN32API O32_ValidateRect( HWND, const RECT *);

BOOL    OPEN32API O32_ValidateRgn( HWND, HRGN );

SHORT   OPEN32API O32_VkKeyScan( char keyScan);

DWORD   OPEN32API O32_WaitForMultipleObjects( DWORD, const HANDLE *, BOOL, DWORD );

DWORD   OPEN32API O32_WaitForSingleObject( HANDLE, DWORD );

DWORD   OPEN32API O32_WaitForInputIdle(HANDLE hProcess, DWORD dwTimeOut);

BOOL    OPEN32API O32_WaitMessage( void);

BOOL    OPEN32API O32_WidenPath( HDC);

UINT    OPEN32API O32_WinExec( LPCSTR, UINT );

BOOL    OPEN32API O32_WinHelp( HWND, LPCSTR, UINT, DWORD );

HWND    OPEN32API O32_WindowFromPoint( POINT );

HWND    OPEN32API O32_WindowFromDC( HDC );

BOOL    OPEN32API O32_WriteFile( HANDLE, const VOID *, DWORD, PDWORD, LPOVERLAPPED );

BOOL    OPEN32API O32_WritePrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPCSTR );

BOOL    OPEN32API O32_WriteProfileString( LPCSTR, LPCSTR, LPCSTR );

VOID    OPEN32API O32_ZeroMemory( PVOID, DWORD );

HFILE   OPEN32API O32__lclose( HFILE );

HFILE   OPEN32API O32__lcreat( LPCSTR, int );

HFILE   OPEN32API O32__lopen( LPCSTR, int );

UINT    OPEN32API O32__lread( HFILE, PVOID, UINT );

LONG    OPEN32API O32__llseek( HFILE, LONG, int );

UINT    OPEN32API O32__lwrite( HFILE, const VOID *, UINT );

DWORD   OPEN32API O32_timeGetSystemTime( LPMMTIME, UINT );

DWORD   OPEN32API O32_timeGetTime( VOID );

int     OPEN32API O32_wsprintf( LPSTR, LPCSTR, ... );
int     OPEN32API O32_wvsprintf( LPSTR, LPCSTR, const VOID *);

LPSTR   OPEN32API O32_CharNext( LPCSTR );

LPSTR   OPEN32API O32_CharPrev( LPCSTR, LPCSTR );

HWND    OPEN32API O32_GetDesktopWindow( VOID );

BOOL    OPEN32API O32_CharToOem( LPCSTR, LPSTR );

BOOL    OPEN32API O32_OemToChar( LPCSTR, LPSTR );

BOOL    OPEN32API O32_CharToOemBuff( LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API O32_OemToCharBuff( LPCSTR, LPSTR, DWORD );

BOOL    OPEN32API O32_IsBadHugeReadPtr( const void *, UINT );

BOOL    OPEN32API O32_IsBadHugeWritePtr( PVOID, UINT );

BOOL    OPEN32API O32_FloodFill(HDC, int, int, COLORREF );

BOOL    OPEN32API O32_IsCharAlpha( WCHAR );

BOOL    OPEN32API O32_IsCharAlphaNumeric( WCHAR );

BOOL    OPEN32API O32_IsCharLower( WCHAR );

BOOL    OPEN32API O32_IsCharUpper( WCHAR );

LPSTR   OPEN32API O32_lstrcat( LPSTR, LPCSTR );

int       OPEN32API O32_lstrcmp( LPCSTR, LPCSTR );

int       OPEN32API O32_lstrcmpi( LPCSTR, LPCSTR );

LPSTR   OPEN32API O32_lstrcpy( LPSTR, LPCSTR );

int       OPEN32API O32_lstrlen( LPCSTR );

/* -----------------------------------------------------------------*/


int       OPEN32API O32_WinCallWinMain(int, char *[], FNMAIN_O32, int);

/* The following functions are used for translating data between the native
 * Presentation Manager format and the Developer API Extensions equivalents.
 */
typedef enum { WINX2PM, PM2WINX} XLATEDIR;

/* Use the GDI object type to specify the type of handle passing in
 */
BOOL    OPEN32API O32_WinTranslateDevicePoints( HDC, HWND, PPOINT, INT, XLATEDIR );
BOOL    OPEN32API O32_WinTranslateDeviceRects ( HDC, HWND, PRECT,  INT, XLATEDIR );
ULONG   OPEN32API O32_WinTranslateGraphicsObjectHandle( HGDIOBJ, XLATEDIR, ULONG );
ULONG   OPEN32API O32_WinTranslateMnemonicString( LPCSTR, LPSTR *, ULONG, XLATEDIR );
DWORD   OPEN32API O32_WinQueryTranslateMode( VOID );
BOOL    OPEN32API O32_WinSetTranslateMode( DWORD );

VOID    OPEN32API O32_GetStartupInfo(LPSTARTUPINFOA lpStartupInfo);
HBITMAP OPEN32API O32_CreateBitmapFromPMHandle(HBITMAP hPMBitmap);
HPALETTE OPEN32API O32_CreatePaletteFromPMHandle(HPALETTE hPMPalette);
HANDLE  OPEN32API O32_GetPMHandleFromGDIHandle(HGDIOBJ hGDI);

#endif    /* ifdef _OS2WIN_H */
