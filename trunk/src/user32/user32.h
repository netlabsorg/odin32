/* $Id: user32.h,v 1.2 1999-05-31 22:08:16 phaller Exp $ */

/*****************************************************************************
 * Name      : USER32.H
 * Purpose   : This module maps all Win32 functions contained in USER32.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *****************************************************************************/


#ifndef __USER32_H_
#define __USER32_H_

#include <os2win.h>
#include "misc.h"

#include <nameid.h>
#include <stdlib.h>
#include <string.h>

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
