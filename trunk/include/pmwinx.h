/* $Id: pmwinx.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

#ifndef __PMWINX_H__
#define __PMWINX_H__



#define GetWindowTask(hWnd) ((HANDLE)GetWindowThreadProcessId(hWnd, NULL))


/* Macro'd APIs
 */
#define CreateWindow(lpClassName, lpWindowName, dwStyle, x, y,\
                        nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
        CreateWindowEx(0, lpClassName, lpWindowName, dwStyle, x, y,\
                          nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

#define AnsiToOem     CharToOem
#define OemToAnsi     OemToChar
#define AnsiToOemBuff CharToOemBuff
#define OemToAnsiBuff OemToCharBuff
#define AnsiUpper     CharUpper
#define AnsiUpperBuff CharUpperBuff
#define AnsiLower     CharLower
#define AnsiLowerBuff CharLowerBuff
#define AnsiNext      CharNext
#define AnsiPrev      CharPrev

/* Name Mapped API's
 */
#define CreateBitmapIndirect            _CreateBitmapIndirect
#define CreateDialogIndirect            _CreateDialogIndirect
#define CreateDialogIndirectParam       _CreateDialogIndirectParam
#define CreateIconIndirect              _CreateIconIndirect
#define DialogBoxIndirect               _DialogBoxIndirect
#define DialogBoxIndirectParam          _DialogBoxIndirectParam
#define LoadMenuIndirect                _LoadMenuIndirect
#define LoadResource                    _LoadResource


/* API Prototypes
 */
int       WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int       WINAPI AbortDoc( HDC );

BOOL    WINAPI AbortPath( HDC );

ATOM    WINAPI AddAtom( LPCSTR );

int       WINAPI AddFontResource( LPCSTR );

BOOL    WINAPI AdjustWindowRect( PRECT, DWORD, BOOL );

BOOL    WINAPI AdjustWindowRectEx( PRECT, DWORD, BOOL, DWORD );

BOOL    WINAPI AngleArc( HDC, int, int, DWORD, float startAngle, float sweepAngle );

BOOL    WINAPI AnimatePalette( HPALETTE, UINT, UINT, const PALETTEENTRY *);

BOOL    WINAPI AppendMenu( HMENU, UINT, UINT, LPCSTR );

BOOL    WINAPI Arc( HDC, int, int, int, int, int, int, int, int );

BOOL    WINAPI ArcTo( HDC, int, int, int, int, int xRad1, int yRad1, int xRad2, int yRad2);

UINT    WINAPI ArrangeIconicWindows( HWND );

BOOL    WINAPI Beep( DWORD, DWORD );

HDWP    WINAPI BeginDeferWindowPos( int );

HDC     WINAPI BeginPaint( HWND, PPAINTSTRUCT );

BOOL    WINAPI BeginPath( HDC );

BOOL    WINAPI BitBlt( HDC , int, int, int, int, HDC , int, int, DWORD );

BOOL    WINAPI BringWindowToTop( HWND );

BOOL    WINAPI CallMsgFilter( PMSG, int );

LRESULT WINAPI CallNextHookEx( HHOOK, int, WPARAM, LPARAM );

LRESULT WINAPI CallWindowProc( WNDPROC, HWND, UINT, WPARAM, LPARAM );

BOOL    WINAPI ChangeClipboardChain( HWND, HWND );

LPSTR   WINAPI CharLower( LPSTR );

DWORD   WINAPI CharLowerBuff( LPSTR, DWORD );

LPSTR   WINAPI CharUpper( LPSTR );

DWORD   WINAPI CharUpperBuff( LPSTR, DWORD );

BOOL    WINAPI CheckDlgButton( HWND, int, UINT );

DWORD   WINAPI CheckMenuItem( HMENU, UINT, UINT );

BOOL    WINAPI CheckRadioButton( HWND, int, int, int );

HWND    WINAPI ChildWindowFromPoint( HWND, POINT );

BOOL    WINAPI ChooseColor( PCHOOSECOLOR );

BOOL    WINAPI ChooseFont( LPCHOOSEFONT );

BOOL    WINAPI Chord( HDC , int, int, int, int, int xRadial1, int yRadial1, int xRadial2, int yRadial2);

BOOL    WINAPI ClientToScreen( HWND, PPOINT );

BOOL    WINAPI ClipCursor( const RECT *);

BOOL    WINAPI CloseClipboard( VOID );

HENHMETAFILE WINAPI CloseEnhMetaFile( HDC );

BOOL    WINAPI CloseFigure( HDC );

BOOL    WINAPI CloseHandle( HANDLE );

HMETAFILE WINAPI CloseMetaFile( HDC );

BOOL    WINAPI CloseWindow( HWND );

int       WINAPI CombineRgn( HRGN, HRGN hrgnSrc1, HRGN hrgnSrc2, int );

DWORD   WINAPI CommDlgExtendedError( VOID );

LONG    WINAPI CompareFileTime( const FILETIME *, const FILETIME * );

HCURSOR WINAPI CopyCursor( HCURSOR );

HENHMETAFILE WINAPI CopyEnhMetaFile( HENHMETAFILE, LPCSTR );

BOOL    WINAPI CopyFile( LPCSTR, LPCSTR, BOOL );

HICON   WINAPI CopyIcon( HICON );

HMETAFILE WINAPI CopyMetaFile( HMETAFILE, LPCSTR );

BOOL    WINAPI CopyRect( PRECT, const RECT *);

int       WINAPI CountClipboardFormats( VOID );

HACCEL  WINAPI CreateAcceleratorTable( PACCEL, int );

HBITMAP WINAPI CreateBitmap( int, int, UINT, UINT, const void * );

HBITMAP WINAPI CreateBitmapIndirect( const BITMAP * );

HBRUSH  WINAPI CreateBrushIndirect( PLOGBRUSH );

BOOL    WINAPI CreateCaret( HWND, HBITMAP, int, int );

HBITMAP WINAPI CreateCompatibleBitmap( HDC , int, int );

HDC     WINAPI CreateCompatibleDC( HDC );

HCURSOR WINAPI CreateCursor( HINSTANCE, int, int, int, int, const VOID *, const VOID *);

HDC     WINAPI CreateDC( LPCSTR, LPCSTR, LPCSTR, const DEVMODE *);

HBRUSH  WINAPI CreateDIBPatternBrushPt( const VOID *, UINT );

HBITMAP WINAPI CreateDIBitmap( HDC, const BITMAPINFOHEADER *, DWORD, const void *, const BITMAPINFO *, UINT );

HWND    WINAPI CreateDialog( HINSTANCE, LPCSTR, HWND, DLGPROC );

HWND    WINAPI CreateDialogParam( HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM );

HWND    WINAPI CreateDialogIndirect( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC );

HWND    WINAPI CreateDialogIndirectParam( HINSTANCE, const DLGTEMPLATE *, HWND, DLGPROC, LPARAM );

BOOL    WINAPI CreateDirectory( LPCSTR, PSECURITY_ATTRIBUTES );

HRGN    WINAPI CreateEllipticRgn( int, int, int, int );

HRGN    WINAPI CreateEllipticRgnIndirect( const RECT * );

HENHMETAFILE WINAPI CreateEnhMetaFile( HDC, LPCSTR, const RECT *, LPCSTR );

HANDLE  WINAPI CreateEvent( PSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR );

HANDLE  WINAPI CreateFile( LPCSTR, DWORD, DWORD, PSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE );

HFONT   WINAPI CreateFont( int, int, int, int, int, DWORD, DWORD, DWORD, DWORD,
                                  DWORD, DWORD, DWORD, DWORD, LPCSTR );

HFONT   WINAPI CreateFontIndirect( const  LOGFONT *);

HBRUSH  WINAPI CreateHatchBrush( int, COLORREF );

HDC     WINAPI CreateIC( LPCSTR, LPCSTR, LPCSTR, const DEVMODE *);

HICON   WINAPI CreateIcon( HINSTANCE, int, int, BYTE, BYTE, const BYTE *, const BYTE * );

HICON   WINAPI CreateIconFromResource( PBYTE, DWORD, BOOL, DWORD );

HICON   WINAPI CreateIconIndirect( PICONINFO );

HMENU   WINAPI CreateMenu( void);

HDC     WINAPI CreateMetaFile( LPCSTR );

HWND    WINAPI CreateMDIWindow( LPSTR, LPSTR, DWORD, int, int, int, int, HWND, HINSTANCE, LPARAM );

HANDLE  WINAPI CreateMutex( PSECURITY_ATTRIBUTES, BOOL, LPCSTR );

HPALETTE WINAPI CreatePalette( const LOGPALETTE * );

HBRUSH  WINAPI CreatePatternBrush( HBITMAP );

HPEN    WINAPI CreatePen( int, int, COLORREF );

HPEN    WINAPI CreatePenIndirect( const LOGPEN * );

HRGN    WINAPI CreatePolyPolygonRgn( const POINT *, const INT *, int, int );

HRGN    WINAPI CreatePolygonRgn( const POINT *, int, int );

HMENU   WINAPI CreatePopupMenu( VOID );

BOOL    WINAPI CreateProcess( LPCSTR, LPCSTR, PSECURITY_ATTRIBUTES, PSECURITY_ATTRIBUTES,
                         BOOL, DWORD, PVOID, LPCSTR, PSTARTUPINFO,
                         PPROCESS_INFORMATION );

HRGN    WINAPI CreateRectRgn( int, int, int, int );

HRGN    WINAPI CreateRectRgnIndirect( const RECT * );

HRGN    WINAPI CreateRoundRectRgn( int, int, int, int, int, int );

HANDLE  WINAPI CreateSemaphore( PSECURITY_ATTRIBUTES, LONG, LONG, LPSTR );

HBRUSH  WINAPI CreateSolidBrush( COLORREF );

HANDLE  WINAPI CreateThread( PSECURITY_ATTRIBUTES, DWORD, PTHREAD_START_ROUTINE, PVOID, DWORD, PDWORD );

HWND    WINAPI CreateWindowEx( DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID );

BOOL    WINAPI DPtoLP( HDC, PPOINT, int );

BOOL    WINAPI DdeAbandonTransaction( DWORD, HCONV, DWORD );

PBYTE   WINAPI DdeAccessData(HDDEDATA, PDWORD );

HDDEDATA WINAPI DdeAddData( HDDEDATA, PVOID, DWORD, DWORD );

HDDEDATA WINAPI DdeClientTransaction( PVOID, DWORD, HCONV,
                          HSZ, UINT, UINT, DWORD, PDWORD );

int       WINAPI DdeCmpStringHandles( HSZ hsz1, HSZ hsz2);

HCONV   WINAPI DdeConnect( DWORD, HSZ, HSZ, PCONVCONTEXT );

HCONVLIST WINAPI DdeConnectList(DWORD, HSZ, HSZ, HCONVLIST, PCONVCONTEXT );

HDDEDATA WINAPI DdeCreateDataHandle(DWORD, PVOID, DWORD, DWORD, HSZ, UINT, UINT );

HSZ     WINAPI DdeCreateStringHandle(DWORD, LPCSTR, int );

BOOL    WINAPI DdeDisconnect(HCONV );

BOOL    WINAPI DdeDisconnectList( HCONVLIST );

BOOL    WINAPI DdeEnableCallback(DWORD, HCONV, UINT );

BOOL    WINAPI DdeFreeDataHandle( HDDEDATA );

BOOL    WINAPI DdeFreeStringHandle(DWORD, HSZ );

DWORD   WINAPI DdeGetData( HDDEDATA, PVOID, DWORD, DWORD );

UINT    WINAPI DdeGetLastError(DWORD );

UINT    WINAPI DdeInitialize(PDWORD, PFNCALLBACK, DWORD, DWORD );

BOOL    WINAPI DdeKeepStringHandle(DWORD, HSZ );

HDDEDATA WINAPI DdeNameService( DWORD, HSZ hsz1, HSZ hsz2, UINT );

BOOL    WINAPI DdePostAdvise(DWORD, HSZ, HSZ );

UINT    WINAPI DdeQueryConvInfo( HCONV, DWORD, PCONVINFO );

HCONV   WINAPI DdeQueryNextServer( HCONVLIST, HCONV );

DWORD   WINAPI DdeQueryString(DWORD, HSZ, LPSTR, DWORD, int );

HCONV   WINAPI DdeReconnect( HCONV );

BOOL    WINAPI DdeSetUserHandle( HCONV, DWORD, DWORD );

BOOL    WINAPI DdeUnaccessData( HDDEDATA );

BOOL    WINAPI DdeUninitialize(DWORD );

LRESULT WINAPI DefDlgProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HDWP    WINAPI DeferWindowPos( HDWP, HWND, HWND hWndInsertAfter , int, int, int, int, UINT );

LRESULT WINAPI DefFrameProc( HWND, HWND, UINT, WPARAM, LPARAM );

LRESULT WINAPI DefMDIChildProc( HWND, UINT, WPARAM, LPARAM );

LRESULT WINAPI DefWindowProc( HWND, UINT, WPARAM mp1, LPARAM mp2 );

ATOM    WINAPI DeleteAtom( ATOM );

VOID    WINAPI DeleteCriticalSection( PCRITICAL_SECTION );

BOOL    WINAPI DeleteDC( HDC );

BOOL    WINAPI DeleteEnhMetaFile( HENHMETAFILE );

BOOL    WINAPI DeleteFile( LPCSTR );

BOOL    WINAPI DeleteMenu( HMENU, UINT, UINT );

BOOL    WINAPI DeleteMetaFile( HMETAFILE );

BOOL    WINAPI DeleteObject( HANDLE );

BOOL    WINAPI DestroyAcceleratorTable( HACCEL );

BOOL    WINAPI DestroyCaret( VOID );

BOOL    WINAPI DestroyCursor( HCURSOR );

BOOL    WINAPI DestroyIcon( HICON );

BOOL    WINAPI DestroyMenu( HMENU );

BOOL    WINAPI DestroyWindow( HWND );

DWORD   WINAPI DeviceCapabilities( LPCTSTR, LPCTSTR, WORD, LPTSTR, const DEVMODE *);

int       WINAPI DialogBox( HINSTANCE, LPCSTR, HWND, DLGPROC );

int       WINAPI DialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM );

