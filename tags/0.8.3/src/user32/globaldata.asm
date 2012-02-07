;; @file
; Global DLL data for USER32.DLL.
;
; Note: sizes of data variables must be kept in sync with their C declarations!
;

.386
                NAME    globaldata

SHARED_DGROUP   GROUP _GLOBALDATA

_GLOBALDATA     SEGMENT DWORD PUBLIC USE32 'DATA'

;
; win32wndhandle.cpp:
;
                PUBLIC _WindowHandleTable
                PUBLIC _globalwhandlecritsect
                PUBLIC _lastIndex

MAX_WINDOW_HANDLES      = 1024

; extern ULONG                WindowHandleTable[MAX_WINDOW_HANDLES]; // = {0}
_WindowHandleTable      DD MAX_WINDOW_HANDLES DUP (0)
; extern CRITICAL_SECTION_OS2 globalwhandlecritsect; // = {0}
_globalwhandlecritsect  DD 6 DUP (0)
; extern ULONG                lastIndex; // = 0
_lastIndex              DD 0

;
; hook.cpp:
;
IFDEF GLOBAL_HOOKS

                PUBLIC _HOOK_systemHooks

WH_MIN                  = -1
WH_MAX                  = 14
WH_NB_HOOKS             = (WH_MAX-WH_MIN+1)

; extern HANDLE HOOK_systemHooks[WH_NB_HOOKS]; // = { 0 }
_HOOK_systemHooks       DD WH_NB_HOOKS DUP (0)

ENDIF

;
; caret.cpp:
;
                PUBLIC _hwndCaret
                PUBLIC _hbmCaret
                PUBLIC _CaretWidth, _CaretHeight
                PUBLIC _CaretPosX, _CaretPosY
                PUBLIC _CaretIsVisible

; extern HWND hwndCaret; // = 0
_hwndCaret      DD 0
; extern HBITMAP hbmCaret; // = 0
_hbmCaret       DD 0
; extern int CaretWidth, CaretHeight; // = 0
_CaretWidth     DD 0
_CaretHeight    DD 0
; extern int CaretPosX, CaretPosY; // = 0
_CaretPosX      DD 0
_CaretPosY      DD 0
; extern INT CaretIsVisible; // =0, visible if > 0
_CaretIsVisible DD 0

_GLOBALDATA     ENDS

                END

