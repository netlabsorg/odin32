#!/bin/sh

OS2EMX_H=D:/Dev/gcc452/usr/include/os2emx.h
OS2WRAP2_H=os2wrap2.h.new

#
# Fixed header
#

echo \
'/* 
 * A full version of os2wrap.h that prefixes all OS/2 type and constant
 * definitions with "os2_" which makes it possible to use the OS/2 APIs
 * from the same source files that use the Windows APIs.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __OS2WRAP2_H__
#define __OS2WRAP2_H__

#ifdef WINVER
#error "os2wrap2.h must be included BEFORE any Windows include file!"
#endif

#define os2__THUNK_PASCAL_FUNCTION (FUN) os2_APIENTRY _16_##FUN
#define os2_MAKE16P (sel,off)   ((_far16ptr)((sel) << 16 | (off)))
#define os2_MAKEP (sel,off)     _libc_16to32 ( os2_MAKE16P  (sel, off))
#define os2_SELECTOROF (farptr) ((os2_SEL)((farptr) >> 16))
#define os2_OFFSETOF (farptr)   ((os2_USHORT)(farptr))
#define os2__THUNK_PTR_SIZE_OK (ptr,size) \
  (((os2_ULONG)(ptr) & ~0xffff) == (((os2_ULONG)(ptr) + (size) - 1) & ~0xffff))
' > "$OS2WRAP2_H"

#
# Redefine all OS/2 typedefs to those having the "os2_" prefix to avoid
# conflicts with Windows typedefs. Also define "os2_" versions of all  
# macros. The defines are generated with the following command on the
# respective OS/2 headers:
#

DefinePrefixes=\
'BM|CLR|CONTEXT|CREATE|CS|DM|DT|EH|ERROR|EXCEPTION|EXLST|FILE|FDM|FNTM|'\
'HWND|LM|MB|MIA|MM|NP|OBJ|PAG|QS|QSV|RGN|RT|SEM|SEVERITY|SIS|SV|SWP|TA|'\
'WC|WM|WS|XCPT|(WIN|GPI|DEV|SPL)ERR'

Defines=\
'ERRORID|NO_ERROR|PCMDMSG|SEL|HWND|MPARAM|MRESULT|LHANDLE|SHANDLE|BOOL|'\
'ADDRESS|DRIVER_NAME|DRIVER_DATA|DATA_TYPE|COMMENT|PROC_NAME|'\
'PROC_PARAMS|SPL_PARAMS|NETWORK_PARAMS|'\
'(CCH|MPFROM|MAKE)[A-Z0-9]+|'\
'P?VOID|(API|EXP)ENTRY|P(CMD|CHR|MSE)MSG|'\
'(LO|HI|P)?U?(LONG|SHORT|CHAR|BYTE)'

sed -nr '
{
  s/^[[:space:]]*typedef[[:space:]]+struct[[:space:]]+([A-Za-z_][A-Za-z0-9_]+)([[:space:]]*[/]\*.*\*[/])?[[:space:]]*$/#define \1 os2_\1/p
  td
  s/^[[:space:]]*}[[:space:]]*([A-Z_][A-Z0-9_]+)[[:space:]]*;[[:space:]]*$/#define \1 os2_\1/p
  td
  s/^[[:space:]]*typedef[[:space:]]+.+[[:space:]]+\**([A-Z_][A-Z0-9_]+)[[:space:]]*(\[.*\])?[[:space:]]*;([[:space:]]*[/]\*.*\*[/])?[[:space:]]*$/#define \1 os2_\1/p
  td
}
/^[[:space:]]*#define[[:space:]]+/ {
  :a;/\\$/{N;ba}
  s/([^A-Za-z0-9_])(MAKE[A-Z0-9]+)([^A-Za-z0-9_])/\1 \2 \3/g
  s/([^A-Za-z0-9_])((('$DefinePrefixes')_([A-Z0-9_]+))|(('$Defines')([^A-Za-z0-9_])))/\1os2_\2/g
  tb;bd
  :b
  s/^[[:space:]]*#define[[:space:]]+((Dos|Win|Gpi)[A-Za-z_][A-Za-z0-9_]+)([[:space:](].*)//
  s/^[[:space:]]*#define[[:space:]]+(os2_)?([A-Za-z_][A-Za-z0-9_]+)[[:space:]]*(.*)/#define os2_\2 \3/p
}  
:d
' < "$OS2EMX_H" >> "$OS2WRAP2_H"

#
# Fixed middle
#

echo \
'
/* include the main OS/2 API wrapper */
#include <os2wrap.h>

#undef _THUNK_PASCAL_FUNCTION
#undef MAKE16P
#undef MAKEP
#undef SELECTOROF
#undef OFFSETOF
#undef _THUNK_PTR_SIZE_OK
' >> "$OS2WRAP2_H"

#
# Undefine all typedefs prefixed with "os2_" at the beginning and original
# versions of macros. The command to generate this block is:
#

sed -nr '
{
  s/^[[:space:]]*typedef[[:space:]]+struct[[:space:]]+([A-Za-z_][A-Za-z0-9_]+)([[:space:]]*[/]\*.*\*[/])?[[:space:]]*$/#undef \1/p
  td
  s/^[[:space:]]*}[[:space:]]*([A-Z_][A-Z0-9_]+)[[:space:]]*;[[:space:]]*$/#undef \1/p
  td
  s/^[[:space:]]*typedef[[:space:]]+.+[[:space:]]+\**([A-Z_][A-Z0-9_]+)[[:space:]]*(\[.*\])?[[:space:]]*;([[:space:]]*[/]\*.*\*[/])?[[:space:]]*$/#undef \1/p
  td
}
/^[[:space:]]*#define[[:space:]]+/ {
  :a;/\\$/{N;ba}
  s/([^A-Za-z0-9_])(MAKE[A-Z0-9]+)([^A-Za-z0-9_])/\1 \2 \3/g
  s/([^A-Za-z0-9_])((('$DefinePrefixes')_([A-Z0-9_]+))|(('$Defines')([^A-Za-z0-9_])))/\1os2_\2/g
  tb;bd
  :b
  s/^[[:space:]]*#define[[:space:]]+((Dos|Win|Gpi)[A-Za-z_][A-Za-z0-9_]+)([[:space:](].*)/#undef \1\
#define \1\3/p
  s/^[[:space:]]*#define[[:space:]]+(os2_)?([A-Za-z_][A-Za-z0-9_]+)[[:space:]]*(.*)/#undef \2/p
}  
:d
' < "$OS2EMX_H" >> "$OS2WRAP2_H"

#
# Fixed footer
#

echo \
'
#endif /* __OS2WRAP2_H__ */
' >> "$OS2WRAP2_H"
