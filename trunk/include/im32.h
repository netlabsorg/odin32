#ifndef __IM32_H__
#define __IM32_H__

#include <win32type.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WINE_IMM_H

#define LF_FACESIZE 32  // from win/wingdi.h

typedef struct
{
    INT  lfHeight;
    INT  lfWidth;
    INT  lfEscapement;
    INT  lfOrientation;
    INT  lfWeight;
    BYTE   lfItalic;
    BYTE   lfUnderline;
    BYTE   lfStrikeOut;
    BYTE   lfCharSet;
    BYTE   lfOutPrecision;
    BYTE   lfClipPrecision;
    BYTE   lfQuality;
    BYTE   lfPitchAndFamily;
    WCHAR  lfFaceName[LF_FACESIZE];
} LOGFONTW, *PLOGFONTW, *LPLOGFONTW;    // from win/wingdi.h

typedef HANDLE HIMC;
typedef HANDLE HIMCC;
typedef HANDLE HKL;
typedef HKL *LPHKL;

typedef int (* CALLBACK REGISTERWORDENUMPROCA)(LPCSTR, DWORD, LPCSTR, LPVOID);
typedef int (* CALLBACK REGISTERWORDENUMPROCW)(LPCWSTR, DWORD, LPCWSTR, LPVOID);

typedef struct tagCANDIDATEFORM
{
                DWORD   dwIndex;
                DWORD   dwStyle;
                POINT ptCurrentPos;
                RECT  rcArea;
} CANDIDATEFORM, *LPCANDIDATEFORM;

typedef struct tagCANDIDATELIST
{
                DWORD dwSize;
                DWORD dwStyle;
                DWORD dwCount;
                DWORD dwSelection;
                DWORD dwPageStart;
                DWORD dwPageSize;
                DWORD dwOffset[1];
} CANDIDATELIST, *LPCANDIDATELIST;

typedef struct tagSTYLEBUFA
{
                DWORD dwStyle;
                CHAR  szDescription[32];
} STYLEBUFA, *LPSTYLEBUFA;

typedef struct tagSTYLEBUFW
{
                DWORD dwStyle;
                WCHAR  szDescription[32];
} STYLEBUFW, *LPSTYLEBUFW;

typedef struct _tagCOMPOSITIONFORM
{
                DWORD   dwStyle;
                POINT ptCurrentPos;
                RECT  rcArea;
} COMPOSITIONFORM, *LPCOMPOSITIONFORM;

/* wParam for WM_IME_CONTROL */
#define IMC_GETCANDIDATEPOS_W             0x0007
#define IMC_SETCANDIDATEPOS_W             0x0008
#define IMC_GETCOMPOSITIONFONT_W          0x0009
#define IMC_SETCOMPOSITIONFONT_W          0x000A
#define IMC_GETCOMPOSITIONWINDOW_W        0x000B
#define IMC_SETCOMPOSITIONWINDOW_W        0x000C
#define IMC_GETSTATUSWINDOWPOS_W          0x000F
#define IMC_SETSTATUSWINDOWPOS_W          0x0010
#define IMC_CLOSESTATUSWINDOW_W           0x0021
#define IMC_OPENSTATUSWINDOW_W            0x0022

/*
 * wParam for WM_IME_CONTROL to the soft keyboard
 * dwAction for ImmNotifyIME
 */
#define NI_OPENCANDIDATE_W                0x0010
#define NI_CLOSECANDIDATE_W               0x0011
#define NI_SELECTCANDIDATESTR_W           0x0012
#define NI_CHANGECANDIDATELIST_W          0x0013
#define NI_FINALIZECONVERSIONRESULT_W     0x0014
#define NI_COMPOSITIONSTR_W               0x0015
#define NI_SETCANDIDATE_PAGESTART_W       0x0016
#define NI_SETCANDIDATE_PAGESIZE_W        0x0017

/* lParam for WM_IME_SETCONTEXT */
#define ISC_SHOWUICANDIDATEWINDOW_W       0x00000001
#define ISC_SHOWUICOMPOSITIONWINDOW_W     0x80000000
#define ISC_SHOWUIGUIDELINE_W             0x40000000
#define ISC_SHOWUIALLCANDIDATEWINDOW_W    0x0000000F
#define ISC_SHOWUIALL_W                   0xC000000F


