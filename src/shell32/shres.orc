/* $Id: shres.orc,v 1.12 2001-04-23 10:47:23 sandervl Exp $ */
/*
 * Top level resource file for Common Dialogs
 *
 */

#include "winuser.h"
#include "winnls.h"
#include "shlobj.h"
#include "shresdef.h"
#include "odinrsrc.h"

/*--------------------- FIXME --------------------------
 *
 * These must be seperated into the language files
 * and translated. The language 0,0 is a hack to get it
 * loaded properly for all languages by pretending that
 * they are neutral.
 * The menus are not jet properly implemented.
 * Don't localize it yet. (js)
 */

LANGUAGE 0,0

//SvL: Odin logo bitmap
5555 BITMAP DISCARDABLE "Odinlogo.bmp"

MENU_001 MENU DISCARDABLE
BEGIN
        MENUITEM "Lar&ge Icons",        FCIDM_SHVIEW_BIGICON
        MENUITEM "S&mall Icons",        FCIDM_SHVIEW_SMALLICON
        MENUITEM "&List",               FCIDM_SHVIEW_LISTVIEW
        MENUITEM "&Details",            FCIDM_SHVIEW_REPORTVIEW
END

/*
 shellview background menu
*/
MENU_002 MENU DISCARDABLE
BEGIN
	POPUP""
	BEGIN
	  POPUP "&View"
	  BEGIN
	    MENUITEM "Lar&ge Icons",	FCIDM_SHVIEW_BIGICON
	    MENUITEM "S&mall Icons",	FCIDM_SHVIEW_SMALLICON
	    MENUITEM "&List",		FCIDM_SHVIEW_LISTVIEW
	    MENUITEM "&Details",	FCIDM_SHVIEW_REPORTVIEW
	  END
	  MENUITEM SEPARATOR
	  POPUP "Arrange &Icons"
	  BEGIN
	    MENUITEM "By &Name",	0x30	/* column 0 */
	    MENUITEM "By &Type",	0x32	/* column 2 */
	    MENUITEM "By &Size",	0x31	/* ... */
	    MENUITEM "By &Date",	0x33
	    MENUITEM SEPARATOR
	    MENUITEM "&Auto Arrange",	FCIDM_SHVIEW_AUTOARRANGE
	  END
	  MENUITEM "Line up Icons",	FCIDM_SHVIEW_SNAPTOGRID
	  MENUITEM SEPARATOR
	  MENUITEM "Refresh",		FCIDM_SHVIEW_REFRESH
	  MENUITEM SEPARATOR
	  MENUITEM "Paste",		FCIDM_SHVIEW_INSERT
	  MENUITEM "Paste as Link",	FCIDM_SHVIEW_INSERTLINK
	  MENUITEM SEPARATOR
	  POPUP "New"
	  BEGIN
	    MENUITEM "New &Folder",	FCIDM_SHVIEW_NEWFOLDER
	    MENUITEM "New &Link",	FCIDM_SHVIEW_NEWLINK
	    MENUITEM SEPARATOR
	  END
	  MENUITEM SEPARATOR
	  MENUITEM "Properties",	FCIDM_SHVIEW_PROPERTIES
	END
END

/*
 shellview item menu
*/
MENU_SHV_FILE MENU DISCARDABLE
BEGIN
	POPUP""
	BEGIN
	  MENUITEM "E&xplore",		FCIDM_SHVIEW_CUT
	  MENUITEM "&Open",		FCIDM_SHVIEW_COPY
	  MENUITEM SEPARATOR
	  MENUITEM "C&ut",		FCIDM_SHVIEW_CUT
	  MENUITEM "&Copy",		FCIDM_SHVIEW_COPY
	  MENUITEM SEPARATOR
	  MENUITEM "&Create Link",	FCIDM_SHVIEW_CREATELINK
	  MENUITEM "&Delete",		FCIDM_SHVIEW_DELETE
	  MENUITEM "&Rename",		FCIDM_SHVIEW_RENAME
	  MENUITEM SEPARATOR
	  MENUITEM "&Properties",	FCIDM_SHVIEW_PROPERTIES
	END
END

SHBRSFORFOLDER_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 188, 192
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Choose a Directory:"
FONT 8, "Helv"
{
 DEFPUSHBUTTON "OK", 1, 80, 176, 50, 12, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Cancel", 2, 134, 176, 50, 12, WS_GROUP | WS_TABSTOP
 LTEXT "", IDD_TITLE, 4, 4, 18, 12
 LTEXT "", IDD_STATUS, 4, 25, 18, 12
 CONTROL "Tree1",IDD_TREEVIEW,"SysTreeView32",
 	TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT |
	WS_BORDER | WS_TABSTOP,
	4, 40, 180, 120
}

