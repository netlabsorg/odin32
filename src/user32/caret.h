/* $Id: caret.h,v 1.2 1999-10-06 10:36:39 dengert Exp $ */

/*
 * Caret functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

extern void recreateCaret (HWND hwndFocus);
extern BOOL WIN32API SetCaretPos (int x, int y);
extern BOOL WIN32API GetCaretPos (PPOINT pPoint);

