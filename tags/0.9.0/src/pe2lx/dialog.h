/* $Id: dialog.h,v 1.4 1999-07-06 08:50:11 sandervl Exp $ */

/*
 * PE2LX dialog conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DIALOG_H__
#define __DIALOG_H__

#pragma pack(1) /*PLF Sat  97-06-21 20:19:30*/

//dialog structs
typedef struct {
  DWORD  lStyle;
  DWORD  lExtendedStyle;     // new for NT
  WORD   NumberOfItems;
  WORD   x;
  WORD   y;
  WORD   cx;
  WORD   cy;
  WORD   fNameOrd;		//0xFFFF -> ord, else null terminated WCHAR string
} DialogBoxHeader;
//  [Name or Ordinal] MenuName;
////The items marked `Name or Ordinal' are the same format used
////throughout the resource file (most notably in each resource header)
////to store a name or an ordinal ID.  As before, if the first WORD is an
////0xffff, the next two bytes contain an ordinal ID.  Otherwise, the
////first 1 or more WORDS contain a null-terminated string.  An empty
////string is represented by a single WORD zero in the first location.
//  [Name or Ordinal] ClassName;
//  WCHAR  szCaption[];

typedef struct {
  WORD   wPointSize;         // Only here if FONT set for dialog
  WCHAR  szFontName[1];       // This too   /*PLF Sat  97-06-21 20:19:31*/
} FontHeader;

#define DS_SETFONT	0x40

typedef struct {
  DWORD  lStyle;
  DWORD  lExtendedStyle;
  WORD   x;
  WORD   y;
  WORD   cx;
  WORD   cy;
  WORD   wId;
  WORD   fClassId;	//0xFFFF -> ord, 0 -> null string, else null terminated WCHAR string
//  [Name or Ordinal] ClassId;
//  [Name or Ordinal] Text;
//  WORD   nExtraStuff;
} ControlData;


typedef struct tagWINDLGTEMPLATEEX{
    WORD  wDlgVer;          // use 1
    WORD  wSignature;       // use 0xFFFF
    DWORD dwHelpID;         // Dialog's context help ID
    DWORD lExtendedStyle;
    DWORD lStyle;
    WORD  NumberOfItems;    // Number of controls in dialog
    short x;                // Initial position, horizontal
    short y;                // Initial position, vertical
    short cx;               // Width
    short cy;               // Height
    WORD  fNameOrd;  	    // 0xFFFF -> ord, else null terminated WCHAR string
} WINDLGTEMPLATEEX;
//This is followed by menu name, class name, title, and font info if style
//includes DS_SETFONT, which have the same form as documented in the SDK for
//DLGTEMPLATE.

typedef struct tagWINDLGITEMTEMPLATEEX{
    DWORD dwHelpID;         // Context help ID for control
    DWORD lExtendedStyle;
    DWORD lStyle;
    short x;                // Initial position, horizontal
    short y;                // Initial position, vertical
    short cx;               // Width
    short cy;               // Height
    DWORD wId;              // Window ID
    WORD  fClassId;         // 0xFFFF -> ord, 0 -> null string, else null terminated WCHAR string
//  [Name or Ordinal] ClassId;
//  [Name or Ordinal] Text;
} WINDLGITEMTEMPLATEEX;
//This is followed by class name, title, and creation data for the control,
//which have the same form as documented in the SDK for DLGITEMTEMPLATE.

//SvL: Yeah, right!
typedef struct {
  WORD   wPointSize;          // Only here if FONT set for dialog
  DWORD  whatever;
  WCHAR  szFontName[1];       // This too   /*PLF Sat  97-06-21 20:19:31*/
} FontHeaderEx;

#pragma pack()

void ShowDialog(int id, DialogBoxHeader *dhdr, int size, int cp=0);


#define WINWS_POPUP            0x80000000L
#define WINWS_CHILD            0x40000000L
#define WINWS_MINIMIZE         0x20000000L
#define WINWS_VISIBLE          0x10000000L
#define WINWS_DISABLED         0x08000000L
#define WINWS_CLIPSIBLINGS     0x04000000L
#define WINWS_CLIPCHILDREN     0x02000000L
#define WINWS_MAXIMIZE         0x01000000L
#define WINWS_CAPTION          0x00C00000L
#define WINWS_BORDER           0x00800000L
#define WINWS_DLGFRAME         0x00400000L
#define WINWS_VSCROLL          0x00200000L
#define WINWS_HSCROLL          0x00100000L
#define WINWS_SYSMENU          0x00080000L
#define WINWS_THICKFRAME       0x00040000L
#define WINWS_GROUP            0x00020000L
#define WINWS_TABSTOP          0x00010000L
#define WINWS_MINIMIZEBOX      0x00020000L
#define WINWS_MAXIMIZEBOX      0x00010000L

