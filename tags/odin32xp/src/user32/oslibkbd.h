/* $Id: oslibkbd.h,v 1.1 2003-02-16 15:31:10 sandervl Exp $ */
/*
 * Keyboard helpers for OS/2
 *
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBKBD_H__
#define __OSLIBKBD_H__

SHORT  OSLibWinGetScanState(USHORT scan);
BOOL   OSLibWinGetScanStateTable(unsigned char *PMScanState);
LONG   OSLibWinGetPhysKeyState(LONG scan);
BOOL   OSLibWinGetKeyboardStateTable(unsigned char * PMKeyState);
BOOL   OSLibWinSetKeyboardStateTable(unsigned char * PMKeyState);

#define TCF_LSHIFT                 0x0001
#define TCF_RSHIFT                 0x0002
#define TCF_SHIFT                  (TCF_LSHIFT | TCF_RSHIFT)
#define TCF_LCONTROL               0x0004
#define TCF_RCONTROL               0x0008
#define TCF_CONTROL                (TCF_LCONTROL | TCF_RCONTROL)
#define TCF_ALT                    0x0010
#define TCF_ALTGR                  0x0020
#define TCF_CAPSLOCK               0x0040
#define TCF_NUMLOCK                0x0080
#define TCF_OEMSCANCODE            0x0100
#define TCF_EXTENDEDKEY            0x0200

#define TC_CHARTOSCANCODE          0
#define TC_SCANCODETOCHAR          1
#define TC_VIRTUALKEYTOSCANCODE    2
#define TC_SCANCODETOVIRTUALKEY    3
#define TC_SCANTOOEMSCAN           4
#define TC_OEMSCANTOSCAN           5

USHORT OSLibWinTranslateChar(USHORT usScanCode, ULONG type, USHORT shiftstate);

#endif //__OSLIBKBD_H__
