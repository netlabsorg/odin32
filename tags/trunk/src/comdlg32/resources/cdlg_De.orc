/*
 * Copyright 1994 Martin von Loewis
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

LANGUAGE LANG_GERMAN, SUBLANG_DEFAULT

OPEN_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�ffnen"
FONT 8, "Helv"
{
 LTEXT "Datei&name:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Verzeichnisse:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "Liste der Datei&typen:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&Laufwerke:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "�ffnen", IDOK, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Hilfe", pshHelp, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "&Schreibgesch�tzt", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


SAVE_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Speichern als ..."
FONT 8, "Helv"
{
 LTEXT "Datei&name:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Verzeichnisse:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "Liste der Datei&typen:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&Laufwerke:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "Speichern als", IDOK, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Hilfe", pshHelp, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "&Schreibgesch�tzt", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Drucken"
FONT 8, "Helv"
{
 LTEXT "Drucker:", 1088, 6, 6, 40, 9
 LTEXT "", 1089, 60, 6, 150, 9
 GROUPBOX "Druckbereicht", 1072, 6, 30, 160, 65, BS_GROUPBOX
 RADIOBUTTON "&Alles", rad1, 16, 45, 60, 12
 RADIOBUTTON "A&uswahl", rad2, 16, 60, 60, 12
 RADIOBUTTON "&Seiten", rad3, 16, 75, 60, 12
 DEFPUSHBUTTON "Drucken", IDOK, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Einrichten", psh1, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 LTEXT "&Von:", 1090, 60, 80, 30, 9
 LTEXT "&Bis:", 1091, 120, 80, 30, 9
 LTEXT "Druck&qualit�t:", 1092, 6, 100, 76, 9
 COMBOBOX 1136, 80, 100, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 CHECKBOX "In &Datei drucken", 1040, 20, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Verdichtet", 1041, 160, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Druckereinrichtung"
FONT 8, "Helv"
{
 GROUPBOX "Drucker", 1072, 6, 10, 180, 65, BS_GROUPBOX
 RADIOBUTTON "&Standarddrucker", rad1, 16, 20, 80, 12
 LTEXT "[keiner]", 1088, 35, 35, 120, 9
 RADIOBUTTON "Bestimmter &Drucker", rad2, 16, 50, 80, 12
 COMBOBOX 1136, 35, 65, 149, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "Ok", IDOK, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Einrichten", psh1, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 GROUPBOX "Orientierung", 1073, 6, 85, 100, 50, BS_GROUPBOX
 RADIOBUTTON "&Hochformat", rad3, 50, 100, 40, 12
 RADIOBUTTON "&Querformat", rad4, 50, 115, 40, 12
 ICON "LANDSCAP", 1097, 10, 95, 32, 32
 ICON "PORTRAIT", 1098, 10, 95, 32, 32
 GROUPBOX "Paper", 1074, 120, 85, 180, 50, BS_GROUPBOX
 LTEXT "&Gr��e", 1089, 130, 95, 30, 9
 LTEXT "P&apierquelle", 1090, 130, 110, 30, 9
 COMBOBOX 1137, 155, 95, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 COMBOBOX 1138, 155, 110, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
}

PAGESETUPDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 32, 32, 240, 240
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Seite einrichten"
FONT 8, "Helv"
BEGIN
  CONTROL "", rct1, 130, 0x00020006, 80, 8, 80, 80,  SS_WHITERECT
  CONTROL "", rct2, 130, 0x00020005, 160, 12, 4, 80, SS_GRAYRECT
  CONTROL "", rct3, 130, 0x00020005, 84, 88, 80, 4,  SS_GRAYRECT
  GROUPBOX "Papier", grp2, 8, 96, 224, 56, BS_GROUPBOX
  LTEXT "&Gr��e:", stc2, 16, 112, 36, 8
  COMBOBOX cmb2, 64, 110, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  LTEXT "&Zufuhr:", stc3, 16, 132, 36, 8
  COMBOBOX cmb3, 64, 130, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  GROUPBOX "Format", grp1, 8, 156, 64, 56, BS_GROUPBOX
  AUTORADIOBUTTON "&Hochformat", rad1, 16, 170, 52, 12, BS_AUTORADIOBUTTON
  AUTORADIOBUTTON "&Querformat", rad2, 16, 190, 52, 12, BS_AUTORADIOBUTTON
  GROUPBOX "R�nder", grp4, 80, 156, 152, 56, BS_GROUPBOX
  LTEXT "&Links:", stc15, 88, 172, 21, 8
  EDITTEXT edt4, 111, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Rechts:", stc16, 159, 172, 27, 8
  EDITTEXT edt6, 187, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Oben:", stc17, 88, 192, 21, 8
  EDITTEXT edt5, 111, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Unten:", stc18, 159, 192, 23, 8
  EDITTEXT edt7, 187, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  DEFPUSHBUTTON "OK", IDOK, 71, 220, 50, 14, BS_PUSHBUTTON
  PUSHBUTTON "Abbrechen", IDCANCEL, 126, 220, 50, 14
  PUSHBUTTON "&Drucker...", psh3, 184, 220, 48, 14
END


CHOOSE_FONT DIALOG DISCARDABLE  13, 54, 264, 147
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Font"
FONT 8, "Helv"
{
    LTEXT           "Schrift&art:",1088 ,6,3,40,9
    COMBOBOX        1136 ,6,13,94,54,  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    CBS_AUTOHSCROLL | CBS_SORT | WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "Schrift&stil:",1089 ,108,3,44,9
    COMBOBOX        1137,108,13,64,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "Schrift&gr��e:",1090,179,3,30,9
    COMBOBOX        1138,179,13,32,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE | CBS_SORT
    DEFPUSHBUTTON   "OK",IDOK,218,6,40,14,WS_GROUP
    PUSHBUTTON      "Abbrechen",IDCANCEL,218,23,40,14,WS_GROUP
    PUSHBUTTON      "&Zuweisen", psh3,218,40,40,14,WS_GROUP
    PUSHBUTTON      "&Hilfe" , pshHelp,218,57,40,14,WS_GROUP
    GROUPBOX        "Darstellung",1072,6,72,84,34,WS_GROUP
    CHECKBOX	    "&Durchgestrichen", 1040, 10,82,50,10, BS_AUTOCHECKBOX | WS_TABSTOP
    CHECKBOX 	    "&Unterstrichen", 1041, 10,94,50,10, BS_AUTOCHECKBOX
    LTEXT           "&Farbe:", 1091 ,6,110,30,9
    COMBOBOX        1139,6,120,84,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
		    CBS_AUTOHSCROLL |  WS_BORDER | WS_VSCROLL | WS_TABSTOP
    GROUPBOX        "Muster",1073,98,72,160,49,WS_GROUP
    CTEXT           "AaBbYyZz",1093,104,81,149,37,SS_NOPREFIX | WS_VISIBLE
}


CHOOSE_COLOR DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 300, 185
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Farbe"
FONT 8, "Helv"
{
 LTEXT "Gr&undfarben:",   1088, 4,    4,  140, 10
 LTEXT "Selbs&tdefinierte Farben:",  1089, 4,   106, 140, 10
 LTEXT "Farbe |  B&asis",  1090, 150, 151,  48, 10
 LTEXT   "&Rot:", 726 ,249,126,24,10
 EDITTEXT 706, 275,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "&Gr�n:",727/*1095*/,249,140,24,10
 EDITTEXT 707, 275,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "&Blau:",728 /*1096*/,249,154,24,10
 EDITTEXT 708, 275,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Farbe:" ,723 /*1091*/,202,126,22,10
 EDITTEXT 703, 226,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&S�tt:" ,724 /*1092*/,202,140,22,10
 EDITTEXT 704, 226,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "H&ell:" ,725 /*1093*/,202,154,22,10
 EDITTEXT 705, 226,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 CONTROL "" ,720,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,14,140,86
 CONTROL "" ,721,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,116,140,28
 CONTROL "" ,710,"STATIC",WS_BORDER|SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,4,118,116
 CONTROL "" ,702,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 278,4,8,116
 CONTROL "" ,709,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,124,40,26
 DEFPUSHBUTTON "Ok",  IDOK,  4, 166, 44, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 52, 166, 44, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Hilfe", pshHelp,100,166, 44, 14
 PUSHBUTTON "Farbe hin&zuf�gen",    712/*1024*/, 152, 166, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Farben &definieren >>", 719/*1025*/,   4, 150, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON  "&a",713,300,200,4,14   /* just a dummy:  'a' is  like  &a  in "B&asis"  */
}


FINDDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 236, 62
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Suchen"
FONT 8, "Helv"
{
 LTEXT "S&uchen nach:", -1, 4, 8, 42, 8
 EDITTEXT 1152, 47, 7, 128, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "Nur ganze &Worte", 1040, 4, 26, 100, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Beachte Gro�/Kleinschreibung", 1041, 4, 42, 64, 12, BS_AUTOCHECKBOX | WS_TABSTOP
 GROUPBOX "Richtung", 1072, 107, 26, 68, 28
 CONTROL "H&och", rad1, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 111, 38, 20, 12
 CONTROL "&Runter", rad2, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 138, 38, 30, 12
 DEFPUSHBUTTON "&N�chsten finden", IDOK, 182, 5, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 182, 23, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Hilfe", pshHelp, 182, 45, 50, 14, WS_GROUP | WS_TABSTOP
}


REPLACEDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 230, 94
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Ersetzen"
FONT 8, "Helv"
{
 LTEXT "S&uchen nach:", -1, 4, 9, 48, 8
 EDITTEXT 1152, 54, 7, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT "Ersetzen &durch:", -1, 4, 26, 48, 8
 EDITTEXT 1153, 54, 24, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "Nur ganze &Worte", 1040, 5, 46, 104, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Beachte Gro�/Kleinschreibung", 1041, 5, 62, 59, 12, BS_AUTOCHECKBOX | WS_TABSTOP
 DEFPUSHBUTTON "&N�chsten finden", IDOK, 174, 4, 50, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Ersetzen", psh1, 174, 21, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Alle ersetzen", psh2, 174, 38, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Abbrechen", IDCANCEL, 174, 55, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Hilfe", pshHelp, 174, 75, 50, 14, WS_GROUP | WS_TABSTOP
}

NEWFILEOPENORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 0, 0, 307, 165
STYLE DS_MODALFRAME | DS_CONTEXTHELP | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�ffnen"
FONT 8, "helv"
{
    LTEXT	"&Suche in",IDC_LOOKINSTATIC,4,6,43,8, SS_NOTIFY
    COMBOBOX	IDC_LOOKIN,49,3,132,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP

    LTEXT	"" , IDC_TOOLBARSTATIC, 181, 2, 122, 17, NOT WS_GROUP | NOT WS_VISIBLE
    LISTBOX	IDC_SHELLSTATIC,4,20,292,85, LBS_SORT | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | WS_HSCROLL | NOT WS_VISIBLE

    LTEXT	"File&name:",IDC_FILENAMESTATIC,5,112,46,8, SS_NOTIFY
    EDITTEXT	IDC_FILENAME,54,110,155,12,ES_AUTOHSCROLL

    LTEXT	"File&typen",IDC_FILETYPESTATIC,5,128,42,8, SS_NOTIFY
    COMBOBOX	IDC_FILETYPE,54,126,155,53,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP

    CONTROL	"&Schreibgesch�tzt",IDC_OPENREADONLY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,54,145,100,10


    DEFPUSHBUTTON	"�&ffnen",		 IDOK,242,110,50,14
    PUSHBUTTON		"Abbrechen",		 IDCANCEL,242,128,50,14
    PUSHBUTTON		"&Hilfe",		 pshHelp,242,145,50,14
}

PRINT32 DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 186
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Drucken"
FONT 8, "Helv"
{
    DEFPUSHBUTTON   "OK",             IDOK,     180,164, 48,14, WS_GROUP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "Abbrechen",      IDCANCEL, 232,164, 48,14, WS_GROUP
    PUSHBUTTON      "&Hilfe",         pshHelp,  50, 161, 48,14, WS_GROUP

    GROUPBOX        "Drucker",        grp4,   8,  4, 272,84, WS_GROUP
    CONTROL         "In Fi&le drucken", chx1, "Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,212,70,64,12
    PUSHBUTTON      "&Eigenschaften", psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Name:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb4,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Status:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "Dummy State",    stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Typ:",           stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Type",     stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Ort:",           stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Location", stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Kommentar:",     stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Remark",   stc13, 52, 72, 152,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Kopien",         grp2, 160, 92, 120,64, WS_GROUP
    LTEXT           "Anzahl &Kopien:",stc5,168,108,68,8
    ICON            "",               ico3, 162,124,  76,24, WS_GROUP | SS_CENTERIMAGE
    CONTROL         "C&ollate",       chx2,"Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,240,130,36,12
    EDITTEXT                          edt3, 240,106,  32,12, WS_GROUP | ES_NUMBER

    GROUPBOX        "Druck Bereich",  grp1,   8,92,  144,64, WS_GROUP
    CONTROL         "&Alles",         rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,16,106,64,12
    CONTROL         "&Seiten",        rad3,"Button",BS_AUTORADIOBUTTON,16,122,36,12
    CONTROL         "Aus&wahl",       rad2,"Button",BS_AUTORADIOBUTTON,16,138,64,12
    EDITTEXT                          edt1,  74,122,  26,12, WS_GROUP | ES_NUMBER
    EDITTEXT                          edt2, 118,122,  26,12, WS_GROUP | ES_NUMBER
    RTEXT           "&von:",         stc2,  52,124,  20,8
    RTEXT           "&bis:",           stc3, 100,124,  16,8
}

