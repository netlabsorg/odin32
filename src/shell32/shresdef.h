/* $Id: shresdef.h,v 1.9 2001-04-23 10:47:23 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef __WINE_SHELL_RES_H
#define __WINE_SHELL_RES_H


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

/*
 * columntitles for the shellview
 */
#define IDS_SHV_COLUMN1         7
#define IDS_SHV_COLUMN2         8
#define IDS_SHV_COLUMN3         9
#define IDS_SHV_COLUMN4         10
#define IDS_SHV_COLUMN5         11
#define IDS_SHV_COLUMN6		12
#define IDS_SHV_COLUMN7		13
#define IDS_SHV_COLUMN3DV       12
#define IDS_SHV_COLUMN4DV       13

#define IDS_SELECT              22
#define IDS_OPEN                23
#define IDS_VIEW_LARGE          24
#define IDS_VIEW_SMALL          25
#define IDS_VIEW_LIST           26
#define IDS_VIEW_DETAILS        27

#define IDS_CREATEFOLDER_DENIED 30
#define IDS_CREATEFOLDER_CAPTION 31
#define IDS_DELETEITEM_CAPTION	32
#define IDS_DELETEFOLDER_CAPTION 33
#define IDS_DELETEITEM_TEXT	34
#define IDS_DELETEMULTIPLE_TEXT	35
#ifdef __WIN32OS2__
#define	IDS_OVERWRITEFILE_TEXT 36
#define	IDS_OVERWRITEFILE_CAPTION 37
#endif

#define IDS_SHV_INVALID_FILENAME_TITLE 28
#define IDS_SHV_INVALID_FILENAME       29
#define IDS_SHV_INVALID_MOVE_TITLE     30
#define IDS_SHV_INVALID_MOVE           31

#define IDS_CREATEFOLDER_DENIED 30
#define IDS_CREATEFOLDER_CAPTION 31
#define IDS_DELETEFOLDER_TEXT	32
#define IDS_DELETEFOLDER_CAPTION 33

#define ID_LISTVIEW     2000
#define ID_NEWFOLDER    0x7053

/* browse for folder dialog box */
#define IDD_STATUS		97
#define IDD_TITLE		98
#define IDD_TREEVIEW		99

#define IDS_ABOUTBOX                    101
#define IDS_DOCUMENTFOLDERS             102
#define IDS_PERSONAL                    103
#define IDS_FAVORITES                   104
#define IDS_PATH                        105
#define IDS_DESKTOP                     106

#define IDS_FONTS                       108
#define IDS_MYCOMPUTER                  110
#define IDS_SYSTEMFOLDERS               112
#define IDS_LOCALHARDRIVES              113
#define IDS_FILENOTFOUND                114
#define IDS_35FLOPPY                    115
#define IDS_525FLOPPY                   116

#endif /* __WINE_SHELL_RES_H */