BOOL    WINAPI DialogBoxIndirect( HINSTANCE, PDLGTEMPLATE, HWND, DLGPROC );

BOOL    WINAPI DialogBoxIndirectParam( HINSTANCE, PDLGTEMPLATE, HWND, DLGPROC, LPARAM );

LONG    WINAPI DispatchMessage( const MSG * );

int       WINAPI DlgDirList( HWND, LPSTR, int, int, UINT );

int       WINAPI DlgDirListComboBox( HWND, LPSTR, int, int, UINT );

BOOL    WINAPI DlgDirSelectEx( HWND, LPSTR, int, int );

BOOL    WINAPI DlgDirSelectComboBoxEx( HWND, LPSTR, int, int );

BOOL    WINAPI DllEntryPoint( HINSTANCE, DWORD, LPVOID );

BOOL    WINAPI DosDateTimeToFileTime( WORD, WORD, PFILETIME );

VOID    WINAPI DragAcceptFiles( HWND, BOOL );

VOID    WINAPI DragFinish( HDROP );

UINT    WINAPI DragQueryFile( HDROP, int, LPSTR, int );

BOOL    WINAPI DragQueryPoint( HDROP, PPOINT );

BOOL    WINAPI DrawFocusRect( HDC, const RECT * );

BOOL    WINAPI DrawIcon( HDC, int, int, HICON );

