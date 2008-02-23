/* $Id: dialog.cpp,v 1.7 1999-07-06 08:50:11 sandervl Exp $ */

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
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_DOSNLS
#define INCL_WIN
#define INCL_WINMLE
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "dialog.h"
#include "misc.h"

#ifndef BS_TEXT /*PLF Sun  97-06-22 03:07:13  not in watcom's os/2 header */
   #define BS_TEXT                 0x0010
#endif

#define DEFAULT_DLGFONT "9.WarpSans"

static int  ConvertClassAndStyle(int winclass, int style, USHORT *os2class, BOOL *fIconBmp);
static int  ConvertDlgStyle(int style);
static int  ConvertDlgItemStyle(int style);
static void ConvertCaption(ULONG style, char *caption);
static int  ConvertFont(char *font, PRESPARAMS *dlgpparam, int fsize);
static void ShowDialogEx(int id, WINDLGTEMPLATEEX *dhdr, int size, int cp);

//******************************************************************************
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Used (slightly modified) in user32\dlgconvert.cpp too, so synch them when changed!!
//ShowDialogEx is identical except for the different dialog(item) structures
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//******************************************************************************
void ShowDialog(int id, DialogBoxHeader *dhdr, int size, int cp)
{
 WINDLGTEMPLATEEX *windlgex = (WINDLGTEMPLATEEX *)dhdr;
 DLGTEMPLATE *dlgt;
 DLGTITEM    *dlgitem;
 PRESPARAMS  *dlgpparam;
 char        *dlgdata, *dlgcurdata;
 WCHAR       *szCaption;
 WORD        *ptrArray;
 char        *ctrltext, *szClass, *font;
 FontHeader  *fhdr;
 ControlData *ctrldata;
 int          i;
 ULONG        ctrlflag, winclass;
 BOOL         fIconBmp;
 ULONG        ulCpSize, ulCP;

  //First save original win32 resource
  OS2Exe.StoreWin32Resource(id, RT_DIALOG, size, (char *)dhdr);

  if(windlgex->wDlgVer == 1 && windlgex->wSignature == 0xFFFF) {
        ShowDialogEx(id, windlgex, size, cp);
        return;
  }

  //should be enough
  dlgt    = (DLGTEMPLATE *)malloc(sizeof(DLGTEMPLATE) + dhdr->NumberOfItems*sizeof(DLGTITEM) + size);
  dlgitem = &dlgt->adlgti[0];
  dlgdata = (char *)&dlgt->adlgti[dhdr->NumberOfItems+1];  //for control data, presparams & strings
  memset((char *)dlgt, 0, sizeof(DLGTEMPLATE) + dhdr->NumberOfItems*sizeof(DLGTITEM) + size);
  dlgcurdata = dlgdata;

  if(cp == 0) {
	dlgt->codepage         = 437;
  }
  else
  {
     	DosQueryCp(sizeof(ulCP), &ulCP, &ulCpSize);
     	dlgt->codepage = ulCP;
  }
  dlgt->offadlgti        = 14;
  dlgt->fsTemplateStatus = 1;
  dlgt->iItemFocus       = 65535;
  dlgt->coffPresParams   = 0;

  dlgitem->flStyle = ConvertDlgStyle(dhdr->lStyle) | WS_SYNCPAINT | WS_CLIPSIBLINGS;

  ctrlflag = 0;
  if((dhdr->lStyle & WINWS_CAPTION) == WINWS_CAPTION) {
        ctrlflag |= FCF_TITLEBAR;
        dlgitem->flStyle |= FS_BORDER;
  }
  else
  if(dhdr->lStyle & WINWS_DLGFRAME)     //can't have title bar
        dlgitem->flStyle |= FS_DLGBORDER;

  if((dhdr->lStyle & (WINWS_SYSMENU | WINWS_CAPTION)) == (WINWS_SYSMENU | WINWS_CAPTION))
        ctrlflag |= FCF_SYSMENU;
  if(dhdr->lStyle & WINWS_VSCROLL)
        ctrlflag |= FCF_VERTSCROLL;
  if(dhdr->lStyle & WINWS_HSCROLL)
        ctrlflag |= FCF_HORZSCROLL;
  if(dhdr->lStyle & WINWS_MINIMIZEBOX)
        ctrlflag |= FCF_MINBUTTON;
  if(dhdr->lStyle & WINWS_MAXIMIZEBOX)
        ctrlflag |= FCF_MAXBUTTON;
  if(dhdr->lStyle & WINWS_THICKFRAME)
        dlgitem->flStyle |= FS_SIZEBORDER;

  if(ctrlflag) {
        *(ULONG *)dlgcurdata = ctrlflag;
        dlgitem->offCtlData  = (USHORT)((int)dlgcurdata - (int)dlgt);
        dlgcurdata          += sizeof(ULONG);
  }
  else  dlgitem->offCtlData  = 0xFFFF;
  dlgitem->x            = dhdr->x;
  //TODO: SvL: Calc has x = 32768 Special value???
  if(dlgitem->x > 1024 || dlgitem->x < 0)
        dlgitem->x = 64;

  dlgitem->y            = dhdr->y;
  dlgitem->cx           = dhdr->cx;
  dlgitem->cy           = dhdr->cy;
  dlgitem->cChildren    = dhdr->NumberOfItems;
  dlgitem->id           = (USHORT)id;           //ord id or converted name id
  dlgitem->fsItemStatus = 1;
  dlgitem->offClassName = 1;    //WC_FRAME

  cout.setf(ios::hex, ios::basefield);
  cout << "Dialog Style         : " << dhdr->lStyle << endl;
  cout << "Extended Dialog Style: " << dhdr->lExtendedStyle << endl;
  cout.setf(ios::dec, ios::basefield);
  cout << "Position             : (" << dhdr->x << ',' << dhdr->y << ')' << endl;
  cout << "Size                 : (" << dhdr->cx << ',' << dhdr->cy << ')' << endl;
  cout << "Number of Items      : "  << dhdr->NumberOfItems << endl;
  //Menu id
  if(dhdr->fNameOrd == 0xFFFF) {
        cout << "Menu ID            : " << *(WORD *)(dhdr+1) << endl;
        ptrArray = (WORD *)((int)dhdr + sizeof(DialogBoxHeader) + sizeof(WORD));
  }
  else {
        if(dhdr->fNameOrd == 0) {//SvL: 0 == no menu!
                ptrArray = (WORD *)((int)dhdr + sizeof(DialogBoxHeader));
        }
        else {
                cout << "Menu namestring    : " << UnicodeToAscii((WCHAR *)(&dhdr->fNameOrd), cp) << endl;
                ptrArray = (WORD *)((int)&dhdr->fNameOrd + UniStrlen((WCHAR *)(&dhdr->fNameOrd))*2 + sizeof(WCHAR));
        }
  }
  //Class id
  if(*ptrArray == 0xFFFF) {
        cout << "Class ID            : " << *(WORD *)(ptrArray+1) << endl;
        ptrArray = (WORD *)((int)ptrArray + 2*sizeof(WORD));
  }
  else {
        if(*ptrArray == 0) {//SvL: 0 == no menu!
                ptrArray = (WORD *)((int)ptrArray + sizeof(WORD));
        }
        else {
                szClass = UnicodeToAscii((WCHAR *)ptrArray, cp);
                cout << "Class Name            : " << szClass << endl;

                dlgitem->cchClassName = (USHORT)strlen(szClass);
                dlgitem->offClassName = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, szClass);
                //Open32 wants class name in upper case!
                UpCase((char *)dlgcurdata);
                dlgcurdata           += dlgitem->cchClassName + 1;      //include terminating 0 (just to be sure)
//TODO: SvL: Can they also be system classes?
//              dlgitem->flStyle      = dhdr->lStyle;
//              dlgitem->flStyle      = ConvertDlgItemStyle(dhdr->lStyle);
                ptrArray = (WORD *)((int)ptrArray + UniStrlen((WCHAR *)(ptrArray))*2 + sizeof(WORD));
        }
  }

  //Title
  if(*ptrArray == 0) {
        cout << "No title " << endl;
        ctrldata = (ControlData *)((int)(int)ptrArray + sizeof(WORD));
  }
  else {
        ctrltext = UnicodeToAscii((WCHAR *)(ptrArray), cp);
        cout << "Title                : " << ctrltext << endl;
        ctrldata = (ControlData *)((int)(int)ptrArray + UniStrlen((WCHAR *)(ptrArray))*2 + sizeof(WORD));

        if(ctrltext[0] != 0) {
                dlgitem->cchText = (USHORT)strlen(ctrltext);
                dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, ctrltext);
                dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
        } else
        { //CB: cchText == 0, OS/2 uses offText anyway! (entryfields)
          dlgitem->offText = (USHORT)((int)dlgcurdata-(int)dlgt);
          dlgcurdata++; //0 at offText
        }
  }

  if(dhdr->lStyle & DS_SETFONT) {
        fhdr = (FontHeader *)ctrldata;
        font = UnicodeToAscii(fhdr->szFontName, cp);
        cout << "Font Point Size      : " << fhdr->wPointSize << endl;
        cout << "Font Name            : " << font << endl;
        ctrldata = (ControlData *)((int)fhdr + sizeof(FontHeader) - 2 + UniStrlen(fhdr->szFontName)*2 + sizeof(WORD));  /*PLF Sat  97-06-21 20:22:44*/
        //TODO: no pres params yet (font in win32 dialog ignored)
        dlgpparam = (PRESPARAMS *)dlgcurdata;
        dlgpparam->cb = 0;
        dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
        dlgpparam->aparam[0].cb = 0;
        UpCase(font);
        if(ConvertFont(font, dlgpparam, fhdr->wPointSize) == TRUE) {
                dlgpparam->aparam[0].cb = strlen(dlgpparam->aparam[0].ab) + 2*sizeof(ULONG) + 1;
//              dlgpparam->cb           = sizeof(ULONG) + dlgpparam->aparam[0].cb;
                dlgpparam->cb           = dlgpparam->aparam[0].cb;
                dlgitem->offPresParams  = (USHORT)((int)dlgpparam - (int)dlgt);
                dlgcurdata             += dlgpparam->cb + sizeof(ULONG);
        } else
        { //CB: set default font
          dlgpparam = (PRESPARAMS*)dlgcurdata;
          dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
          dlgpparam->aparam[0].cb = strlen(DEFAULT_DLGFONT)+1;
          strcpy((char*)&dlgpparam->aparam[0].ab,DEFAULT_DLGFONT);
          dlgpparam->cb           = dlgpparam->aparam[0].cb+2*sizeof(ULONG);
          dlgitem->offPresParams  = (USHORT)((int)dlgpparam-(int)dlgt);
          dlgt->coffPresParams++;
          dlgcurdata             += dlgpparam->cb+sizeof(dlgpparam->cb);
        }
  } else
  { //CB: set default font
    dlgpparam = (PRESPARAMS*)dlgcurdata;
    dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
    dlgpparam->aparam[0].cb = strlen(DEFAULT_DLGFONT)+1;
    strcpy((char*)&dlgpparam->aparam[0].ab,DEFAULT_DLGFONT);
    dlgpparam->cb           = dlgpparam->aparam[0].cb+2*sizeof(ULONG);
    dlgitem->offPresParams  = (USHORT)((int)dlgpparam-(int)dlgt);
    dlgt->coffPresParams++;
    dlgcurdata             += dlgpparam->cb+sizeof(dlgpparam->cb);
  }
  dlgitem++;

  ctrldata = (ControlData *)(((int)ctrldata+3) & ~3);
  for(i=0;i<dhdr->NumberOfItems;i++) {
        //save as OS/2 DLGTITEM
        dlgitem->x             = ctrldata->x;
        //SvL: 3-8-'97
        // OS/2  -> left bottom == origin and y coordinate == left bottom origin control
        // Win32 -> left top    == origin and y coordinate == left top origin
        dlgitem->y             = dlgt->adlgti[0].cy - ctrldata->y - ctrldata->cy;
        dlgitem->cx            = ctrldata->cx;
        dlgitem->cy            = ctrldata->cy;
        dlgitem->id            = ctrldata->wId;
        dlgitem->offCtlData    = 0xFFFF;
        dlgitem->offPresParams = 0xFFFF;

        //TODO: Extended style

        cout.setf(ios::hex, ios::basefield);
        cout << "***** Control Style   : " << ctrldata->lStyle << endl;
        cout << "Extended Control Style: " << ctrldata->lExtendedStyle << endl;
        cout.setf(ios::dec, ios::basefield);
        cout << "Position              : (" << ctrldata->x << ',' << ctrldata->y << ')' << endl;
        cout << "Size                  : (" << ctrldata->cx << ',' << ctrldata->cy << ')' << endl;
        cout << "wID                   : " << ctrldata->wId << endl;
        winclass = 0;
        if(ctrldata->fClassId == 0xFFFF) {
                winclass = *(WORD *)(ctrldata+1);
                cout << "Class ID              : " << winclass << endl;
                szCaption = (WCHAR *)((int)ctrldata + sizeof(ControlData) + sizeof(WORD));
                dlgitem->flStyle      = ConvertClassAndStyle(*(WORD *)(ctrldata+1), ctrldata->lStyle, &dlgitem->offClassName, &fIconBmp);
                dlgitem->cchClassName = 0;
        }
        else {
                szClass = UnicodeToAscii((WCHAR *)&ctrldata->fClassId, cp);
                cout << "Class Name            : " << szClass << endl;
                szCaption = (WCHAR *)((int)ctrldata + sizeof(ControlData) + strlen(szClass)*2);
                dlgitem->cchClassName = (USHORT)strlen(szClass);
                dlgitem->offClassName = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, szClass);
                //Open32 wants class name in upper case!
                UpCase((char *)dlgcurdata);
                dlgcurdata           += dlgitem->cchClassName + 1;      //include terminating 0 (just to be sure)
                dlgitem->flStyle      = ConvertDlgItemStyle(ctrldata->lStyle) ;
        }
        if(*(USHORT *)szCaption == 0xFFFF) {
                szCaption += 2;
                dlgitem->cchText = 0;
                dlgitem->offText = (USHORT)((int)dlgcurdata-(int)dlgt);
                dlgcurdata += 1; //CB: offText == empty string
        }
        else {  //Handle Caption
                ctrltext = UnicodeToAscii(szCaption, cp);
                //SvL: (16-9-'97) Convert '&' chars to '~' (for some classes)
                ConvertCaption(winclass, ctrltext);
                if(fIconBmp == TRUE) {//control contains bitmap or icon
                        dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                        int resid = OS2Exe.ConvertNametoId(ctrltext);
                        *(char *)dlgcurdata = '#';
                        itoa(resid, (char *)dlgcurdata+1, 10);
                        dlgitem->cchText = (USHORT)strlen((char *)dlgcurdata);  //one USHORT for res id
                        dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
                }
                else {
                        if(ctrltext[0] != 0) {
                                dlgitem->cchText = (USHORT)strlen(ctrltext);
                                dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                                strcpy((char *)dlgcurdata, ctrltext);
                                dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
                        } else
                        { //CB: cchText == 0, OS/2 uses offText anyway! (entryfields)
                          dlgitem->offText = (USHORT)((int)dlgcurdata-(int)dlgt);
                          dlgcurdata++; //0 at offText
                        }
                }
                cout << "Text                  : " << UnicodeToAscii(szCaption, cp) << endl;
                szCaption = (WCHAR *)((int)szCaption + UniStrlen(szCaption)*2 + sizeof(WORD));
        }

        cout << "Extra Stuff WORD      : " << *(WORD *)(szCaption) << endl;
        ctrldata = (ControlData *)((int)szCaption + sizeof(WORD));
        ctrldata = (ControlData *)(((int)ctrldata+3) & ~3);
        dlgitem++;
  }
  //calculate dialog box length
  dlgt->cbTemplate = (USHORT)((int)dlgcurdata - (int)dlgt);
  OS2Exe.StoreResource(id, RT_DIALOG, dlgt->cbTemplate, (char *)dlgt);
  free(dlgt);
}
//******************************************************************************
//******************************************************************************
void ShowDialogEx(int id, WINDLGTEMPLATEEX *dhdr, int size, int cp)
{
 DLGTEMPLATE *dlgt;
 DLGTITEM    *dlgitem;
 PRESPARAMS  *dlgpparam;
 char        *dlgdata, *dlgcurdata;
 WCHAR       *szCaption;
 WORD        *ptrArray;
 char        *ctrltext, *szClass, *font;
 FontHeaderEx *fhdr;
 WINDLGITEMTEMPLATEEX *ctrldata;
 int          i;
 ULONG        ctrlflag, winclass;
 BOOL         fIconBmp;
 ULONG        ulCpSize, ulCP;

  //should be enough
  dlgt    = (DLGTEMPLATE *)malloc(sizeof(DLGTEMPLATE) + dhdr->NumberOfItems*sizeof(DLGTITEM) + size);
  dlgitem = &dlgt->adlgti[0];
  dlgdata = (char *)&dlgt->adlgti[dhdr->NumberOfItems+1];  //for control data, presparams & strings
  memset((char *)dlgt, 0, sizeof(DLGTEMPLATE) + dhdr->NumberOfItems*sizeof(DLGTITEM) + size);
  dlgcurdata = dlgdata;

  if(cp == 0) {
	dlgt->codepage         = 437;
  }
  else
  {
     	DosQueryCp(sizeof(ulCP), &ulCP, &ulCpSize);
     	dlgt->codepage = ulCP;
  }
  dlgt->offadlgti        = 14;
  dlgt->fsTemplateStatus = 1;
  dlgt->iItemFocus       = 65535;
  dlgt->coffPresParams   = 0;

  dlgitem->flStyle = ConvertDlgStyle(dhdr->lStyle) | WS_SYNCPAINT | WS_CLIPSIBLINGS;

  ctrlflag = 0;
  if((dhdr->lStyle & WINWS_CAPTION) == WINWS_CAPTION) {
        ctrlflag |= FCF_TITLEBAR;
        dlgitem->flStyle |= FS_BORDER;
  }
  else
  if(dhdr->lStyle & WINWS_DLGFRAME)     //can't have title bar
        dlgitem->flStyle |= FS_DLGBORDER;

  if((dhdr->lStyle & (WINWS_SYSMENU | WINWS_CAPTION)) == (WINWS_SYSMENU | WINWS_CAPTION))
        ctrlflag |= FCF_SYSMENU;
  if(dhdr->lStyle & WINWS_VSCROLL)
        ctrlflag |= FCF_VERTSCROLL;
  if(dhdr->lStyle & WINWS_HSCROLL)
        ctrlflag |= FCF_HORZSCROLL;
  if(dhdr->lStyle & WINWS_MINIMIZEBOX)
        ctrlflag |= FCF_MINBUTTON;
  if(dhdr->lStyle & WINWS_MAXIMIZEBOX)
        ctrlflag |= FCF_MAXBUTTON;
  if(dhdr->lStyle & WINWS_THICKFRAME)
        dlgitem->flStyle |= FS_SIZEBORDER;

  if(ctrlflag) {
        *(ULONG *)dlgcurdata = ctrlflag;
        dlgitem->offCtlData  = (USHORT)((int)dlgcurdata - (int)dlgt);
        dlgcurdata          += sizeof(ULONG);
  }
  else  dlgitem->offCtlData  = 0xFFFF;
  dlgitem->x            = dhdr->x;
  dlgitem->y            = dhdr->y;
  dlgitem->cx           = dhdr->cx;
  dlgitem->cy           = dhdr->cy;
  dlgitem->cChildren    = dhdr->NumberOfItems;
  dlgitem->id           = (USHORT)id;           //ord id or converted name id
  dlgitem->fsItemStatus = 1;
  dlgitem->offClassName = 1;    //WC_FRAME

  cout.setf(ios::hex, ios::basefield);
  cout << "Dialog Style         : " << dhdr->lStyle << endl;
  cout << "Extended Dialog Style: " << dhdr->lExtendedStyle << endl;
  cout.setf(ios::dec, ios::basefield);
  cout << "Position             : (" << dhdr->x << ',' << dhdr->y << ')' << endl;
  cout << "Size                 : (" << dhdr->cx << ',' << dhdr->cy << ')' << endl;
  cout << "Number of Items      : "  << dhdr->NumberOfItems << endl;
  //Menu id
  if(dhdr->fNameOrd == 0xFFFF) {
        cout << "Menu ID            : " << *(WORD *)(dhdr+1) << endl;
        ptrArray = (WORD *)((int)dhdr + sizeof(WINDLGTEMPLATEEX) + sizeof(WORD));
  }
  else {
        if(dhdr->fNameOrd == 0) {//SvL: 0 == no menu!
                ptrArray = (WORD *)((int)dhdr + sizeof(WINDLGTEMPLATEEX));
        }
        else {
                cout << "Menu namestring    : " << UnicodeToAscii((WCHAR *)(&dhdr->fNameOrd, cp)) << endl;
                ptrArray = (WORD *)((int)&dhdr->fNameOrd + UniStrlen((WCHAR *)(&dhdr->fNameOrd))*2 + sizeof(WCHAR));
        }
  }
  //Class id
  if(*ptrArray == 0xFFFF) {
        cout << "Class ID            : " << *(WORD *)(ptrArray+1) << endl;
        ptrArray = (WORD *)((int)ptrArray + 2*sizeof(WORD));
  }
  else {
        if(*ptrArray == 0) {//SvL: 0 == no menu!
                ptrArray = (WORD *)((int)ptrArray + sizeof(WORD));
        }
        else {
                szClass = UnicodeToAscii((WCHAR *)ptrArray);
                cout << "Class Name            : " << szClass << endl;

                dlgitem->cchClassName = (USHORT)strlen(szClass);
                dlgitem->offClassName = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, szClass);
                //Open32 wants class name in upper case!
                UpCase((char *)dlgcurdata);
                dlgcurdata           += dlgitem->cchClassName + 1;      //include terminating 0 (just to be sure)
//TODO: SvL: Can they also be system classes?
//              dlgitem->flStyle      = dhdr->lStyle;
//              dlgitem->flStyle      = ConvertDlgItemStyle(dhdr->lStyle);
                ptrArray = (WORD *)((int)ptrArray + UniStrlen((WCHAR *)(ptrArray))*2 + sizeof(WORD));
        }
  }

  //Title
  if(*ptrArray == 0) {
        cout << "No title " << endl;
        ctrldata = (WINDLGITEMTEMPLATEEX *)((int)(int)ptrArray + sizeof(WORD));
  }
  else {
        ctrltext = UnicodeToAscii((WCHAR *)(ptrArray), cp);
        cout << "Title                : " << ctrltext << endl;
        ctrldata = (WINDLGITEMTEMPLATEEX *)((int)(int)ptrArray + UniStrlen((WCHAR *)(ptrArray))*2 + sizeof(WORD));

        if(ctrltext[0] != 0) {
                dlgitem->cchText = (USHORT)strlen(ctrltext);
                dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, ctrltext);
                dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
        } else
        { //CB: cchText == 0, OS/2 uses offText anyway! (entryfields)
          dlgitem->offText = (USHORT)((int)dlgcurdata-(int)dlgt);
          dlgcurdata++; //0 at offText
        }
  }
  dlgitem++;

  if(dhdr->lStyle & DS_SETFONT) {
        fhdr = (FontHeaderEx *)ctrldata;
        font = UnicodeToAscii(fhdr->szFontName, cp);
        cout << "Font Point Size      : " << fhdr->wPointSize << endl;
        cout << "Font Name            : " << font << endl;
        ctrldata = (WINDLGITEMTEMPLATEEX *)((int)fhdr + sizeof(FontHeaderEx) - 2 + UniStrlen(fhdr->szFontName)*2 + sizeof(WORD));  /*PLF Sat  97-06-21 20:22:44*/
        //TODO: no pres params yet (font in win32 dialog ignored)
        dlgpparam = (PRESPARAMS *)dlgcurdata;
        dlgpparam->cb = 0;
        dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
        dlgpparam->aparam[0].cb = 0;
        UpCase(font);
        if(ConvertFont(font, dlgpparam, fhdr->wPointSize) == TRUE) {
                dlgpparam->aparam[0].cb = strlen(dlgpparam->aparam[0].ab) + 2*sizeof(ULONG) + 1;
                dlgpparam->cb           = sizeof(ULONG) + dlgpparam->aparam[0].cb;
                dlgt->coffPresParams    = (USHORT)((int)dlgpparam - (int)dlgt);
                dlgcurdata             += dlgpparam->cb;
        } else
        { //CB: set default font
          dlgpparam = (PRESPARAMS*)dlgcurdata;
          dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
          dlgpparam->aparam[0].cb = strlen(DEFAULT_DLGFONT)+1;
          strcpy((char*)&dlgpparam->aparam[0].ab,DEFAULT_DLGFONT);
          dlgpparam->cb           = dlgpparam->aparam[0].cb+2*sizeof(ULONG);
          dlgitem->offPresParams  = (USHORT)((int)dlgpparam-(int)dlgt);
          dlgt->coffPresParams++;
          dlgcurdata             += dlgpparam->cb+sizeof(dlgpparam->cb);
        }
  } else
  { //CB: set default font
    dlgpparam = (PRESPARAMS*)dlgcurdata;
    dlgpparam->aparam[0].id = PP_FONTNAMESIZE;
    dlgpparam->aparam[0].cb = strlen(DEFAULT_DLGFONT)+1;
    strcpy((char*)&dlgpparam->aparam[0].ab,DEFAULT_DLGFONT);
    dlgpparam->cb           = dlgpparam->aparam[0].cb+2*sizeof(ULONG);
    dlgitem->offPresParams  = (USHORT)((int)dlgpparam-(int)dlgt);
    dlgt->coffPresParams++;
    dlgcurdata             += dlgpparam->cb+sizeof(dlgpparam->cb);
  }
  ctrldata = (WINDLGITEMTEMPLATEEX *)(((int)ctrldata+3) & ~3);
  for(i=0;i<dhdr->NumberOfItems;i++) {
        //save as OS/2 DLGTITEM
        dlgitem->x             = ctrldata->x;
        //SvL: 3-8-'97
        // OS/2  -> left bottom == origin and y coordinate == left bottom origin control
        // Win32 -> left top    == origin and y coordinate == left top origin
        dlgitem->y             = dlgt->adlgti[0].cy - ctrldata->y - ctrldata->cy;
        dlgitem->cx            = ctrldata->cx;
        dlgitem->cy            = ctrldata->cy;
        dlgitem->id            = (USHORT)ctrldata->wId;
        dlgitem->offCtlData    = 0xFFFF;
        dlgitem->offPresParams = 0xFFFF;

        //TODO: Extended style

        cout.setf(ios::hex, ios::basefield);
        cout << "***** Control Style   : " << ctrldata->lStyle << endl;
        cout << "Extended Control Style: " << ctrldata->lExtendedStyle << endl;
        cout.setf(ios::dec, ios::basefield);
        cout << "Position              : (" << ctrldata->x << ',' << ctrldata->y << ')' << endl;
        cout << "Size                  : (" << ctrldata->cx << ',' << ctrldata->cy << ')' << endl;
        cout << "wID                   : " << ctrldata->wId << endl;
        winclass = 0;
        if(ctrldata->fClassId == 0xFFFF) {
                winclass = *(WORD *)(ctrldata+1);
                cout << "Class ID              : " << winclass << endl;
                szCaption = (WCHAR *)((int)ctrldata + sizeof(WINDLGITEMTEMPLATEEX) + sizeof(WORD));
                dlgitem->flStyle      = ConvertClassAndStyle(*(WORD *)(ctrldata+1), ctrldata->lStyle, &dlgitem->offClassName, &fIconBmp);
                dlgitem->cchClassName = 0;
        }
        else {
                szClass = UnicodeToAscii((WCHAR *)&ctrldata->fClassId, cp);
                cout << "Class Name            : " << szClass << endl;
                szCaption = (WCHAR *)((int)ctrldata + sizeof(WINDLGITEMTEMPLATEEX) + strlen(szClass)*2);
                dlgitem->cchClassName = (USHORT)strlen(szClass);
                dlgitem->offClassName = (USHORT)((int)dlgcurdata - (int)dlgt);
                strcpy((char *)dlgcurdata, szClass);
                //Open32 wants class name in upper case!
                UpCase((char *)dlgcurdata);
                dlgcurdata           += dlgitem->cchClassName + 1;      //include terminating 0 (just to be sure)
                dlgitem->flStyle      = ConvertDlgItemStyle(ctrldata->lStyle) ;
        }
        if(*(USHORT *)szCaption == 0xFFFF) {
                szCaption += 2;
        }
        else {  //Handle Caption
                ctrltext = UnicodeToAscii(szCaption, cp);
                //SvL: (16-9-'97) Convert '&' chars to '~' (for some classes)
                ConvertCaption(winclass, ctrltext);
                if(fIconBmp == TRUE) {//control contains bitmap or icon
                        dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                        int resid = OS2Exe.ConvertNametoId(ctrltext);
                        *(char *)dlgcurdata = '#';
                        itoa(resid, (char *)dlgcurdata+1, 10);
                        dlgitem->cchText = (USHORT)strlen((char *)dlgcurdata);  //one USHORT for res id
                        dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
                }
                else {
                        if(ctrltext[0] != 0) {
                                dlgitem->cchText = (USHORT)strlen(ctrltext);
                                dlgitem->offText = (USHORT)((int)dlgcurdata - (int)dlgt);
                                strcpy((char *)dlgcurdata, ctrltext);
                                dlgcurdata      += dlgitem->cchText + 1;        //include terminating 0 (just to be sure)
                        } else
                        { //CB: cchText == 0, OS/2 uses offText anyway! (entryfields)
                          dlgitem->offText = (USHORT)((int)dlgcurdata-(int)dlgt);
                          dlgcurdata++; //0 at offText
                        }
                }
                cout << "Text                  : " << UnicodeToAscii(szCaption, cp) << endl;
                szCaption = (WCHAR *)((int)szCaption + UniStrlen(szCaption)*2 + sizeof(WORD));
        }

        cout << "Extra Stuff WORD      : " << *(WORD *)(szCaption) << endl;
        ctrldata = (WINDLGITEMTEMPLATEEX *)((int)szCaption + sizeof(WORD));
        ctrldata = (WINDLGITEMTEMPLATEEX *)(((int)ctrldata+3) & ~3);
        dlgitem++;
  }
  //calculate dialog box length
  dlgt->cbTemplate = (USHORT)((int)dlgcurdata - (int)dlgt);
  OS2Exe.StoreResource(id, RT_DIALOG, dlgt->cbTemplate, (char *)dlgt);
  free(dlgt);
}
//******************************************************************************
//******************************************************************************
static int ConvertClassAndStyle(int    winclass,
                                int    style,
                                USHORT *os2class,
                                BOOL   *fIconBmp)
{
 int os2style = ConvertDlgItemStyle(style);

  *fIconBmp = FALSE;

  switch(winclass) {
        case WIN_BUTTON:
#if 0
                if(style & WINBS_LEFTTEXT)
                        os2style |= ; //not supported
#endif
                style &= 0xF;
//BS_TEXT, BS_BITMAP, BS_ICON, BS_MINIICON can only be used with BS_PUSHBUTTON
                *os2class = (int)WC_BUTTON & 0xFFFF;
                if(style == WINBS_PUSHBUTTON)
                        os2style |= BS_PUSHBUTTON;
                else
                if(style == WINBS_DEFPUSHBUTTON)
                        os2style |= (BS_PUSHBUTTON | BS_DEFAULT);       //TODO: Correct?
                else
                if(style == WINBS_CHECKBOX)
                        os2style |= BS_CHECKBOX;
                else
                if(style == WINBS_AUTOCHECKBOX)
                        os2style |= BS_AUTOCHECKBOX;
                else
                if(style == WINBS_RADIOBUTTON)
                        os2style |= BS_RADIOBUTTON;
                else
                if(style == WINBS_3STATE)
                        os2style |= BS_3STATE;
                else
                if(style == WINBS_AUTO3STATE)
                        os2style |= BS_AUTO3STATE;
                else
                if(style == WINBS_USERBUTTON) //obsolete
                        os2style |= BS_USERBUTTON;
                else
                if(style == WINBS_AUTORADIOBUTTON)
                        os2style |= BS_AUTORADIOBUTTON;
                else
                if(style == WINBS_GROUPBOX) {
                         *os2class = (int)WC_STATIC & 0xFFFF;    /*PLF Sat  97-09-20 23:58:28*/
                         os2style  |= SS_GROUPBOX;   /*PLF Sun  97-09-21 00:11:07*/
                }
                else
                if(style & WINBS_OWNERDRAW)
                        os2style |= BS_USERBUTTON;      //TODO:Correct??
                else    os2style |= (BS_TEXT | BS_PUSHBUTTON);
                os2style |= BS_AUTOSIZE;
                break;

  case WIN_EDIT:
    /* @@@PH 98/06/13: other styles might require a MLE-class control !      */
    /* please note this is experimental since MLE might behave different from */
    /* the standard EF-control, maybe we'll have to build an own MLE therefore*/

    if (style & WINES_MULTILINE)
    {
      *os2class = (int)WC_MLE & 0xFFFF;

      os2style |= MLS_BORDER | MLS_WORDWRAP;

      //if(style & WINES_MULTILINE)   os2style |= ;
      //if(style & WINES_UPPERCASE)   os2style |= ;
      //if(style & WINES_LOWERCASE)   os2style |= ;
      if(style & WINES_AUTOVSCROLL) os2style |= MLS_VSCROLL;
      //if(style & WINES_NOHIDESEL)   os2style |= ;
      //if(style & WINES_OEMCONVERT)  os2style |= ;
      //if(style & WINES_WANTRETURN)  os2style |= ;
      //if(style & WINES_LEFT)        os2style |= ES_LEFT;
      //if(style & WINES_CENTER)      os2style |= ES_CENTER;
      //if(style & WINES_RIGHT)       os2style |= ES_RIGHT;
      if(style & WINES_AUTOHSCROLL) os2style |= MLS_HSCROLL;
      if(style & WINES_READONLY)    os2style |= MLS_READONLY;
      //if(style & WINES_PASSWORD)    os2style |= ES_UNREADABLE;
    }
    else
    {
      *os2class = (int)WC_ENTRYFIELD & 0xFFFF;
      if(style & WINES_LEFT)        os2style |= ES_LEFT;
      if(style & WINES_CENTER)      os2style |= ES_CENTER;
      if(style & WINES_RIGHT)       os2style |= ES_RIGHT;
      if(style & WINES_AUTOHSCROLL) os2style |= ES_AUTOSCROLL;
      if(style & WINES_READONLY)    os2style |= ES_READONLY;
      if(style & WINES_PASSWORD)    os2style |= ES_UNREADABLE;
      if(style & WINWS_BORDER)      os2style |= ES_MARGIN;

    }
    break;

        case WIN_STATIC:
                style &= 0xFFFF;
                *os2class = (int)WC_STATIC & 0xFFFF;
                if(style == WINSS_LEFT)
                        os2style |= SS_TEXT | DT_LEFT;
                else
                if(style == WINSS_CENTER)
                        os2style |= SS_TEXT | DT_CENTER;
                else
                if(style == WINSS_RIGHT)
                        os2style |= SS_TEXT | DT_RIGHT;
                else
                if(style == WINSS_SIMPLE)
                        os2style |= SS_TEXT | DT_LEFT;
                else
                if(style == WINSS_ICON) {
                        os2style |= SS_ICON;
                        *fIconBmp = TRUE;
                }
                else
                if(style == WINSS_BLACKRECT)
                        os2style |= SS_FGNDRECT;
                else
                if(style == WINSS_GRAYRECT)
                        os2style |= SS_HALFTONERECT;
                else
                if(style == WINSS_WHITERECT)
                        os2style |= SS_BKGNDRECT;
                else
                if(style == WINSS_BLACKFRAME)
                        os2style |= SS_FGNDFRAME;
                else
                if(style == WINSS_GRAYFRAME)
                        os2style |= SS_HALFTONEFRAME;
                else
                if(style == WINSS_WHITEFRAME)
                        os2style |= SS_BKGNDFRAME;
                else    os2style |= SS_TEXT;

//TODO
#if 0
                if(style == WINSS_LEFTNOWORDWRAP)
                        os2style |= ;
                if(style == WINSS_USERITEM)
                        os2style |= ;
                if(style == WINSS_NOPREFIX)
                        os2style |= ;
#endif
                break;
        case WIN_LISTBOX:
                *os2class = (int)WC_LISTBOX & 0xFFFF;
#if 0
                if(style & WINLBS_NOTIFY)
                        os2style |= ;
                if(style & WINLBS_SORT)
                        os2style |= ;
                if(style & WINLBS_NOREDRAW)
                        os2style |= ;
#endif
                if(style & WINLBS_MULTIPLESEL)
                        os2style |= LS_MULTIPLESEL;
                if(style & WINLBS_OWNERDRAWFIXED)
                        os2style |= LS_OWNERDRAW;       //TODO: Correct?
                if(style & WINLBS_OWNERDRAWVARIABLE)
                        os2style |= LS_OWNERDRAW;       //TODO: Correct?
#if 0
                if(style & WINLBS_HASSTRINGS)
                        os2style |= ;
                if(style & WINLBS_USETABSTOPS)
                        os2style |= ;
                if(style & WINLBS_NOINTEGRALHEIGHT)
                        os2style |= ;
                if(style & WINLBS_MULTICOLUMN)
                        os2style |= ;
                if(style & WINLBS_WANTKEYBOARDINPUT)
                        os2style |= ;
                if(style & WINLBS_EXTENDEDSEL)
                        os2style |= LS_EXTENDEDSEL;
                if(style & WINLBS_DISABLENOSCROLL)
                        os2style |= ;
#endif
//              if(style & WINLBS_NODATA)
//                      os2style |= ;
      break;

    case WIN_SCROLLBAR:
      *os2class = (int)WC_SCROLLBAR & 0xFFFF;

      if(style & WINSBS_HORZ) os2style |= SBS_HORZ;
      else
        if(style & WINSBS_VERT) os2style |= SBS_VERT;

      //if(style & WINSBS_TOPALIGN)                os2style |= ;
      //if(style & WINSBS_LEFTALIGN)               os2style |= ;
      //if(style & WINSBS_BOTTOMALIGN)             os2style |= ;
      //if(style & WINSBS_RIGHTALIGN)              os2style |= ;
      //if(style & WINSBS_SIZEBOXTOPLEFTALIGN)     os2style |= ;
      //if(style & WINSBS_SIZEBOXBOTTOMRIGHTALIGN) os2style |= ;

      if(style & WINSBS_SIZEBOX) os2style |= SBS_AUTOSIZE;      //TODO: Correct?
      break;

    case WIN_COMBOBOX:
      *os2class = (int)WC_COMBOBOX & 0xFFFF;

      /* @@@PH 98/06/13 corrected style translation */
      switch (style & 0x00000003)
      {
        case WINCBS_SIMPLE:       os2style |= CBS_SIMPLE;       break;
        case WINCBS_DROPDOWN:     os2style |= CBS_DROPDOWN;     break;
        case WINCBS_DROPDOWNLIST: os2style |= CBS_DROPDOWNLIST; break;
        default:                  os2style |= CBS_SIMPLE;       break;
      }

      //if(style & WINCBS_OWNERDRAWFIXED)    os2style |= ;
      //if(style & WINCBS_OWNERDRAWVARIABLE) os2style |= ;
      //if(style & WINCBS_AUTOHSCROLL)       os2style |= ;
      //if(style & WINCBS_OEMCONVERT)        os2style |= ;
      //if(style & WINCBS_SORT)              os2style |= ;
      //if(style & WINCBS_HASSTRINGS )       os2style |= ;
      //if(style & WINCBS_NOINTEGRALHEIGHT)  os2style |= ;
      //if(style & WINCBS_DISABLENOSCROLL)   os2style |= ;
      break;

    default:
      //SvL: 9nov97 Special class control. Just copy style
      os2style = style;
      break;
  }
  return(os2style);
}

