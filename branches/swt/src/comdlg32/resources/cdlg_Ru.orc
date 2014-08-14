/*
 * Copyright 1999 Alexander Kanavin
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

LANGUAGE LANG_RUSSIAN, SUBLANG_DEFAULT

OPEN_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�������"
FONT 8, "Ms Sans Serif"
{
 LTEXT "&��� �����:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&��������:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "���������� ����� &����:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&�����:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "�������", IDOK, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&�������", 1038, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "�&����� ��� ������", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


SAVE_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "��������� ��� ..."
FONT 8, "Ms Sans Serif"
{
 LTEXT "&��� �����:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&��������:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "���������� ����� &����:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&�����:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "��������� ���", IDOK, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&�������", 1038, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "&������ ��� ������", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "������"
FONT 8, "Ms Sans Serif"
{
 LTEXT "�������:", 1088, 6, 6, 40, 9
 LTEXT "", 1089, 60, 6, 150, 9
 GROUPBOX "��������", 1072, 6, 30, 160, 65, BS_GROUPBOX
 RADIOBUTTON "&���", 1056, 16, 45, 60, 12
 RADIOBUTTON "�&��������� ��������", 1057, 16, 60, 60, 12
 RADIOBUTTON "&��������", 1058, 16, 75, 60, 12
 DEFPUSHBUTTON "������", IDOK, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "���&�����", 1024, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 LTEXT "&�:", 1090, 60, 80, 30, 9
 LTEXT "&��:", 1091, 120, 80, 30, 9
 LTEXT "&�������� ������:", 1092, 6, 100, 76, 9
 COMBOBOX 1136, 80, 100, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 CHECKBOX "�&����� � ����", 1040, 20, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Condensed", 1041, 160, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "��������� ��������"
FONT 8, "Ms Sans Serif"
{
 GROUPBOX "�������", 1072, 6, 10, 180, 65, BS_GROUPBOX
 RADIOBUTTON "������� �� &���������", 1056, 16, 20, 80, 12
 LTEXT "[none]", 1088, 35, 35, 120, 9
 RADIOBUTTON "&������ �������", 1057, 16, 50, 80, 12
 COMBOBOX 1136, 35, 65, 149, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "OK", IDOK, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&���������", 1024, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 GROUPBOX "����������", 1073, 6, 85, 100, 50, BS_GROUPBOX
 RADIOBUTTON "&�������", 1058, 50, 100, 40, 12
 RADIOBUTTON "&���������", 1059, 50, 115, 40, 12
 ICON "LANDSCAP", 1097, 10, 95, 32, 32
 ICON "PORTRAIT", 1098, 10, 95, 32, 32
 GROUPBOX "������", 1074, 120, 85, 180, 50, BS_GROUPBOX
 LTEXT "��&����", 1089, 130, 95, 30, 9
 LTEXT "&��������", 1090, 130, 110, 30, 9
 COMBOBOX 1137, 155, 95, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 COMBOBOX 1138, 155, 110, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
}


CHOOSE_FONT DIALOG DISCARDABLE  13, 54, 264, 147
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�����"
FONT 8, "Ms Sans Serif"
{
    LTEXT           "&�����:",1088 ,6,3,40,9
    COMBOBOX        1136 ,6,13,94,54,  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    CBS_AUTOHSCROLL | CBS_SORT | WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "&����������:",1089 ,108,3,44,9
    COMBOBOX        1137,108,13,64,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "&������:",1090,179,3,30,9
    COMBOBOX        1138,179,13,32,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE | CBS_SORT
    DEFPUSHBUTTON   "OK",IDOK,218,6,40,14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "������",IDCANCEL,218,23,40,14,WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&���������", 1026,218,40,40,14,WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&�������" , 1038,218,57,40,14,WS_GROUP | WS_TABSTOP
    GROUPBOX        "��������",1072,6,72,84,34,WS_GROUP
    CHECKBOX	    "&�����������", 1040, 10,82,50,10, BS_AUTOCHECKBOX | WS_TABSTOP
    CHECKBOX 	    "���&���������", 1041, 10,94,50,10, BS_AUTOCHECKBOX
    LTEXT           "�&���:", 1091 ,6,110,30,9
    COMBOBOX        1139,6,120,84,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
		    CBS_AUTOHSCROLL |  WS_BORDER | WS_VSCROLL | WS_TABSTOP
    GROUPBOX        "�������",1073,98,72,160,49,WS_GROUP
    CTEXT           "AaBbYyZz",1093,104,81,149,37,SS_NOPREFIX | WS_VISIBLE
}


CHOOSE_COLOR DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 300, 185
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "��������� �������"
FONT 8, "Ms Sans Serif"
{
 LTEXT "&������� �������:",   1088, 4,    4,  140, 10
 LTEXT "��&������������ �����:",  1089, 4,   106, 140, 10
 LTEXT "���� |  ��&�����",  1090, 150, 151,  48, 10
 LTEXT   "����&���:", 726 /*1094*/,249,126,24,10
 EDITTEXT 706, 275,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "&�������:",727/*1095*/,249,140,24,10
 EDITTEXT 707, 275,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "�&����:",728 /*1096*/,249,154,24,10
 EDITTEXT 708, 275,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "�&������:" ,723 /*1091*/,202,126,22,10
 EDITTEXT 703, 226,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&��������:" ,724 /*1092*/,202,140,22,10
 EDITTEXT 704, 226,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&�������:" ,725 /*1093*/,202,154,22,10
 EDITTEXT 705, 226,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 CONTROL "" ,720,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,14,140,86
 CONTROL "" ,721,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,116,140,28
 CONTROL "" ,710,"STATIC",WS_BORDER|SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,4,118,116
 CONTROL "" ,702,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 278,4,8,116
 CONTROL "" ,709,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,124,40,26
 DEFPUSHBUTTON "OK",  IDOK,  4, 166, 44, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL, 52, 166, 44, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "C������", 1038,100,166, 44, 14
 PUSHBUTTON "&�������� � �����",    712/*1024*/, 152, 166, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&���������� ���� >>", 719/*1025*/,   4, 150, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON  "&i",713,300,200,4,14   /* just a dummy:  'i' is  like  &i  in "sol&id"  */
}


FINDDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 236, 62
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�����"
FONT 8, "Ms Sans Serif"
{
 LTEXT "&�������:", -1, 4, 8, 42, 8
 EDITTEXT 1152, 47, 7, 128, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "&������ ����� �������", 1040, 4, 26, 100, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "C &������ ��������", 1041, 4, 42, 64, 12, BS_AUTOCHECKBOX | WS_TABSTOP
 GROUPBOX "�����������", 1072, 107, 26, 68, 28
 CONTROL "&�����", 1056, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 111, 38, 20, 12
 CONTROL "�&���", 1057, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 138, 38, 30, 12

 DEFPUSHBUTTON "&����� �����", IDOK, 182,  5, 50, 14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
 PUSHBUTTON "������", IDCANCEL	 , 182, 23, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&�������", pshHelp	 , 182, 45, 50, 14, WS_GROUP | WS_TABSTOP
}


REPLACEDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 230, 94
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "������"
FONT 8, "Ms Sans Serif"
{
 LTEXT "&�������:", -1, 4, 9, 48, 8
 EDITTEXT 1152, 54, 7, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT "�&������� ��:", -1, 4, 26, 48, 8
 EDITTEXT 1153, 54, 24, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "&������ ����� �������", 1040, 5, 46, 104, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "� &������ ��������", 1041, 5, 62, 59, 12, BS_AUTOCHECKBOX | WS_TABSTOP

 DEFPUSHBUTTON "&����� �����", IDOK, 174,  4, 50, 14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
 PUSHBUTTON "&��������", psh1	 , 174, 21, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "�������� &���", psh2 , 174, 38, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "������", IDCANCEL	 , 174, 55, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&�������", pshHelp	 , 174, 75, 50, 14, WS_GROUP | WS_TABSTOP
}

PRINT32 DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 186
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "������"
FONT 8, "Ms Sans Serif"
{
    DEFPUSHBUTTON   "OK",     IDOK,     180,164, 48,14, WS_GROUP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "������", IDCANCEL, 232,164, 48,14, WS_GROUP
    PUSHBUTTON      "&�������",  pshHelp,  50, 161, 48,14, WS_GROUP

    GROUPBOX        "�������",        grp4,   8,  4, 272,84, WS_GROUP
    CONTROL         "������ � ���&�", chx1, "Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,212,70,64,12
    PUSHBUTTON      "&��������",    psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&���:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb4,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "������:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "����������",    stc12, 60, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "���:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "����������",     stc11, 60, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "�����:",         stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "�����������", stc14, 60, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "����������:",       stc13,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "�����������",   stc13, 60, 72, 152,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "�����",         grp2, 160, 92, 120,64, WS_GROUP
    LTEXT           "����� &�����:",stc5,168,108,68,8
    ICON            "",               ico3, 162,124,  76,24, WS_GROUP | SS_CENTERIMAGE
    CONTROL         "&�������",       chx2,"Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,230,130,40,12
    EDITTEXT                          edt3, 240,106,  32,12, WS_GROUP | ES_NUMBER

    GROUPBOX        "��������",    grp1,   8,92,  144,64, WS_GROUP
    CONTROL         "&���",           rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,16,106,64,12
    CONTROL         "&��������",         rad3,"Button",BS_AUTORADIOBUTTON,16,122,50,12
    CONTROL         "&���������",     rad2,"Button",BS_AUTORADIOBUTTON,16,138,64,12
    EDITTEXT                          edt1,  85,122,  20,12, WS_GROUP | ES_NUMBER
    EDITTEXT                          edt2, 120,122,  20,12, WS_GROUP | ES_NUMBER
    RTEXT           "&��:",         stc2,  70,124,  10,8
    RTEXT           "&��:",           stc3, 106,124,  10,8
}

