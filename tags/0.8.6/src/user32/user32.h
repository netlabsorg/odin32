/* $Id: user32.h,v 1.9 2001-04-04 09:01:25 sandervl Exp $ */

/*****************************************************************************
 * Name      : USER32.H
 * Purpose   : This module maps all Win32 functions contained in USER32.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *****************************************************************************/


#ifndef __USER32_H_
#define __USER32_H_

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <unicode.h>

inline void ConvertFontAW(LOGFONTA *fa, LOGFONTW *fw)
{
   memcpy(fw, fa, ((int)&fa->lfFaceName - (int)fa));
   AsciiToUnicode(fa->lfFaceName, (LPWSTR)fw->lfFaceName);
   return;
}

inline void ConvertFontWA(LOGFONTW *fw, LOGFONTA *fa)
{
   memcpy(fa, fa, ((int)&fw->lfFaceName - (int)fw));
   UnicodeToAscii((LPWSTR)fw->lfFaceName, fa->lfFaceName);
   return;
}

#endif
