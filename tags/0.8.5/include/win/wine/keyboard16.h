/* $Id: keyboard16.h,v 1.5 1999-08-22 22:52:05 sandervl Exp $ */
#ifndef __WINE_WINE_KEYBOARD16_H
#define __WINE_WINE_KEYBOARD16_H

#include "windef.h"

INT16       WINAPI ToAscii16(UINT16,UINT16,LPBYTE,LPVOID,UINT16);
WORD        WINAPI VkKeyScan16(CHAR);
INT16       WINAPI AnsiToOem16(LPCSTR,LPSTR);
VOID        WINAPI AnsiToOemBuff16(LPCSTR,LPSTR,UINT16);
INT16       WINAPI OemToAnsi16(LPCSTR,LPSTR);
VOID        WINAPI OemToAnsiBuff16(LPCSTR,LPSTR,UINT16);


#endif /* __WINE_WINE_KEYBOARD16_H */