/* dwIndex for ImmNotifyIME/NI_COMPOSITIONSTR */
#define CPS_COMPLETE_W                    0x0001
#define CPS_CONVERT_W                     0x0002
#define CPS_REVERT_W                      0x0003
#define CPS_CANCEL_W                      0x0004


/* the modifiers of hot key */
#define MOD_ALT_W                         0x0001
#define MOD_CONTROL_W                     0x0002
#define MOD_SHIFT_W                       0x0004

#define MOD_LEFT_W                        0x8000
#define MOD_RIGHT_W                       0x4000

#define MOD_ON_KEYUP_W                    0x0800
#define MOD_IGNORE_ALL_MODIFIER_W         0x0400


/* Windows for Simplified Chinese Edition hot key ID from 0x10 - 0x2F */
#define IME_CHOTKEY_IME_NONIME_TOGGLE_W           0x10
#define IME_CHOTKEY_SHAPE_TOGGLE_W                0x11
#define IME_CHOTKEY_SYMBOL_TOGGLE_W               0x12

/* Windows for Japanese Edition hot key ID from 0x30 - 0x4F */
#define IME_JHOTKEY_CLOSE_OPEN_W                  0x30

/* Windows for Korean Edition hot key ID from 0x50 - 0x6F */
#define IME_KHOTKEY_SHAPE_TOGGLE_W                0x50
#define IME_KHOTKEY_HANJACONVERT_W                0x51
#define IME_KHOTKEY_ENGLISH_W                     0x52

/* Windows for Tranditional Chinese Edition hot key ID from 0x70 - 0x8F */
#define IME_THOTKEY_IME_NONIME_TOGGLE_W           0x70
#define IME_THOTKEY_SHAPE_TOGGLE_W                0x71
#define IME_THOTKEY_SYMBOL_TOGGLE_W               0x72

/* direct switch hot key ID from 0x100 - 0x11F */
#define IME_HOTKEY_DSWITCH_FIRST_W                0x100
#define IME_HOTKEY_DSWITCH_LAST_W                 0x11F

/* IME private hot key from 0x200 - 0x21F */
#define IME_HOTKEY_PRIVATE_FIRST_W                0x200
#define IME_ITHOTKEY_RESEND_RESULTSTR_W           0x200
#define IME_ITHOTKEY_PREVIOUS_COMPOSITION_W       0x201
#define IME_ITHOTKEY_UISTYLE_TOGGLE_W             0x202
#define IME_HOTKEY_PRIVATE_LAST_W                 0x21F


/*
 * dwSystemInfoFlags bits
 * parameter of ImmGetCompositionString
 */
#define GCS_COMPREADSTR_W                 0x0001
#define GCS_COMPREADATTR_W                0x0002
#define GCS_COMPREADCLAUSE_W              0x0004
#define GCS_COMPSTR_W                     0x0008
#define GCS_COMPATTR_W                    0x0010
#define GCS_COMPCLAUSE_W                  0x0020
#define GCS_CURSORPOS_W                   0x0080
#define GCS_DELTASTART_W                  0x0100
#define GCS_RESULTREADSTR_W               0x0200
#define GCS_RESULTREADCLAUSE_W            0x0400
#define GCS_RESULTSTR_W                   0x0800
#define GCS_RESULTCLAUSE_W                0x1000

/* style bit flags for WM_IME_COMPOSITION */
#define CS_INSERTCHAR_W                   0x2000
#define CS_NOMOVECARET_W                  0x4000


/*
 * bits of fdwInit of INPUTCONTEXT
 * IME version constants
 */
#define IMEVER_0310_W                     0x0003000A
#define IMEVER_0400_W                     0x00040000


/* IME property bits */
#define IME_PROP_AT_CARET_W               0x00010000
#define IME_PROP_SPECIAL_UI_W             0x00020000
#define IME_PROP_CANDLIST_START_FROM_1_W  0x00040000
#define IME_PROP_UNICODE_W                0x00080000


