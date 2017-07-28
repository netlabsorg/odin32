/* $Id: shell32_De.orc,v 1.2 2003-11-12 14:04:53 sandervl Exp $ */
LANGUAGE LANG_GERMAN, SUBLANG_DEFAULT

SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 216, 187
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Info �ber %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 158, 167, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 105, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 194, 75, 0, 0
 LTEXT "", 100, 8, 75, 176, 20
 LTEXT "Odin %d.%d.%d (%s) wurde entwickelt von:", 98, 8, 95, 176, 10
 CONTROL "", 2001, "Static", SS_BITMAP, 8, 8, 0, 0
}

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
	IDS_DELETEITEM_CAPTION "Best�tige L�schen der Datei"
	IDS_DELETEFOLDER_CAPTION "Best�tige L�schen des Ordners"
	IDS_DELETEITEM_TEXT "Sind Sie sicher, da� Sie '%1' l�schen m�chten?"
	IDS_DELETEMULTIPLE_TEXT "Are you sure you want to delete these %1 items?"
	IDS_OVERWRITEFILE_TEXT "Soll Datei '%1' �berschrieben werden?"
	IDS_OVERWRITEFILE_CAPTION "Best�tigen Sie das �berschreiben der Datei"
    IDS_COPY_X_OF_TEXT "Kopie%svon %s"
}

