// XFont.h  Version 1.1
//
// Copyright (C) 2003 Hans Dietrich
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XFONT_H
#define XFONT_H

typedef struct _tagFONT_PROPERTIES
{
    TCHAR csName[1024];
    TCHAR csCopyright[1024];
    TCHAR csTrademark[1024];
    TCHAR csFamily[1024];
} FONT_PROPERTIES, *LPFONT_PROPERTIES;

#ifdef __cplusplus
extern "C" {
#endif

BOOL GetFontFile(LPCTSTR lpszFontName,
                 LPTSTR lpszDisplayName,
                 int nDisplayNameSize,
                 LPTSTR lpszFontFile,
                 int nFontFileSize);


BOOL GetFontProperties(LPCTSTR lpszFilePath,
                       LPFONT_PROPERTIES lpFontProps);

#ifdef __cplusplus
}
#endif

#endif //XFONT_H