/* IME UICapability bits */
#define UI_CAP_2700_W                     0x00000001
#define UI_CAP_ROT90_W                    0x00000002
#define UI_CAP_ROTANY_W                   0x00000004
/* ImmSetCompositionString Capability bits */
#define SCS_CAP_COMPSTR_W                 0x00000001
#define SCS_CAP_MAKEREAD_W                0x00000002


/* IME WM_IME_SELECT inheritance Capability bits */
#define SELECT_CAP_CONVERSION_W           0x00000001
#define SELECT_CAP_SENTENCE_W             0x00000002


/* ID for deIndex of ImmGetGuideLine */
#define GGL_LEVEL_W                       0x00000001
#define GGL_INDEX_W                       0x00000002
#define GGL_STRING_W                      0x00000003
#define GGL_PRIVATE_W                     0x00000004

/* ID for dwLevel of GUIDELINE Structure */
#define GL_LEVEL_NOGUIDELINE_W            0x00000000
#define GL_LEVEL_FATAL_W                  0x00000001
#define GL_LEVEL_ERROR_W                  0x00000002
#define GL_LEVEL_WARNING_W                0x00000003
#define GL_LEVEL_INFORMATION_W            0x00000004

/* ID for dwIndex of GUIDELINE Structure */
#define GL_ID_UNKNOWN_W                   0x00000000
#define GL_ID_NOMODULE_W                  0x00000001
#define GL_ID_NODICTIONARY_W              0x00000010
#define GL_ID_CANNOTSAVE_W                0x00000011
#define GL_ID_NOCONVERT_W                 0x00000020
#define GL_ID_TYPINGERROR_W               0x00000021
#define GL_ID_TOOMANYSTROKE_W             0x00000022
#define GL_ID_READINGCONFLICT_W           0x00000023
#define GL_ID_INPUTREADING_W              0x00000024
#define GL_ID_INPUTRADICAL_W              0x00000025
#define GL_ID_INPUTCODE_W                 0x00000026
#define GL_ID_INPUTSYMBOL_W               0x00000027
#define GL_ID_CHOOSECANDIDATE_W           0x00000028
#define GL_ID_REVERSECONVERSION_W         0x00000029
#define GL_ID_PRIVATE_FIRST_W             0x00008000
#define GL_ID_PRIVATE_LAST_W              0x0000FFFF

/* ID for dwIndex of ImmGetProperty */
#define IGP_GETIMEVERSION_W               (DWORD)(-4)
#define IGP_PROPERTY_W                    0x00000004
#define IGP_CONVERSION_W                  0x00000008
#define IGP_SENTENCE_W                    0x0000000c
#define IGP_UI_W                          0x00000010
#define IGP_SETCOMPSTR_W                  0x00000014
#define IGP_SELECT_W                      0x00000018


/* dwIndex for ImmSetCompositionString API */
#define SCS_SETSTR_W          (GCS_COMPREADSTR_W|GCS_COMPSTR_W)
#define SCS_CHANGEATTR_W      (GCS_COMPREADATTR_W|GCS_COMPATTR_W)
#define SCS_CHANGECLAUSE_W    (GCS_COMPREADCLAUSE_W|GCS_COMPCLAUSE_W)

/* attribute for COMPOSITIONSTRING Structure */
#define ATTR_INPUT_W                      0x00
#define ATTR_TARGET_CONVERTED_W           0x01
#define ATTR_CONVERTED_W                  0x02
#define ATTR_TARGET_NOTCONVERTED_W        0x03
#define ATTR_INPUT_ERROR_W                0x04


/* bit field for IMC_SETCOMPOSITIONWINDOW, IMC_SETCANDIDATEWINDOW */
#define CFS_DEFAULT_W                     0x0000
#define CFS_RECT_W                        0x0001
#define CFS_POINT_W                       0x0002
#define CFS_FORCE_POSITION_W              0x0020
#define CFS_CANDIDATEPOS_W                0x0040
#define CFS_EXCLUDE_W                     0x0080

/* conversion direction for ImmGetConversionList */
#define GCL_CONVERSION_W                  0x0001
#define GCL_REVERSECONVERSION_W           0x0002
#define GCL_REVERSE_LENGTH_W              0x0003


