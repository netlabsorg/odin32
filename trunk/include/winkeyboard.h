/* $Id: winkeyboard.h,v 1.1 1999-11-08 13:45:35 sandervl Exp $ */
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

#endif //__WINKEYBOARD_H__
