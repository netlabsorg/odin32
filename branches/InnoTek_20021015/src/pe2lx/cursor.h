/* $Id: cursor.h,v 1.3 1999-06-10 17:08:52 phaller Exp $ */

/*
 * PE2LX cursor conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __CURSOR_H__
#define __CURSOR_H__

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

class OS2Cursor
{
public:
                              // Constructors and destructors
                               OS2Cursor(int id, CursorComponent *bmpHdr, int size);
    	                      ~OS2Cursor();

                         int   QueryCursorSize();
		        void   SetCursorHdrOffset(int offset);
	   BITMAPFILEHEADER   *GetCursorHeader(int wWidth, int wHeight, int wPlanes, int wBitCount);

    static OS2Cursor          *GetCursor(int id);
    static               void  DestroyAll();

protected:
	
private:
              int             id, cursorsize, prevoffset;
    BITMAPFILEHEADER         *cursorhdr;

                              // Linked list management
              OS2Cursor*      next;                   // Next Cursor
    static    OS2Cursor*      cursors;                // List of Cursors
};

#endif
