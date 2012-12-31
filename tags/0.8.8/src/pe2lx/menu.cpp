/* $Id: menu.cpp,v 1.5 1999-07-06 13:24:04 sandervl Exp $ */

/*
 * PE2LX menu conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_DOSNLS
#define INCL_WIN
#include <os2.h>
#include <pmmenu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "menu.h"
#include "misc.h"

static int  ProcessSubMenu(PopupMenuItem *popupitem, MT_OS2 *os2menu, MTI_OS2 *menuitem, int size, int cp);
static int  ProcessMenuItem(NormalMenuItem *popupitem, MT_OS2 *os2menu, MTI_OS2 *menuitem, int size, int cp);
static void menustrcpy(char *dest, char *src);
//******************************************************************************
//******************************************************************************
void ShowMenu(int id, MenuHeader *menu, int size, int cp)
{
  PopupMenuItem  *popupitem = (PopupMenuItem *)((char *)menu + sizeof(MenuHeader));
  NormalMenuItem *normalitem = (NormalMenuItem *)((char *)menu + sizeof(MenuHeader));
  MT_OS2  *os2menu, *submenu;
  MTI_OS2 *menuitem;
  int newsize;
  ULONG   ulCpSize, ulCP;

  //First save original win32 resource
  OS2Exe.StoreWin32Resource(id, RT_MENU, size, (char *)menu);

  os2menu           = (MT_OS2 *)malloc(size*4); //should always be sufficient
  os2menu->len      = sizeof(MT_OS2) - sizeof(MTI_OS2);
  if(cp == 0)
    os2menu->codepage         = 437;
  else
  {
    DosQueryCp(sizeof(ulCP), &ulCP, &ulCpSize);
    os2menu->codepage = ulCP;
  }
  os2menu->reserved = 4;
  os2menu->cMti     = 0;
  menuitem          = &os2menu->rgMti[0];
  submenu           = os2menu;
  while(size > 4) {
    if(popupitem->fItemFlags & POPUP) {
        cout << "POPUP ";
        newsize = ProcessSubMenu(popupitem, os2menu, menuitem, size, cp);
    }
    else    newsize = ProcessMenuItem(normalitem, os2menu, menuitem, size, cp);

    if(popupitem->fItemFlags & ENDMENU) break;

    popupitem  = (PopupMenuItem *)((char *)popupitem + (size - newsize));
    normalitem = (NormalMenuItem *)((char *)normalitem + (size - newsize));

        menuitem   = (MTI_OS2 *)((int)os2menu + os2menu->len);
    size       = newsize;
  }
  OS2Exe.StoreResource(id, RT_MENU, os2menu->len, (char *)os2menu);
  free(os2menu);
}
//******************************************************************************
//******************************************************************************
static int ProcessSubMenu(PopupMenuItem *popupitem, MT_OS2 *os2menu, MTI_OS2 *menuitem, int size, int cp)
{
NormalMenuItem *normalitem;
MT_OS2         *submenu;
int             len = 0, newsize;
ULONG           ulCpSize, ulCP;

  menuitem->afStyle = MIS_SUBMENU;

  //A unique id is sufficient, since it's not possible to get info about
  //the SUBMENU item being selected with the (win32) WM_INITMENU message
  //(this is possible in OS/2)
  menuitem->idItem  = (USHORT)OS2Exe.GetUniqueId();
  menuitem->pad     = 0;

  if(popupitem->fItemFlags & GRAYED) {
    cout << "GRAYED ";
    menuitem->pad     |= MIA_DISABLED;
  }
  if(popupitem->fItemFlags & INACTIVE) {
    cout << "INACTIVE ";
    menuitem->pad     |= MIA_DISABLED;
  }
  if(popupitem->fItemFlags & BITMAP) {
    cout << "BITMAP ";
    //where's the bitmap stored? (same id as id of this menu item?)
    menuitem->afStyle |= MIS_BITMAP;
  }
  if(popupitem->fItemFlags & OWNERDRAW) {
    cout << "OWNERDRAW ";
    menuitem->afStyle |= MIS_OWNERDRAW;
  }
  if(popupitem->fItemFlags & CHECKED) {
    menuitem->pad     |= MIA_CHECKED;
    cout << "CHECKED ";
  }
  if(popupitem->fItemFlags & MENUBREAK) {
    menuitem->afStyle |= MIS_BREAK;
    menuitem->pad     |= MIA_DISABLED;
    cout << "MENUBREAK ";
  }
  if(popupitem->fItemFlags & MENUBARBREAK) {
    cout << "MENUBARBREAK" << endl;
    menuitem->afStyle |= MIS_BREAKSEPARATOR;
    menuitem->pad     |= MIA_DISABLED;
    menuitem->c[0]     = 0;
    menuitem->c[1]     = 0;
  }
  else {
    len = (UniStrlen(popupitem->szItemText)+1)*2;
    cout << UnicodeToAscii(popupitem->szItemText, cp) << endl;
    menustrcpy(menuitem->c, UnicodeToAscii(popupitem->szItemText, cp));
  }
  os2menu->len += sizeof(MTI_OS2);
  if(len > 4) os2menu->len += (len/2 - sizeof(menuitem->c));
  os2menu->cMti++;

  size -= (sizeof(PopupMenuItem)-2 + len);  /*PLF Sat  97-06-21 22:17:51*/
  //process next menu item
  submenu           = (MT_OS2 *)(menuitem+1);
  if(len > 4)
        submenu   = (MT_OS2 *)((char *)submenu + len/2 - sizeof(menuitem->c));
  submenu->len      = sizeof(MT_OS2) - sizeof(MTI_OS2);

  if(cp == 0)
    submenu->codepage = 437;
  else
  {
    DosQueryCp(sizeof(ulCP), &ulCP, &ulCpSize);
    submenu->codepage = ulCP;
  }

  submenu->reserved = 4;
  submenu->cMti     = 0;
  menuitem          = &submenu->rgMti[0];

  //normalitem = (NormalMenuItem *)(popupitem + 1);
  normalitem = (NormalMenuItem *)((char*)popupitem + sizeof(PopupMenuItem)-2);  /*PLF Sat  97-06-21 23:54:50*/
  normalitem = (NormalMenuItem *)((int)normalitem + len);

  while(size > 4) {
        if(normalitem->fItemFlags & POPUP) {
            cout << "POPUP ";
            newsize = ProcessSubMenu((PopupMenuItem *)normalitem, submenu, menuitem, size, cp);
        }
        else    newsize = ProcessMenuItem(normalitem, submenu, menuitem, size, cp);

                menuitem = (MTI_OS2 *)((int)submenu + submenu->len);
        if(normalitem->fItemFlags & ENDMENU) {
            os2menu->len += submenu->len;
            return(newsize);
        }
        normalitem = (NormalMenuItem *)((char *)normalitem + (size - newsize));
        size       = newsize;
  }
  os2menu->len += submenu->len;
  return(size);
}
//******************************************************************************
//******************************************************************************
static int ProcessMenuItem(NormalMenuItem *normalitem, MT_OS2 *os2menu, MTI_OS2 *menuitem, int size, int cp)
{
WCHAR          *menustring;
int             len = 0;

  cout << "ID " << normalitem->wMenuID << " ";
  menustring        = normalitem->szItemText;
  menuitem->afStyle = MIS_TEXT;
  menuitem->idItem  = normalitem->wMenuID;
  menuitem->pad     = 0;

  if(normalitem->fItemFlags & ENDMENU) {
    //signals the end of a (sub)menu
    cout << "ENDMENU ";
    len   = (UniStrlen(normalitem->szItemText)+1)*2;
    //don't store this menu item, is it's empty
    if(len == 2) {
        size -= (sizeof(NormalMenuItem)-2 + len);
        return(size);
    }
  }
  if(normalitem->fItemFlags & GRAYED) {
    cout << "GRAYED ";
    menuitem->pad     |= MIA_DISABLED;
  }
  if(normalitem->fItemFlags & INACTIVE) {
    cout << "INACTIVE ";
    menuitem->pad     |= MIA_DISABLED;
  }
  if(normalitem->fItemFlags & BITMAP) {
    cout << "BITMAP ";
    //where's the bitmap stored? (same id as id of this menu item?)
    menuitem->afStyle |= MIS_BITMAP;
  }
  if(normalitem->fItemFlags & OWNERDRAW) {
    cout << "OWNERDRAW ";
    menuitem->afStyle |= MIS_OWNERDRAW;
  }
  if(normalitem->fItemFlags & CHECKED) {
    menuitem->pad     |= MIA_CHECKED;
    cout << "CHECKED ";
  }
  if(normalitem->fItemFlags & MENUBREAK) {
    menuitem->afStyle |= MIS_BREAK;
    menuitem->pad     |= MIA_DISABLED;
    cout << "MENUBREAK ";
  }
  if(normalitem->fItemFlags & MENUBARBREAK) {
    cout << "MENUBARBREAK" << endl;
    menuitem->afStyle |= MIS_BREAKSEPARATOR;
    menuitem->pad     |= MIA_DISABLED;
    menuitem->c[0]     = 0;
    menuitem->c[1]     = 0;
  }
  else {
    len = (UniStrlen(normalitem->szItemText)+1)*2;
        if(normalitem->fItemFlags == 0 && len == 2) {
        cout << "SEPARATOR" << endl;
        menuitem->afStyle  = MIS_SEPARATOR;
        menuitem->pad      = MIA_DISABLED;
        menuitem->idItem   = 0xFFFF;
        menuitem->c[0]     = 0;
        menuitem->c[1]     = 0;
    }
    else {
        cout << UnicodeToAscii(normalitem->szItemText, cp) << endl;
        menustrcpy(menuitem->c, UnicodeToAscii(normalitem->szItemText, cp));
    }
  }
  os2menu->len += sizeof(MTI_OS2);
  //SvL: Bugfix (>4), 19-10-'97
  if(len >= 4) os2menu->len += (len/2 - sizeof(menuitem->c));
  else         os2menu->len -= 2;   //REQUIRED (even though mi->c[2])

  os2menu->cMti++;

  size -= (sizeof(NormalMenuItem)-2 + len); /*PLF Sat  97-06-21 22:19:57*/

  return(size);
}
//******************************************************************************
//******************************************************************************
static void menustrcpy(char *dest, char *src)
{
 int i, len = strlen(src);

  for(i=0;i<len;i++) {
    if(src[i] == '&')   dest[i] = '~';
    else            dest[i] = src[i];
  }
  dest[len] = 0;
}
//******************************************************************************
//******************************************************************************