BOOL    WINAPI DrawMenuBar( HWND );

int       WINAPI DrawText( HDC, LPCSTR, int, PRECT, UINT );

BOOL    WINAPI DuplicateHandle( HANDLE, HANDLE, HANDLE, PHANDLE, DWORD, BOOL, DWORD );

BOOL    WINAPI Ellipse( HDC, int, int, int, int );

BOOL    WINAPI EmptyClipboard( void);

BOOL    WINAPI EnableMenuItem( HMENU, UINT, UINT );

BOOL    WINAPI EnableScrollBar( HWND, UINT, UINT );

BOOL    WINAPI EnableWindow( HWND, BOOL );

BOOL    WINAPI EndDeferWindowPos( HDWP );

BOOL    WINAPI EndDialog( HWND, int );

int       WINAPI EndDoc( HDC );

int       WINAPI EndPage( HDC );

BOOL    WINAPI EndPath( HDC );

BOOL    WINAPI EndPaint( HWND, const PAINTSTRUCT *);

VOID    WINAPI EnterCriticalSection( PCRITICAL_SECTION );

BOOL    WINAPI EnumChildWindows( HWND, WNDENUMPROC, LPARAM );

UINT    WINAPI EnumClipboardFormats( UINT );

BOOL    WINAPI EnumEnhMetaFile( HDC , HENHMETAFILE, ENHMFENUMPROC, PVOID, const RECT * );

int       WINAPI EnumFonts( HDC , LPCSTR, OLDFONTENUMPROC, LPARAM );

int       WINAPI EnumFontFamilies( HDC , LPCSTR, FONTENUMPROC, LPARAM );

BOOL    WINAPI EnumMetaFile( HDC , HMETAFILE, MFENUMPROC, LPARAM );

int       WINAPI EnumObjects( HDC , int, GOBJENUMPROC, LPARAM );

BOOL    WINAPI EnumPrinters( DWORD, LPTSTR, DWORD, LPBYTE, DWORD, LPDWORD, LPDWORD);

int       WINAPI EnumProps( HWND, PROPENUMPROC );

int       WINAPI EnumPropsEx( HWND, PROPENUMPROCEX, LPARAM );

BOOL    WINAPI EnumThreadWindows( DWORD, WNDENUMPROC, LPARAM );

BOOL    WINAPI EnumWindows( WNDENUMPROC, LPARAM );

BOOL    WINAPI EqualRect( const RECT *, const RECT * );

BOOL    WINAPI EqualRgn( HRGN, HRGN );

int       WINAPI Escape( HDC, int, int, LPCSTR, PVOID );

int       WINAPI ExcludeClipRect( HDC, int, int, int, int );

BOOL    WINAPI ExcludeUpdateRgn( HDC, HWND );

VOID    WINAPI ExitProcess( UINT );

VOID    WINAPI ExitThread( DWORD );

BOOL    WINAPI ExitWindows( DWORD, UINT );

BOOL    WINAPI ExitWindowsEx( UINT, DWORD );

HPEN    WINAPI ExtCreatePen( DWORD, DWORD, const LOGBRUSH *, DWORD, const DWORD * );

HRGN    WINAPI ExtCreateRegion( const XFORM *, DWORD, const RGNDATA * );

BOOL    WINAPI ExtFloodFill( HDC, int, int, COLORREF, UINT );

int       WINAPI ExtSelectClipRgn( HDC, HRGN, int );

BOOL    WINAPI ExtTextOut( HDC, int, int, UINT, const RECT *, LPCSTR, UINT, const int * );

VOID    WINAPI FatalAppExit( UINT, LPCSTR );

VOID    WINAPI FatalExit( UINT );

BOOL    WINAPI FileTimeToDosDateTime( const FILETIME *, PWORD, PWORD );

BOOL    WINAPI FileTimeToLocalFileTime( const FILETIME *, PFILETIME );

BOOL    WINAPI FileTimeToSystemTime( const FILETIME *, PSYSTEMTIME );

BOOL    WINAPI FillPath( HDC );

int       WINAPI FillRect( HDC, const RECT *, HBRUSH );

BOOL    WINAPI FillRgn( HDC, HRGN, HBRUSH );

ATOM    WINAPI FindAtom( LPCSTR );

BOOL    WINAPI FindClose( HANDLE );

HANDLE  WINAPI FindFirstFile( LPCSTR, PWIN32_FIND_DATA );

BOOL    WINAPI FindNextFile( HANDLE, PWIN32_FIND_DATA );

HRSRC   WINAPI FindResource( HINSTANCE, LPCSTR, LPCSTR );

HWND    WINAPI FindText( PFINDREPLACE );

HWND    WINAPI FindWindow( LPCSTR lpClassName , LPCSTR );

BOOL    WINAPI FlashWindow( HWND, BOOL );

BOOL    WINAPI FlattenPath( HDC );

BOOL    WINAPI FlushFileBuffers( HANDLE );

int       WINAPI FrameRect( HDC, const RECT *, HBRUSH );

BOOL    WINAPI FrameRgn( HDC, HRGN, HBRUSH, int, int );

BOOL    WINAPI FreeDDElParam( UINT, LONG );

BOOL    WINAPI FreeLibrary( HINSTANCE );

UINT    WINAPI GetACP( VOID );

HWND    WINAPI GetActiveWindow( void);

int       WINAPI GetArcDirection( HDC );

BOOL    WINAPI GetAspectRatioFilterEx( HDC, PSIZE );

UINT    WINAPI GetAtomName( ATOM, LPSTR, int );

LONG    WINAPI GetBitmapBits( HBITMAP, LONG, PVOID );

BOOL    WINAPI GetBitmapDimensionEx( HBITMAP, PSIZE );

COLORREF WINAPI GetBkColor( HDC );

int       WINAPI GetBkMode( HDC );

UINT    WINAPI GetBoundsRect( HDC, PRECT, UINT );

BOOL    WINAPI GetBrushOrgEx( HDC, PPOINT );

HWND    WINAPI GetCapture( void);

UINT    WINAPI GetCaretBlinkTime( VOID );

BOOL    WINAPI GetCaretPos( PPOINT );

BOOL    WINAPI GetCharABCWidths( HDC, UINT, UINT, PABC );

BOOL    WINAPI GetCharWidth( HDC, UINT, UINT, PINT );

BOOL    WINAPI GetClassInfo( HINSTANCE hInstance , LPCSTR, PWNDCLASS );

DWORD   WINAPI GetClassLong( HWND, int );

int       WINAPI GetClassName( HWND, LPSTR, int );

WORD    WINAPI GetClassWord( HWND, int );

BOOL    WINAPI GetClientRect( HWND, PRECT );

HANDLE  WINAPI GetClipboardData( UINT );

int       WINAPI GetClipboardFormatName( UINT, LPSTR, int );

HWND    WINAPI GetClipboardOwner( void);

HWND    WINAPI GetClipboardViewer( VOID );

int       WINAPI GetClipBox( HDC, PRECT );

BOOL    WINAPI GetClipCursor( PRECT );

int       WINAPI GetClipRgn( HDC, HRGN );

LPSTR   WINAPI GetCommandLine( VOID );

int       WINAPI GetCurrentDirectory( DWORD, LPSTR );

HGDIOBJ WINAPI GetCurrentObject( HDC, DWORD );

BOOL    WINAPI GetCurrentPositionEx( HDC, PPOINT );