/*
        special folders
*/
STRINGTABLE DISCARDABLE
{
        IDS_DESKTOP             "Desktop"
        IDS_MYCOMPUTER          "My Computer"
}

/*
        context menus
*/
STRINGTABLE DISCARDABLE
{
        IDS_VIEW_LARGE          "Lar&ge Icons"
        IDS_VIEW_SMALL          "S&mall Icons"
        IDS_VIEW_LIST           "&List"
        IDS_VIEW_DETAILS        "&Details"
        IDS_SELECT              "Select"
        IDS_OPEN                "Open"
}

STRINGTABLE DISCARDABLE
{
	IDS_CREATEFOLDER_DENIED "Can not create new Folder: Permission denied."
	IDS_CREATEFOLDER_CAPTION "Error during creating a new folder"
	IDS_DELETEITEM_CAPTION "Confirm file delete"
	IDS_DELETEFOLDER_CAPTION "Confirm folder delete"
	IDS_DELETEITEM_TEXT "Are you sure you want to delete '%1'?"
	IDS_DELETEMULTIPLE_TEXT "Are you sure you want to delete these %1 items?"
#ifdef __WIN32OS2__
	IDS_OVERWRITEFILE_TEXT "OverWrite File %1?"
	IDS_OVERWRITEFILE_CAPTION "Confirm File OverWrite"
#endif
}

shv_accel ACCELERATORS
BEGIN
	VK_F5, FCIDM_SHVIEW_REFRESH, VIRTKEY
END

/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6 + IE 5.01)
//

1 VERSIONINFO
 FILEVERSION    0x05, 0x00, 0x0B67, 0x18A0
 PRODUCTVERSION 0x05, 0x00, 0x0B67, 0x18A0
 FILEFLAGSMASK 0x3fL
 FILEFLAGS 0xaL
 FILEOS 0x10001L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "Comments", "Odin32 System Dll\0"
            VALUE "CompanyName", "Odin Team\0"
            VALUE "FileDescription", "\0"
            VALUE "FileVersion", "5.00.2919.6304\0"
            VALUE "InternalName", "Odin32 - SHELL32\0"
            VALUE "LegalCopyright", "Copyright (C) 2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "\0"
            VALUE "ProductName", "Odin32 - SHELL32\0"
            VALUE "ProductVersion", "5.00.2919.6304\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 0x04E4
    END
END

/* These are the first 45 icons from shell32.dll */
/* See ico/readme.txt for copyright info         */
#include "shellicon.h"

