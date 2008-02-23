#ifndef __FONT_H__
#define __FONT_H__


extern HFONT hFntDefaultGui;

BOOL WIN32API IsSystemFont(HFONT hFont);

BOOL RegisterFont(HFONT hFont, LPSTR lfFaceName);

LPWSTR FONT_mbtowc(HDC hdc, LPCSTR str, INT count, INT *plenW, UINT *pCP);

//text.cpp
BOOL WIN32API GetStringWidthW(HDC hdc, LPWSTR lpszString, UINT cbString, PINT pWidthArray);

#endif //__FONT_H__