/* bit field for conversion mode */
#define IME_CMODE_ALPHANUMERIC_W          0x0000
#define IME_CMODE_NATIVE_W                0x0001
#define IME_CMODE_CHINESE_W               IME_CMODE_NATIVE_W
/* IME_CMODE_HANGEUL is old name of IME_CMODE_HANGUL. It will be gone eventually. */
#define IME_CMODE_HANGEUL_W               IME_CMODE_NATIVE_W
#define IME_CMODE_HANGUL_W                IME_CMODE_NATIVE_W
#define IME_CMODE_JAPANESE_W              IME_CMODE_NATIVE_W
#define IME_CMODE_KATAKANA_W              0x0002  /* only effect under IME_CMODE_NATIVE */
#define IME_CMODE_LANGUAGE_W              0x0003
#define IME_CMODE_FULLSHAPE_W             0x0008
#define IME_CMODE_ROMAN_W                 0x0010
#define IME_CMODE_CHARCODE_W              0x0020
#define IME_CMODE_HANJACONVERT_W          0x0040
#define IME_CMODE_SOFTKBD_W               0x0080
#define IME_CMODE_NOCONVERSION_W          0x0100
#define IME_CMODE_EUDC_W                  0x0200
#define IME_CMODE_SYMBOL_W                0x0400


#define IME_SMODE_NONE_W                  0x0000
#define IME_SMODE_PLAURALCLAUSE_W         0x0001
#define IME_SMODE_SINGLECONVERT_W         0x0002
#define IME_SMODE_AUTOMATIC_W             0x0004
#define IME_SMODE_PHRASEPREDICT_W         0x0008

/* style of candidate */
#define IME_CAND_UNKNOWN_W                0x0000
#define IME_CAND_READ_W                   0x0001
#define IME_CAND_CODE_W                   0x0002
#define IME_CAND_MEANING_W                0x0003
#define IME_CAND_RADICAL_W                0x0004
#define IME_CAND_STROKE_W                 0x0005


/* wParam of report message WM_IME_NOTIFY */
#define IMN_CLOSESTATUSWINDOW_W           0x0001
#define IMN_OPENSTATUSWINDOW_W            0x0002
#define IMN_CHANGECANDIDATE_W             0x0003
#define IMN_CLOSECANDIDATE_W              0x0004
#define IMN_OPENCANDIDATE_W               0x0005
#define IMN_SETCONVERSIONMODE_W           0x0006
#define IMN_SETSENTENCEMODE_W             0x0007
#define IMN_SETOPENSTATUS_W               0x0008
#define IMN_SETCANDIDATEPOS_W             0x0009
#define IMN_SETCOMPOSITIONFONT_W          0x000A
#define IMN_SETCOMPOSITIONWINDOW_W        0x000B
#define IMN_SETSTATUSWINDOWPOS_W          0x000C
#define IMN_GUIDELINE_W                   0x000D
#define IMN_PRIVATE_W                     0x000E


/* error code of ImmGetCompositionString */
#define IMM_ERROR_NODATA_W                (-1)
#define IMM_ERROR_GENERAL_W               (-2)


/* dialog mode of ImmConfigureIME */
#define IME_CONFIG_GENERAL_W              1
#define IME_CONFIG_REGISTERWORD_W         2
#define IME_CONFIG_SELECTDICTIONARY_W     3

/* dialog mode of ImmEscape */
#define IME_ESC_QUERY_SUPPORT_W           0x0003
#define IME_ESC_RESERVED_FIRST_W          0x0004
#define IME_ESC_RESERVED_LAST_W           0x07FF
#define IME_ESC_PRIVATE_FIRST_W           0x0800
#define IME_ESC_PRIVATE_LAST_W            0x0FFF
#define IME_ESC_SEQUENCE_TO_INTERNAL_W    0x1001
#define IME_ESC_GET_EUDC_DICTIONARY_W     0x1003
#define IME_ESC_SET_EUDC_DICTIONARY_W     0x1004
#define IME_ESC_MAX_KEY_W                 0x1005
#define IME_ESC_IME_NAME_W                0x1006
#define IME_ESC_SYNC_HOTKEY_W             0x1007
#define IME_ESC_HANJA_MODE_W              0x1008
#define IME_ESC_AUTOMATA_W                0x1009
#define IME_ESC_PRIVATE_HOTKEY_W          0x100a


