/* $Id: shell32_En.orc,v 1.3 2003-11-12 14:04:53 sandervl Exp $ */
LANGUAGE LANG_ENGLISH, SUBLANG_DEFAULT

SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 216, 187
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "About %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 158, 167, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 105, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 194, 75, 0, 0
 LTEXT "", 100, 8, 75, 176, 20
 LTEXT "Odin %d.%d.%d (%s) was brought to you by:", 98, 8, 95, 176, 10
 CONTROL "", 2001, "Static", SS_BITMAP, 8, 8, 0, 0
}

SHELL_RUN_DLG DIALOG LOADONCALL MOVEABLE DISCARDABLE 0, 0, 227, 95
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION ""
FONT 8, "Helv"
{
 ICON "", 12297, 7, 11, 18, 20, WS_VISIBLE
 LTEXT "Type the name of a program, folder, document, or Internet resource, and Wine will open it for you.", 12289, 36, 11, 182, 18
 LTEXT "&Open:", 12305, 7, 39, 24, 10
 CONTROL "", 12298, "COMBOBOX", WS_GROUP | WS_VSCROLL | WS_VISIBLE | CBS_DISABLENOSCROLL | CBS_AUTOHSCROLL | CBS_DROPDOWN, 36, 37, 183, 100
 DEFPUSHBUTTON "OK", IDOK, 62, 63, 50, 14, WS_TABSTOP
 PUSHBUTTON "Cancel", IDCANCEL, 116, 63, 50, 14, WS_TABSTOP
 PUSHBUTTON "&Browse...", 12288, 170, 63, 50, 14, WS_TABSTOP
}

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
#if 0
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
    IDS_COPY_X_OF_TEXT "Copy () of "
#endif
}
#endif