SHLICON_FILE   ICON LOADONCALL DISCARDABLE "ico/file.ico"
SHLICON_TEXTFILE   ICON LOADONCALL DISCARDABLE "ico/textfile.ico"
SHLICON_APPLICATION   ICON LOADONCALL DISCARDABLE "ico/application.ico"
SHLICON_FOLDER_CLOSED   ICON LOADONCALL DISCARDABLE "ico/folder-c.ico"
SHLICON_FOLDER_OPEN   ICON LOADONCALL DISCARDABLE "ico/folder-o.ico"
SHLICON_FLOPPY525   ICON LOADONCALL DISCARDABLE "ico/floppy5.ico"
SHLICON_FLOPPY35   ICON LOADONCALL DISCARDABLE "ico/floppy3.ico"
SHLICON_REMOVABLE_DISK   ICON LOADONCALL DISCARDABLE "ico/removable.ico"
SHLICON_HARDDISK   ICON LOADONCALL DISCARDABLE "ico/harddrive.ico"
SHLICON_NETDRIVE_CONN  ICON LOADONCALL DISCARDABLE "ico/netdrive-c.ico"
SHLICON_NETDRIVE_DISCON  ICON LOADONCALL DISCARDABLE "ico/netdrive-d.ico"
SHLICON_CDROM_DRIVE  ICON LOADONCALL DISCARDABLE "ico/cdromdrive.ico"
SHLICON_RAMDRIVE  ICON LOADONCALL DISCARDABLE "ico/ramdrive.ico"
SHLICON_ENTIRENETWORK  ICON LOADONCALL DISCARDABLE "ico/entirenetwork.ico"
SHLICON_NETWORK  ICON LOADONCALL DISCARDABLE "ico/network.ico"
SHLICON_NETWORK_COMP  ICON LOADONCALL DISCARDABLE "ico/networkcomputer.ico"
SHLICON_PRINTER  ICON LOADONCALL DISCARDABLE "ico/printer.ico"
SHLICON_NETWORK_PLACE  ICON LOADONCALL DISCARDABLE "ico/networkplaces.ico"
SHLICON_NETWORK_GROUP  ICON LOADONCALL DISCARDABLE "ico/networkgroup.ico"
SHLICON_PROGRAMS  ICON LOADONCALL DISCARDABLE "ico/programs.ico"
SHLICON_RECENTDOCS  ICON LOADONCALL DISCARDABLE "ico/recentdocs.ico"
SHLICON_SETTINGS  ICON LOADONCALL DISCARDABLE "ico/settings.ico"
SHLICON_SEARCH  ICON LOADONCALL DISCARDABLE "ico/search.ico"
SHLICON_HELP  ICON LOADONCALL DISCARDABLE "ico/help.ico"
SHLICON_RUN  ICON LOADONCALL DISCARDABLE "ico/run.ico"
SHLICON_SUSPEND  ICON LOADONCALL DISCARDABLE "ico/suspend.ico"
SHLICON_UNDOCK  ICON LOADONCALL DISCARDABLE "ico/undock.ico"
SHLICON_SHUTDOWN  ICON LOADONCALL DISCARDABLE "ico/shutdown.ico"
SHLICON_SHAREDRES  ICON LOADONCALL DISCARDABLE "ico/sharedres.ico"
SHLICON_SHORTCUT  ICON LOADONCALL DISCARDABLE "ico/shortcut.ico"
SHLICON_ARROW  ICON LOADONCALL DISCARDABLE "ico/arrow.ico"
SHLICON_TRASHCAN  ICON LOADONCALL DISCARDABLE "ico/trash-e.ico"
SHLICON_TRASHCAN_FULL  ICON LOADONCALL DISCARDABLE "ico/trash-f.ico"
SHLICON_DIALUP_FOLDER  ICON LOADONCALL DISCARDABLE "ico/dialup.ico"
SHLICON_DESKTOP  ICON LOADONCALL DISCARDABLE "ico/desktop.ico"
SHLICON_CONTROLPANEL  ICON LOADONCALL DISCARDABLE "ico/controlpanel.ico"
SHLICON_PROGRAMGROUP  ICON LOADONCALL DISCARDABLE "ico/programgroup.ico"
SHLICON_PRINTER_FOLDER  ICON LOADONCALL DISCARDABLE "ico/printers.ico"
SHLICON_FONT_FOLDER  ICON LOADONCALL DISCARDABLE "ico/fonts.ico"
SHLICON_LOGO  ICON LOADONCALL DISCARDABLE "ico/logo.ico"
SHLICON_AUDIO_CD  ICON LOADONCALL DISCARDABLE "ico/audiocd.ico"
SHLICON_TREE  ICON LOADONCALL DISCARDABLE "ico/tree.ico"
SHLICON_COMPUTERS  ICON LOADONCALL DISCARDABLE "ico/computers.ico"
SHLICON_FAVORITES  ICON LOADONCALL DISCARDABLE "ico/favorites.ico"
SHLICON_LOGOFF  ICON LOADONCALL DISCARDABLE "ico/logoff.ico"

/* Here are still missing lots of icons from shell32! */

/* This one is originally in explorer.exe, index 1 */
SHLICON_MYCOMPUTER ICON LOADONCALL DISCARDABLE "ico/mycomputer.ico"

/*--------------------- END FIXME ------------------------*/

/*
 * Everything that does not depend on language,
 * like textless bitmaps etc, go into the
 * neutral language. This will prevent them from
 * being duplicated for each language.
 */
#include "shell32_xx.orc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */
#include "shell32_En.orc"

/*
 CB: the new about dialog isn't implemented for these languages
     other resources aren't translated so far
     -> don't use these resources for now

     do you want to translate them? just do it.
*/

//#include "shell32_Ca.orc"
//#include "shell32_Cs.orc"
//#include "shell32_Da.orc"
//#include "shell32_De.orc"
//#include "shell32_Eo.orc"
//#include "shell32_Es.orc"
//#include "shell32_Fi.orc"
//#include "shell32_Fr.orc"
//#include "shell32_Hu.orc"
//#include "shell32_It.orc"
//#include "shell32_Ko.orc"
//#include "shell32_Nl.orc"
//#include "shell32_No.orc"
//#include "shell32_Pl.orc"
//#include "shell32_Pt.orc"
//#include "shell32_Ru.orc"
//#include "shell32_Sv.orc"
//#include "shell32_Wa.orc"

