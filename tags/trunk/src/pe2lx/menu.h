/* $Id: menu.h,v 1.4 1999-07-06 08:50:11 sandervl Exp $ */

/*
 * PE2LX menus
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __MENU_H__
#define __MENU_H__

#define    GRAYED         0x0001   // 'GRAYED' keyword
#define    INACTIVE       0x0002   // 'INACTIVE' keyword
#define    BITMAP         0x0004   // 'BITMAP' keyword
#define    OWNERDRAW      0x0100   // 'OWNERDRAW' keyword
#define    CHECKED        0x0008   // 'CHECKED' keyword
#define    POPUP          0x0010   // Used internally
#define    MENUBARBREAK   0x0020   // 'MENUBARBREAK' keyword (== separator)
#define    MENUBREAK      0x0040   // 'MENUBREAK' keyword    (witregel + nieuw item)
#define    ENDMENU        0x0080   // Used internally

#pragma pack(1) /*PLF Sat  97-06-21 22:17:31*/

typedef struct {
  WORD   wVersion;           // Currently zero
  WORD   cbHeaderSize;       // Also zero
} MenuHeader;

//Popup menu items (signalled by fItemFlags & POPUP):

typedef struct {
  WORD   fItemFlags;
  WCHAR  szItemText[1]; /*PLF Sat  97-06-21 22:17:41*/
} PopupMenuItem;

//Normal menu items (signalled by !(fItemFlags & POPUP)):

typedef struct {
  WORD   fItemFlags;
  WORD   wMenuID;
  WCHAR  szItemText[1]; /*PLF Sat  97-06-21 22:17:44*/
} NormalMenuItem;

#pragma pack() /*PLF Sat  97-06-21 22:17:31*/

void ShowMenu(int id, MenuHeader *menu, int size, int cp = 0);

#endif