//******************************************************************************
//******************************************************************************
static int ConvertDlgStyle(int style)
{
 int os2style = 0;

///  if(style & WINWS_POPUP)
///     os2style |=
///  if(style & WINWS_CHILD)
///     os2style |=
  if(style & WINWS_MINIMIZE)
        os2style |= WS_MINIMIZED;
  if(style & WINWS_MAXIMIZE)
        os2style |= WS_MAXIMIZED;
  if(style & WINWS_VISIBLE)
        os2style |= WS_VISIBLE;
  if(style & WINWS_DISABLED)
        os2style |= WS_DISABLED;
  if(style & WINWS_CLIPSIBLINGS)
        os2style |= WS_CLIPSIBLINGS;
  if(style & WINWS_CLIPCHILDREN)
        os2style |= WS_CLIPCHILDREN;
  if(style & WINWS_TABSTOP)
        os2style |= WS_TABSTOP;

  return(os2style);
}
//******************************************************************************
//******************************************************************************
static int ConvertDlgItemStyle(int style)
{
 int os2style = 0;

  if(style & WINWS_VISIBLE)
        os2style |= WS_VISIBLE;
  if(style & WINWS_DISABLED)
        os2style |= WS_DISABLED;
  if(style & WINWS_CLIPSIBLINGS)
        os2style |= WS_CLIPSIBLINGS;
  if(style & WINWS_CLIPCHILDREN)
        os2style |= WS_CLIPCHILDREN;
  if(style & WINWS_TABSTOP)
        os2style |= WS_TABSTOP;
  if(style & WINWS_GROUP)
        os2style |= WS_GROUP;

  return(os2style);
}
//******************************************************************************
//******************************************************************************
static void ConvertCaption(ULONG style, char *caption)
{
 char os2caption[512];
 int i, len = strlen(caption), j;

  switch(style) {
        case WIN_BUTTON:
        case WIN_EDIT:
        case WIN_LISTBOX:
        case WIN_SCROLLBAR:
        case WIN_COMBOBOX:
                for(i=0;i<len;i++) {
                        if(caption[i] == '&')   caption[i] = '~';
                }
                break;
        case WIN_STATIC:
                j = 0;
                for(i=0;i<=len;i++) {
                        if(caption[i] != '&')   os2caption[j++] = caption[i];
                }
                strcpy(caption, os2caption);
                break;
  }
}
//******************************************************************************
//******************************************************************************
static int ConvertFont(char *font, PRESPARAMS *dlgpparam, int fsize)
{
#if 0
//  if(strcmp(font, "MS SHELL DLG") == 0) {
        sprintf(dlgpparam->aparam[0].ab, "%d.", fsize);
//      strcat(dlgpparam->aparam[0].ab, font);
        strcat(dlgpparam->aparam[0].ab, "Helv");
        return(TRUE);
//  }
//  else TODO: More fonts!!!
// "MS Sans Serif"
//  return(FALSE);      //not found
#else
    return(FALSE);
#endif
}
//******************************************************************************
//******************************************************************************