PRINT32_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 178
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Print Setup"
FONT 8, "Helv"
BEGIN
    DEFPUSHBUTTON   "OK",IDOK,180,156,48,14,WS_GROUP
    PUSHBUTTON      "Abbrechen",IDCANCEL,232,156,48,14
/*    PUSHBUTTON      "Netzwerk...", psh5, 284,156,48,14 */

    GROUPBOX        "Drucker",        grp4,   8,  4, 272,84, WS_GROUP
    PUSHBUTTON      "&Eigenschaften", psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Name:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb1,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Status:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "[keiner]",       stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Typ:",           stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "[keiner]",       stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Ort:",           stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "[keiner]",       stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Kommentar:",     stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "[keiner]",       stc13, 52, 72, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Papier",         grp2,   8, 92, 164,56, WS_GROUP
    LTEXT           "&Gr��e:",        stc2,  16,108,  36, 8
    COMBOBOX                          cmb2,  52,106, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "&Zufuhr:",       stc3,  16,128,  36, 8
    COMBOBOX                          cmb3,  52,126, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP

    GROUPBOX        "Orientierung",   grp1, 180, 92, 100,56, WS_GROUP
    ICON            "",               ico1, 195,112,  18,20, WS_GROUP
    CONTROL         "&Hochformat",    rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP |WS_TABSTOP,224,106,52,12
    CONTROL         "&Querformat",    rad2,"Button",BS_AUTORADIOBUTTON,224,126,52,12
END

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "�&ber den Folderpicker"
    IDS_DOCUMENTFOLDERS     "Dokumenten Ordner"
    IDS_PERSONAL            "Meine Dokumente"
    IDS_FAVORITES           "Meine Favoriten"
    IDS_PATH                "System Directory"
    IDS_DESKTOP             "Desktop"
    IDS_FONTS               "Schriftarten"
    IDS_MYCOMPUTER          "Mein Computer"
}

STRINGTABLE DISCARDABLE
{
    IDS_SYSTEMFOLDERS       "System Ordner"
    IDS_LOCALHARDRIVES      "Lokale Hard Disks"
    IDS_FILENOTFOUND        "Datei nicht gefunden"
    IDS_VERIFYFILE          "Bitte �berpr�fen sie, ob der korrekte Filename angegeben wurde"
    IDS_CREATEFILE          "Die Datei existiert nicht.\nWollen Sie sie neu anlegen?"
    IDL_OVERWRITEFILE       "Die Datei existiert bereits.\nWollen Sie sie �berschreiben?"
    IDS_INVALID_FILENAME_TITLE "Unzul�ssige Zeichen im Pfad"
    IDS_INVALID_FILENAME    "Ein Filename darf folgende Zeichen nicht enthalten:\n                          / : < > |"
    IDS_PATHNOTEXISTING     "Der Pfad existiert nicht"
    IDS_FILENOTEXISTING     "Die Datei existiert nicht"
}

STRINGTABLE DISCARDABLE
{
    IDS_UPFOLDER         "Eine Verzeichnisebene h�her"
    IDS_NEWFOLDER        "Neues Directory anlegen"
    IDS_LISTVIEW         "List"
    IDS_REPORTVIEW       "Details"
    IDS_TODESKTOP        "Desktop anzeigen"
}