PRINT32_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 178
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "��������� ��������"
FONT 8, "Ms Sans Serif"
BEGIN
    DEFPUSHBUTTON   "OK",IDOK,180,156,48,14,WS_GROUP
    PUSHBUTTON      "������",IDCANCEL,232,156,48,14
/*    PUSHBUTTON      "����...", psh5, 284,156,48,14 */

    GROUPBOX        "�������",        grp4,   8,  4, 272,84, WS_GROUP
    PUSHBUTTON      "&��������",    psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&���:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb1,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "������:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "����������",    stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "���:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "����������",     stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "�����:",         stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "�����������", stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "�������:",       stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "�����������",   stc13, 52, 72, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "������",          grp2,   8, 92, 164,56, WS_GROUP
    LTEXT           "���&���:",         stc2,  16,108,  36, 8
    COMBOBOX                          cmb2,  52,106, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "&��������:",       stc3,  16,128,  36, 8
    COMBOBOX                          cmb3,  52,126, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP

    GROUPBOX        "����������",    grp1, 180, 92, 100,56, WS_GROUP
    ICON            "",               ico1, 195,112,  18,20, WS_GROUP
    CONTROL         "&�������",      rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP |WS_TABSTOP,224,106,52,12
    CONTROL         "�&�������",     rad2,"Button",BS_AUTORADIOBUTTON,224,126,52,12
END


NEWFILEOPENORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 0, 0, 287, 165
STYLE DS_MODALFRAME | DS_CONTEXTHELP | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "�������"
FONT 8, "Ms Sans Serif"
{
    LTEXT	"������ &�",IDC_LOOKINSTATIC,4,6,43,8, SS_NOTIFY
    COMBOBOX	IDC_LOOKIN,49,3,132,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP

    LTEXT	"" , IDC_TOOLBARSTATIC, 181, 2, 102, 17, NOT WS_GROUP | NOT WS_VISIBLE
    LISTBOX	IDC_SHELLSTATIC,4,20,272,85, LBS_SORT | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | WS_HSCROLL | NOT WS_VISIBLE

    LTEXT	"��� &�����:",IDC_FILENAMESTATIC,5,112,46,8, SS_NOTIFY
    EDITTEXT	IDC_FILENAME,54,110,155,12,ES_AUTOHSCROLL

    LTEXT	"����� &����",IDC_FILETYPESTATIC,5,128,42,8, SS_NOTIFY
    COMBOBOX	IDC_FILETYPE,54,126,155,53,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP

    CONTROL	"������ ��� &������",IDC_OPENREADONLY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,54,145,100,10

    DEFPUSHBUTTON	"&�������",		 IDOK,222,110,50,14
    PUSHBUTTON		"������",		 IDCANCEL,222,128,50,14
    PUSHBUTTON		"&�������",		 pshHelp,222,145,50,14
}

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "&� ����� ����������"
    IDS_DOCUMENTFOLDERS     "����� ����������"
    IDS_PERSONAL            "��� ���������"
    IDS_FAVORITES           "���������"
    IDS_PATH                "��������� ����"
    IDS_DESKTOP             "������� ����"
    IDS_FONTS               "Fonts"
    IDS_MYCOMPUTER          "��� ���������"
}

STRINGTABLE DISCARDABLE
{
    IDS_SYSTEMFOLDERS       "��������� �����"
    IDS_LOCALHARDRIVES      "��������� ������� �����"
    IDS_FILENOTFOUND        "���� �� ������"
    IDS_VERIFYFILE          "���������, ��������� �� ������� ��� �����"
    IDS_CREATEFILE          "����� �� ����������\n������ �� �� ��� �������?"
    IDL_OVERWRITEFILE       "���� ��� ����������.\n�������� ���?"
    IDS_INVALID_FILENAME_TITLE "������������ ������ � ������ ����"
    IDS_INVALID_FILENAME    "��� ����� �� ����� ��������� ��������� �������: \n                          / : < > |"
    IDS_PATHNOTEXISTING     "���� �� ����������"
    IDS_FILENOTEXISTING     "���� �� ����������"
}

