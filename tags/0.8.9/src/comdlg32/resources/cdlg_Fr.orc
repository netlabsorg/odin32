/*
 * Copyright 1995 Alexandre Julliard
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

LANGUAGE LANG_FRENCH, SUBLANG_DEFAULT

OPEN_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Ouvrir"
FONT 8, "Helv"
{
 LTEXT "&Nom de fichier:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&R�pertoires", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "Liste des fichiers de &type:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&Lecteurs:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "OK", 1, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Aide", 1038, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "Lecture seule", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


SAVE_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Enregistrer sous ..."
FONT 8, "Helv"
{
 LTEXT "&Nom de fichier:", 1090, 6, 6, 76, 9
 EDITTEXT 1152, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&R�pertoires:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "Liste des fichiers de &type:", 1089, 6, 104, 90, 9
 COMBOBOX 1136, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "&Lecteurs:", 1091, 110, 104, 92, 9
 COMBOBOX 1137, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "OK", 1, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Aide", 1038, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "Lecture seule", 1040, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Imprimer"
FONT 8, "Helv"
{
 LTEXT "Imprimante:", 1088, 6, 6, 40, 9
 LTEXT "", 1089, 60, 6, 150, 9
 GROUPBOX "Etendue", 1072, 6, 30, 160, 65, BS_GROUPBOX
 RADIOBUTTON "&Tout", 1056, 16, 45, 60, 12
 RADIOBUTTON "&S�lection", 1057, 16, 60, 60, 12
 RADIOBUTTON "&Pages", 1058, 16, 75, 60, 12
 DEFPUSHBUTTON "OK", 1, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Configurer...", 1024, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 LTEXT "&De:", 1090, 60, 80, 30, 9
 LTEXT "&A:", 1091, 120, 80, 30, 9
 LTEXT "&Qualit� d'impression:", 1092, 6, 100, 76, 9
 COMBOBOX 1136, 80, 100, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 CHECKBOX "&Fichier d'impression", 1040, 20, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Copies t&ri�es", 1041, 160, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Configuration de l'impression"
FONT 8, "Helv"
{
 GROUPBOX "Imprimante", 1072, 6, 10, 180, 65, BS_GROUPBOX
 RADIOBUTTON "&Imprimante par d�faut", 1056, 16, 20, 80, 12
 LTEXT "[aucune]", 1088, 35, 35, 120, 9
 RADIOBUTTON "Imprimante &sp�cifique", 1057, 16, 50, 80, 12
 COMBOBOX 1136, 35, 65, 149, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "OK", 1, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Options...", 1024, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 GROUPBOX "Orientation", 1073, 6, 85, 100, 50, BS_GROUPBOX
 RADIOBUTTON "&Portrait", 1058, 50, 100, 40, 12
 RADIOBUTTON "Paysag&e", 1059, 50, 115, 40, 12
 ICON "LANDSCAP", 1097, 10, 95, 32, 32
 ICON "PORTRAIT", 1098, 10, 95, 32, 32
 GROUPBOX "Papier", 1074, 120, 85, 180, 50, BS_GROUPBOX
 LTEXT "&Taille :", 1089, 130, 95, 30, 9
 LTEXT "&Ali&mentation :", 1090, 130, 110, 30, 9
 COMBOBOX 1137, 155, 95, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 COMBOBOX 1138, 155, 110, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
}


CHOOSE_FONT DIALOG DISCARDABLE  13, 54, 264, 147
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Polices"
FONT 8, "Helv"
{
    LTEXT           "&Police:",1088 ,6,3,40,9
    COMBOBOX        1136 ,6,13,94,54,  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    CBS_AUTOHSCROLL | CBS_SORT | WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "St&yle de police:",1089 ,108,3,44,9
    COMBOBOX        1137,108,13,64,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "&Taille:",1090,179,3,30,9
    COMBOBOX        1138,179,13,32,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE | CBS_SORT
    DEFPUSHBUTTON   "OK",IDOK,218,6,40,14,WS_GROUP
    PUSHBUTTON      "Annuler",IDCANCEL,218,23,40,14,WS_GROUP
    PUSHBUTTON      "A&ppliquer", 1026,218,40,40,14,WS_GROUP
    PUSHBUTTON      "&Aide" , 1038,218,57,40,14,WS_GROUP
    GROUPBOX        "Effets",1072,6,72,84,34,WS_GROUP
    CHECKBOX        "&Barr�", 1040, 10,82,50,10, BS_AUTOCHECKBOX | WS_TABSTOP
    CHECKBOX        "&Soulign�", 1041, 10,94,50,10, BS_AUTOCHECKBOX
    LTEXT           "&Couleur:", 1091 ,6,110,30,9
    COMBOBOX        1139,6,120,84,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
                    CBS_AUTOHSCROLL |  WS_BORDER | WS_VSCROLL | WS_TABSTOP
    GROUPBOX        "Exemple",1073,98,72,160,49,WS_GROUP
    CTEXT           "AaBbYyZz",1093,104,81,149,37,SS_NOPREFIX | WS_VISIBLE
}


CHOOSE_COLOR DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 300, 185
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Couleurs"
FONT 8, "Helv"
{
 LTEXT "Couleurs de &base:",   1088, 4,    4,  140, 10
 LTEXT "&Couleurs personnalis�es:",  1089, 4,   106, 140, 10
 LTEXT "Couleur |  &Uni",  1090, 150, 151,  48, 10
 LTEXT   "&Rouge:", 726 /*1094*/,249,126,24,10
 EDITTEXT 706, 275,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "&Vert:",727/*1095*/,249,140,24,10
 EDITTEXT 707, 275,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "Bl&eu:",728 /*1096*/,249,154,24,10
 EDITTEXT 708, 275,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Teinte:" ,723 /*1091*/,202,126,22,10
 EDITTEXT 703, 226,124,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Sat.:" ,724 /*1092*/,202,140,22,10
 EDITTEXT 704, 226,138,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Lum.:" ,725 /*1093*/,202,154,22,10
 EDITTEXT 705, 226,152,18,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 CONTROL "" ,720,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,14,140,86
 CONTROL "" ,721,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,116,140,28
 CONTROL "" ,710,"STATIC",WS_BORDER|SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,4,118,116
 CONTROL "" ,702,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 278,4,8,116
 CONTROL "" ,709,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,124,40,26
 DEFPUSHBUTTON "Ok",  1,  4, 166, 44, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 52, 166, 44, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Aide", 1038,100,166, 44, 14
 PUSHBUTTON "A&jouter couleur personnalis�es", 712/*1024*/, 152, 166, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&D�finir couleurs personnalis�es >>", 719/*1025*/,   4, 150, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON  "&i",713,300,200,4,14   /* just a dummy:  'i' is  like  &i in "sol&id" */
}


FINDDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 236, 62
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Rechercher"
FONT 8, "Helv"
{
 LTEXT "&Rechercher:", -1, 4, 8, 42, 8
 EDITTEXT 1152, 47, 7, 128, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "M&ot seulement", 1040, 4, 26, 100, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Respect des &majuscules/minuscules", 1041, 4, 42, 64, 12, BS_AUTOCHECKBOX | WS_TABSTOP
 GROUPBOX "Direction", 1072, 107, 26, 68, 28
 CONTROL "&Haut", 1056, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 111, 38, 20, 12
 CONTROL "&Bas", 1057, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 138, 38, 30, 12

 DEFPUSHBUTTON "&Poursuivre", 1, 182, 5, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Annuler", 2, 182, 23, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Aide", 1038, 182, 45, 50, 14, WS_GROUP | WS_TABSTOP
}


REPLACEDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 230, 94
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Remplacer"
FONT 8, "Helv"
{
 LTEXT "&Rechercher:", -1, 4, 9, 48, 8
 EDITTEXT 1152, 54, 7, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT "R&emplacer par:", -1, 4, 26, 48, 8
 EDITTEXT 1153, 54, 24, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "M&ot seulement", 1040, 5, 46, 104, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Respect des &majuscules/minuscules", 1041, 5, 62, 59, 12, BS_AUTOCHECKBOX | WS_TABSTOP

 DEFPUSHBUTTON "&Poursuivre", 1, 174, 4, 50, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Rempla&cer", 1024, 174, 21, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Remplacer &tout", 1025, 174, 38, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Fermer", 2, 174, 55, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Aide", 1038, 174, 75, 50, 14, WS_GROUP | WS_TABSTOP
}


