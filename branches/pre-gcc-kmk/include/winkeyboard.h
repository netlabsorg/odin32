/* $Id: winkeyboard.h,v 1.3 2001-11-09 15:39:12 phaller Exp $ */
/*
 * Win32 <-> PM key translation
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINKEYBOARD_H__
#define __WINKEYBOARD_H__



BYTE WIN32API KeyTranslatePMToWin(BYTE key);
void WIN32API KeyTranslatePMToWinBuf(BYTE *pmkey, BYTE *winkey, int nrkeys);
void WIN32API KeyTranslatePMScanToWinVKey(BYTE bPMScan,
                                          BOOL bNumLock,
                                          PBYTE pbWinVKey,
                                          WORD* pwWinScan,
                                          PBOOL pfExtended);
BYTE WIN32API KeyTranslateWinVKeyToPMScan(BYTE bWinVKey, BOOL fExtended);
BYTE WIN32API KeyTranslateWinScanToPMScan(BYTE bWinScan, BOOL fExtended);

#define KEYOVERLAYSTATE_DONTCARE 0
#define KEYOVERLAYSTATE_DOWN     1
#define KEYOVERLAYSTATE_UP       2

void WIN32API KeySetOverlayKeyState(int nVirtKey,
                                    char nState);


#endif //__WINKEYBOARD_H__