STRINGTABLE DISCARDABLE
{
    PD32_PRINT_TITLE       "Drucken"

    PD32_VALUE_UREADABLE                  "Unlesbarer Eintrag"
    PD32_INVALID_PAGE_RANGE "Dieser Wert liegt nicht auf der Seite.\n\
Bitte geben sie einen Wert zwischen %d und %d an."
    PD32_FROM_NOT_ABOVE_TO                "Der 'von' Eintrag kann nicht gr��er sein als der 'bis' Eintrag."
    PD32_MARGINS_OVERLAP                  "Die R�nder �berlappen oder sind au�erhalb des Papiers.\nBitte die R�nder neu eingeben."
    PD32_NR_OF_COPIES_EMPTY               "Es muss ein Wert bei der Anzahl der Kopien angegeben werden."
    PD32_TOO_LARGE_COPIES                 "Ihr Drucker unterst�tzt so viele Kopien nicht.\nBitte verwenden Sie eine Kopienzahl zwischen 1 und %d."
    PD32_PRINT_ERROR                      "Ein Druckerfehler ist aufgetreten."
    PD32_NO_DEFAULT_PRINTER               "Es ist kein Standarddrucker eingestellt."
    PD32_CANT_FIND_PRINTER                "Kann den Drucker nicht finden."
    PD32_OUT_OF_MEMORY                    "Kein freier Speicher mehr."
    PD32_GENERIC_ERROR                    "Ein unbekannter Fehler ist aufgetreten."
    PD32_DRIVER_UNKNOWN                   "Unbekannter Druckertreiber."

    PD32_DEFAULT_PRINTER                  "Standard Drucker; "
    PD32_NR_OF_DOCUMENTS_IN_QUEUE         "Es sind %d Dokumente in der Queue"
    PD32_PRINT_ALL_X_PAGES                "&Alle %d Seiten"
    PD32_MARGINS_IN_INCHES                "R�nder [inches/zoll]"
    PD32_MARGINS_IN_MILIMETERS            "R�nder [mm]"
    PD32_MILIMETERS                       "mm"

    PD32_PRINTER_STATUS_READY             "Bereit; "
    PD32_PRINTER_STATUS_PAUSED            "Gestoppt; "
    PD32_PRINTER_STATUS_ERROR             "Fehler; "
    PD32_PRINTER_STATUS_PENDING_DELETION  "Wartet auf L�schung; "
    PD32_PRINTER_STATUS_PAPER_JAM         "Papierstau; "
    PD32_PRINTER_STATUS_PAPER_OUT         "Papierfach leer; "
    PD32_PRINTER_STATUS_MANUAL_FEED       "Papier manuell zuf�hren; "
    PD32_PRINTER_STATUS_PAPER_PROBLEM     "Papierproblem; "
    PD32_PRINTER_STATUS_OFFLINE           "Printer ist offline; "
    PD32_PRINTER_STATUS_IO_ACTIVE         "Daten�bertragung; "
    PD32_PRINTER_STATUS_BUSY              "Besch�ftigt; "
    PD32_PRINTER_STATUS_PRINTING          "Druckend; "
    PD32_PRINTER_STATUS_OUTPUT_BIN_FULL   "Ausgabe Fach ist voll; "
    PD32_PRINTER_STATUS_NOT_AVAILABLE     "Nicht vorhanden; "
    PD32_PRINTER_STATUS_WAITING           "Wartend; "
    PD32_PRINTER_STATUS_PROCESSING        "In Bearbeitung; "
    PD32_PRINTER_STATUS_INITIALIZING      "Initialisierend; "
    PD32_PRINTER_STATUS_WARMING_UP        "Aufw�rmend; "
    PD32_PRINTER_STATUS_TONER_LOW         "Tonerstand niedrig; "
    PD32_PRINTER_STATUS_NO_TONER          "Kein Toner mehr; "
    PD32_PRINTER_STATUS_PAGE_PUNT         "Page punt; "
    PD32_PRINTER_STATUS_USER_INTERVENTION "Durch Benutzer unterbrochen; "
    PD32_PRINTER_STATUS_OUT_OF_MEMORY     "Kein Speicher mehr; "
    PD32_PRINTER_STATUS_DOOR_OPEN         "Druckergeh�use ist offen; "
    PD32_PRINTER_STATUS_SERVER_UNKNOWN    "Druckserver unbekannt; "
    PD32_PRINTER_STATUS_POWER_SAVE        "Stromsparmodus; "
}