/* style of word registration */
#define IME_REGWORD_STYLE_EUDC_W          0x00000001
#define IME_REGWORD_STYLE_USER_FIRST_W    0x80000000
#define IME_REGWORD_STYLE_USER_LAST_W     0xFFFFFFFF

/*
 * type of soft keyboard
 * for Windows Tranditional Chinese Edition
 */
#define SOFTKEYBOARD_TYPE_T1_W            0x0001
/* for Windows Simplified Chinese Edition */
#define SOFTKEYBOARD_TYPE_C1_W            0x0002

typedef struct tagINPUTCONTEXT {
    HWND                hWnd;
    BOOL                fOpen;
    POINT               ptStatusWndPos;
    POINT               ptSoftKbdPos;
    DWORD               fdwConversion;
    DWORD               fdwSentence;
    union   {
        LOGFONTA        A;
        LOGFONTW        W;
    } lfFont;
    COMPOSITIONFORM     cfCompForm;
    CANDIDATEFORM       cfCandForm[4];
    HIMCC               hCompStr;
    HIMCC               hCandInfo;
    HIMCC               hGuideLine;
    HIMCC               hPrivate;
    DWORD               dwNumMsgBuf;
    HIMCC               hMsgBuf;
    DWORD               fdwInit;
    DWORD               dwReserve[3];
} INPUTCONTEXT, *PINPUTCONTEXT, *NPINPUTCONTEXT, *LPINPUTCONTEXT;


#define IMEMENUITEM_STRING_SIZE 80

typedef struct tagIMEMENUITEMINFOA {
    UINT        cbSize;
    UINT        fType;
    UINT        fState;
    UINT        wID;
    HBITMAP     hbmpChecked;
    HBITMAP     hbmpUnchecked;
    DWORD       dwItemData;
    CHAR        szString[IMEMENUITEM_STRING_SIZE];
    HBITMAP     hbmpItem;
} IMEMENUITEMINFOA, *PIMEMENUITEMINFOA, *NPIMEMENUITEMINFOA, *LPIMEMENUITEMINFOA;

typedef struct tagIMEMENUITEMINFOW {
    UINT        cbSize;
    UINT        fType;
    UINT        fState;
    UINT        wID;
    HBITMAP     hbmpChecked;
    HBITMAP     hbmpUnchecked;
    DWORD       dwItemData;
    WCHAR       szString[IMEMENUITEM_STRING_SIZE];
    HBITMAP     hbmpItem;
} IMEMENUITEMINFOW, *PIMEMENUITEMINFOW, *NPIMEMENUITEMINFOW, *LPIMEMENUITEMINFOW;

#endif // __WINE_IMM_H

BOOL IM32Init( VOID );
VOID IM32Term( VOID );

