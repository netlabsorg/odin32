

#ifndef __WINE_WINGDI_H
  typedef DWORD COLORREF, *LPCOLORREF;
#endif
HBRUSH CreatePatternHTBrush(HDC hdc, BYTE *pBits);
void SetupDitherBit1(COLORREF col, BYTE *pBits);
void SetupDitherBit2(COLORREF col, BYTE *pBits, BYTE *ptr);
DWORD InitWing32();

