/* $Id: oslibmsg.h,v 1.14 2001-02-15 00:33:01 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBMSG_H__
#define __OSLIBMSG_H__

ULONG TranslateWinMsg(ULONG msg);

#define MSG_NOREMOVE	0
#define MSG_REMOVE	1

#ifdef OS2DEF_INCLUDED
BOOL  OS2ToWinMsgTranslate(void *pThdb, QMSG *os2Msg, MSG *winMsg, BOOL isUnicode, BOOL fMsgRemoved);
void  WinToOS2MsgTranslate(MSG *winMsg, QMSG *os2Msg, BOOL isUnicode);

#endif

BOOL  OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, 
                     UINT uMsgFilterMax, BOOL isUnicode = FALSE);
BOOL  OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, 
                      UINT uMsgFilterMax, DWORD fRemove, BOOL isUnicode = FALSE);
void  OSLibWinPostQuitMessage(ULONG nExitCode);
LONG  OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode = FALSE);
BOOL  OSLibWinTranslateMessage(MSG *msg);

ULONG OSLibWinQueryMsgTime();
BOOL  OSLibWinWaitMessage();
BOOL  OSLibWinInSendMessage();
DWORD OSLibWinGetMessagePos();
LONG  OSLibWinGetMessageTime();
BOOL  OSLibWinReplyMessage(ULONG result);
ULONG OSLibWinQueryQueueStatus();

BOOL  OSLibPostThreadMessage(ULONG threadid, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode);
BOOL  OSLibPostMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode);
ULONG OSLibSendMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode);
ULONG OSLibWinBroadcastMsg(ULONG msg, ULONG wParam, ULONG lParam, BOOL fSend);

//Direct posting of messages that must remain invisible to the win32 app
BOOL OSLibPostMessageDirect(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam);

#define WINWM_NULL                  0x0000
#define WINWM_CREATE                0x0001
#define WINWM_DESTROY               0x0002
#define WINWM_MOVE                  0x0003
#define WINWM_SIZE                  0x0005
#define WINWM_ACTIVATE              0x0006
#define WINWM_SETFOCUS              0x0007
#define WINWM_KILLFOCUS             0x0008
#define WINWM_ENABLE                0x000A
#define WINWM_SETREDRAW             0x000B
#define WINWM_SETTEXT               0x000C
#define WINWM_GETTEXT               0x000D
#define WINWM_GETTEXTLENGTH         0x000E
#define WINWM_PAINT                 0x000F
#define WINWM_CLOSE                 0x0010
#define WINWM_QUERYENDSESSION       0x0011
#define WINWM_QUIT                  0x0012
#define WINWM_QUERYOPEN             0x0013
#define WINWM_ERASEBKGND            0x0014
#define WINWM_SYSCOLORCHANGE        0x0015
#define WINWM_ENDSESSION            0x0016
#define WINWM_SHOWWINDOW            0x0018
#define WINWM_WININICHANGE          0x001A
#define WINWM_DEVMODECHANGE         0x001B
#define WINWM_ACTIVATEAPP           0x001C
#define WINWM_FONTCHANGE            0x001D
#define WINWM_TIMECHANGE            0x001E
#define WINWM_CANCELMODE            0x001F
#define WINWM_SETCURSOR             0x0020
#define WINWM_MOUSEACTIVATE         0x0021
#define WINWM_CHILDACTIVATE         0x0022
#define WINWM_QUEUESYNC             0x0023
#define WINWM_GETMINMAXINFO         0x0024
#define WINWM_PAINTICON             0x0026
#define WINWM_ICONERASEBKGND        0x0027
#define WINWM_NEXTDLGCTL            0x0028
#define WINWM_SPOOLERSTATUS         0x002A
#define WINWM_DRAWITEM              0x002B
#define WINWM_MEASUREITEM           0x002C
#define WINWM_DELETEITEM            0x002D
#define WINWM_VKEYTOITEM            0x002E
#define WINWM_CHARTOITEM            0x002F
#define WINWM_SETFONT               0x0030
#define WINWM_GETFONT               0x0031
#define WINWM_SETHOTKEY             0x0032
#define WINWM_GETHOTKEY             0x0033
#define WINWM_QUERYDRAGICON         0x0037
#define WINWM_COMPAREITEM           0x0039
#define WINWM_COMPACTING            0x0041
#define WINWM_WINDOWPOSCHANGING     0x0046
#define WINWM_WINDOWPOSCHANGED      0x0047
#define WINWM_POWER                 0x0048
#define WINWM_COPYDATA              0x004A
#define WINWM_CANCELJOURNAL         0x004B
#define WINWM_CONTEXTMENU           0x007b
#define WINWM_STYLECHANGING         0x007c
#define WINWM_STYLECHANGED          0x007d
#define WINWM_DISPLAYCHANGE         0x007e
#define WINWM_GETICON               0x007f
#define WINWM_SETICON               0x0080
#define WINWM_NCCREATE              0x0081
#define WINWM_NCDESTROY             0x0082
#define WINWM_NCCALCSIZE            0x0083
#define WINWM_NCHITTEST             0x0084
#define WINWM_NCPAINT               0x0085
#define WINWM_NCACTIVATE            0x0086
#define WINWM_GETDLGCODE            0x0087
#define WINWM_NCMOUSEMOVE           0x00A0
#define WINWM_NCLBUTTONDOWN         0x00A1
#define WINWM_NCLBUTTONUP           0x00A2
#define WINWM_NCLBUTTONDBLCLK       0x00A3
#define WINWM_NCRBUTTONDOWN         0x00A4
#define WINWM_NCRBUTTONUP           0x00A5
#define WINWM_NCRBUTTONDBLCLK       0x00A6
#define WINWM_NCMBUTTONDOWN         0x00A7
#define WINWM_NCMBUTTONUP           0x00A8
#define WINWM_NCMBUTTONDBLCLK       0x00A9
#define WINWM_KEYFIRST              0x0100
#define WINWM_KEYDOWN               0x0100
#define WINWM_KEYUP                 0x0101
#define WINWM_CHAR                  0x0102
#define WINWM_DEADCHAR              0x0103
#define WINWM_SYSKEYDOWN            0x0104
#define WINWM_SYSKEYUP              0x0105
#define WINWM_SYSCHAR               0x0106
#define WINWM_SYSDEADCHAR           0x0107
#define WINWM_KEYLAST               0x0108
#define WINWM_INITDIALOG            0x0110
#define WINWM_COMMAND               0x0111
#define WINWM_SYSCOMMAND            0x0112
#define WINWM_TIMER                 0x0113
#define WINWM_HSCROLL               0x0114
#define WINWM_VSCROLL               0x0115
#define WINWM_INITMENU              0x0116
#define WINWM_INITMENUPOPUP         0x0117
#define WINWM_SYSTIMER              0x0118
#define WINWM_MENUSELECT            0x011F
#define WINWM_MENUCHAR              0x0120
#define WINWM_ENTERIDLE             0x0121
#define WINWM_CTLCOLORMSGBOX        0x0132
#define WINWM_CTLCOLOREDIT          0x0133
#define WINWM_CTLCOLORLISTBOX       0x0134
#define WINWM_CTLCOLORBTN           0x0135
#define WINWM_CTLCOLORDLG           0x0136
#define WINWM_CTLCOLORSCROLLBAR     0x0137
#define WINWM_CTLCOLORSTATIC        0x0138
#define WINWM_MOUSEFIRST            0x0200
#define WINWM_MOUSEMOVE             0x0200
#define WINWM_LBUTTONDOWN           0x0201
#define WINWM_LBUTTONUP             0x0202
#define WINWM_LBUTTONDBLCLK         0x0203
#define WINWM_RBUTTONDOWN           0x0204
#define WINWM_RBUTTONUP             0x0205
#define WINWM_RBUTTONDBLCLK         0x0206
#define WINWM_MBUTTONDOWN           0x0207
#define WINWM_MBUTTONUP             0x0208
#define WINWM_MBUTTONDBLCLK         0x0209
#define WINWM_MOUSELAST             0x0209
#define WINWM_PARENTNOTIFY          0x0210
#define WINWM_ENTERMENULOOP         0x0211
#define WINWM_EXITMENULOOP          0x0212
#define WINWM_MDICREATE             0x0220
#define WINWM_MDIDESTROY            0x0221
#define WINWM_MDIACTIVATE           0x0222
#define WINWM_MDIRESTORE            0x0223
#define WINWM_MDINEXT               0x0224
#define WINWM_MDIMAXIMIZE           0x0225
#define WINWM_MDITILE               0x0226
#define WINWM_MDICASCADE            0x0227
#define WINWM_MDIICONARRANGE        0x0228
#define WINWM_MDIGETACTIVE          0x0229
#define WINWM_MDISETMENU            0x0230
#define WINWM_DROPFILES             0x0233
#define WINWM_MDIREFRESHMENU        0x0234
#define WINWM_CUT                   0x0300
#define WINWM_COPY                  0x0301
#define WINWM_PASTE                 0x0302
#define WINWM_CLEAR                 0x0303
#define WINWM_UNDO                  0x0304
#define WINWM_RENDERFORMAT          0x0305
#define WINWM_RENDERALLFORMATS      0x0306
#define WINWM_DESTROYCLIPBOARD      0x0307
#define WINWM_DRAWCLIPBOARD         0x0308
#define WINWM_PAINTCLIPBOARD        0x0309
#define WINWM_VSCROLLCLIPBOARD      0x030A
#define WINWM_SIZECLIPBOARD         0x030B
#define WINWM_ASKCBFORMATNAME       0x030C
#define WINWM_CHANGECBCHAIN         0x030D
#define WINWM_HSCROLLCLIPBOARD      0x030E
#define WINWM_QUERYNEWPALETTE       0x030F
#define WINWM_PALETTEISCHANGING     0x0310
#define WINWM_PALETTECHANGED        0x0311
#define WINWM_HOTKEY                0x0312
#define WINWM_DDE_FIRST             0x03E0
#define WINWM_DDE_INITIATE          (WINWM_DDE_FIRST)
#define WINWM_DDE_TERMINATE         (WINWM_DDE_FIRST + 1)
#define WINWM_DDE_ADVISE            (WINWM_DDE_FIRST + 2)
#define WINWM_DDE_UNADVISE          (WINWM_DDE_FIRST + 3)
#define WINWM_DDE_ACK               (WINWM_DDE_FIRST + 4)
#define WINWM_DDE_DATA              (WINWM_DDE_FIRST + 5)
#define WINWM_DDE_REQUEST           (WINWM_DDE_FIRST + 6)
#define WINWM_DDE_POKE              (WINWM_DDE_FIRST + 7)
#define WINWM_DDE_EXECUTE           (WINWM_DDE_FIRST + 8)
#define WINWM_DDE_LAST              (WINWM_DDE_FIRST + 8)
#define WINWM_USER                  0x0400

/*******
#define OSWM_NULL                    0x0000
#define OSWM_CREATE                  0x0001
#define OSWM_DESTROY                 0x0002
#define OSWM_ENABLE                  0x0004
#define OSWM_SHOW                    0x0005
#define OSWM_MOVE                    0x0006
#define OSWM_SIZE                    0x0007
#define OSWM_ADJUSTWINDOWPOS         0x0008
#define OSWM_CALCVALIDRECTS          0x0009
#define OSWM_SETWINDOWPARAMS         0x000a
#define OSWM_QUERYWINDOWPARAMS       0x000b
#define OSWM_HITTEST                 0x000c
#define OSWM_ACTIVATE                0x000d
#define OSWM_SETFOCUS                0x000f
#define OSWM_SETSELECTION            0x0010
#define OSWM_PPAINT                  0x0011
#define OSWM_PSETFOCUS               0x0012
#define OSWM_PSYSCOLORCHANGE         0x0013
#define OSWM_PSIZE                   0x0014
#define OSWM_PACTIVATE               0x0015
#define OSWM_PCONTROL                0x0016
#define OSWM_COMMAND                 0x0020
#define OSWM_SYSCOMMAND              0x0021
#define OSWM_HELP                    0x0022
#define OSWM_PAINT                   0x0023
#define OSWM_TIMER                   0x0024
#define OSWM_SEM1                    0x0025
#define OSWM_SEM2                    0x0026
#define OSWM_SEM3                    0x0027
#define OSWM_SEM4                    0x0028
#define OSWM_CLOSE                   0x0029
#define OSWM_QUIT                    0x002a
#define OSWM_SYSCOLORCHANGE          0x002b
#define OSWM_SYSVALUECHANGED         0x002d
#define OSWM_APPTERMINATENOTIFY      0x002e
#define OSWM_PRESPARAMCHANGED        0x002f
#define OSWM_CONTROL                 0x0030
#define OSWM_VSCROLL                 0x0031
#define OSWM_HSCROLL                 0x0032
#define OSWM_INITMENU                0x0033
#define OSWM_MENUSELECT              0x0034
#define OSWM_MENUEND                 0x0035
#define OSWM_DRAWITEM                0x0036
#define OSWM_MEASUREITEM             0x0037
#define OSWM_CONTROLPOINTER          0x0038
#define OSWM_QUERYDLGCODE            0x003a
#define OSWM_INITDLG                 0x003b
#define OSWM_SUBSTITUTESTRING        0x003c
#define OSWM_MATCHMNEMONIC           0x003d
#define OSWM_SAVEAPPLICATION         0x003e
#define OSWM_FLASHWINDOW             0x0040
#define OSWM_FORMATFRAME             0x0041
#define OSWM_UPDATEFRAME             0x0042
#define OSWM_FOCUSCHANGE             0x0043
#define OSWM_SETBORDERSIZE           0x0044
#define OSWM_TRACKFRAME              0x0045
#define OSWM_MINMAXFRAME             0x0046
#define OSWM_SETICON                 0x0047
#define OSWM_QUERYICON               0x0048
#define OSWM_SETACCELTABLE           0x0049
#define OSWM_QUERYACCELTABLE         0x004a
#define OSWM_TRANSLATEACCEL          0x004b
#define OSWM_QUERYTRACKINFO          0x004c
#define OSWM_QUERYBORDERSIZE         0x004d
#define OSWM_NEXTMENU                0x004e
#define OSWM_ERASEBACKGROUND         0x004f
#define OSWM_QUERYFRAMEINFO          0x0050
#define OSWM_QUERYFOCUSCHAIN         0x0051
#define OSWM_OWNERPOSCHANGE          0x0052
#define OSWM_CALCFRAMERECT           0x0053
#define OSWM_WINDOWPOSCHANGED        0x0055
#define OSWM_ADJUSTFRAMEPOS          0x0056
#define OSWM_QUERYFRAMECTLCOUNT      0x0059
#define OSWM_QUERYHELPINFO           0x005B
#define OSWM_SETHELPINFO             0x005C
#define OSWM_ERROR                   0x005D
#define OSWM_REALIZEPALETTE          0x005E
#define OSWM_RENDERFMT               0x0060
#define OSWM_RENDERALLFMTS           0x0061
#define OSWM_DESTROYCLIPBOARD        0x0062
#define OSWM_PAINTCLIPBOARD          0x0063
#define OSWM_SIZECLIPBOARD           0x0064
#define OSWM_HSCROLLCLIPBOARD        0x0065
#define OSWM_VSCROLLCLIPBOARD        0x0066
#define OSWM_DRAWCLIPBOARD           0x0067
#define OSWM_MOUSEMOVE               0x0070
#define OSWM_BUTTON1DOWN             0x0071
#define OSWM_BUTTON1UP               0x0072
#define OSWM_BUTTON1DBLCLK           0x0073
#define OSWM_BUTTON2DOWN             0x0074
#define OSWM_BUTTON2UP               0x0075
#define OSWM_BUTTON2DBLCLK           0x0076
#define OSWM_BUTTON3DOWN             0x0077
#define OSWM_BUTTON3UP               0x0078
#define OSWM_BUTTON3DBLCLK           0x0079
#define OSWM_CHAR                    0x007a
#define OSWM_VIOCHAR                 0x007b
#define OSWM_JOURNALNOTIFY           0x007c
#define OSWM_MOUSEMAP                0x007D
#define OSWM_VRNDISABLED             0x007e
#define OSWM_VRNENABLED              0x007f
#define OSWM_DDE_FIRST               0x00A0
#define OSWM_DDE_INITIATE            0x00A0
#define OSWM_DDE_REQUEST             0x00A1
#define OSWM_DDE_ACK                 0x00A2
#define OSWM_DDE_DATA                0x00A3
#define OSWM_DDE_ADVISE              0x00A4
#define OSWM_DDE_UNADVISE            0x00A5
#define OSWM_DDE_POKE                0x00A6
#define OSWM_DDE_EXECUTE             0x00A7
#define OSWM_DDE_TERMINATE           0x00A8
#define OSWM_DDE_INITIATEACK         0x00A9
#define OSWM_DDE_LAST                0x00AF
#define OSWM_QUERYCONVERTPOS         0x00b0
#define OSWM_DBCSFIRST               0x00b0
#define OSWM_DBCSLAST                0x00cf
#define OSWM_MSGBOXINIT              0x010E
#define OSWM_MSGBOXDISMISS           0x010F
#define OSWM_CTLCOLORCHANGE          0x0129
#define OSWM_QUERYCTLTYPE            0x0130
#define OSWM_EXTMOUSEFIRST           0x0410
#define OSWM_EXTMOUSELAST            0x0419
#define OSWM_CHORD                   0x0410
#define OSWM_BUTTON1MOTIONSTART      0x0411
#define OSWM_BUTTON1MOTIONEND        0x0412
#define OSWM_BUTTON1CLICK            0x0413
#define OSWM_BUTTON2MOTIONSTART      0x0414
#define OSWM_BUTTON2MOTIONEND        0x0415
#define OSWM_BUTTON2CLICK            0x0416
#define OSWM_BUTTON3MOTIONSTART      0x0417
#define OSWM_BUTTON3MOTIONEND        0x0418
#define OSWM_BUTTON3CLICK            0x0419
#define OSWM_MOUSETRANSLATEFIRST     0x0420
#define OSWM_MOUSETRANSLATELAST      0x0428
#define OSWM_BEGINDRAG               0x0420
#define OSWM_ENDDRAG                 0x0421
#define OSWM_SINGLESELECT            0x0422
#define OSWM_OPEN                    0x0423
#define OSWM_CONTEXTMENU             0x0424
#define OSWM_CONTEXTHELP             0x0425
#define OSWM_TEXTEDIT                0x0426
#define OSWM_BEGINSELECT             0x0427
#define OSWM_ENDSELECT               0x0428
#define OSWM_PICKUP                  0x0429
#define OSWM_PENFIRST                0x04C0
#define OSWM_PENLAST                 0x04FF
#define OSWM_MMPMFIRST               0x0500
#define OSWM_MMPMLAST                0x05FF
#define OSWM_STDDLGFIRST             0x0600
#define OSWM_STDDLGLAST              0x065F
#define OSWM_BIDI_FIRST              0x0BD0
#define OSWM_BIDI_LAST               0x0BFF
#define OSWM_USER                    0x1000
************/

#endif //__OSLIBMSG_H__