HIMC    IM32AssociateContext(HWND hWnd, HIMC hIMC);
BOOL    IM32AssociateContextEx(HWND hWnd, HIMC hIMC, DWORD dword);
BOOL    IM32ConfigureIME(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData, BOOL fUnicode );
HIMC    IM32CreateContext(VOID);
HIMCC   IM32CreateIMCC(DWORD dword);
HWND    IM32CreateSoftKeyboard(UINT uint, HWND hwnd, int in1, int in2);
BOOL    IM32DestroyContext(HIMC hIMC);
HIMCC   IM32DestroyIMCC(HIMCC himcc);
BOOL    IM32DestroySoftKeyboard(HWND hwnd);
BOOL    IM32DisableIME(DWORD dword);
UINT    IM32EnumRegisterWord( HKL hKL, LPVOID lpfnEnumProc, LPVOID lpszReading, DWORD dwStyle,  LPVOID lpszRegister, LPVOID lpData, BOOL fUnicode );
LRESULT IM32Escape(HKL hKL, HIMC hIMC,   UINT uEscape, LPVOID lpData, BOOL fUnicode);
BOOL    IM32GenerateMessage(HIMC himc);
DWORD   IM32GetCandidateList(HIMC hIMC, DWORD deIndex, LPCANDIDATELIST lpCandList, DWORD dwBufLen, BOOL fUnicode);
DWORD   IM32GetCandidateListCount( HIMC hIMC, LPDWORD lpdwListCount, BOOL fUnicode);
BOOL    IM32GetCandidateWindow(HIMC hIMC, DWORD dwBufLen, LPCANDIDATEFORM lpCandidate);
BOOL    IM32GetCompositionFont(HIMC hIMC, LPVOID lplf, BOOL fUnicode);
LONG    IM32GetCompositionString(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen, BOOL fUnicode);
BOOL    IM32GetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm);
HIMC    IM32GetContext(HWND hWnd);
DWORD   IM32GetConversionList( HKL hKL, HIMC hIMC, LPVOID pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag, BOOL fUnicode);
BOOL    IM32GetConversionStatus(HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence);
HWND    IM32GetDefaultIMEWnd(HWND hWnd);
UINT    IM32GetDescription(HKL hKL, LPVOID lpszDescription, UINT uBufLen, BOOL fUnicode);
DWORD   IM32GetGuideLine(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen, BOOL fUnicode);
BOOL    IM32GetHotKey(DWORD dword, LPUINT lpuModifiers, LPUINT lpuVKey, LPHKL lphkl);
DWORD   IM32GetIMCCLockCount(HIMCC himcc);
DWORD   IM32GetIMCCSize(HIMCC himcc);
DWORD   IM32GetIMCLockCount(HIMC himc);
UINT    IM32GetIMEFileName( HKL hKL, LPVOID lpszFileName, UINT uBufLen, BOOL fUnicode);
DWORD   IM32GetImeMenuItems(HIMC himc, DWORD dword, DWORD dword2, LPVOID pimen, LPVOID lpimen2, DWORD dword3, BOOL fUnicode);
BOOL    IM32GetOpenStatus(HIMC hIMC);
DWORD   IM32GetProperty(HKL hKL, DWORD fdwIndex);
UINT    IM32GetRegisterWordStyle(HKL hKL, UINT nItem, LPVOID lpStyleBuf, BOOL fUnicode);
BOOL    IM32GetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos);
UINT    IM32GetVirtualKey(HWND hWnd);
HKL     IM32InstallIME(LPVOID lpszIMEFileName, LPVOID lpszLayoutText, BOOL fUnicode);
BOOL    IM32IsIME(HKL hKL);
BOOL    IM32IsUIMessage(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode);
LPINPUTCONTEXT IM32LockIMC(HIMC himc);
LPVOID  IM32LockIMCC(HIMCC himcc);
BOOL    IM32NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue);
HIMCC   IM32ReSizeIMCC(HIMCC himcc, DWORD dword);
BOOL    IM32RegisterWord( HKL hKL, LPVOID lpszReading, DWORD dwStyle, LPVOID lpszRegister, BOOL fUnicode);
BOOL    IM32ReleaseContext(HWND hWnd, HIMC hIMC);
LRESULT IM32RequestMessage(HIMC himc, WPARAM wparam, LPARAM lparam, BOOL fUnicode);
BOOL    IM32SetCandidateWindow(HIMC hIMC, LPCANDIDATEFORM lpCandidate);
BOOL    IM32SetCompositionFont(HIMC hIMC, LPVOID lplf, BOOL fUnicode);
BOOL    IM32SetCompositionString(  HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen, BOOL fUnicode);
BOOL    IM32SetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm);
BOOL    IM32SetConversionStatus(HIMC hIMC, DWORD fdwConversion, DWORD fdwSentence);
BOOL    IM32SetHotKey(DWORD dword, UINT uint, UINT uint2, HKL hkl);
BOOL    IM32SetOpenStatus(HIMC hIMC, BOOL fOpen);
BOOL    IM32SetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos);
BOOL    IM32ShowSoftKeyboard(HWND hwnd, int in1);
BOOL    IM32SimulateHotKey(HWND hWnd, DWORD dwHotKeyID);
BOOL    IM32UnlockIMC(HIMC himc);
BOOL    IM32UnlockIMCC(HIMCC himcc);
BOOL    IM32UnregisterWord(HKL hKL, LPVOID lpszReading, DWORD dwStyle, LPVOID lpszUnregister, BOOL fUnicode);

#ifdef __cplusplus
}
#endif

#endif