/*
 * Extended Window Styles
 */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L

#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L


#define WIN_BUTTON              0x80
#define WIN_EDIT                0x81
#define WIN_STATIC              0x82
#define WIN_LISTBOX             0x83
#define WIN_SCROLLBAR           0x84
#define WIN_COMBOBOX            0x85

#define WINBS_PUSHBUTTON        0x00L
#define WINBS_DEFPUSHBUTTON     0x01L
#define WINBS_CHECKBOX          0x02L
#define WINBS_AUTOCHECKBOX      0x03L
#define WINBS_RADIOBUTTON       0x04L
#define WINBS_3STATE            0x05L
#define WINBS_AUTO3STATE        0x06L
#define WINBS_GROUPBOX          0x07L
#define WINBS_USERBUTTON        0x08L
#define WINBS_AUTORADIOBUTTON   0x09L
#define WINBS_OWNERDRAW         0x0BL
#define WINBS_LEFTTEXT          0x20L

#define WINCBS_SIMPLE            0x0001L
#define WINCBS_DROPDOWN          0x0002L
#define WINCBS_DROPDOWNLIST      0x0003L
#define WINCBS_OWNERDRAWFIXED    0x0010L
#define WINCBS_OWNERDRAWVARIABLE 0x0020L
#define WINCBS_AUTOHSCROLL       0x0040L
#define WINCBS_OEMCONVERT        0x0080L
#define WINCBS_SORT              0x0100L
#define WINCBS_HASSTRINGS        0x0200L
#define WINCBS_NOINTEGRALHEIGHT  0x0400L
#define WINCBS_DISABLENOSCROLL   0x0800L

#define WINES_LEFT              0x0000L
#define WINES_CENTER            0x0001L
#define WINES_RIGHT             0x0002L
#define WINES_MULTILINE         0x0004L
#define WINES_UPPERCASE         0x0008L
#define WINES_LOWERCASE         0x0010L
#define WINES_PASSWORD          0x0020L
#define WINES_AUTOVSCROLL       0x0040L
#define WINES_AUTOHSCROLL       0x0080L
#define WINES_NOHIDESEL         0x0100L
#define WINES_OEMCONVERT        0x0400L
#define WINES_READONLY          0x0800L
#define WINES_WANTRETURN        0x1000L

#define WINLBS_NOTIFY               0x0001L
#define WINLBS_SORT                 0x0002L
#define WINLBS_NOREDRAW             0x0004L
#define WINLBS_MULTIPLESEL          0x0008L
#define WINLBS_OWNERDRAWFIXED       0x0010L
#define WINLBS_OWNERDRAWVARIABLE    0x0020L
#define WINLBS_HASSTRINGS           0x0040L
#define WINLBS_USETABSTOPS          0x0080L
#define WINLBS_NOINTEGRALHEIGHT     0x0100L
#define WINLBS_MULTICOLUMN          0x0200L
#define WINLBS_WANTKEYBOARDINPUT    0x0400L
#define WINLBS_EXTENDEDSEL          0x0800L
#define WINLBS_DISABLENOSCROLL      0x1000L
#define WINLBS_NODATA               0x2000L

#define WINSBS_HORZ                    0x0000L
#define WINSBS_VERT                    0x0001L
#define WINSBS_TOPALIGN                0x0002L
#define WINSBS_LEFTALIGN               0x0002L
#define WINSBS_BOTTOMALIGN             0x0004L
#define WINSBS_RIGHTALIGN              0x0004L
#define WINSBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define WINSBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define WINSBS_SIZEBOX                 0x0008L

#define WINSS_LEFT              0x00L
#define WINSS_CENTER            0x01L
#define WINSS_RIGHT             0x02L
#define WINSS_ICON              0x03L
#define WINSS_BLACKRECT         0x04L
#define WINSS_GRAYRECT          0x05L
#define WINSS_WHITERECT         0x06L
#define WINSS_BLACKFRAME        0x07L
#define WINSS_GRAYFRAME         0x08L
#define WINSS_WHITEFRAME        0x09L
#define WINSS_USERITEM          0x0AL
#define WINSS_SIMPLE            0x0BL
#define WINSS_LEFTNOWORDWRAP    0x0CL
#define WINSS_NOPREFIX          0x80L

#endif
