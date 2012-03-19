/* $Id: shell32_De.orc,v 1.2 2003-11-12 14:04:53 sandervl Exp $ */
LANGUAGE LANG_GERMAN, SUBLANG_DEFAULT

#if 1
SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 216, 170
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Info über %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 156, 147, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 85, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 189, 50, 14, 16
 LTEXT "", 100, 8, 50, 137, 33
 LTEXT "Odin wurde entwickelt von (%d.%d.%d, %s):", 98, 8, 75, 137, 10
 CONTROL "",2001,"Static",SS_BITMAP,8,2,148,26
}
#else
SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 210, 152
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Info über %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 153, 130, 50, 12
 LISTBOX 99, 8, 65, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 189, 10, 14, 16
 LTEXT "", 100, 8, 10, 137, 33
 LTEXT "Odin wurde entwickelt von:", 98, 8, 55, 137, 10
}
#endif

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "&About FolderPicker Test"
    IDS_DOCUMENTFOLDERS     "Dokumenten Ordner"
    IDS_PERSONAL            "Meine Dokumente"
    IDS_FAVORITES           "Meine Favoriten"
    IDS_PATH                "System Path"
    IDS_DESKTOP             "Arbeitsplatz"
    IDS_FONTS               "Fonts"
    IDS_MYCOMPUTER          "Mein Computer"
    IDS_SYSTEMFOLDERS       "System Ordner"
    IDS_LOCALHARDRIVES      "Lokal Festplatten"
    IDS_FILENOTFOUND        "Datei nicht gefunden"
    IDS_35FLOPPY            "3.5 disk"
    IDS_525FLOPPY           "5.25 disk"
}

STRINGTABLE DISCARDABLE
{
	IDS_CREATEFOLDER_DENIED "Kann den neuen Ordner nicht anlegen: Zugriff abgelehnt."
	IDS_CREATEFOLDER_CAPTION "Fehler beim Ordneranlegen"
	IDS_DELETEITEM_CAPTION "Bestätige Löschen der Datei"
	IDS_DELETEFOLDER_CAPTION "Bestätige Löschen des Ordners"
	IDS_DELETEITEM_TEXT "Sind Sie sicher, daß Sie '%1' löschen möchten?"
	IDS_DELETEMULTIPLE_TEXT "Are you sure you want to delete these %1 items?"
	IDS_OVERWRITEFILE_TEXT "Soll Datei '%1' überschrieben werden?"
	IDS_OVERWRITEFILE_CAPTION "Bestätigen Sie das Überschreiben der Datei"
    IDS_COPY_X_OF_TEXT "Kopie%svon %s"
}

