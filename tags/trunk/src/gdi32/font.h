#ifndef __FONT_H__
#define __FONT_H__


extern HFONT hFntDefaultGui;

BOOL WIN32API IsSystemFont(HFONT hFont);

LPWSTR FONT_mbtowc(HDC hdc, LPCSTR str, INT count, INT *plenW, UINT *pCP);

#endif //__FONT_H__