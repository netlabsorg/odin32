/* $Id: wndmsg.cpp,v 1.16 2001-10-08 23:24:43 phaller Exp $ */
/*
 * Win32 window message text function for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
#include <win\winmfcmsg.h>
#include <spy.h>

#define DBG_LOCALLOG	DBG_wndmsg
#include "dbglocal.h"

#ifdef DEBUG
typedef struct
{
    LPSTR pszMsg;
    UINT msg;
    UINT Flags;
} MSGDESC, *PMSGDESC;

//
// Message Table Flags
//
#define MTF_SELECTED                0x00000001

#define MTF_TYPE_MOUSE              0x00010000
#define MTF_TYPE_DDE                0x00020000
#define MTF_TYPE_CLIP               0x00040000
#define MTF_TYPE_NC                 0x00080000
#define MTF_TYPE_KEYBD              0x00100000
#define MTF_TYPE_LB                 0x00200000
#define MTF_TYPE_BM                 0x00400000
#define MTF_TYPE_STM                0x00800000
#define MTF_TYPE_CB                 0x01000000
#define MTF_TYPE_EM                 0x02000000
#ifdef FE_IME
#define MTF_TYPE_IME                0x04000000
#endif

//
// Message description table.  Describes each message that can be spied on.
// This table must be kept in sorted order.
//
MSGDESC gaMsgs[] =
{
    { "WM_NULL", WM_NULL,                                   // 0x0000
        0},
    { "WM_CREATE", WM_CREATE,                               // 0x0001
        0},
    { "WM_DESTROY", WM_DESTROY,                             // 0x0002
        0},
    { "WM_MOVE", WM_MOVE,                                   // 0x0003
        0},
    { "WM_SIZE", WM_SIZE,                                   // 0x0005
        0},
    { "WM_ACTIVATE", WM_ACTIVATE,                           // 0x0006
        0},
    { "WM_SETFOCUS", WM_SETFOCUS,                           // 0x0007
        0},
    { "WM_KILLFOCUS", WM_KILLFOCUS,                         // 0x0008
        0},
    { "WM_SETVISIBLE", WM_SETVISIBLE,                       // 0x0009
        0},
    { "WM_ENABLE", WM_ENABLE,                               // 0x000A
        0},
    { "WM_SETREDRAW", WM_SETREDRAW,                         // 0x000B
        0},
    { "WM_SETTEXT", WM_SETTEXT,                             // 0x000C
        0},
    { "WM_GETTEXT", WM_GETTEXT,                             // 0x000D
        0},
    { "WM_GETTEXTLENGTH", WM_GETTEXTLENGTH,                 // 0x000E
        0},
    { "WM_PAINT", WM_PAINT,                                 // 0x000F
        0},
    { "WM_CLOSE", WM_CLOSE,                                 // 0x0010
        0},
    { "WM_QUERYENDSESSION", WM_QUERYENDSESSION,             // 0x0011
        0},
    { "WM_QUIT", WM_QUIT,                                   // 0x0012
        0},
    { "WM_QUERYOPEN", WM_QUERYOPEN,                         // 0x0013
        0},
    { "WM_ERASEBKGND", WM_ERASEBKGND,                       // 0x0014
        0},
    { "WM_SYSCOLORCHANGE", WM_SYSCOLORCHANGE,               // 0x0015
        0},
    { "WM_ENDSESSION", WM_ENDSESSION,                       // 0x0016
        0},
    { "WM_SHOWWINDOW", WM_SHOWWINDOW,                       // 0x0018
        0},
    { "WM_WININICHANGE", WM_WININICHANGE,                   // 0x001A
        0},
    { "WM_DEVMODECHANGE", WM_DEVMODECHANGE,                 // 0x001B
        0},
    { "WM_ACTIVATEAPP", WM_ACTIVATEAPP,                     // 0x001C
        0},
    { "WM_FONTCHANGE", WM_FONTCHANGE,                       // 0x001D
        0},
    { "WM_TIMECHANGE", WM_TIMECHANGE,                       // 0x001E
        0},
    { "WM_CANCELMODE", WM_CANCELMODE,                       // 0x001F
        0},
    { "WM_SETCURSOR", WM_SETCURSOR,                         // 0x0020
        MTF_TYPE_MOUSE},
    { "WM_MOUSEACTIVATE", WM_MOUSEACTIVATE,                 // 0x0021
        MTF_TYPE_MOUSE},
    { "WM_CHILDACTIVATE", WM_CHILDACTIVATE,                 // 0x0022
        0},
    { "WM_QUEUESYNC", WM_QUEUESYNC,                         // 0x0023
        0},
    { "WM_GETMINMAXINFO", WM_GETMINMAXINFO,                 // 0x0024
        0},
    { "WM_PAINTICON", WM_PAINTICON,                         // 0x0026
        0},
    { "WM_ICONERASEBKGND", WM_ICONERASEBKGND,               // 0x0027
        0},
    { "WM_NEXTDLGCTL", WM_NEXTDLGCTL,                       // 0x0028
        0},
    { "WM_SPOOLERSTATUS", WM_SPOOLERSTATUS,                 // 0x002A
        0},
    { "WM_DRAWITEM", WM_DRAWITEM,                           // 0x002B
        0},
    { "WM_MEASUREITEM", WM_MEASUREITEM,                     // 0x002C
        0},
    { "WM_DELETEITEM", WM_DELETEITEM,                       // 0x002D
        0},
    { "WM_VKEYTOITEM", WM_VKEYTOITEM,                       // 0x002E
        MTF_TYPE_KEYBD},
    { "WM_CHARTOITEM", WM_CHARTOITEM,                       // 0x002F
        MTF_TYPE_KEYBD},
    { "WM_SETFONT", WM_SETFONT,                             // 0x0030
        0},
    { "WM_GETFONT", WM_GETFONT,                             // 0x0031
        0},
    { "WM_SETHOTKEY", WM_SETHOTKEY,                         // 0x0032
        MTF_TYPE_KEYBD},
    { "WM_GETHOTKEY", WM_GETHOTKEY,                         // 0x0033
        MTF_TYPE_KEYBD},
    { "WM_QUERYDRAGICON", WM_QUERYDRAGICON,                 // 0x0037
        0},
    { "WM_COMPAREITEM", WM_COMPAREITEM,                     // 0x0039
        0},
    { "WM_GETOBJECT", WM_GETOBJECT,                         // 0x003D
        0},
    { "WM_COMPACTING", WM_COMPACTING,                       // 0x0041
        0},
    { "WM_COMMNOTIFY", WM_COMMNOTIFY,                       // 0x0044
        0},
    { "WM_WINDOWPOSCHANGING", WM_WINDOWPOSCHANGING,         // 0x0046
        0},
    { "WM_WINDOWPOSCHANGED", WM_WINDOWPOSCHANGED,           // 0x0047
        0},
    { "WM_POWER", WM_POWER,                                 // 0x0048
        0},
    { "WM_COPYDATA", WM_COPYDATA,                           // 0x004A
        0},
    { "WM_CANCELJOURNAL", WM_CANCELJOURNAL,                 // 0x004B
        0},
    { "WM_NOTIFY", WM_NOTIFY, 0},                           // 0x4E
    { "WM_INPUTLANGCHANGEREQUEST", WM_INPUTLANGCHANGEREQUEST, 0},       //0x50
    { "WM_INPUTLANGCHANGE", WM_INPUTLANGCHANGE, 0},         // 0x51
    { "WM_TCARD", WM_TCARD, 0},                             // 0x52
    { "WM_HELP", WM_HELP, 0},                               // 0x53
    { "WM_USERCHANGED", WM_USERCHANGED, 0},                 // 0x54
    { "WM_NOTIFYFORMAT", WM_NOTIFYFORMAT, 0},               // 0x55
    { "WM_CONTEXTMENU" , WM_CONTEXTMENU, 0},                // 0x007B
    { "WM_STYLECHANGING", WM_STYLECHANGING, 0},             // 0x7C
    { "WM_STYLECHANGED", WM_STYLECHANGED, 0},               // 0x7D
    { "WM_DISPLAYCHANGE", WM_DISPLAYCHANGE, 0},             // 0x7E
    { "WM_GETICON", WM_GETICON, 0},                         // 0x7F
    { "WM_SETICON", WM_SETICON, 0},                         // 0x80
    { "WM_NCCREATE", WM_NCCREATE,                           // 0x0081
        MTF_TYPE_NC},
    { "WM_NCDESTROY", WM_NCDESTROY,                         // 0x0082
        MTF_TYPE_NC},
    { "WM_NCCALCSIZE", WM_NCCALCSIZE,                       // 0x0083
        MTF_TYPE_NC},
    { "WM_NCHITTEST", WM_NCHITTEST,                         // 0x0084
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCPAINT", WM_NCPAINT,                             // 0x0085
        MTF_TYPE_NC},
    { "WM_NCACTIVATE", WM_NCACTIVATE,                       // 0x0086
        MTF_TYPE_NC},
    { "WM_GETDLGCODE", WM_GETDLGCODE,                       // 0x0087
        0},
    { "WM_SYNCPAINT", WM_SYNCPAINT,                         // 0x0088
        0},
    { "WM_NCMOUSEMOVE", WM_NCMOUSEMOVE,                     // 0x00A0
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONDOWN", WM_NCLBUTTONDOWN,                 // 0x00A1
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONUP", WM_NCLBUTTONUP,                     // 0x00A2
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONDBLCLK", WM_NCLBUTTONDBLCLK,             // 0x00A3
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONDOWN", WM_NCRBUTTONDOWN,                 // 0x00A4
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONUP", WM_NCRBUTTONUP,                     // 0x00A5
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONDBLCLK", WM_NCRBUTTONDBLCLK,             // 0x00A6
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONDOWN", WM_NCMBUTTONDOWN,                 // 0x00A7
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONUP", WM_NCMBUTTONUP,                     // 0x00A8
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONDBLCLK", WM_NCMBUTTONDBLCLK,             // 0x00A9
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "EM_GETSEL", EM_GETSEL,                               // 0x00B0
        MTF_TYPE_EM},
    { "EM_SETSEL", EM_SETSEL,                               // 0x00B1
        MTF_TYPE_EM},
    { "EM_GETRECT", EM_GETRECT,                             // 0x00B2
        MTF_TYPE_EM},
    { "EM_SETRECT", EM_SETRECT,                             // 0x00B3
        MTF_TYPE_EM},
    { "EM_SETRECTNP", EM_SETRECTNP,                         // 0x00B4
        MTF_TYPE_EM},
    { "EM_SCROLL", EM_SCROLL,                               // 0x00B5
        MTF_TYPE_EM},
    { "EM_LINESCROLL", EM_LINESCROLL,                       // 0x00B6
        MTF_TYPE_EM},
    { "EM_SCROLLCARET", EM_SCROLLCARET,                     // 0x00B7
        MTF_TYPE_EM},
    { "EM_GETMODIFY", EM_GETMODIFY,                         // 0x00B8
        MTF_TYPE_EM},
    { "EM_SETMODIFY", EM_SETMODIFY,                         // 0x00B9
        MTF_TYPE_EM},
    { "EM_GETLINECOUNT", EM_GETLINECOUNT,                   // 0x00BA
        MTF_TYPE_EM},
    { "EM_LINEINDEX", EM_LINEINDEX,                         // 0x00BB
        MTF_TYPE_EM},
    { "EM_SETHANDLE", EM_SETHANDLE,                         // 0x00BC
        MTF_TYPE_EM},
    { "EM_GETHANDLE", EM_GETHANDLE,                         // 0x00BD
        MTF_TYPE_EM},
    { "EM_GETTHUMB", EM_GETTHUMB,                           // 0x00BE
        MTF_TYPE_EM},
    { "EM_LINELENGTH", EM_LINELENGTH,                       // 0x00C1
        MTF_TYPE_EM},
    { "EM_REPLACESEL", EM_REPLACESEL,                       // 0x00C2
        MTF_TYPE_EM},
    { "EM_GETLINE", EM_GETLINE,                             // 0x00C4
        MTF_TYPE_EM},
    { "EM_LIMITTEXT", EM_LIMITTEXT,                         // 0x00C5
        MTF_TYPE_EM},
    { "EM_CANUNDO", EM_CANUNDO,                             // 0x00C6
        MTF_TYPE_EM},
    { "EM_UNDO", EM_UNDO,                                   // 0x00C7
        MTF_TYPE_EM},
    { "EM_FMTLINES", EM_FMTLINES,                           // 0x00C8
        MTF_TYPE_EM},
    { "EM_LINEFROMCHAR", EM_LINEFROMCHAR,                   // 0x00C9
        MTF_TYPE_EM},
    { "EM_SETTABSTOPS", EM_SETTABSTOPS,                     // 0x00CB
        MTF_TYPE_EM},
    { "EM_SETPASSWORDCHAR", EM_SETPASSWORDCHAR,             // 0x00CC
        MTF_TYPE_EM},
    { "EM_EMPTYUNDOBUFFER", EM_EMPTYUNDOBUFFER,             // 0x00CD
        MTF_TYPE_EM},
    { "EM_GETFIRSTVISIBLELINE", EM_GETFIRSTVISIBLELINE,     // 0x00CE
        MTF_TYPE_EM},
    { "EM_SETREADONLY", EM_SETREADONLY,                     // 0x00CF
        MTF_TYPE_EM},
    { "EM_SETWORDBREAKPROC", EM_SETWORDBREAKPROC,           // 0x00D0
        MTF_TYPE_EM},
    { "EM_GETWORDBREAKPROC", EM_GETWORDBREAKPROC,           // 0x00D1
        MTF_TYPE_EM},
    { "EM_GETPASSWORDCHAR", EM_GETPASSWORDCHAR,             // 0x00D2
        MTF_TYPE_EM},
    { "SBM_SETPOS", SBM_SETPOS,                             // 0x00E0
        0},
    { "SBM_GETPOS", SBM_GETPOS,                             // 0x00E1
        0},
    { "SBM_SETRANGE", SBM_SETRANGE,                         // 0x00E2
        0},
    { "SBM_GETRANGE", SBM_GETRANGE,                         // 0x00E3
        0},
    { "SBM_ENABLE_ARROWS", SBM_ENABLE_ARROWS,               // 0x00E4
        0},
    { "SBM_SETRANGEREDRAW", SBM_SETRANGEREDRAW,             // 0x00E6
        0},
    { "BM_GETCHECK", BM_GETCHECK,                           // 0x00F0
        MTF_TYPE_BM},
    { "BM_SETCHECK", BM_SETCHECK,                           // 0x00F1
        MTF_TYPE_BM},
    { "BM_GETSTATE", BM_GETSTATE,                           // 0x00F2
        MTF_TYPE_BM},
    { "BM_SETSTATE", BM_SETSTATE,                           // 0x00F3
        MTF_TYPE_BM},
    { "BM_SETSTYLE", BM_SETSTYLE,                           // 0x00F4
        MTF_TYPE_BM},
    { "WM_KEYDOWN", WM_KEYDOWN,                             // 0x0100
        MTF_TYPE_KEYBD},
    { "WM_KEYUP", WM_KEYUP,                                 // 0x0101
        MTF_TYPE_KEYBD},
    { "WM_CHAR", WM_CHAR,                                   // 0x0102
        MTF_TYPE_KEYBD},
    { "WM_DEADCHAR", WM_DEADCHAR,                           // 0x0103
        MTF_TYPE_KEYBD},
    { "WM_SYSKEYDOWN", WM_SYSKEYDOWN,                       // 0x0104
        MTF_TYPE_KEYBD},
    { "WM_SYSKEYUP", WM_SYSKEYUP,                           // 0x0105
        MTF_TYPE_KEYBD},
    { "WM_SYSCHAR", WM_SYSCHAR,                             // 0x0106
        MTF_TYPE_KEYBD},
    { "WM_SYSDEADCHAR", WM_SYSDEADCHAR,                     // 0x0107
        MTF_TYPE_KEYBD},
#ifdef  FE_IME
    { "WM_WNT_CONVERTREQUESTEX", WM_WNT_CONVERTREQUESTEX,   // 0x0109
        MTF_TYPE_IME},
    { "WM_CONVERTREQUEST", WM_CONVERTREQUEST,               // 0x010A
        MTF_TYPE_IME},
    { "WM_CONVERTRESULT", WM_CONVERTRESULT,                 // 0x010B
        MTF_TYPE_IME},
    { "WM_INTERIM", WM_INTERIM,                             // 0x010C
        MTF_TYPE_IME},
#   define TMP_MTF_TYPE_IME     MTF_TYPE_IME
#else // not FE_IME
#   define TMP_MTF_TYPE_IME     0
#endif
    { "WM_IME_STARTCOMPOSITION", WM_IME_STARTCOMPOSITION,   // 0x010D
        TMP_MTF_TYPE_IME},
    { "WM_IME_ENDCOMPOSITION",   WM_IME_ENDCOMPOSITION,     // 0x010E
        TMP_MTF_TYPE_IME},
    { "WM_IME_COMPOSITION",      WM_IME_COMPOSITION,        // 0x010F
        TMP_MTF_TYPE_IME},
    { "WM_INITDIALOG", WM_INITDIALOG,                       // 0x0110
        0},
    { "WM_COMMAND", WM_COMMAND,                             // 0x0111
        0},
    { "WM_SYSCOMMAND", WM_SYSCOMMAND,                       // 0x0112
        0},
    { "WM_TIMER", WM_TIMER,                                 // 0x0113
        0},
    { "WM_HSCROLL", WM_HSCROLL,                             // 0x0114
        0},
    { "WM_VSCROLL", WM_VSCROLL,                             // 0x0115
        0},
    { "WM_INITMENU", WM_INITMENU,                           // 0x0116
        0},
    { "WM_INITMENUPOPUP", WM_INITMENUPOPUP,                 // 0x0117
        0},
    { "WM_SYSTIMER", WM_SYSTIMER,                           // 0x0118
        0},
    { "WM_MENUSELECT", WM_MENUSELECT,                       // 0x011F
        0},
    { "WM_MENUCHAR", WM_MENUCHAR,                           // 0x0120
        0},
    { "WM_ENTERIDLE", WM_ENTERIDLE,                         // 0x0121
        0},
    { "WM_MENURBUTTONUP", WM_MENURBUTTONUP,                 // 0x0122
        0},
    { "WM_MENUDRAG", WM_MENUDRAG,                           // 0x0123
        0},
    { "WM_MENUGETOBJECT", WM_MENUGETOBJECT,                 // 0x0124
        0},
    { "WM_UNINITMENUPOPUP", WM_UNINITMENUPOPUP,             // 0x0125
        0},
    { "WM_MENUCOMMAND", WM_MENUCOMMAND,                     // 0x0126
        0},
    { "WM_KEYBOARDCUES", WM_KEYBOARDCUES,                   // 0x0127
        0},
    { "WM_CTLCOLORMSGBOX", WM_CTLCOLORMSGBOX,               // 0x0132
        0},
    { "WM_CTLCOLOREDIT", WM_CTLCOLOREDIT,                   // 0x0133
        0},
    { "WM_CTLCOLORLISTBOX", WM_CTLCOLORLISTBOX,             // 0x0134
        0},
    { "WM_CTLCOLORBTN", WM_CTLCOLORBTN,                     // 0x0135
        0},
    { "WM_CTLCOLORDLG", WM_CTLCOLORDLG,                     // 0x0136
        0},
    { "WM_CTLCOLORSCROLLBAR", WM_CTLCOLORSCROLLBAR,         // 0x0137
        0},
    { "WM_CTLCOLORSTATIC", WM_CTLCOLORSTATIC,               // 0x0138
        0},
    { "CB_GETEDITSEL", CB_GETEDITSEL,                       // 0x0140
        MTF_TYPE_CB},
    { "CB_LIMITTEXT", CB_LIMITTEXT,                         // 0x0141
        MTF_TYPE_CB},
    { "CB_SETEDITSEL", CB_SETEDITSEL,                       // 0x0142
        MTF_TYPE_CB},
    { "CB_ADDSTRING", CB_ADDSTRING,                         // 0x0143
        MTF_TYPE_CB},
    { "CB_DELETESTRING", CB_DELETESTRING,                   // 0x0144
        MTF_TYPE_CB},
    { "CB_DIR", CB_DIR,                                     // 0x0145
        MTF_TYPE_CB},
    { "CB_GETCOUNT", CB_GETCOUNT,                           // 0x0146
        MTF_TYPE_CB},
    { "CB_GETCURSEL", CB_GETCURSEL,                         // 0x0147
        MTF_TYPE_CB},
    { "CB_GETLBTEXT", CB_GETLBTEXT,                         // 0x0148
        MTF_TYPE_CB},
    { "CB_GETLBTEXTLEN", CB_GETLBTEXTLEN,                   // 0x0149
        MTF_TYPE_CB},
    { "CB_INSERTSTRING", CB_INSERTSTRING,                   // 0x014A
        MTF_TYPE_CB},
    { "CB_RESETCONTENT", CB_RESETCONTENT,                   // 0x014B
        MTF_TYPE_CB},
    { "CB_FINDSTRING", CB_FINDSTRING,                       // 0x014C
        MTF_TYPE_CB},
    { "CB_SELECTSTRING", CB_SELECTSTRING,                   // 0x014D
        MTF_TYPE_CB},
    { "CB_SETCURSEL", CB_SETCURSEL,                         // 0x014E
        MTF_TYPE_CB},
    { "CB_SHOWDROPDOWN", CB_SHOWDROPDOWN,                   // 0x014F
        MTF_TYPE_CB},
    { "CB_GETITEMDATA", CB_GETITEMDATA,                     // 0x0150
        MTF_TYPE_CB},
    { "CB_SETITEMDATA", CB_SETITEMDATA,                     // 0x0151
        MTF_TYPE_CB},
    { "CB_GETDROPPEDCONTROLRECT", CB_GETDROPPEDCONTROLRECT, // 0x0152
        MTF_TYPE_CB},
    { "CB_SETITEMHEIGHT", CB_SETITEMHEIGHT,                 // 0x0153
        MTF_TYPE_CB},
    { "CB_GETITEMHEIGHT", CB_GETITEMHEIGHT,                 // 0x0154
        MTF_TYPE_CB},
    { "CB_SETEXTENDEDUI", CB_SETEXTENDEDUI,                 // 0x0155
        MTF_TYPE_CB},
    { "CB_GETEXTENDEDUI", CB_GETEXTENDEDUI,                 // 0x0156
        MTF_TYPE_CB},
    { "CB_GETDROPPEDSTATE", CB_GETDROPPEDSTATE,             // 0x0157
        MTF_TYPE_CB},
    { "CB_FINDSTRINGEXACT", CB_FINDSTRINGEXACT,             // 0x0158
        MTF_TYPE_CB},
    { "CB_SETLOCALE", CB_SETLOCALE,                         // 0x0159
        MTF_TYPE_CB},
    { "CB_GETLOCALE", CB_GETLOCALE,                         // 0x015A
        MTF_TYPE_CB},
    { "STM_SETICON", STM_SETICON,                           // 0x0170
        MTF_TYPE_STM},
    { "STM_GETICON", STM_GETICON,                           // 0x0171
        MTF_TYPE_STM},
    { "STM_SETIMAGE", STM_SETIMAGE,                         // 0x0172
        MTF_TYPE_STM},
    { "STM_GETIMAGE", STM_GETIMAGE,                         // 0x0173
        MTF_TYPE_STM},
    { "LB_ADDSTRING", LB_ADDSTRING,                         // 0x0180
        MTF_TYPE_LB},
    { "LB_INSERTSTRING", LB_INSERTSTRING,                   // 0x0181
        MTF_TYPE_LB},
    { "LB_DELETESTRING", LB_DELETESTRING,                   // 0x0182
        MTF_TYPE_LB},
    { "LB_SELITEMRANGEEX", LB_SELITEMRANGEEX,               // 0x0183
        MTF_TYPE_LB},
    { "LB_RESETCONTENT", LB_RESETCONTENT,                   // 0x0184
        MTF_TYPE_LB},
    { "LB_SETSEL", LB_SETSEL,                               // 0x0185
        MTF_TYPE_LB},
    { "LB_SETCURSEL", LB_SETCURSEL,                         // 0x0186
        MTF_TYPE_LB},
    { "LB_GETSEL", LB_GETSEL,                               // 0x0187
        MTF_TYPE_LB},
    { "LB_GETCURSEL", LB_GETCURSEL,                         // 0x0188
        MTF_TYPE_LB},
    { "LB_GETTEXT", LB_GETTEXT,                             // 0x0189
        MTF_TYPE_LB},
    { "LB_GETTEXTLEN", LB_GETTEXTLEN,                       // 0x018A
        MTF_TYPE_LB},
    { "LB_GETCOUNT", LB_GETCOUNT,                           // 0x018B
        MTF_TYPE_LB},
    { "LB_SELECTSTRING", LB_SELECTSTRING,                   // 0x018C
        MTF_TYPE_LB},
    { "LB_DIR", LB_DIR,                                     // 0x018D
        MTF_TYPE_LB},
    { "LB_GETTOPINDEX", LB_GETTOPINDEX,                     // 0x018E
        MTF_TYPE_LB},
    { "LB_FINDSTRING", LB_FINDSTRING,                       // 0x018F
        MTF_TYPE_LB},
    { "LB_GETSELCOUNT", LB_GETSELCOUNT,                     // 0x0190
        MTF_TYPE_LB},
    { "LB_GETSELITEMS", LB_GETSELITEMS,                     // 0x0191
        MTF_TYPE_LB},
    { "LB_SETTABSTOPS", LB_SETTABSTOPS,                     // 0x0192
        MTF_TYPE_LB},
    { "LB_GETHORIZONTALEXTENT", LB_GETHORIZONTALEXTENT,     // 0x0193
        MTF_TYPE_LB},
    { "LB_SETHORIZONTALEXTENT", LB_SETHORIZONTALEXTENT,     // 0x0194
        MTF_TYPE_LB},
    { "LB_SETCOLUMNWIDTH", LB_SETCOLUMNWIDTH,               // 0x0195
        MTF_TYPE_LB},
    { "LB_ADDFILE", LB_ADDFILE,                             // 0x0196
        MTF_TYPE_LB},
    { "LB_SETTOPINDEX", LB_SETTOPINDEX,                     // 0x0197
        MTF_TYPE_LB},
    { "LB_GETITEMRECT", LB_GETITEMRECT,                     // 0x0198
        MTF_TYPE_LB},
    { "LB_GETITEMDATA", LB_GETITEMDATA,                     // 0x0199
        MTF_TYPE_LB},
    { "LB_SETITEMDATA", LB_SETITEMDATA,                     // 0x019A
        MTF_TYPE_LB},
    { "LB_SELITEMRANGE", LB_SELITEMRANGE,                   // 0x019B
        MTF_TYPE_LB},
    { "LB_SETANCHORINDEX", LB_SETANCHORINDEX,               // 0x019C
        MTF_TYPE_LB},
    { "LB_GETANCHORINDEX", LB_GETANCHORINDEX,               // 0x019D
        MTF_TYPE_LB},
    { "LB_SETCARETINDEX", LB_SETCARETINDEX,                 // 0x019E
        MTF_TYPE_LB},
    { "LB_GETCARETINDEX", LB_GETCARETINDEX,                 // 0x019F
        MTF_TYPE_LB},
    { "LB_SETITEMHEIGHT", LB_SETITEMHEIGHT,                 // 0x01A0
        MTF_TYPE_LB},
    { "LB_GETITEMHEIGHT", LB_GETITEMHEIGHT,                 // 0x01A1
        MTF_TYPE_LB},
    { "LB_FINDSTRINGEXACT", LB_FINDSTRINGEXACT,             // 0x01A2
        MTF_TYPE_LB},
    { "LB_CARETON", LB_CARETON,                             // 0x01A3
        MTF_TYPE_LB},
    { "LB_CARETOFF", LB_CARETOFF,                           // 0x01A4
        MTF_TYPE_LB},
    { "LB_SETLOCALE", LB_SETLOCALE,                         // 0x01A5
        MTF_TYPE_LB},
    { "LB_GETLOCALE", LB_GETLOCALE,                         // 0x01A6
        MTF_TYPE_LB},
    { "LB_SETCOUNT", LB_SETCOUNT,                           // 0x01A7
        MTF_TYPE_LB},
    { "WM_MOUSEMOVE", WM_MOUSEMOVE,                         // 0x0200
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONDOWN", WM_LBUTTONDOWN,                     // 0x0201
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONUP", WM_LBUTTONUP,                         // 0x0202
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONDBLCLK", WM_LBUTTONDBLCLK,                 // 0x0203
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONDOWN", WM_RBUTTONDOWN,                     // 0x0204
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONUP", WM_RBUTTONUP,                         // 0x0205
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONDBLCLK", WM_RBUTTONDBLCLK,                 // 0x0206
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONDOWN", WM_MBUTTONDOWN,                     // 0x0207
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONUP", WM_MBUTTONUP,                         // 0x0208
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONDBLCLK", WM_MBUTTONDBLCLK,                 // 0x0209
        MTF_TYPE_MOUSE},
    { "WM_MOUSEWHEEL", WM_MOUSEWHEEL,                       // 0x020A
        MTF_TYPE_MOUSE},
    { "WM_PARENTNOTIFY", WM_PARENTNOTIFY,                   // 0x0210
        MTF_TYPE_MOUSE},
    { "WM_ENTERMENULOOP", WM_ENTERMENULOOP,                 // 0x0211
        0},
    { "WM_EXITMENULOOP", WM_EXITMENULOOP,                   // 0x0212
        0},
    { "WM_NEXTMENU", WM_NEXTMENU,                           // 0x0213
        0},
    { "WM_SIZING", WM_SIZING,                               // 0x0214
        0},
    { "WM_CAPTURECHANGED", WM_CAPTURECHANGED,               // 0x0215
        0},
    { "WM_MOVING", WM_MOVING,                               // 0x0216
        0},
    { "WM_POWERBROADCAST", WM_POWERBROADCAST,               // 0x0218
        0},
    { "WM_DEVICECHANGE", WM_DEVICECHANGE,                   // 0x0219
        0},
    { "WM_MDICREATE", WM_MDICREATE,                         // 0x0220
        0},
    { "WM_MDIDESTROY", WM_MDIDESTROY,                       // 0x0221
        0},
    { "WM_MDIACTIVATE", WM_MDIACTIVATE,                     // 0x0222
        0},
    { "WM_MDIRESTORE", WM_MDIRESTORE,                       // 0x0223
        0},
    { "WM_MDINEXT", WM_MDINEXT,                             // 0x0224
        0},
    { "WM_MDIMAXIMIZE", WM_MDIMAXIMIZE,                     // 0x0225
        0},
    { "WM_MDITILE", WM_MDITILE,                             // 0x0226
        0},
    { "WM_MDICASCADE", WM_MDICASCADE,                       // 0x0227
        0},
    { "WM_MDIICONARRANGE", WM_MDIICONARRANGE,               // 0x0228
        0},
    { "WM_MDIGETACTIVE", WM_MDIGETACTIVE,                   // 0x0229
        0},
    { "WM_MDISETMENU", WM_MDISETMENU,                       // 0x0230
        0},
    { "WM_ENTERSIZEMOVE", WM_ENTERSIZEMOVE,                 // 0x0231
        0},
    { "WM_EXITSIZEMOVE", WM_EXITSIZEMOVE,                   // 0x0232
        0},
    { "WM_DROPFILES", WM_DROPFILES,                         // 0x0233
        0},
    { "WM_MDIREFRESHMENU", WM_MDIREFRESHMENU,               // 0x0234
        0},
#ifdef  FE_IME
    { "WM_IME_REPORT", WM_IME_REPORT,                       // 0x0280
        MTF_TYPE_IME},
#endif
    { "WM_IME_SETCONTEXT",      WM_IME_SETCONTEXT,          // 0x0281
        TMP_MTF_TYPE_IME},
    { "WM_IME_NOTIFY",          WM_IME_NOTIFY,              // 0x0282
        TMP_MTF_TYPE_IME},
    { "WM_IME_CONTROL",         WM_IME_CONTROL,             // 0x0283
        TMP_MTF_TYPE_IME},
    { "WM_IME_COMPOSITIONFULL", WM_IME_COMPOSITIONFULL,     // 0x0284
        TMP_MTF_TYPE_IME},
    { "WM_IME_SELECT",          WM_IME_SELECT,              // 0x0285
        TMP_MTF_TYPE_IME},
    { "WM_IME_CHAR",            WM_IME_CHAR,                // 0x0286
        TMP_MTF_TYPE_IME},
    { "WM_IME_REQUEST",         WM_IME_REQUEST,             // 0x0288
        TMP_MTF_TYPE_IME},
#ifdef  FE_IME
    { "WM_IMEKEYDOWN", WM_IMEKEYDOWN,                       // 0x0290
        MTF_TYPE_IME},
    { "WM_IMEKEYUP", WM_IMEKEYUP,                           // 0x0291
        MTF_TYPE_IME},
#endif
    { "WM_MOUSEHOVER", WM_MOUSEHOVER,                       // 0x02A1
      MTF_TYPE_MOUSE},
    { "WM_MOUSELEAVE", WM_MOUSELEAVE,                       // 0x02A3
      MTF_TYPE_MOUSE},
    { "WM_NCMOUSEHOVER", WM_NCMOUSEHOVER,                   // 0x02A0
      MTF_TYPE_MOUSE},
    { "WM_NCMOUSELEAVE", WM_NCMOUSELEAVE,                   // 0x02A2
      MTF_TYPE_MOUSE},
    { "WM_CUT", WM_CUT,                                     // 0x0300
        MTF_TYPE_CLIP},
    { "WM_COPY", WM_COPY,                                   // 0x0301
        MTF_TYPE_CLIP},
    { "WM_PASTE", WM_PASTE,                                 // 0x0302
        MTF_TYPE_CLIP},
    { "WM_CLEAR", WM_CLEAR,                                 // 0x0303
        MTF_TYPE_CLIP},
    { "WM_UNDO", WM_UNDO,                                   // 0x0304
        MTF_TYPE_CLIP},
    { "WM_RENDERFORMAT", WM_RENDERFORMAT,                   // 0x0305
        MTF_TYPE_CLIP},
    { "WM_RENDERALLFORMATS", WM_RENDERALLFORMATS,           // 0x0306
        MTF_TYPE_CLIP},
    { "WM_DESTROYCLIPBOARD", WM_DESTROYCLIPBOARD,           // 0x0307
        MTF_TYPE_CLIP},
    { "WM_DRAWCLIPBOARD", WM_DRAWCLIPBOARD,                 // 0x0308
        MTF_TYPE_CLIP},
    { "WM_PAINTCLIPBOARD", WM_PAINTCLIPBOARD,               // 0x0309
        MTF_TYPE_CLIP},
    { "WM_VSCROLLCLIPBOARD", WM_VSCROLLCLIPBOARD,           // 0x030A
        MTF_TYPE_CLIP},
    { "WM_SIZECLIPBOARD", WM_SIZECLIPBOARD,                 // 0x030B
        MTF_TYPE_CLIP},
    { "WM_ASKCBFORMATNAME", WM_ASKCBFORMATNAME,             // 0x030C
        MTF_TYPE_CLIP},
    { "WM_CHANGECBCHAIN", WM_CHANGECBCHAIN,                 // 0x030D
        MTF_TYPE_CLIP},
    { "WM_HSCROLLCLIPBOARD", WM_HSCROLLCLIPBOARD,           // 0x030E
        MTF_TYPE_CLIP},
    { "WM_QUERYNEWPALETTE", WM_QUERYNEWPALETTE,             // 0x030F
        0},
    { "WM_PALETTEISCHANGING", WM_PALETTEISCHANGING,         // 0x0310
        0},
    { "WM_PALETTECHANGED", WM_PALETTECHANGED,               // 0x0311
        0},
    { "WM_HOTKEY", WM_HOTKEY,                               // 0x0312
        MTF_TYPE_KEYBD},
    { "WM_PRINT", WM_PRINT,                                 // 0x0317
        0},
    { "WM_PRINTCLIENT", WM_PRINTCLIENT,                     // 0x0318
        0},
    { "WM_QUERYAFXWNDPROC(MFC)", WM_QUERYAFXWNDPROC,        // 0x0360
        0},
    { "WM_SIZEPARENT(MFC)", WM_SIZEPARENT,                  // 0x0361
        0},
    { "WM_SETMESSAGESTRING(MFC)", WM_SETMESSAGESTRING,      // 0x0362
        0},
    { "WM_IDLEUPDATECMDUI(MFC)", WM_IDLEUPDATECMDUI,        // 0x0363
        0},
    { "WM_INITIALUPDATE(MFC)", WM_INITIALUPDATE,            // 0x0364
        0},
    { "WM_COMMANDHELP(MFC)", WM_COMMANDHELP,                // 0x0365
        0},
    { "WM_HELPHITTEST(MFC)", WM_HELPHITTEST,                // 0x0366
        0},
    { "WM_EXITHELPMODE(MFC)", WM_EXITHELPMODE,              // 0x0367
        0},
    { "WM_RECALCPARENT(MFC)", WM_RECALCPARENT,              // 0x0368
        0},
    { "WM_SIZECHILD(MFC)", WM_SIZECHILD,                    // 0x0369
        0},
    { "WM_KICKIDLE(MFC)", WM_KICKIDLE,                      // 0x036A
        0},
    { "WM_QUERYCENTERWND(MFC)", WM_QUERYCENTERWND,          // 0x036B
        0},
    { "WM_DISABLEMODAL(MFC)", WM_DISABLEMODAL,              // 0x036C
        0},
    { "WM_FLOATSTATUS(MFC)", WM_FLOATSTATUS,                // 0x036D
        0},
    { "WM_ACTIVATETOPLEVEL(MFC)", WM_ACTIVATETOPLEVEL,      // 0x036E
        0},
    { "WM_QUERY3DCONTROLS(MFC)", WM_QUERY3DCONTROLS,        // 0x036F
        0},
    { "WM_SOCKET_NOTIFY(MFC)", WM_SOCKET_NOTIFY,            // 0x0373
        0},
    { "WM_SOCKET_DEAD(MFC)", WM_SOCKET_DEAD,                // 0x0374
        0},
    { "WM_POPMESSAGESTRING(MFC)", WM_POPMESSAGESTRING,      // 0x0375
        0},
    { "WM_OCC_LOADFROMSTREAM(MFC)", WM_OCC_LOADFROMSTREAM,  // 0x0376
        0},
    { "WM_OCC_LOADFROMSTORAGE(MFC)", WM_OCC_LOADFROMSTORAGE, // 0x0377
        0},
    { "WM_OCC_INITNEW(MFC)", WM_OCC_INITNEW,                // 0x0378
        0},
    { "WM_QUEUE_SENTINEL(MFC)", WM_QUEUE_SENTINEL,          // 0x0379
        0},
    { "WM_OCC_LOADFROMSTREAM_EX(MFC)", WM_OCC_LOADFROMSTREAM_EX, // 0x037A
        0},
    { "WM_OCC_LOADFROMSTORAGE_EX(MFC)", WM_OCC_LOADFROMSTORAGE_EX, // 0x037B
        0},
    { "WM_FORWARDMSG", WM_FORWARDMSG,                       // 0x037F
        0},
    { "WM_DDE_INITIATE", WM_DDE_INITIATE,                   // 0x03E0
        MTF_TYPE_DDE},
    { "WM_DDE_TERMINATE", WM_DDE_TERMINATE,                 // 0x03E1
        MTF_TYPE_DDE},
    { "WM_DDE_ADVISE", WM_DDE_ADVISE,                       // 0x03E2
        MTF_TYPE_DDE},
    { "WM_DDE_UNADVISE", WM_DDE_UNADVISE,                   // 0x03E3
        MTF_TYPE_DDE},
    { "WM_DDE_ACK", WM_DDE_ACK,                             // 0x03E4
        MTF_TYPE_DDE},
    { "WM_DDE_DATA", WM_DDE_DATA,                           // 0x03E5
        MTF_TYPE_DDE},
    { "WM_DDE_REQUEST", WM_DDE_REQUEST,                     // 0x03E6
        MTF_TYPE_DDE},
    { "WM_DDE_POKE", WM_DDE_POKE,                           // 0x03E7
        MTF_TYPE_DDE},
    { "WM_DDE_EXECUTE", WM_DDE_EXECUTE,                     // 0x03E8
        MTF_TYPE_DDE},
    //WM_USER                         0x0400
      //CB: some dialog message are WM_USER+x
      //    shlobj.h, aclui.h, shellapi.h, prsht.h, commdlg.h, commctrl.h, tapi.h, cpl.h, ras.h, richedit.h, winwlx.h, wfext.h, scrmsave.h, olectl.h, ftsiface.h, dispdib.h, proppage.h, vfw.h, mapiwz.h
    //WM_APP                          0x8000
};

INT gcMessages = sizeof(gaMsgs) / sizeof(MSGDESC);


char *GetMsgText(int Msg)
{
 static char msgtxt[64];
 int i;

  for(i=0;i<gcMessages;i++) {
        if(gaMsgs[i].msg == Msg)
                return(gaMsgs[i].pszMsg);
  }
  if (Msg >= WM_USER)
    sprintf(msgtxt,"User Message %X (WM_USER+%d)",Msg,Msg-WM_USER);
  else
    sprintf(msgtxt, "%s %X ","Unknown Message ", Msg);
  return(msgtxt);
}


void DebugPrintMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode, BOOL fInternalMsg)
{
 char unicode;

  if(fUnicode)
        unicode = 'W';
  else  unicode = 'A';

  if(Msg != WM_GETDLGCODE && Msg != WM_ENTERIDLE && Msg != WM_NCHITTEST && Msg != WM_KICKIDLE)
  {//sent *very* often
        if(PostSpyMessage(hwnd, Msg, wParam, lParam) == FALSE)
        {
            if(fInternalMsg) {
                    dprintf(("SendInternalMessage%c %s for %x %x %x", unicode, GetMsgText(Msg), hwnd, wParam, lParam));
            }
            else    dprintf(("SendMessage%c %s for %x %x %x", unicode, GetMsgText(Msg), hwnd, wParam, lParam));
        }
  }
  else {
        if(fInternalMsg) {
            	dprintf2(("SendInternalMessage%c %s for %x %x %x", unicode, GetMsgText(Msg), hwnd, wParam, lParam));
        }
        else    dprintf2(("SendMessage%c %s for %x %x %x", unicode, GetMsgText(Msg), hwnd, wParam, lParam));
  }
}

#endif
