/* $Id: KERNEL32.CPP,v 1.49 2000-07-06 21:18:38 sandervl Exp $ */

/*
 * Win32 compatibility file functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1998 Knut St. Osmundsen
 *
 * @(#) KERNEL32.CPP            1.0.1   1998/06/12 PH added HandleManager support
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "misc.h"
#include <builtin.h>
#include "heap.h"
#include "handlemanager.h"
#include "wprocess.h"
#include <versionos2.h>

#define DBG_LOCALLOG	DBG_kernel32
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(KERNEL32-KERNEL32)

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1



/*****************************************************************************
 * Name      : BOOL WIN32API CloseHandle
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/
BOOL WIN32API CloseHandle(HANDLE hHandle)
{
  dprintf(("KERNEL32: CloseHandle(%08xh)\n",
           hHandle));

  return HMCloseHandle(hHandle);
}


//******************************************************************************
HANDLE WIN32API GetStdHandle(DWORD fdwDevice)
{
 HANDLE handle;

  /* @@@PH 1998/02/12 Handle Manager Support */
  handle = HMGetStdHandle(fdwDevice);

  //@@@PH translate handle

  /* handle = GetStdHandle(fdwDevice); */
  dprintf(("KERNEL32:  GetStdHandle for device %X returned %X\n", fdwDevice, handle));
  return(handle);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetStdHandle(DWORD  IDStdHandle,
                              HANDLE hHandle)
{
  dprintf(("KERNEL32:  SetStdHandle\n"));

  ///@@@PH translate handle

  return (HMSetStdHandle(IDStdHandle,
                         hHandle));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadWritePtr(LPVOID lpvPtr, UINT cbBytes)
{
#ifdef DEBUG
 BOOL rc;

  rc = O32_IsBadWritePtr(lpvPtr, cbBytes);
  dprintf(("KERNEL32:  IsBadWritePtr: 0x%X size %d rc = %d\n", (int)lpvPtr, cbBytes, rc));
  return(rc);
#else
  return(O32_IsBadWritePtr(lpvPtr, cbBytes));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadReadPtr(CONST VOID *lpvPtr, UINT cbBytes)
{
#ifdef DEBUG
 BOOL rc;

  rc = O32_IsBadReadPtr(lpvPtr, cbBytes);
  dprintf(("KERNEL32:  IsBadWritePtr: 0x%X size %d rc = %d\n", (int)lpvPtr, cbBytes, rc));
  return(rc);
#else
  return(O32_IsBadReadPtr(lpvPtr, cbBytes));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadCodePtr( FARPROC arg1)
{
    dprintf(("KERNEL32:  IsBadCodePtr\n"));
    return O32_IsBadCodePtr(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadStringPtrA( LPCSTR arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  IsBadStringPtr"));
    return O32_IsBadStringPtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadStringPtrW(LPCWSTR arg1, UINT arg2)
{
    dprintf(("KERNEL32:  OS2IsBadStringPtrW"));
    return O32_IsBadReadPtr((CONST VOID *)arg1, arg2*2+2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetLastError()
{
 DWORD rc;

  rc = O32_GetLastError();
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32:  GetLastError returned %d\n", rc));
#endif
  return(rc);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API SetLastError( DWORD dwError)
{
    if(dwError != 0) {
    	dprintf2(("KERNEL32:  SetLastError to %d\n", dwError));
    }
    O32_SetLastError(dwError);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOEMCP(VOID)
{
  dprintf(("KERNEL32:  GetOEMCP\n"));
  return(O32_GetOEMCP());
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetACP(VOID)
{
  dprintf(("KERNEL32:  GetACP\n"));
  return(O32_GetACP());
}
//******************************************************************************
//******************************************************************************

static const unsigned char CT_CType2_LUT[] = {
  C2_NOTAPPLICABLE, /*   -   0 */
  C2_NOTAPPLICABLE, /*   -   1 */
  C2_NOTAPPLICABLE, /*   -   2 */
  C2_NOTAPPLICABLE, /*   -   3 */
  C2_NOTAPPLICABLE, /*   -   4 */
  C2_NOTAPPLICABLE, /*   -   5 */
  C2_NOTAPPLICABLE, /*   -   6 */
  C2_NOTAPPLICABLE, /*   -   7 */
  C2_NOTAPPLICABLE, /*   -   8 */
  C2_SEGMENTSEPARATOR, /*   -   9 */
  C2_NOTAPPLICABLE, /*   -  10 */
  C2_NOTAPPLICABLE, /*   -  11 */
  C2_NOTAPPLICABLE, /*   -  12 */
  C2_NOTAPPLICABLE, /*   -  13 */
  C2_NOTAPPLICABLE, /*   -  14 */
  C2_NOTAPPLICABLE, /*   -  15 */
  C2_NOTAPPLICABLE, /*   -  16 */
  C2_NOTAPPLICABLE, /*   -  17 */
  C2_NOTAPPLICABLE, /*   -  18 */
  C2_NOTAPPLICABLE, /*   -  19 */
  C2_NOTAPPLICABLE, /*   -  20 */
  C2_NOTAPPLICABLE, /*   -  21 */
  C2_NOTAPPLICABLE, /*   -  22 */
  C2_NOTAPPLICABLE, /*   -  23 */
  C2_NOTAPPLICABLE, /*   -  24 */
  C2_NOTAPPLICABLE, /*   -  25 */
  C2_NOTAPPLICABLE, /*   -  26 */
  C2_NOTAPPLICABLE, /*   -  27 */
  C2_NOTAPPLICABLE, /*   -  28 */
  C2_NOTAPPLICABLE, /*   -  29 */
  C2_NOTAPPLICABLE, /*   -  30 */
  C2_NOTAPPLICABLE, /*   -  31 */
  C2_WHITESPACE, /*   -  32 */
  C2_OTHERNEUTRAL, /* ! -  33 */
  C2_OTHERNEUTRAL, /* " -  34 */ /* " */
  C2_EUROPETERMINATOR, /* # -  35 */
  C2_EUROPETERMINATOR, /* $ -  36 */
  C2_EUROPETERMINATOR, /* % -  37 */
  C2_LEFTTORIGHT, /* & -  38 */
  C2_OTHERNEUTRAL, /* ' -  39 */
  C2_OTHERNEUTRAL, /* ( -  40 */
  C2_OTHERNEUTRAL, /* ) -  41 */
  C2_OTHERNEUTRAL, /* * -  42 */
  C2_EUROPETERMINATOR, /* + -  43 */
  C2_COMMONSEPARATOR, /* , -  44 */
  C2_EUROPETERMINATOR, /* - -  45 */
  C2_EUROPESEPARATOR, /* . -  46 */
  C2_EUROPESEPARATOR, /* / -  47 */
  C2_EUROPENUMBER, /* 0 -  48 */
  C2_EUROPENUMBER, /* 1 -  49 */
  C2_EUROPENUMBER, /* 2 -  50 */
  C2_EUROPENUMBER, /* 3 -  51 */
  C2_EUROPENUMBER, /* 4 -  52 */
  C2_EUROPENUMBER, /* 5 -  53 */
  C2_EUROPENUMBER, /* 6 -  54 */
  C2_EUROPENUMBER, /* 7 -  55 */
  C2_EUROPENUMBER, /* 8 -  56 */
  C2_EUROPENUMBER, /* 9 -  57 */
  C2_COMMONSEPARATOR, /* : -  58 */
  C2_OTHERNEUTRAL, /* ; -  59 */
  C2_OTHERNEUTRAL, /* < -  60 */
  C2_OTHERNEUTRAL, /* = -  61 */
  C2_OTHERNEUTRAL, /* > -  62 */
  C2_OTHERNEUTRAL, /* ? -  63 */
  C2_LEFTTORIGHT, /* @ -  64 */
  C2_LEFTTORIGHT, /* A -  65 */
  C2_LEFTTORIGHT, /* B -  66 */
  C2_LEFTTORIGHT, /* C -  67 */
  C2_LEFTTORIGHT, /* D -  68 */
  C2_LEFTTORIGHT, /* E -  69 */
  C2_LEFTTORIGHT, /* F -  70 */
  C2_LEFTTORIGHT, /* G -  71 */
  C2_LEFTTORIGHT, /* H -  72 */
  C2_LEFTTORIGHT, /* I -  73 */
  C2_LEFTTORIGHT, /* J -  74 */
  C2_LEFTTORIGHT, /* K -  75 */
  C2_LEFTTORIGHT, /* L -  76 */
  C2_LEFTTORIGHT, /* M -  77 */
  C2_LEFTTORIGHT, /* N -  78 */
  C2_LEFTTORIGHT, /* O -  79 */
  C2_LEFTTORIGHT, /* P -  80 */
  C2_LEFTTORIGHT, /* Q -  81 */
  C2_LEFTTORIGHT, /* R -  82 */
  C2_LEFTTORIGHT, /* S -  83 */
  C2_LEFTTORIGHT, /* T -  84 */
  C2_LEFTTORIGHT, /* U -  85 */
  C2_LEFTTORIGHT, /* V -  86 */
  C2_LEFTTORIGHT, /* W -  87 */
  C2_LEFTTORIGHT, /* X -  88 */
  C2_LEFTTORIGHT, /* Y -  89 */
  C2_LEFTTORIGHT, /* Z -  90 */
  C2_OTHERNEUTRAL, /* [ -  91 */
  C2_OTHERNEUTRAL, /* \ -  92 */
  C2_OTHERNEUTRAL, /* ] -  93 */
  C2_OTHERNEUTRAL, /* ^ -  94 */
  C2_OTHERNEUTRAL, /* _ -  95 */
  C2_OTHERNEUTRAL, /* ` -  96 */
  C2_LEFTTORIGHT, /* a -  97 */
  C2_LEFTTORIGHT, /* b -  98 */
  C2_LEFTTORIGHT, /* c -  99 */
  C2_LEFTTORIGHT, /* d - 100 */
  C2_LEFTTORIGHT, /* e - 101 */
  C2_LEFTTORIGHT, /* f - 102 */
  C2_LEFTTORIGHT, /* g - 103 */
  C2_LEFTTORIGHT, /* h - 104 */
  C2_LEFTTORIGHT, /* i - 105 */
  C2_LEFTTORIGHT, /* j - 106 */
  C2_LEFTTORIGHT, /* k - 107 */
  C2_LEFTTORIGHT, /* l - 108 */
  C2_LEFTTORIGHT, /* m - 109 */
  C2_LEFTTORIGHT, /* n - 110 */
  C2_LEFTTORIGHT, /* o - 111 */
  C2_LEFTTORIGHT, /* p - 112 */
  C2_LEFTTORIGHT, /* q - 113 */
  C2_LEFTTORIGHT, /* r - 114 */
  C2_LEFTTORIGHT, /* s - 115 */
  C2_LEFTTORIGHT, /* t - 116 */
  C2_LEFTTORIGHT, /* u - 117 */
  C2_LEFTTORIGHT, /* v - 118 */
  C2_LEFTTORIGHT, /* w - 119 */
  C2_LEFTTORIGHT, /* x - 120 */
  C2_LEFTTORIGHT, /* y - 121 */
  C2_LEFTTORIGHT, /* z - 122 */
  C2_OTHERNEUTRAL, /* { - 123 */
  C2_OTHERNEUTRAL, /* | - 124 */
  C2_OTHERNEUTRAL, /* } - 125 */
  C2_OTHERNEUTRAL, /* ~ - 126 */
  C2_NOTAPPLICABLE, /*  - 127 */
  C2_NOTAPPLICABLE, /* Ä - 128 */
  C2_NOTAPPLICABLE, /* Å - 129 */
  C2_OTHERNEUTRAL, /* Ç - 130 */
  C2_LEFTTORIGHT, /* É - 131 */
  C2_OTHERNEUTRAL, /* Ñ - 132 */
  C2_OTHERNEUTRAL, /* Ö - 133 */
  C2_OTHERNEUTRAL, /* Ü - 134 */
  C2_OTHERNEUTRAL, /* á - 135 */
  C2_LEFTTORIGHT, /* à - 136 */
  C2_EUROPETERMINATOR, /* â - 137 */
  C2_LEFTTORIGHT, /* ä - 138 */
  C2_OTHERNEUTRAL, /* ã - 139 */
  C2_LEFTTORIGHT, /* å - 140 */
  C2_NOTAPPLICABLE, /* ç - 141 */
  C2_NOTAPPLICABLE, /* é - 142 */
  C2_NOTAPPLICABLE, /* è - 143 */
  C2_NOTAPPLICABLE, /* ê - 144 */
  C2_OTHERNEUTRAL, /* ë - 145 */
  C2_OTHERNEUTRAL, /* í - 146 */
  C2_OTHERNEUTRAL, /* ì - 147 */
  C2_OTHERNEUTRAL, /* î - 148 */
  C2_OTHERNEUTRAL, /* ï - 149 */
  C2_OTHERNEUTRAL, /* ñ - 150 */
  C2_OTHERNEUTRAL, /* ó - 151 */
  C2_LEFTTORIGHT, /* ò - 152 */
  C2_OTHERNEUTRAL, /* ô - 153 */
  C2_LEFTTORIGHT, /* ö - 154 */
  C2_OTHERNEUTRAL, /* õ - 155 */
  C2_LEFTTORIGHT, /* ú - 156 */
  C2_NOTAPPLICABLE, /* ù - 157 */
  C2_NOTAPPLICABLE, /* û - 158 */
  C2_LEFTTORIGHT, /* ü - 159 */
  C2_WHITESPACE, /* † - 160 */
  C2_OTHERNEUTRAL, /* ° - 161 */
  C2_EUROPETERMINATOR, /* ¢ - 162 */
  C2_EUROPETERMINATOR, /* £ - 163 */
  C2_EUROPETERMINATOR, /* § - 164 */
  C2_EUROPETERMINATOR, /* • - 165 */
  C2_OTHERNEUTRAL, /* ¶ - 166 */
  C2_OTHERNEUTRAL, /* ß - 167 */
  C2_OTHERNEUTRAL, /* ® - 168 */
  C2_OTHERNEUTRAL, /* © - 169 */
  C2_OTHERNEUTRAL, /* ™ - 170 */
  C2_OTHERNEUTRAL, /* ´ - 171 */
  C2_OTHERNEUTRAL, /* ¨ - 172 */
  C2_OTHERNEUTRAL, /* ≠ - 173 */
  C2_OTHERNEUTRAL, /* Æ - 174 */
  C2_OTHERNEUTRAL, /* Ø - 175 */
  C2_EUROPETERMINATOR, /* ∞ - 176 */
  C2_EUROPETERMINATOR, /* ± - 177 */
  C2_EUROPENUMBER, /* ≤ - 178 */
  C2_EUROPENUMBER, /* ≥ - 179 */
  C2_OTHERNEUTRAL, /* ¥ - 180 */
  C2_OTHERNEUTRAL, /* µ - 181 */
  C2_OTHERNEUTRAL, /* ∂ - 182 */
  C2_OTHERNEUTRAL, /* ∑ - 183 */
  C2_OTHERNEUTRAL, /* ∏ - 184 */
  C2_EUROPENUMBER, /* π - 185 */
  C2_OTHERNEUTRAL, /* ∫ - 186 */
  C2_OTHERNEUTRAL, /* ª - 187 */
  C2_OTHERNEUTRAL, /* º - 188 */
  C2_OTHERNEUTRAL, /* Ω - 189 */
  C2_OTHERNEUTRAL, /* æ - 190 */
  C2_OTHERNEUTRAL, /* ø - 191 */
  C2_LEFTTORIGHT, /* ¿ - 192 */
  C2_LEFTTORIGHT, /* ¡ - 193 */
  C2_LEFTTORIGHT, /* ¬ - 194 */
  C2_LEFTTORIGHT, /* √ - 195 */
  C2_LEFTTORIGHT, /* ƒ - 196 */
  C2_LEFTTORIGHT, /* ≈ - 197 */
  C2_LEFTTORIGHT, /* ∆ - 198 */
  C2_LEFTTORIGHT, /* « - 199 */
  C2_LEFTTORIGHT, /* » - 200 */
  C2_LEFTTORIGHT, /* … - 201 */
  C2_LEFTTORIGHT, /*   - 202 */
  C2_LEFTTORIGHT, /* À - 203 */
  C2_LEFTTORIGHT, /* Ã - 204 */
  C2_LEFTTORIGHT, /* Õ - 205 */
  C2_LEFTTORIGHT, /* Œ - 206 */
  C2_LEFTTORIGHT, /* œ - 207 */
  C2_LEFTTORIGHT, /* – - 208 */
  C2_LEFTTORIGHT, /* — - 209 */
  C2_LEFTTORIGHT, /* “ - 210 */
  C2_LEFTTORIGHT, /* ” - 211 */
  C2_LEFTTORIGHT, /* ‘ - 212 */
  C2_LEFTTORIGHT, /* ’ - 213 */
  C2_LEFTTORIGHT, /* ÷ - 214 */
  C2_OTHERNEUTRAL, /* ◊ - 215 */
  C2_LEFTTORIGHT, /* ÿ - 216 */
  C2_LEFTTORIGHT, /* Ÿ - 217 */
  C2_LEFTTORIGHT, /* ⁄ - 218 */
  C2_LEFTTORIGHT, /* € - 219 */
  C2_LEFTTORIGHT, /* ‹ - 220 */
  C2_LEFTTORIGHT, /* › - 221 */
  C2_LEFTTORIGHT, /* ﬁ - 222 */
  C2_LEFTTORIGHT, /* ﬂ - 223 */
  C2_LEFTTORIGHT, /* ‡ - 224 */
  C2_LEFTTORIGHT, /* · - 225 */
  C2_LEFTTORIGHT, /* ‚ - 226 */
  C2_LEFTTORIGHT, /* „ - 227 */
  C2_LEFTTORIGHT, /* ‰ - 228 */
  C2_LEFTTORIGHT, /* Â - 229 */
  C2_LEFTTORIGHT, /* Ê - 230 */
  C2_LEFTTORIGHT, /* Á - 231 */
  C2_LEFTTORIGHT, /* Ë - 232 */
  C2_LEFTTORIGHT, /* È - 233 */
  C2_LEFTTORIGHT, /* Í - 234 */
  C2_LEFTTORIGHT, /* Î - 235 */
  C2_LEFTTORIGHT, /* Ï - 236 */
  C2_LEFTTORIGHT, /* Ì - 237 */
  C2_LEFTTORIGHT, /* Ó - 238 */
  C2_LEFTTORIGHT, /* Ô - 239 */
  C2_LEFTTORIGHT, /*  - 240 */
  C2_LEFTTORIGHT, /* Ò - 241 */
  C2_LEFTTORIGHT, /* Ú - 242 */
  C2_LEFTTORIGHT, /* Û - 243 */
  C2_LEFTTORIGHT, /* Ù - 244 */
  C2_LEFTTORIGHT, /* ı - 245 */
  C2_LEFTTORIGHT, /* ˆ - 246 */
  C2_OTHERNEUTRAL, /* ˜ - 247 */
  C2_LEFTTORIGHT, /* ¯ - 248 */
  C2_LEFTTORIGHT, /* ˘ - 249 */
  C2_LEFTTORIGHT, /* ˙ - 250 */
  C2_LEFTTORIGHT, /* ˚ - 251 */
  C2_LEFTTORIGHT, /* ¸ - 252 */
  C2_LEFTTORIGHT, /* ˝ - 253 */
  C2_LEFTTORIGHT, /* ˛ - 254 */
  C2_LEFTTORIGHT /* ˇ - 255 */
};

const WORD OLE2NLS_CT_CType3_LUT[] = {
  0x0000, /*   -   0 */
  0x0000, /*   -   1 */
  0x0000, /*   -   2 */
  0x0000, /*   -   3 */
  0x0000, /*   -   4 */
  0x0000, /*   -   5 */
  0x0000, /*   -   6 */
  0x0000, /*   -   7 */
  0x0000, /*   -   8 */
  0x0008, /*   -   9 */
  0x0008, /*   -  10 */
  0x0008, /*   -  11 */
  0x0008, /*   -  12 */
  0x0008, /*   -  13 */
  0x0000, /*   -  14 */
  0x0000, /*   -  15 */
  0x0000, /*   -  16 */
  0x0000, /*   -  17 */
  0x0000, /*   -  18 */
  0x0000, /*   -  19 */
  0x0000, /*   -  20 */
  0x0000, /*   -  21 */
  0x0000, /*   -  22 */
  0x0000, /*   -  23 */
  0x0000, /*   -  24 */
  0x0000, /*   -  25 */
  0x0000, /*   -  26 */
  0x0000, /*   -  27 */
  0x0000, /*   -  28 */
  0x0000, /*   -  29 */
  0x0000, /*   -  30 */
  0x0000, /*   -  31 */
  0x0048, /*   -  32 */
  0x0048, /* ! -  33 */
  0x0448, /* " -  34 */ /* " */
  0x0048, /* # -  35 */
  0x0448, /* $ -  36 */
  0x0048, /* % -  37 */
  0x0048, /* & -  38 */
  0x0440, /* ' -  39 */
  0x0048, /* ( -  40 */
  0x0048, /* ) -  41 */
  0x0048, /* * -  42 */
  0x0048, /* + -  43 */
  0x0048, /* , -  44 */
  0x0440, /* - -  45 */
  0x0048, /* . -  46 */
  0x0448, /* / -  47 */
  0x0040, /* 0 -  48 */
  0x0040, /* 1 -  49 */
  0x0040, /* 2 -  50 */
  0x0040, /* 3 -  51 */
  0x0040, /* 4 -  52 */
  0x0040, /* 5 -  53 */
  0x0040, /* 6 -  54 */
  0x0040, /* 7 -  55 */
  0x0040, /* 8 -  56 */
  0x0040, /* 9 -  57 */
  0x0048, /* : -  58 */
  0x0048, /* ; -  59 */
  0x0048, /* < -  60 */
  0x0448, /* = -  61 */
  0x0048, /* > -  62 */
  0x0048, /* ? -  63 */
  0x0448, /* @ -  64 */
  0x8040, /* A -  65 */
  0x8040, /* B -  66 */
  0x8040, /* C -  67 */
  0x8040, /* D -  68 */
  0x8040, /* E -  69 */
  0x8040, /* F -  70 */
  0x8040, /* G -  71 */
  0x8040, /* H -  72 */
  0x8040, /* I -  73 */
  0x8040, /* J -  74 */
  0x8040, /* K -  75 */
  0x8040, /* L -  76 */
  0x8040, /* M -  77 */
  0x8040, /* N -  78 */
  0x8040, /* O -  79 */
  0x8040, /* P -  80 */
  0x8040, /* Q -  81 */
  0x8040, /* R -  82 */
  0x8040, /* S -  83 */
  0x8040, /* T -  84 */
  0x8040, /* U -  85 */
  0x8040, /* V -  86 */
  0x8040, /* W -  87 */
  0x8040, /* X -  88 */
  0x8040, /* Y -  89 */
  0x8040, /* Z -  90 */
  0x0048, /* [ -  91 */
  0x0448, /* \ -  92 */
  0x0048, /* ] -  93 */
  0x0448, /* ^ -  94 */
  0x0448, /* _ -  95 */
  0x0448, /* ` -  96 */
  0x8040, /* a -  97 */
  0x8040, /* b -  98 */
  0x8040, /* c -  99 */
  0x8040, /* d - 100 */
  0x8040, /* e - 101 */
  0x8040, /* f - 102 */
  0x8040, /* g - 103 */
  0x8040, /* h - 104 */
  0x8040, /* i - 105 */
  0x8040, /* j - 106 */
  0x8040, /* k - 107 */
  0x8040, /* l - 108 */
  0x8040, /* m - 109 */
  0x8040, /* n - 110 */
  0x8040, /* o - 111 */
  0x8040, /* p - 112 */
  0x8040, /* q - 113 */
  0x8040, /* r - 114 */
  0x8040, /* s - 115 */
  0x8040, /* t - 116 */
  0x8040, /* u - 117 */
  0x8040, /* v - 118 */
  0x8040, /* w - 119 */
  0x8040, /* x - 120 */
  0x8040, /* y - 121 */
  0x8040, /* z - 122 */
  0x0048, /* { - 123 */
  0x0048, /* | - 124 */
  0x0048, /* } - 125 */
  0x0448, /* ~ - 126 */
  0x0000, /*  - 127 */
  0x0000, /* Ä - 128 */
  0x0000, /* Å - 129 */
  0x0008, /* Ç - 130 */
  0x8000, /* É - 131 */
  0x0008, /* Ñ - 132 */
  0x0008, /* Ö - 133 */
  0x0008, /* Ü - 134 */
  0x0008, /* á - 135 */
  0x0001, /* à - 136 */
  0x0008, /* â - 137 */
  0x8003, /* ä - 138 */
  0x0008, /* ã - 139 */
  0x8000, /* å - 140 */
  0x0000, /* ç - 141 */
  0x0000, /* é - 142 */
  0x0000, /* è - 143 */
  0x0000, /* ê - 144 */
  0x0088, /* ë - 145 */
  0x0088, /* í - 146 */
  0x0088, /* ì - 147 */
  0x0088, /* î - 148 */
  0x0008, /* ï - 149 */
  0x0400, /* ñ - 150 */
  0x0400, /* ó - 151 */
  0x0408, /* ò - 152 */
  0x0000, /* ô - 153 */
  0x8003, /* ö - 154 */
  0x0008, /* õ - 155 */
  0x8000, /* ú - 156 */
  0x0000, /* ù - 157 */
  0x0000, /* û - 158 */
  0x8003, /* ü - 159 */
  0x0008, /* † - 160 */
  0x0008, /* ° - 161 */
  0x0048, /* ¢ - 162 */
  0x0048, /* £ - 163 */
  0x0008, /* § - 164 */
  0x0048, /* • - 165 */
  0x0048, /* ¶ - 166 */
  0x0008, /* ß - 167 */
  0x0408, /* ® - 168 */
  0x0008, /* © - 169 */
  0x0400, /* ™ - 170 */
  0x0008, /* ´ - 171 */
  0x0048, /* ¨ - 172 */
  0x0408, /* ≠ - 173 */
  0x0008, /* Æ - 174 */
  0x0448, /* Ø - 175 */
  0x0008, /* ∞ - 176 */
  0x0008, /* ± - 177 */
  0x0000, /* ≤ - 178 */
  0x0000, /* ≥ - 179 */
  0x0408, /* ¥ - 180 */
  0x0008, /* µ - 181 */
  0x0008, /* ∂ - 182 */
  0x0008, /* ∑ - 183 */
  0x0408, /* ∏ - 184 */
  0x0000, /* π - 185 */
  0x0400, /* ∫ - 186 */
  0x0008, /* ª - 187 */
  0x0000, /* º - 188 */
  0x0000, /* Ω - 189 */
  0x0000, /* æ - 190 */
  0x0008, /* ø - 191 */
  0x8003, /* ¿ - 192 */
  0x8003, /* ¡ - 193 */
  0x8003, /* ¬ - 194 */
  0x8003, /* √ - 195 */
  0x8003, /* ƒ - 196 */
  0x8003, /* ≈ - 197 */
  0x8000, /* ∆ - 198 */
  0x8003, /* « - 199 */
  0x8003, /* » - 200 */
  0x8003, /* … - 201 */
  0x8003, /*   - 202 */
  0x8003, /* À - 203 */
  0x8003, /* Ã - 204 */
  0x8003, /* Õ - 205 */
  0x8003, /* Œ - 206 */
  0x8003, /* œ - 207 */
  0x8000, /* – - 208 */
  0x8003, /* — - 209 */
  0x8003, /* “ - 210 */
  0x8003, /* ” - 211 */
  0x8003, /* ‘ - 212 */
  0x8003, /* ’ - 213 */
  0x8003, /* ÷ - 214 */
  0x0008, /* ◊ - 215 */
  0x8003, /* ÿ - 216 */
  0x8003, /* Ÿ - 217 */
  0x8003, /* ⁄ - 218 */
  0x8003, /* € - 219 */
  0x8003, /* ‹ - 220 */
  0x8003, /* › - 221 */
  0x8000, /* ﬁ - 222 */
  0x8000, /* ﬂ - 223 */
  0x8003, /* ‡ - 224 */
  0x8003, /* · - 225 */
  0x8003, /* ‚ - 226 */
  0x8003, /* „ - 227 */
  0x8003, /* ‰ - 228 */
  0x8003, /* Â - 229 */
  0x8000, /* Ê - 230 */
  0x8003, /* Á - 231 */
  0x8003, /* Ë - 232 */
  0x8003, /* È - 233 */
  0x8003, /* Í - 234 */
  0x8003, /* Î - 235 */
  0x8003, /* Ï - 236 */
  0x8003, /* Ì - 237 */
  0x8003, /* Ó - 238 */
  0x8003, /* Ô - 239 */
  0x8000, /*  - 240 */
  0x8003, /* Ò - 241 */
  0x8003, /* Ú - 242 */
  0x8003, /* Û - 243 */
  0x8003, /* Ù - 244 */
  0x8003, /* ı - 245 */
  0x8003, /* ˆ - 246 */
  0x0008, /* ˜ - 247 */
  0x8003, /* ¯ - 248 */
  0x8003, /* ˘ - 249 */
  0x8003, /* ˙ - 250 */
  0x8003, /* ˚ - 251 */
  0x8003, /* ¸ - 252 */
  0x8003, /* ˝ - 253 */
  0x8000, /* ˛ - 254 */
  0x8003  /* ˇ - 255 */
};



ODINFUNCTION4(BOOL,    GetStringTypeW,
              DWORD,   dwInfoType,
              LPCWSTR, lpSrcStr,
              int,     cchSrc,
              LPWORD,  lpCharType)
{
  return GetStringTypeExW(0,
                          dwInfoType,
                          lpSrcStr,
                          cchSrc,
                          lpCharType);
}


ODINFUNCTION5(BOOL,    GetStringTypeExW,
              LCID,    locale,
              DWORD,   dwInfoType,
              LPCWSTR, lpSrcStr,
              int,     cchSrc,
              LPWORD,  lpCharType)
{
  int i;

  dprintf(("KERNEL32:  GetStringTypeW, not properly implemented\n"));
  if((DWORD)lpSrcStr == (DWORD)lpCharType ||
     !lpSrcStr ||
     !lpCharType)
  {
    O32_SetLastError(ERROR_INVALID_PARAMETER);
    return(FALSE);
  }

  if(cchSrc == -1)
    cchSrc = UniStrlen((UniChar*)lpSrcStr);

  switch(dwInfoType)
  {
    case CT_CTYPE1:
      for(i=0;i<cchSrc;i++)
      {
        lpCharType[i] = 0;
        if (isdigit(lpSrcStr[i])) lpCharType[i]|=C1_DIGIT;
        if (isalpha(lpSrcStr[i])) lpCharType[i]|=C1_ALPHA;
        if (islower(lpSrcStr[i])) lpCharType[i]|=C1_LOWER;
        if (isupper(lpSrcStr[i])) lpCharType[i]|=C1_UPPER;
        if (isspace(lpSrcStr[i])) lpCharType[i]|=C1_SPACE;
        if (ispunct(lpSrcStr[i])) lpCharType[i]|=C1_PUNCT;
        if (iscntrl(lpSrcStr[i])) lpCharType[i]|=C1_CNTRL;
        if ( (lpSrcStr[i] == ' ') ||
             (lpSrcStr[i] == '\t') )
           lpCharType[i]|=C1_BLANK;
      }
      return TRUE;
      break;

    case CT_CTYPE2:
    case CT_CTYPE3: //not supported right now
      return FALSE;
      break;
  }

  return FALSE;
}

//******************************************************************************
//NOTE: This has one parameter more than the W version! (@#$@#$)
//******************************************************************************
ODINFUNCTION5(BOOL,    GetStringTypeA,
              LCID,    locale,
              DWORD,   dwInfoType,
              LPCSTR,  lpSrcStr,
              int,     cchSrc,
              LPWORD,  lpCharType)
{
  return GetStringTypeExA(locale,
                          dwInfoType,
                          lpSrcStr,
                          cchSrc,
                          lpCharType);
}

/******************************************************************************
*GetStringTypeExA[KERNEL32.397]
*
* FIXME: Ignores the locale.
*/
ODINFUNCTION5(BOOL,    GetStringTypeExA,
              LCID,    locale,
              DWORD,   dwInfoType,
              LPCSTR,  src,
              INT,     cchSrc,
              LPWORD,  chartype)
{
  int i;

  if ((src==NULL) ||
      (chartype==NULL) ||
      (src==(LPSTR)chartype))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  if (cchSrc==-1)
    cchSrc=lstrlenA(src)+1;

  switch (dwInfoType)
  {
    case CT_CTYPE1:
      for (i=0;i<cchSrc;i++)
      {
        chartype[i] = 0;
        if (isdigit(src[i])) chartype[i]|=C1_DIGIT;
        if (isalpha(src[i])) chartype[i]|=C1_ALPHA;
        if (islower(src[i])) chartype[i]|=C1_LOWER;
        if (isupper(src[i])) chartype[i]|=C1_UPPER;
        if (isspace(src[i])) chartype[i]|=C1_SPACE;
        if (ispunct(src[i])) chartype[i]|=C1_PUNCT;
        if (iscntrl(src[i])) chartype[i]|=C1_CNTRL;
        /* FIXME: isblank() is a GNU extension */
        /*if (isblank(src[i])) chartype[i]|=C1_BLANK; */
        if ((src[i] == ' ') || (src[i] == '\t')) chartype[i]|=C1_BLANK;
        /* C1_XDIGIT */
      }
      return TRUE;

    case CT_CTYPE2:
      for (i=0;i<cchSrc;i++)
      {
        chartype[i]=(WORD)CT_CType2_LUT[i];
      }
      return TRUE;

    case CT_CTYPE3:
      for (i=0;i<cchSrc;i++)
      {
        chartype[i]=OLE2NLS_CT_CType3_LUT[i];
      }
      return TRUE;

    default:
      dprintf(("Unknown dwInfoType:%ld\n",
               dwInfoType));
      return FALSE;
  }
}


//******************************************************************************
VOID WIN32API GlobalMemoryStatus( MEMORYSTATUS *arg1)
{
    dprintf(("KERNEL32:  GlobalMemoryStatus\n"));
    O32_GlobalMemoryStatus(arg1);
    dprintf(("dwMemoryLoad    %X\n", arg1->dwMemoryLoad));
    dprintf(("dwTotalPhys     %X\n", arg1->dwTotalPhys));
    dprintf(("dwAvailPhys     %X\n", arg1->dwAvailPhys));
    dprintf(("dwTotalPageFile %X\n", arg1->dwTotalPageFile));
    dprintf(("dwAvailPageFile %X\n", arg1->dwAvailPageFile));
    dprintf(("dwTotalVirtual  %X\n", arg1->dwTotalVirtual));
    dprintf(("dwAvailVirtual  %X\n", arg1->dwAvailVirtual));
}
VOID WIN32API Sleep(DWORD arg1)
{
    dprintf2(("KERNEL32:  Sleep %d\n", arg1));
    O32_Sleep(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetPriorityClass(HANDLE arg1)
{
    dprintf(("KERNEL32:  GetPriorityClass\n"));
    return O32_GetPriorityClass(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPriorityClass(HANDLE arg1, DWORD  arg2)
{
    dprintf(("KERNEL32:  SetPriorityClass\n"));
    return O32_SetPriorityClass(arg1, arg2);
}
//******************************************************************************
//TODO!
//******************************************************************************
int WIN32API LCMapStringW(
    DWORD /*LCID*/     Locale,
    DWORD    dwMapFlags,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWSTR   lpDestStr,
    int      cchDest)
{
    // quick hack!  this code is not done!
    dprintf(("LCMapStringW: NOT IMPLEMENTED!"));
    if(cchSrc == -1)
        cchSrc = strlen((const char *)lpSrcStr);
    if(!cchDest)
        return cchSrc;
    lstrcpynW(lpDestStr, lpSrcStr, max(cchSrc, cchDest));
    return max(cchSrc, cchDest);
}
//******************************************************************************
//TODO!
//******************************************************************************
int WIN32API LCMapStringA(
    DWORD /*LCID*/     Locale,
    DWORD    dwMapFlags,
    LPCSTR   lpSrcStr,
    int      cchSrc,
    LPSTR    lpDestStr,
    int      cchDest)
{
    dprintf(("KERNEL32:  LCMapStringA not implemented\n"));
    if(cchSrc == -1)
        cchSrc = strlen((const char *)lpSrcStr);
    if(!cchDest)
        return cchSrc;
    strncpy((char *)lpDestStr, (const char *)lpSrcStr, max(cchSrc, cchDest));
    return max(cchSrc, cchDest);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API Beep( DWORD arg1, DWORD  arg2)
{
    dprintf(("KERNEL32:  OS2Beep\n"));
    return O32_Beep(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitA( UINT arg1, LPCSTR  arg2)
{
    dprintf(("KERNEL32:  OS2FatalAppExitA\n"));
    O32_FatalAppExit(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitW(UINT arg1, LPCWSTR arg2)
{
 char *astring;

    dprintf(("KERNEL32:  OS2FatalAppExitW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg2);
    O32_FatalAppExit(arg1, astring);
    //probably won't return, but who cares..
    FreeAsciiString(astring);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalExit( UINT arg1)
{
    dprintf(("KERNEL32:  OS2FatalExit\n"));
    O32_FatalExit(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeReadPtr( const void * arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  OS2IsBadHugeReadPtr\n"));
    return O32_IsBadHugeReadPtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeWritePtr( PVOID arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  OS2IsBadHugeWritePtr\n"));
    return O32_IsBadHugeWritePtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDBCSLeadByte(BYTE arg1)
{
    dprintf2(("KERNEL32:  OS2IsDBCSLeadByte\n"));
    return O32_IsDBCSLeadByte(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API LoadModule( LPCSTR arg1, PVOID  arg2)
{
    dprintf(("KERNEL32:  OS2LoadModule\n"));
    return O32_LoadModule(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API MulDiv(int arg1, int arg2, int  arg3)
{
    dprintf2(("KERNEL32:  MulDiv %d*%d/%d\n", arg1, arg2, arg3));
    if(arg3 == 0)
	return 0;

    return O32_MulDiv(arg1, arg2, arg3);
}
//******************************************************************************
UINT WIN32API CompareStringA(LCID lcid, DWORD fdwStyle, LPCSTR lpString1,
                             DWORD cch1, LPCSTR lpString2, DWORD cch2)
{
  int i;
  int fEqual = TRUE;
  char *string1 = (char *)lpString1, *string2 = (char *)lpString2;

   SetLastError(0);

#ifdef DEBUG
   if(fdwStyle & SORT_STRINGSORT)
         dprintf(("KERNEL32:  SORT_STRINGSORT not supported!\n"));
   if(fdwStyle & NORM_IGNORENONSPACE)
         dprintf(("KERNEL32:  NORM_IGNORENONSPACE not supported!\n"));
   if(fdwStyle & NORM_IGNORESYMBOLS)
         dprintf(("KERNEL32:  NORM_IGNORESYMBOLS not supported!\n"));
#endif

   if(lpString1 == NULL || lpString2 == NULL)
   {
    	dprintf(("CompareStringA: (s1=%s,s2=%s): Invalid NULL string\n", lpString1, lpString2));
    	SetLastError(ERROR_INVALID_PARAMETER);
    	return 0;
   }

   if(cch1 == -1) cch1 = strlen(string1);
   if(cch2 == -1) cch2 = strlen(string2);

   if(fdwStyle) {
         //TODO!
         if(fdwStyle != 0 && fdwStyle != NORM_IGNORECASE)
                 return(0);  /*PLF Fri  98-03-13 04:09:32 was return 1 */
   }
   if(fdwStyle & NORM_IGNORECASE)
       fEqual = strnicmp(string1, string2, min(cch1, cch2));
   else
       fEqual = strncmp(string1, string2, min(cch1, cch2));

   if     (fEqual < 0 ) fEqual = 1;
   else if(fEqual == 0) fEqual = 2;
   else if(fEqual >  0) fEqual = 3;

   //If equal, but different length, largest one is the greatest in lexical value
   if(fEqual == 2 && cch1 != cch2){
      if(cch1 < cch2)
         fEqual = 1;
      else
         fEqual = 3;
   }
   dprintf2(("KERNEL32:  OS2CompareStringA '%s' - '%s' returned %d\n", lpString1, lpString2, fEqual));
   return(fEqual);
}

//******************************************************************************
//TODO: Not complete (fdwStyle flags specify compare method)
//******************************************************************************
UINT WIN32API CompareStringW(LCID lcid, DWORD fdwStyle, LPCWSTR lpString1,
                             DWORD cch1, LPCWSTR lpString2, DWORD cch2)
{
 int i;
 int fEqual = TRUE;
 char *string1;
 char *string2;

   dprintf2(("KERNEL32:  OS2CompareStringW '%s' - '%s'\n", string1, string2));

   if(lpString1 == NULL || lpString2 == NULL)
   {
    	dprintf(("CompareStringW: (s1=%s,s2=%s): Invalid NULL string\n", lpString1, lpString2));
    	SetLastError(ERROR_INVALID_PARAMETER);
    	return 0;
   }

   if(cch1 == -1) cch1 = lstrlenW(lpString1);
   if(cch2 == -1) cch2 = lstrlenW(lpString2);

   string1 = UnicodeToAsciiStringN((LPWSTR)lpString1, cch1);
   string2 = UnicodeToAsciiStringN((LPWSTR)lpString2, cch2);

   fEqual = CompareStringA(lcid, fdwStyle, string1, cch1, string2, cch2);
   FreeAsciiString(string1);
   FreeAsciiString(string2);

   return(fEqual);
}
//******************************************************************************
//Borrowed from Wine
//******************************************************************************
ODINPROCEDURE1(GetStartupInfoA,LPSTARTUPINFOA,lpStartupInfo)
{
  lpStartupInfo->cb               = sizeof(STARTUPINFOA);
  lpStartupInfo->lpReserved       = "<Reserved>";
  lpStartupInfo->lpDesktop        = "Desktop";
  lpStartupInfo->lpTitle          = "Title";
  lpStartupInfo->dwX              = 0;
  lpStartupInfo->dwY              = 0;
  lpStartupInfo->dwXSize          = 640;
  lpStartupInfo->dwYSize          = 480;
  lpStartupInfo->dwXCountChars    = 80;      // for console
  lpStartupInfo->dwYCountChars    = 25;
  lpStartupInfo->dwFillAttribute  = 0x0720;
  lpStartupInfo->dwFlags          = STARTF_USESHOWWINDOW   |
                                    STARTF_USEPOSITION     |
                                    STARTF_USESIZE         |
                                    STARTF_USECOUNTCHARS   |
                                    STARTF_USEFILLATTRIBUTE|
                                    STARTF_USESTDHANDLES;
  lpStartupInfo->wShowWindow      = SW_SHOWDEFAULT;
  lpStartupInfo->cbReserved2      = 0;
  lpStartupInfo->lpReserved2      = NULL; /* must be NULL for VC runtime */
  /* @@@PH 98/07/13 Handlemanager support */
  lpStartupInfo->hStdInput        = GetStdHandle(STD_INPUT_HANDLE);
  lpStartupInfo->hStdOutput       = GetStdHandle(STD_OUTPUT_HANDLE);
  lpStartupInfo->hStdError        = GetStdHandle(STD_ERROR_HANDLE);
  return;
}
//******************************************************************************
//Borrowed from Wine
//******************************************************************************
ODINPROCEDURE1(GetStartupInfoW,LPSTARTUPINFOW,lpStartupInfo)
{
  static WCHAR lpReserved[] = {'<', 'R','e','s','e','r','v','e','d','>', 0};
  static WCHAR lpDesktop[]  = {'D', 'e','s','k','t','o','p', 0};
  static WCHAR lpTitle[]    = {'T', 'i','t','l','e', 0};

  // forward call to ascii variant
  GetStartupInfoA((LPSTARTUPINFOA)lpStartupInfo);
  lpStartupInfo->cb          = sizeof(STARTUPINFOW);
  lpStartupInfo->lpReserved  = lpReserved;
  lpStartupInfo->lpDesktop   = lpDesktop;
  lpStartupInfo->lpTitle     = lpTitle;
  return;
}
//******************************************************************************
//TODO: Not complete or correct, but sufficient for now
//******************************************************************************
BOOL WIN32API GetBinaryTypeA(LPCTSTR lpApplicationName, LPDWORD lpBinaryType)
{
  dprintf(("KERNEL32:  OS2GetBinaryTypeA %s\n", lpApplicationName));
  if(strstr(lpApplicationName, ".EXE") ||   strstr(lpApplicationName, ".exe"))
        *lpBinaryType = SCS_32BIT_BINARY;
  else
  if(strstr(lpApplicationName, ".COM") ||   strstr(lpApplicationName, ".com"))
        *lpBinaryType = SCS_DOS_BINARY;
  else
  if(strstr(lpApplicationName, ".PIF") ||   strstr(lpApplicationName, ".pif"))
        *lpBinaryType = SCS_PIF_BINARY;
  else  return(FALSE);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBinaryTypeW(LPCWSTR lpApplicationName, LPDWORD lpBinaryType)
{
 BOOL rc;
 char  *astring;

    dprintf(("KERNEL32:  OS2GetBinaryTypeW\n"));
    astring = UnicodeToAsciiString((LPWSTR)lpApplicationName);
    rc = GetBinaryTypeA(astring, lpBinaryType);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//TODO: SetLastError
//******************************************************************************
BOOL WIN32API GetVersionExA(OSVERSIONINFOA *lpVersionInformation)
{
   dprintf(("KERNEL32:  OS2GetVersionExA\n"));

   if(lpVersionInformation == NULL || lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA))
        return(FALSE);

   lpVersionInformation->dwMajorVersion = ODINNT_MAJOR_VERSION;    //pretend we're NT 4.0
   lpVersionInformation->dwMinorVersion = ODINNT_MINOR_VERSION;
   lpVersionInformation->dwBuildNumber  = ODINNT_BUILD_NR;
   lpVersionInformation->dwPlatformId   = VER_PLATFORM_WIN32_NT;
   strcpy(lpVersionInformation->szCSDVersion, ODINNT_CSDVERSION);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVersionExW(OSVERSIONINFOW *lpVersionInformation)
{
   dprintf(("KERNEL32:  OS2GetVersionExW\n"));

   if(lpVersionInformation == NULL || lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
        return(FALSE);

   lpVersionInformation->dwMajorVersion = ODINNT_MAJOR_VERSION;    //pretend we're NT 4.0
   lpVersionInformation->dwMinorVersion = ODINNT_MINOR_VERSION;
   lpVersionInformation->dwBuildNumber  = ODINNT_BUILD_NR;
   lpVersionInformation->dwPlatformId   = VER_PLATFORM_WIN32_NT;
   lstrcpyW(lpVersionInformation->szCSDVersion, ODINNT_CSDVERSION_W);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetVersion()
{
  dprintf(("KERNEL32:  GetVersion\n"));
  // highword 0 = NT, lowword high byte major ver, low byte minor ver
/* @@@PH 98/04/04 MFC30 makes assumptions about process control block        */
/*                structures that lead to crashes if we don't identify as NT */

  return ODINNT_VERSION;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlushInstructionCache(     /*PLF Mon  98-02-09 23:56:49 : STUB STUB STUB STUB STUB */
    HANDLE hProcess,    /* process with cache to flush  */
    LPCVOID lpvBase,    /* address of region to flush   */
    DWORD cbFlush)      /* length of region to flush    */

{
        dprintf(("FlushInstructionCache() - NIY\n"));
        return TRUE;
}


//******************************************************************************

/***********************************************************************
*           RtlFillMemory   (KERNEL32.441)
*/
VOID WIN32API RtlFillMemory(LPVOID ptr,
                            UINT len,
                            UINT fill )
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: RtlFillMemory(%08x,%08x,%08x)\n",
           ptr,
           len,
           fill));
#endif

  memset(ptr,
         fill,
         len );
}


/***********************************************************************
*           RtlMoveMemory   (KERNEL32.442)
*/
VOID WIN32API RtlMoveMemory(LPVOID  dst,
                            LPCVOID src,
                            UINT  len )
{
  dprintf2(("KERNEL32: RtlMoveMemory(%08x,%08x,%08x)\n",
           dst,
           src,
           len));

  memmove(dst,
          src,
          len );
}


/***********************************************************************
*           RtlZeroMemory   (KERNEL32.444)
*/
VOID WIN32API RtlZeroMemory(LPVOID ptr,
                            UINT len)
{
  dprintf2(("KERNEL32: RtlZeroMemory(%08x,%08x)\n",
           ptr,
           len));

  memset(ptr,
         0,
         len);
}

//******************************************************************************
/*KSO Thu 21.05.1998*/
BOOL WIN32API IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar)
{
    dprintf(("KERNEL32:  OS2IsDBCSLeadByteEx - not correctly implemented\n"));
    return O32_IsDBCSLeadByte(TestChar);
}
//******************************************************************************








/*****************************************************************************
 * Name      : BOOL GetSystemPowerStatus
 * Purpose   : The GetSystemPowerStatus function retrieves the power status of
 *             the system. The status indicates whether the system is running
 *             on AC or DC power, whether the battery is currently charging,
 *             and how much battery life currently remains.
 * Parameters: LPSYSTEM_POWER_STATUS lpSystemPowerStatus
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetSystemPowerStatus(LPSYSTEM_POWER_STATUS lpSystemPowerStatus)
{
  dprintf(("Kernel32: GetSystemPowerStatus(%08xh) not properly implemented.\n",
           lpSystemPowerStatus));

  if(lpSystemPowerStatus == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  lpSystemPowerStatus->ACLineStatus       = 1; //online
  lpSystemPowerStatus->BatteryFlag        = 128; //no system battery
  lpSystemPowerStatus->BatteryLifePercent = 255; //unknown
  lpSystemPowerStatus->reserved           = 0;
  lpSystemPowerStatus->BatteryLifeTime    = -1; //unknown
  lpSystemPowerStatus->BatteryFullLifeTime= -1; //unknown

  return TRUE;
}