HANDLE  WINAPI GetCurrentProcess( VOID );

DWORD   WINAPI GetCurrentProcessId( VOID );

HANDLE  WINAPI GetCurrentThread( VOID );

DWORD   WINAPI GetCurrentThreadId( VOID );

DWORD   WINAPI GetCurrentTime( VOID );

HCURSOR WINAPI GetCursor( VOID );

BOOL    WINAPI GetCursorPos( PPOINT );

HDC     WINAPI GetDC( HWND );

HDC     WINAPI GetDCEx( HWND, HRGN, DWORD );

BOOL    WINAPI GetDCOrgEx( HDC, PPOINT );

int       WINAPI GetDIBits( HDC, HBITMAP, UINT, UINT, void *, PBITMAPINFO, UINT );

int       WINAPI GetDeviceCaps( HDC, int );

LONG    WINAPI GetDialogBaseUnits( void);

BOOL    WINAPI GetDiskFreeSpace( LPCSTR, PDWORD, PDWORD, PDWORD, PDWORD );

int       WINAPI GetDlgCtrlID( HWND );

HWND    WINAPI GetDlgItem( HWND, int );

UINT    WINAPI GetDlgItemInt( HWND, int, PBOOL, BOOL );

UINT    WINAPI GetDlgItemText( HWND, int, LPSTR, int );

UINT    WINAPI GetDoubleClickTime( VOID );

UINT    WINAPI GetDriveType( LPCSTR );

HENHMETAFILE WINAPI GetEnhMetaFile( LPCSTR );

UINT    WINAPI GetEnhMetaFileBits( HENHMETAFILE, UINT, PBYTE );

UINT    WINAPI GetEnhMetaFileHeader( HENHMETAFILE, UINT, PENHMETAHEADER );

UINT    WINAPI GetEnhMetaFilePaletteEntries( HENHMETAFILE, UINT, PPALETTEENTRY );

PVOID   WINAPI GetEnvironmentStrings( VOID );

DWORD   WINAPI GetEnvironmentVariable(LPCSTR, LPSTR, DWORD );

BOOL    WINAPI GetExitCodeProcess( HANDLE, LPDWORD );

BOOL    WINAPI GetExitCodeThread( HANDLE, LPDWORD );

DWORD   WINAPI GetFileAttributes( LPSTR );

DWORD   WINAPI GetFileInformationByHandle( HANDLE, PBY_HANDLE_FILE_INFORMATION );

DWORD   WINAPI GetFileSize( HANDLE, PDWORD );

BOOL    WINAPI GetFileTime( HANDLE, PFILETIME, PFILETIME, PFILETIME );

short     WINAPI GetFileTitle( LPCSTR, LPSTR, WORD );

DWORD   WINAPI GetFileType( HANDLE );

HWND    WINAPI GetFocus( VOID );

HWND    WINAPI GetForegroundWindow( VOID );

DWORD   WINAPI GetFullPathName( LPCSTR, DWORD, LPSTR, LPSTR * );

int       WINAPI GetGraphicsMode(HDC);

BOOL    WINAPI GetIconInfo( HICON, PICONINFO );

DWORD   WINAPI GetKerningPairs( HDC, DWORD, PKERNINGPAIR );

int       WINAPI GetKeyboardType( int );

int       WINAPI GetKeyNameText( LPARAM, LPSTR, int );

SHORT   WINAPI GetKeyState( int );

HWND    WINAPI GetLastActivePopup( HWND );

DWORD   WINAPI GetLastError( VOID );

VOID    WINAPI GetLocalTime( PSYSTEMTIME );

DWORD   WINAPI GetLogicalDriveStrings( DWORD, LPSTR );

DWORD   WINAPI GetLogicalDrives( VOID );

int       WINAPI GetMapMode( HDC);

HMENU   WINAPI GetMenu( HWND );

LONG    WINAPI GetMenuCheckMarkDimensions(void);

int       WINAPI GetMenuItemCount( HMENU );

UINT    WINAPI GetMenuItemID( HMENU, int );

UINT    WINAPI GetMenuState( HMENU, UINT, UINT );

int       WINAPI GetMenuString( HMENU, UINT, LPSTR, int, UINT );

BOOL    WINAPI GetMessage( PMSG, HWND hwnd , UINT, UINT );

LONG    WINAPI GetMessageExtraInfo( VOID );

DWORD   WINAPI GetMessagePos( VOID );

LONG    WINAPI GetMessageTime( VOID );

HMETAFILE WINAPI GetMetaFile( LPCSTR );

UINT    WINAPI GetMetaFileBitsEx( HMETAFILE, UINT, LPVOID );

BOOL    WINAPI GetMiterLimit( HDC, float *);

DWORD   WINAPI GetModuleFileName( HINSTANCE, LPSTR, DWORD );

HMODULE WINAPI GetModuleHandle( LPCSTR );

COLORREF WINAPI GetNearestColor( HDC, COLORREF );

UINT    WINAPI GetNearestPaletteIndex( HPALETTE, COLORREF );

HWND    WINAPI GetNextDlgGroupItem( HWND, HWND, BOOL );

HWND    WINAPI GetNextDlgTabItem( HWND, HWND, BOOL );

HWND    WINAPI GetNextWindow( HWND, UINT );

int       WINAPI GetOEMCP( VOID );

int       WINAPI GetObject( HGDIOBJ, int, void * );

DWORD   WINAPI GetObjectType( HGDIOBJ );

HWND    WINAPI GetOpenClipboardWindow( VOID );

BOOL    WINAPI GetOpenFileName( POPENFILENAME );

UINT    WINAPI GetOutlineTextMetrics( HDC, UINT, POUTLINETEXTMETRIC );

BOOL    WINAPI GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL );

UINT    WINAPI GetPaletteEntries( HPALETTE, UINT, UINT, PPALETTEENTRY );

HWND    WINAPI GetParent( HWND );

UINT    WINAPI GetPath( HDC, PPOINT, PBYTE, int );

COLORREF WINAPI GetPixel( HDC, int, int );

int       WINAPI GetPolyFillMode( HDC);

DWORD   WINAPI GetPriorityClass( HANDLE );

int       WINAPI GetPriorityClipboardFormat( PUINT, int );

UINT    WINAPI GetPrivateProfileInt( LPCSTR, LPCSTR, INT, LPCSTR );

DWORD   WINAPI GetPrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR );

FARPROC WINAPI GetProcAddress( HMODULE, LPCSTR );

UINT    WINAPI GetProfileInt( LPCSTR, LPCSTR, INT );

DWORD   WINAPI GetProfileString( LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD );

HANDLE  WINAPI GetProp( HWND, LPCSTR );

DWORD   WINAPI GetQueueStatus( UINT );

int       WINAPI GetROP2( HDC );

BOOL    WINAPI GetRasterizerCaps( PRASTERIZER_STATUS, UINT );

int       WINAPI GetRgnBox( HRGN, PRECT );

DWORD   WINAPI GetRegionData( HRGN, DWORD, PRGNDATA );

BOOL    WINAPI GetSaveFileName( POPENFILENAME );

int       WINAPI GetScrollPos( HWND, int );

BOOL    WINAPI GetScrollRange( HWND, int, int *, int * );

HANDLE  WINAPI GetStdHandle( DWORD );

HGDIOBJ WINAPI GetStockObject( int );

int       WINAPI GetStretchBltMode( HDC );

HMENU   WINAPI GetSubMenu( HWND, int );

DWORD   WINAPI GetSysColor( int );

UINT    WINAPI GetSystemDirectory( LPSTR, UINT );

HMENU   WINAPI GetSystemMenu( HWND, BOOL );

int       WINAPI GetSystemMetrics( int );