STRINGTABLE DISCARDABLE
{
    IDS_UPFOLDER         "����� �� ���� �������"
    IDS_NEWFOLDER        "������� ����� �����"
    IDS_LISTVIEW         "������"
    IDS_REPORTVIEW       "�����������"
    IDS_TODESKTOP        "������� �� ������� ����"
}

STRINGTABLE DISCARDABLE
{
    PD32_PRINT_TITLE       "������"

    PD32_VALUE_UREADABLE                  "���������� �������"
    PD32_INVALID_PAGE_RANGE "��� �������� �� ����� ������ ��������� �������\n\
������� �������� ����� %d � %d"
    PD32_FROM_NOT_ABOVE_TO                "�������� �� �� ������ ��������� ��:"
    PD32_MARGINS_OVERLAP                  "������� ����������� ��� ��������� \
������� ������.\n������� �� ������"
    PD32_NR_OF_COPIES_EMPTY               "�������� ����� ����� �� ����� ���� \
������"
    PD32_TOO_LARGE_COPIES                 "����� ������� ���������� ����� �� \
����� ���� ���������� ����� ���������.\n������� �������� ����� 1 � %d"
    PD32_PRINT_ERROR                      "��������� ������ ��������"
    PD32_NO_DEFAULT_PRINTER               "��� ��������, ������������� �� ���������"
    PD32_CANT_FIND_PRINTER                "�� ������� ����� �������"
    PD32_OUT_OF_MEMORY                    "���� ������"
    PD32_GENERIC_ERROR                    "���������(�) ������(�)"
    PD32_DRIVER_UNKNOWN                   "����������� ������� ��������"

    PD32_DEFAULT_PRINTER                  "������� �� ���������; "
    PD32_NR_OF_DOCUMENTS_IN_QUEUE         "%d ���������� � �������"
    PD32_PRINT_ALL_X_PAGES                "&��� %d �������"
    PD32_MARGINS_IN_INCHES                "������� [�����)"
    PD32_MARGINS_IN_MILIMETERS            "������� [��]"
    PD32_MILIMETERS                       "��"

    PD32_PRINTER_STATUS_READY             "������"
    PD32_PRINTER_STATUS_PAUSED            "��������������; "
    PD32_PRINTER_STATUS_ERROR             "������; "
    PD32_PRINTER_STATUS_PENDING_DELETION  "Pending deletion; "
    PD32_PRINTER_STATUS_PAPER_JAM         "Paper jam; "
    PD32_PRINTER_STATUS_PAPER_OUT         "�� ������� ������; "
    PD32_PRINTER_STATUS_MANUAL_FEED       "�������� ������ �������; "
    PD32_PRINTER_STATUS_PAPER_PROBLEM     "�������� � �������; "
    PD32_PRINTER_STATUS_OFFLINE           "������� � ������ offline; "
    PD32_PRINTER_STATUS_IO_ACTIVE         "I/O Active; "
    PD32_PRINTER_STATUS_BUSY              "�����; "
    PD32_PRINTER_STATUS_PRINTING          "���� ������; "
    PD32_PRINTER_STATUS_OUTPUT_BIN_FULL   "����� ���������� �������; "
    PD32_PRINTER_STATUS_NOT_AVAILABLE     "�� ��������; "
    PD32_PRINTER_STATUS_WAITING           "��������; "
    PD32_PRINTER_STATUS_PROCESSING        "���������; "
    PD32_PRINTER_STATUS_INITIALIZING      "I����������; "
    PD32_PRINTER_STATUS_WARMING_UP        "�������; "
    PD32_PRINTER_STATUS_TONER_LOW         "Toner low; "
    PD32_PRINTER_STATUS_NO_TONER          "��� ������; "
    PD32_PRINTER_STATUS_PAGE_PUNT         "Page punt; "
    PD32_PRINTER_STATUS_USER_INTERVENTION "�������� �������������; "
    PD32_PRINTER_STATUS_OUT_OF_MEMORY     "���� ������; "
    PD32_PRINTER_STATUS_DOOR_OPEN         "������ �������� �������; "
    PD32_PRINTER_STATUS_SERVER_UNKNOWN    "����������� ������ ��������; "
    PD32_PRINTER_STATUS_POWER_SAVE        "������� � ���������� ������; "
}
