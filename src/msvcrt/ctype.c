/*
 * msvcrt.dll ctype functions
 *
 * Copyright 2000 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "msvcrt.h"
#include <ctype.h>
#include "msvcrt/ctype.h"


#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/* Some abbreviations to make the following table readable */
#define _C_ _CONTROL
#define _S_ _SPACE
#define _P_ _PUNCT
#define _D_ _DIGIT
#define _H_ _HEX
#define _U_ _UPPER
#define _L_ _LOWER

WORD MSVCRT__ctype [257] = {
  0, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _S_|_C_, _S_|_C_,
  _S_|_C_, _S_|_C_, _S_|_C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_,
  _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _C_, _S_|_BLANK,
  _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _P_,
  _P_, _D_|_H_, _D_|_H_, _D_|_H_, _D_|_H_, _D_|_H_, _D_|_H_, _D_|_H_,
  _D_|_H_, _D_|_H_, _D_|_H_, _P_, _P_, _P_, _P_, _P_, _P_, _P_, _U_|_H_,
  _U_|_H_, _U_|_H_, _U_|_H_, _U_|_H_, _U_|_H_, _U_, _U_, _U_, _U_, _U_,
  _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_, _U_,
  _U_, _P_, _P_, _P_, _P_, _P_, _P_, _L_|_H_, _L_|_H_, _L_|_H_, _L_|_H_,
  _L_|_H_, _L_|_H_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_,
  _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _P_, _P_, _P_, _P_,
  _C_, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Internal: Current ctype table for locale */
WORD MSVCRT_current_ctype[257];

/* pctype is used by macros in the Win32 headers. It must point
 * To a table of flags exactly like ctype. To allow locale
 * changes to affect ctypes (i.e. isleadbyte), we use a second table
 * and update its flags whenever the current locale changes.
 */
WORD* MSVCRT__pctype = MSVCRT_current_ctype + 1;

/* mbctype data */
extern int MSVCRT___mb_cur_max;
extern LCID MSVCRT_current_lc_all_lcid;

/*********************************************************************
 *		__p__pctype (MSVCRT.@)
 */
WORD** __p__pctype(void)
{
  return &MSVCRT__pctype;
}

/*********************************************************************
 *		_isctype (MSVCRT.@)
 */
int _isctype(int c, int type)
{
  dprintf(("MSVCRT: _isctype %d %d",c, type));
  if (c >= -1 && c <= 255)
    return MSVCRT__pctype[c] & type;

  if (MSVCRT___mb_cur_max != 1 && c > 0)
  {
    /* FIXME: Is there a faster way to do this? */
    WORD typeInfo;
    char convert[3], *pconv = convert;

    if (MSVCRT__pctype[(UINT)c >> 8] & _LEADBYTE)
      *pconv++ = (UINT)c >> 8;
    *pconv++ = c & 0xff;
    *pconv = 0;
    /* FIXME: Use ctype LCID, not lc_all */
    if (GetStringTypeExA(MSVCRT_current_lc_all_lcid, CT_CTYPE1,
                         convert, convert[1] ? 2 : 1, &typeInfo))
      return typeInfo & type;
  }
  return 0;
}

/*********************************************************************
 *		isalnum (MSVCRT.@)
 */
int MSVCRT_isalnum(int c)
{
  return _isctype( c, _ALPHA | _DIGIT );
}

/*********************************************************************
 *		isalpha (MSVCRT.@)
 */
int MSVCRT_isalpha(int c)
{
  return _isctype( c, _ALPHA );
}

/*********************************************************************
 *		iscntrl (MSVCRT.@)
 */
int MSVCRT_iscntrl(int c)
{
  return _isctype( c, _CONTROL );
}

/*********************************************************************
 *		isdigit (MSVCRT.@)
 */
int MSVCRT_isdigit(int c)
{
  return _isctype( c, _DIGIT );
}

/*********************************************************************
 *		isgraph (MSVCRT.@)
 */
int MSVCRT_isgraph(int c)
{
  return _isctype( c, _ALPHA | _DIGIT | _PUNCT );
}

/*********************************************************************
 *		isleadbyte (MSVCRT.@)
 */
int MSVCRT_isleadbyte(int c)
{
  return _isctype( c, _LEADBYTE );
}

/*********************************************************************
 *		islower (MSVCRT.@)
 */
int MSVCRT_islower(int c)
{
  return _isctype( c, _LOWER );
}

/*********************************************************************
 *		isprint (MSVCRT.@)
 */
int MSVCRT_isprint(int c)
{
  return _isctype( c, _ALPHA | _DIGIT | _BLANK | _PUNCT );
}

/*********************************************************************
 *		ispunct (MSVCRT.@)
 */
int MSVCRT_ispunct(int c)
{
  return _isctype( c, _PUNCT );
}

/*********************************************************************
 *		isspace (MSVCRT.@)
 */
int MSVCRT_isspace(int c)
{
  return _isctype( c, _SPACE );
}

/*********************************************************************
 *		isupper (MSVCRT.@)
 */
int MSVCRT_isupper(int c)
{
  return _isctype( c, _UPPER );
}

/*********************************************************************
 *		isxdigit (MSVCRT.@)
 */
int MSVCRT_isxdigit(int c)
{
  return _isctype( c, _HEX );
}

/*********************************************************************
 *		__isascii (MSVCRT.@)
 */
int MSVCRT___isascii(int c)
{
  return isascii((unsigned)c);
}

/*********************************************************************
 *		__toascii (MSVCRT.@)
 */
int MSVCRT___toascii(int c)
{
  return (unsigned)c & 0x7f;
}

/*********************************************************************
 *		iswascii (MSVCRT.@)
 *
 */
int MSVCRT_iswascii(MSVCRT_wchar_t c)
{
  return ((unsigned)c < 0x80);
}

/*********************************************************************
 *		__iscsym (MSVCRT.@)
 */
int MSVCRT___iscsym(int c)
{
  return (c < 127 && (isalnum(c) || c == '_'));
}

/*********************************************************************
 *		__iscsymf (MSVCRT.@)
 */
int MSVCRT___iscsymf(int c)
{
  return (c < 127 && (isalpha(c) || c == '_'));
}

/*********************************************************************
 *		_toupper (MSVCRT.@)
 */
int MSVCRT__toupper(int c)
{
    return c - 0x20;  /* sic */
}

/*********************************************************************
 *		_tolower (MSVCRT.@)
 */
int MSVCRT__tolower(int c)
{
    return c + 0x20;  /* sic */
}
