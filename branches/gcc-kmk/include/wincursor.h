/* $Id: wincursor.h,v 1.1 1999-08-19 19:51:17 sandervl Exp $ */

/*
 * PE2LX cursor conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINCURSOR_H__
#define __WINCURSOR_H__

#pragma pack(1)

typedef struct {
  WORD   wReserved;          // Currently zero
  WORD   wType;              // 2 for cursors
  WORD   cwCount;            // Number of components
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} CursorHeader;

typedef struct {
  WORD   wWidth;
  WORD   wHeight;
  WORD   wPlanes;
  WORD   wBitCount;
  DWORD  lBytesInRes;
  WORD   wNameOrdinal;       // Points to component
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} CursorResDir;

typedef struct {
  short  xHotspot;
  short  yHotspot;
}  CursorComponent;

#pragma pack()

#endif