PRINT32 DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 186
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Impression"
FONT 8, "Helv"
{
    DEFPUSHBUTTON   "OK",             IDOK,     180,164, 48,14, WS_GROUP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "Annuler",        IDCANCEL, 232,164, 48,14, WS_GROUP
    PUSHBUTTON      "&Aide",          pshHelp,  50, 161, 48,14, WS_GROUP

    GROUPBOX        "Imprimante",     grp4,   8,  4, 272,84, WS_GROUP
    CONTROL         "Imprimer dans un fichier", chx1, "Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,212,70,64,12
    PUSHBUTTON      "Propri�t�s",     psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Nom:",          stc6,  16, 20,  36,8
    COMBOBOX                          cmb4,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Etat:",          stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "Dummy State",    stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Type:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Type",     stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Emplacement:",   stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Location", stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Commentaire:",   stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Remark",   stc13, 52, 72, 152,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Copies",         grp2, 160, 92, 120,64, WS_GROUP
    LTEXT           "Nombre de copies:",stc5,168,108,68,8
    ICON            "",               ico3, 162,124,  76,24, WS_GROUP | SS_CENTERIMAGE
    CONTROL         "Copies assembl�es",chx2,"Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,240,130,36,12
    EDITTEXT                          edt3, 240,106,  32,12, WS_GROUP | ES_NUMBER

    GROUPBOX        "Etendue de pages",  grp1,   8,92,  144,64, WS_GROUP
    CONTROL         "Toutes",         rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,16,106,64,12
    CONTROL         "Pages",          rad3,"Button",BS_AUTORADIOBUTTON,16,122,36,12
    CONTROL         "Selection",      rad2,"Button",BS_AUTORADIOBUTTON,16,138,64,12
    EDITTEXT                          edt1,  74,122,  26,12, WS_GROUP | ES_NUMBER
    EDITTEXT                          edt2, 118,122,  26,12, WS_GROUP | ES_NUMBER
    RTEXT           "&de:",           stc2,  52,124,  20,8
    RTEXT           "&�:",            stc3, 100,124,  16,8
}

PRINT32_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 178
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Configuration de l'imprimante"
FONT 8, "Helv"
BEGIN
    DEFPUSHBUTTON   "OK",IDOK,180,156,48,14,WS_GROUP
    PUSHBUTTON      "Annuler",IDCANCEL,232,156,48,14
/*    PUSHBUTTON      "R�seau...", psh5, 284,156,48,14 */

    GROUPBOX        "Imprimante",     grp4,   8,  4, 272,84, WS_GROUP
    PUSHBUTTON      "&Propri�t�s", psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Nom:",          stc6,  16, 20,  36,8
    COMBOBOX                          cmb1,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Etat:",          stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "[aucun]",        stc12, 60, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Type:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "[aucun]",       stc11, 60, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Emplacement:",   stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "[aucun]",        stc14, 60, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Commentaire:",   stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "[aucun]",        stc13, 60, 72, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Papier",         grp2,   8, 92, 164,56, WS_GROUP
    LTEXT           "&Taille:",       stc2,  16,108,  36, 8
    COMBOBOX                          cmb2,  52,106, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Source:",        stc3,  16,128,  36, 8
    COMBOBOX                          cmb3,  52,126, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP

    GROUPBOX        "Orientation",   grp1, 180, 92, 100,56, WS_GROUP
    ICON            "",               ico1, 195,112,  18,20, WS_GROUP
    CONTROL         "&Portrait",      rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP |WS_TABSTOP,224,106,52,12
    CONTROL         "&Paysage",       rad2,"Button",BS_AUTORADIOBUTTON,224,126,52,12
END

PAGESETUPDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 32, 32, 240, 240
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Mise en page"
FONT 8, "Helv"
BEGIN
  CONTROL "", rct1, 130, 0x00020006, 80, 8, 80, 80,  SS_WHITERECT
  CONTROL "", rct2, 130, 0x00020005, 160, 12, 4, 80, SS_GRAYRECT
  CONTROL "", rct3, 130, 0x00020005, 84, 88, 80, 4,  SS_GRAYRECT
  GROUPBOX "Papier", grp2, 8, 96, 224, 56, BS_GROUPBOX
  LTEXT "&Taille:", stc2, 16, 112, 36, 8
  COMBOBOX cmb2, 64, 110, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  LTEXT "&Source:", stc3, 16, 132, 36, 8
  COMBOBOX cmb3, 64, 130, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  GROUPBOX "Orientation", grp1, 8, 156, 64, 56, BS_GROUPBOX
  AUTORADIOBUTTON "&Portrait", rad1, 16, 170, 52, 12, BS_AUTORADIOBUTTON
  AUTORADIOBUTTON "P&aysage", rad2, 16, 190, 52, 12, BS_AUTORADIOBUTTON
  GROUPBOX "Marges", grp4, 80, 156, 152, 56, BS_GROUPBOX
  LTEXT "&Droite:", stc15, 88, 172, 21, 8
  EDITTEXT edt4, 111, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Gauche:", stc16, 159, 172, 27, 8
  EDITTEXT edt6, 187, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Haut:", stc17, 88, 192, 21, 8
  EDITTEXT edt5, 111, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Bas:", stc18, 159, 192, 23, 8
  EDITTEXT edt7, 187, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  DEFPUSHBUTTON "OK", IDOK, 71, 220, 50, 14, BS_PUSHBUTTON
  PUSHBUTTON "Annuler", IDCANCEL, 126, 220, 50, 14
  PUSHBUTTON "&Imprimante...", psh3, 184, 220, 48, 14
END



NEWFILEOPENORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 0, 0, 307, 165
STYLE DS_MODALFRAME | DS_CONTEXTHELP | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Ouvrir"
FONT 8, "helv"
{
    LTEXT	"&Rechercher dans",IDC_LOOKINSTATIC,4,6,43,8, SS_NOTIFY
    COMBOBOX	IDC_LOOKIN,49,3,132,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP

    LTEXT	"" , IDC_TOOLBARSTATIC, 181, 2, 122, 17, NOT WS_GROUP | NOT WS_VISIBLE
    LISTBOX	IDC_SHELLSTATIC,4,20,292,85, LBS_SORT | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | WS_HSCROLL | NOT WS_VISIBLE

    LTEXT	"&Nom du fichier:",IDC_FILENAMESTATIC,5,112,46,8, SS_NOTIFY
    EDITTEXT	IDC_FILENAME,54,110,155,12,ES_AUTOHSCROLL

    LTEXT	"&Type du fichier",IDC_FILETYPESTATIC,5,128,42,8, SS_NOTIFY
    COMBOBOX	IDC_FILETYPE,54,126,155,53,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP

    CONTROL	"&Protection �criture",IDC_OPENREADONLY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,54,145,100,10

    DEFPUSHBUTTON	"Ouvrir",		 IDOK,242,110,50,14
    PUSHBUTTON		"Annuler",		 IDCANCEL,242,128,50,14
    PUSHBUTTON		"&Aide",		 pshHelp,242,145,50,14
}

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "A propos du choix de r�pertoires"
    IDS_DOCUMENTFOLDERS     "R�pertoire Documents"
    IDS_PERSONAL            "Mes Documents"
    IDS_FAVORITES           "Mes Favoris"
    IDS_PATH                "R�pertoire syst�me"
    IDS_DESKTOP             "Bureau"
    IDS_FONTS               "Polices"
    IDS_MYCOMPUTER          "Poste de travail"
}

STRINGTABLE DISCARDABLE
{
    IDS_SYSTEMFOLDERS       "R�pertoires syst�me"
    IDS_LOCALHARDRIVES      "Disques locaux"
    IDS_FILENOTFOUND        "Fichier non trouv�"
    IDS_VERIFYFILE          "Pri�re de v�rifier que le nom de fichier indiqu� est correct"
    IDS_CREATEFILE          "Ce fichier n'existe pas.\nSouhaitez-vous le cr�er ?"
    IDL_OVERWRITEFILE       "Ce fichier existe d�j�.\nVoulez-vous le remplacer ?"
    IDS_INVALID_FILENAME_TITLE "Le chemin d'acc�s contient des caract�res invalides"
    IDS_INVALID_FILENAME    "Un nom de fichier ne peut contenir un des caract�res suivants : \n                          / : < > |"
    IDS_PATHNOTEXISTING     "Le chemin d'acc�s n'existe pas"
    IDS_FILENOTEXISTING     "Le fichier n'existe pas"
}

STRINGTABLE DISCARDABLE
{
    IDS_UPFOLDER         "Remonter d'un niveau"
    IDS_NEWFOLDER        "Cr�er un nouveau dossier"
    IDS_LISTVIEW         "Liste"
    IDS_REPORTVIEW       "D�tails"
    IDS_TODESKTOP        "Voir le bureau"
}

