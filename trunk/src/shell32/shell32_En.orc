/* $Id: shell32_En.orc,v 1.4 2000-10-28 14:36:45 sandervl Exp $ */
LANGUAGE LANG_ENGLISH, SUBLANG_DEFAULT

#if 1
SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 216, 170
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "About %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 156, 147, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 85, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 189, 50, 14, 16
 LTEXT "", 100, 8, 50, 137, 33
 LTEXT "Odin was brought to you by:", 98, 8, 75, 90, 10
 LTEXT "", 2002, 100, 75, 90, 10
 CONTROL "",2001,"Static",SS_BITMAP,8,2,148,26
}
#else
SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 210, 152
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "About %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 153, 130, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 65, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 189, 10, 14, 16
 LTEXT "", 100, 8, 10, 137, 33
 LTEXT "Odin was brought to you by:", 98, 8, 55, 137, 10
}
#endif

/*      columns in the shellview        */
STRINGTABLE LANGUAGE LANG_NEUTRAL, SUBLANG_NEUTRAL
BEGIN
        IDS_SHV_COLUMN1         "File"
        IDS_SHV_COLUMN2         "Size"
        IDS_SHV_COLUMN3         "Type"
        IDS_SHV_COLUMN4         "Modified"
        IDS_SHV_COLUMN5         "Attrs"
        IDS_SHV_COLUMN3DV       "Size"
        IDS_SHV_COLUMN4DV       "Size available"

END

STRINGTABLE DISCARDABLE
{
        IDS_SHV_INVALID_FILENAME_TITLE  "Rename"
        IDS_SHV_INVALID_FILENAME        "A filename cannot contain any of the following characters: \n                          / \\ : * ? \" < > |"
        IDS_SHV_INVALID_MOVE_TITLE      "Error Renaming File"
        IDS_SHV_INVALID_MOVE            "Cannot rename %s: A file with the same you specified already exists.  Specify a different filename."
}

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "&About FolderPicker Test"
    IDS_DOCUMENTFOLDERS     "Document Folders"
    IDS_PERSONAL            "My Documents"
    IDS_FAVORITES           "My Favorites"
    IDS_PATH                "System Path"
    IDS_DESKTOP             "Desktop"
    IDS_FONTS               "Fonts"
    IDS_MYCOMPUTER          "My Computer"
    IDS_SYSTEMFOLDERS       "System Folders"
    IDS_LOCALHARDRIVES      "Local Hard Drives"
    IDS_FILENOTFOUND        "File not found"
    IDS_35FLOPPY            "3.5 disk"
    IDS_525FLOPPY           "5.25 disk"
}