UINT    WINAPI GetSystemPaletteEntries( HDC, UINT, UINT, PPALETTEENTRY );

void      WINAPI GetSystemTime( PSYSTEMTIME );

DWORD   WINAPI GetTabbedTextExtent( HDC, LPCSTR, int, int, int * );

UINT    WINAPI GetTempFileName( LPCSTR, LPCSTR, UINT, LPSTR );

DWORD   WINAPI GetTempPath( DWORD, LPSTR );

UINT    WINAPI GetTextAlign( HDC);

int       WINAPI GetTextCharacterExtra( HDC);

COLORREF WINAPI GetTextColor( HDC );

BOOL    WINAPI GetTextExtentPoint( HDC, LPCSTR, int, PSIZE );

BOOL    WINAPI GetTextExtentPoint32( HDC, LPCSTR, int, PSIZE );

int       WINAPI GetTextFace( HDC, int, LPSTR );

BOOL    WINAPI GetTextMetrics( HDC, PTEXTMETRIC );

DWORD   WINAPI GetThreadPriority( HANDLE );

DWORD   WINAPI GetTickCount( VOID );

DWORD   WINAPI GetTimeZoneInformation( PTIME_ZONE_INFORMATION );

HWND    WINAPI GetTopWindow( HWND );

BOOL    WINAPI GetUpdateRect( HWND, PRECT, BOOL );

int       WINAPI GetUpdateRgn( HWND, HRGN, BOOL );

BOOL    WINAPI GetViewportExtEx( HDC, PSIZE );

BOOL    WINAPI GetViewportOrgEx( HDC, PPOINT );

DWORD   WINAPI GetVolumeInformation( LPCSTR, LPSTR, DWORD, PDWORD,
                       PDWORD, PDWORD, LPSTR, DWORD );

UINT    WINAPI GetWinMetaFileBits( HENHMETAFILE, UINT, PBYTE, int, HDC);

HWND    WINAPI GetWindow( HWND, UINT );

HDC     WINAPI GetWindowDC( HWND );

BOOL    WINAPI GetWindowExtEx( HDC, PSIZE );

LONG    WINAPI GetWindowLong( HWND, int );

BOOL    WINAPI GetWindowOrgEx( HDC, PPOINT );

BOOL    WINAPI GetWindowPlacement( HWND, PWINDOWPLACEMENT );

BOOL    WINAPI GetWindowRect( HWND, PRECT );

UINT    WINAPI GetWindowsDirectory( LPSTR, UINT );

int       WINAPI GetWindowText( HWND, LPSTR, int );

int       WINAPI GetWindowTextLength( HWND );

DWORD   WINAPI GetWindowThreadProcessId(HWND, PDWORD );

WORD    WINAPI GetWindowWord( HWND, int );

BOOL    WINAPI GetWorldTransform( HDC, PXFORM );

ATOM    WINAPI GlobalAddAtom( LPCSTR );

HGLOBAL WINAPI GlobalAlloc( UINT, DWORD );

ATOM    WINAPI GlobalDeleteAtom( ATOM );

HGLOBAL WINAPI GlobalDiscard( HGLOBAL );

ATOM    WINAPI GlobalFindAtom( LPCSTR );

UINT    WINAPI GlobalFlags( HGLOBAL );

HGLOBAL WINAPI GlobalFree( HGLOBAL );

UINT    WINAPI GlobalGetAtomName( ATOM, LPSTR, int );

HGLOBAL WINAPI GlobalHandle( PVOID memPtr );

PVOID   WINAPI GlobalLock( HGLOBAL hglbMem );

VOID    WINAPI GlobalMemoryStatus( PMEMORYSTATUS );

HGLOBAL WINAPI GlobalReAlloc( HGLOBAL, DWORD, UINT );

DWORD   WINAPI GlobalSize( HGLOBAL );

BOOL    WINAPI GlobalUnlock( HGLOBAL hglbMem );

PVOID   WINAPI HeapAlloc( HANDLE, DWORD, DWORD );

HANDLE  WINAPI HeapCreate( DWORD, DWORD, DWORD );

BOOL    WINAPI HeapDestroy( HANDLE );

BOOL    WINAPI HeapFree( HANDLE, DWORD, PVOID );

PVOID   WINAPI HeapReAlloc( HANDLE, DWORD, PVOID, DWORD );

DWORD   WINAPI HeapSize( HANDLE, DWORD, PVOID );

BOOL    WINAPI HideCaret( HWND );

BOOL    WINAPI HiliteMenuItem( HWND, HMENU, UINT, UINT );

BOOL    WINAPI InflateRect( PRECT, int, int );

BOOL    WINAPI InSendMessage( VOID );

BOOL    WINAPI InitAtomTable( DWORD );

VOID    WINAPI InitializeCriticalSection( PCRITICAL_SECTION );

BOOL    WINAPI InsertMenu( HMENU, UINT, UINT, UINT, LPCSTR );

LONG    WINAPI InterlockedDecrement( PLONG );

LONG    WINAPI InterlockedExchange( PLONG, LONG );

LONG    WINAPI InterlockedIncrement( PLONG );

int       WINAPI IntersectClipRect( HDC, int, int, int, int );

BOOL    WINAPI IntersectRect( PRECT, const RECT * rect1, const RECT * rect2);

BOOL    WINAPI InvalidateRect( HWND, const RECT *, BOOL );

BOOL    WINAPI InvalidateRgn( HWND, HRGN, BOOL );

BOOL    WINAPI InvertRect( HDC, const RECT *);

BOOL    WINAPI InvertRgn( HDC, HRGN );

BOOL    WINAPI IsBadCodePtr( FARPROC );

BOOL    WINAPI IsBadReadPtr( const VOID *, UINT );

BOOL    WINAPI IsBadStringPtr( LPCSTR, UINT );

BOOL    WINAPI IsBadWritePtr( PVOID, UINT );

BOOL    WINAPI IsChild( HWND, HWND );

BOOL    WINAPI IsClipboardFormatAvailable( UINT );

BOOL    WINAPI IsDBCSLeadByte( BYTE );

BOOL    WINAPI IsDialogMessage( HWND, PMSG );

BOOL    WINAPI IsDlgButtonChecked( HWND, int );

BOOL    WINAPI IsIconic( HWND );

BOOL    WINAPI IsMenu( HMENU );

BOOL    WINAPI IsRectEmpty( const RECT *);

BOOL    WINAPI IsWindow( HWND );

BOOL    WINAPI IsWindowEnabled( HWND );

BOOL    WINAPI IsWindowVisible( HWND );

BOOL    WINAPI IsZoomed( HWND );

BOOL    WINAPI KillTimer(HWND, UINT );

BOOL    WINAPI LPtoDP( HDC, PPOINT, int );

VOID    WINAPI LeaveCriticalSection( PCRITICAL_SECTION );

BOOL    WINAPI LineDDA( int, int, int, int, LINEDDAPROC, LPARAM );

 BOOL   WINAPI LineTo( HDC, int, int );

HACCEL  WINAPI LoadAccelerators( HINSTANCE, LPCSTR );

HBITMAP WINAPI LoadBitmap( HINSTANCE, LPCSTR );

HCURSOR WINAPI LoadCursor( HINSTANCE, LPCSTR );

HICON   WINAPI LoadIcon( HINSTANCE, LPCSTR );

HINSTANCE WINAPI LoadLibrary( LPCSTR );

HMENU   WINAPI LoadMenu( HINSTANCE, LPCSTR );

HMENU   WINAPI LoadMenuIndirect( const MENUITEMTEMPLATEHEADER * );

DWORD   WINAPI LoadModule( LPCSTR, PVOID );

HGLOBAL WINAPI LoadResource( HINSTANCE, HRSRC );