STRINGTABLE DISCARDABLE
{
    PD32_PRINT_TITLE       "Imprimer"

    PD32_VALUE_UREADABLE                  "Entr�e illisible"
    PD32_INVALID_PAGE_RANGE "Cette valeur n'est pas dans l'intervalle de pages\n\
Veuillez indiquer une valeur entre %d et %d."
    PD32_FROM_NOT_ABOVE_TO                "Le champ 'de' ne peut �tre plus grand \
que le champ '�'."
    PD32_MARGINS_OVERLAP                  "Les marges se recouvrent ou d�passent du papier\n\
Veuillez indiquer de nouvelles valeurs."
    PD32_NR_OF_COPIES_EMPTY               "Le champ 'Copies' est vide.\n\
Veuillez entrer un nombre et r&essayer."
    PD32_TOO_LARGE_COPIES                 "Votre imprimante ne peut g�rer autant de copies.\n\
Veuillez indiquer un nombre de copies entre 1 et %d."
    PD32_PRINT_ERROR                      "Une erreur d'impression est intervenue."
    PD32_NO_DEFAULT_PRINTER               "Pas d'imprimante par d�faut."
    PD32_CANT_FIND_PRINTER                "Impossible de trouver l'imprimante."
    PD32_OUT_OF_MEMORY                    "Pas assez de m�moire."
    PD32_GENERIC_ERROR                    "Une erreur est intervenue."
    PD32_DRIVER_UNKNOWN                   "Pilote d'imprimante inconnu."

    PD32_DEFAULT_PRINTER                  "Imprimante par d�faut; "
    PD32_NR_OF_DOCUMENTS_IN_QUEUE         "Il y a %d documents dans la file d'attente"
    PD32_PRINT_ALL_X_PAGES                "&Toutes les %d pages"
    PD32_MARGINS_IN_INCHES                "Marges [pouces]"
    PD32_MARGINS_IN_MILIMETERS            "Marges [mm]"
    PD32_MILIMETERS                       "mm"

    PD32_PRINTER_STATUS_READY             "Pr�t; "
    PD32_PRINTER_STATUS_PAUSED            "Suspendu; "
    PD32_PRINTER_STATUS_ERROR             "Erreur; "
    PD32_PRINTER_STATUS_PENDING_DELETION  "En attente de suppression; "
    PD32_PRINTER_STATUS_PAPER_JAM         "Bourrage papier; "
    PD32_PRINTER_STATUS_PAPER_OUT         "Plus de papier; "
    PD32_PRINTER_STATUS_MANUAL_FEED       "Insertion manuelle du papier; "
    PD32_PRINTER_STATUS_PAPER_PROBLEM     "Probl�me de papier; "
    PD32_PRINTER_STATUS_OFFLINE           "L'imprimante n'est pas en ligne; "
    PD32_PRINTER_STATUS_IO_ACTIVE         "IO Active; "
    PD32_PRINTER_STATUS_BUSY              "Occup�e; "
    PD32_PRINTER_STATUS_PRINTING          "En cours d'impression; "
    PD32_PRINTER_STATUS_OUTPUT_BIN_FULL   "Le bac de sortie est plein; "
    PD32_PRINTER_STATUS_NOT_AVAILABLE     "Non disponible; "
    PD32_PRINTER_STATUS_WAITING           "En attente; "
    PD32_PRINTER_STATUS_PROCESSING        "Traitement en cours; "
    PD32_PRINTER_STATUS_INITIALIZING      "D�marrage en cours; "
    PD32_PRINTER_STATUS_WARMING_UP        "Pr�chauffage; "
    PD32_PRINTER_STATUS_TONER_LOW         "Toner presque vide; "
    PD32_PRINTER_STATUS_NO_TONER          "Pas de toner; "
    PD32_PRINTER_STATUS_PAGE_PUNT         "Page punt; "
    PD32_PRINTER_STATUS_USER_INTERVENTION "Annul� par utilisateur; "
    PD32_PRINTER_STATUS_OUT_OF_MEMORY     "Plus assez de m�moire; "
    PD32_PRINTER_STATUS_DOOR_OPEN         "La porte de l'imprimante est ouverte; "
    PD32_PRINTER_STATUS_SERVER_UNKNOWN    "Serveur d'impression inconnu; "
    PD32_PRINTER_STATUS_POWER_SAVE        "Mode �conomie d'�nergie; "
}
