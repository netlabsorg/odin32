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

#ifdef __cplusplus
extern "C" {
#endif

void WIN32API KeyTranslatePMToWinBuf(PUCHAR pmkey, PUCHAR winkey, int nrkeys);
void WIN32API KeyTranslatePMScanToWinVKey(UCHAR uchPMScan,
                                          BOOL bNumLock,
                                          PUCHAR puchWinVKey,
                                          WORD *pwWinScan,
                                          PBOOL pfExtended);
UCHAR WIN32API KeyTranslateWinVKeyToPMScan(UCHAR uchWinVKey, BOOL fExtended);
UCHAR WIN32API KeyTranslateWinScanToPMScan(UCHAR uchWinScan, BOOL fExtended);

#define KEYOVERLAYSTATE_DONTCARE 0
#define KEYOVERLAYSTATE_DOWN     1
#define KEYOVERLAYSTATE_UP       2

void WIN32API KeySetOverlayKeyState(int nVirtKey,
                                    char nState);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__WINKEYBOARD_H__