PVOID   WINAPI LockResource( HGLOBAL );

int       WINAPI LoadString( HINSTANCE, UINT, LPSTR, int );

HLOCAL  WINAPI LocalAlloc( UINT, UINT );

HLOCAL  WINAPI LocalDiscard( HLOCAL );

UINT    WINAPI LocalFlags( HLOCAL );

BOOL    WINAPI LocalFileTimeToFileTime( const FILETIME *, PFILETIME );

HLOCAL  WINAPI LocalFree( HLOCAL );

HLOCAL  WINAPI LocalHandle( PVOID lpvMem );

BOOL    WINAPI LocalUnlock( HLOCAL hlocal);

HLOCAL  WINAPI LocalReAlloc( HLOCAL, UINT, UINT );

UINT    WINAPI LocalSize( HLOCAL );

PVOID   WINAPI LocalLock( HLOCAL hlocal );

BOOL    WINAPI LockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    WINAPI LockWindowUpdate( HWND );

BOOL    WINAPI MapDialogRect( HWND, PRECT );

UINT    WINAPI MapVirtualKey( UINT, UINT );

int       WINAPI MapWindowPoints( HWND, HWND, PPOINT, int );

BOOL    WINAPI MaskBlt( HDC, int, int, int, int, HDC  hdcSrc, int, int, HBITMAP, int, int, DWORD );

int       WINAPI MessageBox( HWND hwnd , LPCSTR, LPCSTR, UINT );

BOOL    WINAPI MessageBeep( UINT );

DWORD   WINAPI MsgWaitForMultipleObjects( DWORD, LPHANDLE, BOOL, DWORD, DWORD );

BOOL    WINAPI ModifyMenu( HMENU, UINT, UINT, UINT, LPCSTR );

BOOL    WINAPI ModifyWorldTransform( HDC, PXFORM, DWORD );

BOOL    WINAPI MoveFile( LPCSTR, LPCSTR );

BOOL    WINAPI MoveToEx( HDC, int, int, PPOINT );

BOOL    WINAPI MoveWindow( HWND, int, int, int, int, BOOL );

int       WINAPI MulDiv(int, int, int );

int       WINAPI OffsetClipRgn( HDC, int, int );

BOOL    WINAPI OffsetRect( PRECT, int, int );

int       WINAPI OffsetRgn( HRGN, int, int );

BOOL    WINAPI OffsetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    WINAPI OffsetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    WINAPI OpenClipboard( HWND );

HANDLE  WINAPI OpenEvent( DWORD, BOOL, LPCSTR );

HFILE   WINAPI OpenFile( LPCSTR, POFSTRUCT, UINT );

HANDLE  WINAPI OpenMutex( DWORD, BOOL, LPCSTR );

HANDLE  WINAPI OpenProcess( DWORD, BOOL, DWORD );

HANDLE  WINAPI OpenSemaphore( DWORD, BOOL, LPCSTR );

VOID    WINAPI OutputDebugString( LPCSTR );

LONG    WINAPI PackDDElParam(UINT, UINT, UINT );

BOOL    WINAPI PaintRgn( HDC, HRGN );

BOOL    WINAPI PatBlt( HDC, int, int, int, int, DWORD );

HRGN    WINAPI PathToRegion( HDC);

BOOL    WINAPI PeekMessage( PMSG, HWND, UINT, UINT, UINT );

BOOL    WINAPI Pie( HDC, int, int, int, int, int, int, int, int );

BOOL    WINAPI PlayEnhMetaFile( HDC, HENHMETAFILE, const RECT * );

BOOL    WINAPI PlayMetaFile( HDC, HMETAFILE );

BOOL    WINAPI PlayMetaFileRecord( HDC, PHANDLETABLE, PMETARECORD, int );

BOOL    WINAPI PolyBezier( HDC, const POINT *, int );

BOOL    WINAPI PolyBezierTo( HDC, const POINT *, DWORD );

BOOL    WINAPI PolyDraw( HDC, const POINT *, const BYTE *, int );

BOOL    WINAPI Polygon( HDC, const POINT *, int );

BOOL    WINAPI Polyline( HDC, const POINT *, int );

BOOL    WINAPI PolylineTo( HDC, const POINT *, DWORD );

BOOL    WINAPI PolyPolygon( HDC, const POINT *, const int *, int );

BOOL    WINAPI PolyPolyline( HDC, const POINT *, const DWORD *, DWORD );

VOID    WINAPI PostQuitMessage( int );

BOOL    WINAPI PostMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

BOOL    WINAPI PostThreadMessage( DWORD, UINT, WPARAM, LPARAM );

BOOL    WINAPI PtInRect( const RECT *, POINT );

BOOL    WINAPI PtInRegion( HRGN, int, int );

BOOL    WINAPI PtVisible( HDC, int, int );

BOOL    WINAPI PrintDlg( PPRINTDLG );

BOOL    WINAPI PulseEvent ( HANDLE );

BOOL    WINAPI ReadFile( HANDLE, PVOID, DWORD, PDWORD, POVERLAPPED );

UINT    WINAPI RealizePalette( HDC);

BOOL    WINAPI RectInRegion( HRGN, const RECT * );

BOOL    WINAPI RectVisible( HDC, const RECT *);

BOOL    WINAPI Rectangle( HDC, int, int, int, int );

BOOL    WINAPI RedrawWindow( HWND, const RECT *, HRGN, DWORD );

LONG    WINAPI   RegCloseKey( HKEY );

LONG    WINAPI    RegCreateKey( HKEY, LPCTSTR, PHKEY );

LONG    WINAPI    RegCreateKeyEx( HKEY, LPCTSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, PDWORD );

LONG    WINAPI    RegDeleteKey( HKEY, LPCTSTR );

LONG    WINAPI    RegDeleteValue( HKEY, LPSTR );

LONG    WINAPI   RegEnumKey( HKEY, DWORD, LPSTR, DWORD );

LONG    WINAPI   RegEnumKeyEx( HKEY, DWORD, LPSTR, PDWORD, PDWORD, LPSTR, PDWORD, PFILETIME );

