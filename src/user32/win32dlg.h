/* $Id: win32dlg.h,v 1.6 1999-10-30 18:40:47 cbratschi Exp $ */
/*
 * Win32 Dialog Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32DLG_H__
#define __WIN32DLG_H__

#include <win32wbase.h>

#ifdef __cplusplus

#define DF_END  0x0001

  /* Dialog control information */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    INT        x;
    INT        y;
    INT        cx;
    INT        cy;
    UINT       id;
    LPCSTR     className;
    LPCSTR     windowName;
    LPVOID     data;
} DLG_CONTROL_INFO;

/* Dialog template */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    UINT       nbItems;
    INT        x;
    INT        y;
    INT        cx;
    INT        cy;
    LPCSTR     menuName;
    LPCSTR     className;
    LPCSTR     caption;
    WORD       pointSize;
    WORD       weight;
    BOOL       italic;
    LPCSTR     faceName;
    BOOL       dialogEx;
} DLG_TEMPLATE;

class Win32Dialog : public Win32BaseWindow
{
public:
         Win32Dialog(HINSTANCE hInst, LPCSTR dlgTemplate, HWND owner,
                     DLGPROC dlgProc, LPARAM param, BOOL isUnicode);

virtual ~Win32Dialog();

       LRESULT  DefDlgProcA(UINT Msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefDlgProcW(UINT Msg, WPARAM wParam, LPARAM lParam);

Win32BaseWindow *getDlgItem(int id) { return FindWindowById(id); };

         BOOL   endDialog(int retval);

virtual  ULONG  MsgCreate(HWND hwndFrame, HWND hwndClient);

virtual  LONG   SetWindowLongA(int index, ULONG value);
virtual  ULONG  GetWindowLongA(int index);

           INT  doDialogBox();

protected:
        LPCSTR  parseTemplate( LPCSTR dlgtemplate, DLG_TEMPLATE *result);
        WORD   *getControl(const WORD *p, DLG_CONTROL_INFO *info, BOOL dialogEx);
        BOOL    createControls(LPCSTR dlgtemplate, HINSTANCE hInst);

        LRESULT DefDlg_Proc(UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT DefDlg_Epilog(UINT msg, BOOL fResult);

        BOOL    setDefButton(HWND hwndNew );
        HWND    findDefButton();
        BOOL    saveFocus();
        BOOL    restoreFocus();
        void    setFocus(HWND hwndCtrl );

        // values normally contained in the standard dialog words
      DLGPROC   Win32DlgProc;   //DWL_WNDPROC
        ULONG   msgResult;      //DWL_MSGRESULT
        ULONG   userDlgData;    //DWL_USER

   DLG_TEMPLATE dlgInfo;
        WORD    xUnit;
        WORD    yUnit;
        HWND    hwndFocus;
        HFONT   hUserFont;
        HMENU   hMenu;
        DWORD   idResult;
        DWORD   dialogFlags;

        DWORD   tmpParam;       //set in ctor, used in MsgCreate method
        LPSTR   tmpDlgTemplate; //set in ctor, used in MsgCreate method
};

/* Built-in class names (see _Undocumented_Windows_ p.418) */
#define DIALOG_CLASS_NAMEA    "#32770"  /* Dialog */
#define DIALOG_CLASS_NAMEW   L"#32770"  /* Dialog */
#define DIALOG_CLASS_ATOM       32770   /* Dialog */

BOOL DIALOG_Register();
BOOL DIALOG_Unregister();

#endif //__cplusplus

#endif //__WIN32WND_H__