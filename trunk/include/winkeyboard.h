/* $Id: winkeyboard.h,v 1.2 2001-10-26 11:28:59 phaller Exp $ */
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


#endif //__WINKEYBOARD_H__