LONG    WINAPI   RegEnumValue( HKEY, DWORD, LPSTR, PDWORD, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    WINAPI   RegOpenKey( HKEY, LPCTSTR, PHKEY );

LONG    WINAPI   RegOpenKeyEx( HKEY, LPCTSTR, DWORD, REGSAM, PHKEY );

LONG    WINAPI   RegQueryInfoKey( HKEY, LPSTR, PDWORD, PDWORD, PDWORD, PDWORD,
                                      PDWORD, PDWORD, PDWORD, PDWORD, PDWORD, PFILETIME );

LONG    WINAPI   RegQueryValue( HKEY, LPCTSTR, LPSTR, PLONG );

LONG    WINAPI   RegQueryValueEx( HKEY, LPCTSTR, PDWORD, PDWORD, LPBYTE, PDWORD );

LONG    WINAPI   RegSetValue( HKEY, LPCTSTR, DWORD, LPCTSTR, DWORD );

LONG    WINAPI   RegSetValueEx( HKEY, LPCTSTR, DWORD, DWORD, const BYTE *, DWORD );

WORD    WINAPI RegisterClass( const WNDCLASS *);

UINT    WINAPI RegisterClipboardFormat( LPCSTR );

UINT    WINAPI RegisterWindowMessage( LPCSTR );

BOOL    WINAPI ReleaseCapture( void );

int       WINAPI ReleaseDC( HWND, HDC );

BOOL    WINAPI ReleaseMutex( HANDLE );

BOOL    WINAPI ReleaseSemaphore( HANDLE, LONG, PLONG );

BOOL    WINAPI RemoveDirectory( LPCSTR );

BOOL    WINAPI RemoveFontResource( LPCSTR );

BOOL    WINAPI RemoveMenu( HMENU, UINT, UINT );

HANDLE  WINAPI RemoveProp( HWND, LPCSTR );

HWND    WINAPI ReplaceText( PFINDREPLACE );

BOOL    WINAPI ReplyMessage( LRESULT );

BOOL    WINAPI ResetDC( HDC, const DEVMODE * );

BOOL    WINAPI ResetEvent( HANDLE );

BOOL    WINAPI ResizePalette( HPALETTE, UINT );

BOOL    WINAPI RestoreDC( HDC, int );

DWORD   WINAPI ResumeThread( HANDLE );

LONG    WINAPI ReuseDDElParam( LONG, UINT, UINT, UINT, UINT );

BOOL    WINAPI RoundRect( HDC, int, int, int, int, int, int );

int       WINAPI SaveDC( HDC );

BOOL    WINAPI ScaleViewportExtEx( HDC, int, int, int, int, PSIZE );

BOOL    WINAPI ScaleWindowExtEx( HDC, int, int, int, int, PSIZE );

BOOL    WINAPI ScreenToClient( HWND, PPOINT );

BOOL    WINAPI ScrollDC( HDC, int, int, const RECT *, const RECT *, HRGN, PRECT );

BOOL    WINAPI ScrollWindow( HWND, int, int, const RECT *, const RECT * );

BOOL    WINAPI ScrollWindowEx( HWND, int, int, const RECT *, const RECT *, HRGN, PRECT, UINT );

DWORD   WINAPI SearchPath( LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPSTR *);

int       WINAPI SelectClipRgn( HDC, HRGN );

HGDIOBJ WINAPI SelectObject( HDC, HGDIOBJ );

HPALETTE WINAPI SelectPalette( HDC, HPALETTE, BOOL );

LONG    WINAPI SendDlgItemMessage( HWND, int, UINT, WPARAM, LPARAM );

LRESULT WINAPI SendMessage( HWND, UINT, WPARAM mp1, LPARAM mp2 );

HWND    WINAPI SetActiveWindow( HWND );

int       WINAPI SetArcDirection( HDC, int );

LONG    WINAPI SetBitmapBits( HBITMAP, DWORD, const VOID * );

BOOL    WINAPI SetBitmapDimensionEx( HBITMAP, int, int, PSIZE );

COLORREF WINAPI SetBkColor( HDC, COLORREF );

int       WINAPI SetBkMode( HDC, int );

LONG    WINAPI SetBoundsRect( HDC, const RECT *, UINT );

BOOL    WINAPI SetBrushOrgEx( HDC, int, int, PPOINT );

HWND    WINAPI SetCapture( HWND );

BOOL    WINAPI SetCaretBlinkTime( UINT );

BOOL    WINAPI SetCaretPos( int, int );

DWORD   WINAPI SetClassLong( HWND, int, LONG );

WORD    WINAPI SetClassWord( HWND, int, WORD );

HANDLE  WINAPI SetClipboardData( UINT, HANDLE );

HWND    WINAPI SetClipboardViewer( HWND );

BOOL    WINAPI SetCurrentDirectory(  LPSTR );

HCURSOR WINAPI SetCursor( HCURSOR );

BOOL    WINAPI SetCursorPos( int, int );

int       WINAPI SetDIBits( HDC, HBITMAP, UINT, UINT, const VOID *, const BITMAPINFO *, UINT );

int       WINAPI SetDIBitsToDevice( HDC, int, int, int, int, int, int, UINT, UINT, PVOID, PBITMAPINFO, UINT );

BOOL    WINAPI SetDlgItemInt( HWND, int, UINT, BOOL );

BOOL    WINAPI SetDlgItemText( HWND, int, LPCSTR );

BOOL    WINAPI SetDoubleClickTime( UINT );

BOOL    WINAPI SetEndOfFile( HANDLE );

HENHMETAFILE WINAPI SetEnhMetaFileBits( UINT, const BYTE * );

BOOL    WINAPI SetEnvironmentVariable( LPCSTR, LPCSTR );

BOOL    WINAPI SetEvent( HANDLE );

BOOL    WINAPI SetFileAttributes( LPCSTR, DWORD );

DWORD   WINAPI SetFilePointer( HANDLE, LONG, PLONG, DWORD );

BOOL    WINAPI SetFileTime( HANDLE, const FILETIME *, const FILETIME *, const FILETIME * );

HWND    WINAPI SetFocus( HWND );

BOOL    WINAPI SetForegroundWindow( HWND );

int       WINAPI SetGraphicsMode(HDC, int );

UINT    WINAPI SetHandleCount( UINT );

VOID    WINAPI SetLastError( DWORD );

BOOL    WINAPI SetLocalTime( const SYSTEMTIME * );

int       WINAPI SetMapMode( HDC, int );

DWORD   WINAPI SetMapperFlags( HDC, DWORD );

BOOL    WINAPI SetMenu( HWND, HMENU );

BOOL    WINAPI SetMenuItemBitmaps( HMENU, UINT, UINT, HBITMAP, HBITMAP );

HMETAFILE WINAPI SetMetaFileBitsEx( UINT, PBYTE );

BOOL    WINAPI SetMiterLimit( HDC, float limit, float * );

UINT    WINAPI SetPaletteEntries( HPALETTE, UINT, UINT, const PALETTEENTRY *);

HWND    WINAPI SetParent( HWND, HWND );

COLORREF
          WINAPI SetPixel( HDC, int, int, COLORREF );

int       WINAPI SetPolyFillMode( HDC, int );

BOOL    WINAPI SetPriorityClass( HANDLE, DWORD );

BOOL    WINAPI SetProp( HWND, LPCSTR, HANDLE );

BOOL    WINAPI SetRect( PRECT, int, int, int, int );

BOOL    WINAPI SetRectEmpty( PRECT );

BOOL    WINAPI SetRectRgn( HRGN, int, int, int, int );

int       WINAPI SetROP2( HDC, int );

int       WINAPI SetScrollPos( HWND, int, int, BOOL );

BOOL    WINAPI SetScrollRange( HWND, int, int, int, BOOL );

BOOL    WINAPI SetStdHandle( DWORD, HANDLE );

int       WINAPI SetStretchBltMode( HDC, int );

UINT    WINAPI SetSysColors( int, const int *, const COLORREF * );

BOOL    WINAPI SetSystemTime( const SYSTEMTIME * );

BOOL    WINAPI SetTimeZoneInformation( const TIME_ZONE_INFORMATION * );

UINT    WINAPI SetTextAlign( HDC, UINT );

int       WINAPI SetTextCharacterExtra( HDC, int );

COLORREF WINAPI SetTextColor( HDC, COLORREF );

BOOL    WINAPI SetTextJustification( HDC, int, int );

BOOL    WINAPI SetThreadPriority( HANDLE, int );

UINT    WINAPI SetTimer( HWND, UINT, UINT, TIMERPROC );

BOOL    WINAPI SetViewportExtEx( HDC, int, int, PSIZE );

BOOL    WINAPI SetViewportOrgEx( HDC, int, int, PPOINT );

BOOL    WINAPI SetVolumeLabel( LPCSTR, LPCSTR );

BOOL    WINAPI SetWindowExtEx( HDC, int, int, PSIZE );

LONG    WINAPI SetWindowLong( HWND, int, LONG );

BOOL    WINAPI SetWindowOrgEx( HDC, int, int, PPOINT );

BOOL    WINAPI SetWindowPlacement( HWND, const WINDOWPLACEMENT * );

BOOL    WINAPI SetWindowPos( HWND, HWND, int, int, int, int, UINT );

HHOOK   WINAPI SetWindowsHookEx( int, HOOKPROC, HINSTANCE, DWORD );

BOOL    WINAPI SetWindowText( HWND, LPCSTR );

WORD    WINAPI SetWindowWord( HWND, int, WORD );

HENHMETAFILE WINAPI SetWinMetaFileBits( UINT, const BYTE *, HDC, const METAFILEPICT * );

BOOL    WINAPI SetWorldTransform( HDC, PXFORM );

BOOL    WINAPI ShowCaret( HWND );

int       WINAPI ShowCursor( BOOL );

BOOL    WINAPI ShowOwnedPopups( HWND, BOOL );

BOOL    WINAPI ShowScrollBar( HWND, int, BOOL );

BOOL    WINAPI ShowWindow( HWND, int );

DWORD   WINAPI SizeofResource( HINSTANCE, HRSRC );

VOID    WINAPI Sleep( DWORD );

int       WINAPI StartDoc( HDC, PDOCINFO );

int       WINAPI StartPage( HDC );

int       WINAPI StretchDIBits( HDC, int, int, int, int, int, int, int, int, void *, PBITMAPINFO, UINT, DWORD );

BOOL    WINAPI StretchBlt( HDC, int, int, int, int, HDC, int, int, int, int,  DWORD );

BOOL    WINAPI StrokeAndFillPath( HDC );

BOOL    WINAPI StrokePath( HDC );

BOOL    WINAPI SubtractRect( PRECT, const RECT *, const RECT *);

DWORD   WINAPI SuspendThread( HANDLE );

BOOL    WINAPI SwapMouseButton( BOOL );

UINT    WINAPI SystemParametersInfo( UINT, UINT, PVOID, UINT );

BOOL    WINAPI SystemTimeToFileTime( const SYSTEMTIME *, PFILETIME );

BOOL    WINAPI SystemTimeToTzSpecificLocalTime( PTIME_ZONE_INFORMATION, PSYSTEMTIME, PSYSTEMTIME );

LONG    WINAPI TabbedTextOut( HDC, int, int, LPCSTR, int, int, int *, int );

BOOL    WINAPI TerminateProcess( HANDLE, UINT );

BOOL    WINAPI TerminateThread( HANDLE, DWORD );

BOOL    WINAPI TextOut( HDC, int, int, LPCSTR, int );

DWORD   WINAPI TlsAlloc( VOID );

BOOL    WINAPI TlsFree( DWORD );

PVOID   WINAPI TlsGetValue( DWORD );

BOOL    WINAPI TlsSetValue( DWORD, PVOID );

BOOL    WINAPI TrackPopupMenu( HMENU, UINT, int, int, int, HWND, const RECT * );

int       WINAPI TranslateAccelerator( HWND, HACCEL, PMSG );

BOOL    WINAPI TranslateMDISysAccel( HWND, PMSG );

BOOL    WINAPI TranslateMessage( const MSG * );

BOOL    WINAPI UnhookWindowsHookEx( HHOOK );

BOOL    WINAPI UnionRect( PRECT, const RECT *, const RECT * );

BOOL    WINAPI UnlockFile( HANDLE, DWORD, DWORD, DWORD, DWORD );

BOOL    WINAPI UnpackDDElParam(UINT, LONG, PUINT, PUINT );

BOOL    WINAPI UnrealizeObject( HGDIOBJ );

BOOL    WINAPI UnregisterClass( LPCSTR, HINSTANCE );

BOOL    WINAPI UpdateWindow( HWND );

BOOL    WINAPI ValidateRect( HWND, const RECT *);

BOOL    WINAPI ValidateRgn( HWND, HRGN );

SHORT   WINAPI VkKeyScan( char keyScan);

DWORD   WINAPI WaitForMultipleObjects( DWORD, const HANDLE *, BOOL, DWORD );

DWORD   WINAPI WaitForSingleObject( HANDLE, DWORD );

BOOL    WINAPI WaitMessage( void);

BOOL    WINAPI WidenPath( HDC);

UINT    WINAPI WinExec( LPCSTR, UINT );

BOOL    WINAPI WinHelp( HWND, LPCSTR, UINT, DWORD );

HWND    WINAPI WindowFromPoint( POINT );

HWND    WINAPI WindowFromDC( HDC );

BOOL    WINAPI WriteFile( HANDLE, const VOID *, DWORD, PDWORD, POVERLAPPED );

BOOL    WINAPI WritePrivateProfileString( LPCSTR, LPCSTR, LPCSTR, LPCSTR );

BOOL    WINAPI WriteProfileString( LPCSTR, LPCSTR, LPCSTR );

VOID    WINAPI ZeroMemory( PVOID, DWORD );

HFILE   WINAPI _lclose( HFILE );

HFILE   WINAPI _lcreat( LPCSTR, int );

HFILE   WINAPI _lopen( LPCSTR, int );

UINT    WINAPI _lread( HFILE, PVOID, UINT );

LONG    WINAPI _llseek( HFILE, LONG, int );

UINT    WINAPI _lwrite( HFILE, const VOID *, UINT );

MMRESULT WINAPI timeGetSystemTime( PMMTIME, UINT );

DWORD   WINAPI timeGetTime( VOID );

int       WINAPI wsprintf( LPSTR, LPCSTR, ... );

int       WINAPI wvsprintf( LPSTR, LPCSTR, const VOID *);

LPSTR   WINAPI CharNext( LPCSTR );

LPSTR   WINAPI CharPrev( LPCSTR, LPCSTR );

HWND    WINAPI GetDesktopWindow( VOID );

BOOL    WINAPI CharToOem( LPCSTR, LPSTR );

BOOL    WINAPI OemToChar( LPCSTR, LPSTR );

BOOL    WINAPI CharToOemBuff( LPCSTR, LPSTR, DWORD );

BOOL    WINAPI OemToCharBuff( LPCSTR, LPSTR, DWORD );

BOOL    WINAPI IsBadHugeReadPtr( const void *, UINT );

BOOL    WINAPI IsBadHugeWritePtr( PVOID, UINT );

BOOL    WINAPI FloodFill(HDC, int, int, COLORREF );

BOOL    WINAPI IsCharAlpha( wchar_t );

BOOL    WINAPI IsCharAlphaNumeric( wchar_t );

BOOL    WINAPI IsCharLower( wchar_t );

BOOL    WINAPI IsCharUpper( wchar_t );

LPSTR   WINAPI lstrcat( LPSTR, LPCSTR );

int       WINAPI lstrcmp( LPCSTR, LPCSTR );

int       WINAPI lstrcmpi( LPCSTR, LPCSTR );

LPSTR   WINAPI lstrcpy( LPSTR, LPCSTR );

int       WINAPI lstrlen( LPCSTR );

/* -----------------------------------------------------------------*/


int       WINAPI WinCallWinMain(int, char *[], FNMAIN, int);

/* The following functions are used for translating data between the native
 * Presentation Manager format and the Developer API Extensions equivalents.
 */
typedef enum { WINX2PM, PM2WINX} XLATEDIR;

/* Use the GDI object type to specify the type of handle passing in
 */
BOOL    WINAPI WinTranslateDevicePoints( HDC, HWND, PPOINT, INT, XLATEDIR );
BOOL    WINAPI WinTranslateDeviceRects ( HDC, HWND, PRECT,  INT, XLATEDIR );
ULONG   WINAPI WinTranslateGraphicsObjectHandle( HGDIOBJ, XLATEDIR, ULONG );
ULONG   WINAPI WinTranslateMnemonicString( LPCSTR, LPSTR *, ULONG, XLATEDIR );
DWORD   WINAPI WinQueryTranslateMode( VOID );
BOOL    WINAPI WinSetTranslateMode( DWORD );



#pragma pack()

#endif    /* ifdef __PMWINX_H__ */
