/* $Id: conprop.cpp,v 1.11 2001-03-13 18:45:32 sandervl Exp $ */

/*
 * Win32 Console API Translation for OS/2
 *
 * 1998/03/06 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) conprop.cpp             1.0.0   1998/03/06 PH Start from scratch
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 - save / load properties from EAs

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define  INCL_GPI
#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_AVIO
#include <os2wrap.h>    //Odin32 OS/2 api wrappers

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <odin.h>
#include "win32type.h"
#include "misc.h"

#include "conwin.h"          // Windows Header for console only
#include "console.h"
#include "console2.h"
#include "conprop.h"

#define DBG_LOCALLOG	DBG_conprop
#include "dbglocal.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/



#define HWNDERR( hwnd )         \
       (ERRORIDERROR( WinGetLastError( WinQueryAnchorBlock( hwnd ) ) ))


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct                     // VARIABLES USED FOR A NOTEBOOK PAGE
{
  ULONG     ulID;                  // Resource ID for dialog page
  PFNWP     wpDlgProc;             // dialog window procedure
  PSZ       szStatusLineText;      // Text to go on status line
  PSZ       szTabText;             // Text to go on major tab
  ULONG     idFocus;               // ID of the control to get the focus first
  BOOL      fParent;               // Is this a Parent page with minor pages
  USHORT    usTabType;             // BKA_MAJOR or BKA_MINOR
  USHORT    fsPageStyles;          // BKA_ styles for the page
} NBPAGE, *PNBPAGE;

#define TAB_WIDTH_MARGIN      10   // Padding for the width of a notebook tab
#define TAB_HEIGHT_MARGIN     6    // Padding for the height of a notebook tab
#define DEFAULT_NB_TAB_HEIGHT 16   // Default if Gpi calls fail

#define PAGE_COUNT(a) (sizeof( a ) / sizeof( NBPAGE ))
#define SPEAKERFREQ_LOW  200
#define SPEAKERFREQ_HIGH 2000
#define SPEAKERDUR_LOW 10
#define SPEAKERDUR_HIGH 2000

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

static VOID ErrorMsg( PSZ szFormat,
                     ... );


static HWND NBCreateNotebook(HWND    hwndClient,
                             ULONG   id,
                             HMODULE hModule,
                             PNBPAGE pPage ,
                             ULONG   ulPages,
                             PVOID   pCreationParameters);

static BOOL NBSetUpPage( HWND    hwndClient,
                         HWND    hwndNB,
                         HMODULE hModule,
                         PNBPAGE pPage,
                         PHWND   phwnd,
                         PVOID   pCreationParameters);

static BOOL NBSetFramePos( HWND hwndFrame );

static BOOL NBTurnToFirstPage( HWND hwndNB );

static BOOL NBSetTabDimensions(HWND hwndNB);

static VOID NBSetNBPage(HWND              hwndClient,
                        HMODULE           hModule,
                        PPAGESELECTNOTIFY ppsn,
                        PVOID             pCreationParameters);

static HWND NBCreateDialogPage(HWND    hwndParent,
                               HWND    hwndNB,
                               HMODULE hModule,
                               ULONG   ulPageID,
                               ULONG   idDlg,
                               FNWP    fpDlg,
                               PVOID   pCreationParameters);

static MRESULT EXPENTRY wpDlgProcPage1(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static MRESULT EXPENTRY wpDlgProcPage2(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static MRESULT EXPENTRY wpDlgProcPage3(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static MRESULT EXPENTRY wpDlgProcPage4(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static MRESULT EXPENTRY wpDlgProcPage5(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static MRESULT EXPENTRY wpDlgProcPage6(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

static INT WinGetStringSize(HPS hps,
                            PSZ szString );


static APIRET ConPropPage1Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions);

static APIRET ConPropPage2Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions);

static APIRET ConPropPage3Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions);

static APIRET ConPropPage4Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions);

static APIRET ConPropPage5Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions);

/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#define __PAGE__ 0, FALSE, BKA_MAJOR, BKA_AUTOPAGESIZE | BKA_STATUSTEXTON

/* pfncreate, szStatusLineText, szTabText, idFocus, fParent, usTabType, fsPageStyles */
static NBPAGE nbpage[] =
{
  {DLG_CONSOLE_PAGE1, wpDlgProcPage1, "General settings",         "~Settings", __PAGE__ },
  {DLG_CONSOLE_PAGE2, wpDlgProcPage2, "Speaker settings",         "S~peaker",  __PAGE__ },
  {DLG_CONSOLE_PAGE3, wpDlgProcPage3, "Window position and size", "~Window",   __PAGE__ },
  {DLG_CONSOLE_PAGE4, wpDlgProcPage4, "Colors",                   "C~olors",   __PAGE__ },
  {DLG_CONSOLE_PAGE5, wpDlgProcPage5, "Priorities",               "P~riority", __PAGE__ },
  {DLG_CONSOLE_PAGE6, wpDlgProcPage6, "About ...",                "~About",    __PAGE__ }
};

static PSZ priorities[] = {"idle", "normal", "critical", "server"};
#define NUMPRIORITIES 4

/**********************************************************************/
/*------------------------------- Msg --------------------------------*/
/*                                                                    */
/*  DISPLAY A MESSAGE TO THE USER.                                    */
/*                                                                    */
/*  INPUT: a message in printf format with its parms                  */
/*                                                                    */
/*  1. Format the message using vsprintf.                             */
/*  2. Sound a warning sound.                                         */
/*  3. Display the message in a message box.                          */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

#define MESSAGE_SIZE 1024

static VOID ErrorMsg( PSZ szFormat,
                     ... )
{
  PSZ     szMsg;
  va_list argptr;

  if( (szMsg = (PSZ)malloc( MESSAGE_SIZE )) == NULL )
    return;

  va_start( argptr, szFormat );
  vsprintf( szMsg, szFormat, argptr );
  va_end( argptr );

  szMsg[ MESSAGE_SIZE - 1 ] = 0;

  (void) WinMessageBox( HWND_DESKTOP,
                       HWND_DESKTOP,
                       szMsg,
                       "Error ...",
                       1,
                       MB_OK | MB_MOVEABLE );
  free( szMsg );
  return;
}


/*****************************************************************************
 * Name      : static MRESULT ConsolePropertyDlgProc
 * Purpose   : window procedure for the console property dialog
 * Parameters: HWND   hwndDialog
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Autor     : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

MRESULT EXPENTRY ConsolePropertyDlgProc(HWND   hwnd,
                                        ULONG  ulMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
{
  switch (ulMessage)
  {
    /*************************************************************************
     * initialize the dialog                                                 *
     *************************************************************************/
    case WM_INITDLG:
    {
      PICONSOLEOPTIONS pConsoleOptions = (PICONSOLEOPTIONS)mp2;    /* save ptr */
      HWND            hwndNotebook;                /* notebook window handle */


      /* @@@PH we should work on a copy of the console options for */
      /* UNDO and APPLY to work properly */

      WinSetWindowULong (hwnd,            /* store the data pointer in local */
                         QWL_USER,        /* variable storage attached to the*/
                         (ULONG)pConsoleOptions); /* window itself.          */

      hwndNotebook = NBCreateNotebook(hwnd,      /* fill notebook with pages */
                                      NB_CONSOLE_NOTEBOOK,
                                      pConsoleOptions->hmodResources,
                                      nbpage,
                                      PAGE_COUNT(nbpage),
                                      (PVOID)NULL);

      /* this message makes the client pages to write new values into their  */
      /* controls.                                                           */
      WinBroadcastMsg(hwnd,                      /* broadcast to all dialogs */
                      UM_PROPERTY_UNDO,
                      (MPARAM)pConsoleOptions,
                      (MPARAM)NULL,
                      BMSG_SEND |
                      BMSG_DESCENDANTS);

      return ((MPARAM)FALSE);
    }


    /*************************************************************************
     * control command messages                                              *
     *************************************************************************/

    case WM_COMMAND:
    {
      PICONSOLEOPTIONS pConsoleOptions;      /* the console options structure */

      pConsoleOptions = (PICONSOLEOPTIONS)
                        WinQueryWindowULong(hwnd,  /* query pointer from wnd */
                                            QWL_USER);

      switch (SHORT1FROMMP(mp1))
      {
        case ID_BTN_UNDO:
                                  /* broadcast user message UM_PROPERTY_UNDO */
          WinBroadcastMsg(hwnd,                  /* broadcast to all dialogs */
                          UM_PROPERTY_UNDO,
                          (MPARAM)pConsoleOptions,
                          (MPARAM)NULL,
                          BMSG_SEND |
                          BMSG_DESCENDANTS);
          return ( (MPARAM) FALSE);

        case ID_BTN_SAVE:
          /* @@@PH save console option work buffer */
          /* @@@PH ConsolePropertyWrite(pConsoleOptions) */
          WinDismissDlg(hwnd,
                        ID_BTN_SAVE);
          return ( (MPARAM) FALSE);

        case ID_BTN_APPLY:
          /* @@@PH save console option work buffer */
          WinDismissDlg(hwnd,
                        ID_BTN_APPLY);
          return ( (MPARAM) FALSE);

        case BS_HELP:
          return ( (MPARAM) FALSE);
      }
    }
  }

  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Autor     : Patrick Haller [Wed, 1998/03/07 11:46]
 *****************************************************************************/

static HWND NBCreateNotebook(HWND    hwndClient,
                             ULONG   id,
                             HMODULE hModule,
                             PNBPAGE pPage ,
                             ULONG   ulPages,
                             PVOID   pCreationParameters)
{
  BOOL      fSuccess = TRUE;
  HWND      hwndNB;
  INT       i;
  PNBPAGE   pP;                      /* Zeiger auf die Seiten des Notebookes */


  if (pPage == NULL)                                 /* ParameterÅberprÅfung */
    return ( NULLHANDLE );

  hwndNB = WinWindowFromID(hwndClient,         /* get notebook window handle */
                           id);

  if( hwndNB )
  {
      // Set the page background color to grey so it is the same as a dlg box.
    if( !WinSendMsg(hwndNB,
                    BKM_SETNOTEBOOKCOLORS,
                    MPFROMLONG( SYSCLR_FIELDBACKGROUND ),
                    MPFROMSHORT( BKA_BACKGROUNDPAGECOLORINDEX ) ) )
      ErrorMsg("BKM_SETNOTEBOOKCOLORS failed! RC(%X)",
               HWNDERR( hwndClient ));

      // Insert all the pages into the notebook and configure them. The dialog
      // boxes are not going to be loaded and associated with those pages yet.

    for(i = 0,
        pP = pPage;

        (i < ulPages) &&
        (fSuccess == TRUE);

        i++,
        pP++ )

      if (NBSetUpPage(hwndClient,
                      hwndNB,
                      hModule,
                      pP,
                      NULL,
                      pCreationParameters) == NULLHANDLE)
         fSuccess = FALSE;
  }
  else
  {
    fSuccess = FALSE;
    ErrorMsg("Notebook creation failed! RC(%X)",
             HWNDERR( hwndClient ) );
  }

  if (fSuccess == TRUE)                       /* set notebook tab dimensions */
    if( !NBSetTabDimensions(hwndNB))
          fSuccess = FALSE;


  if (fSuccess == TRUE)                         /* Haben wir Erfolg gehabt ? */
    return (hwndNB);                                                   /* Ja */
  else
    return ( NULLHANDLE );                                           /* Nein */
}



/*****************************************************************************
 *----------------------------- SetUpPage ----------------------------
 *
 *  SET UP A NOTEBOOK PAGE.
 *
 *  INPUT: window handle of notebook control,
 *         index into nbpage array
 *
 *  1.
 *
 *  OUTPUT: TRUE or FALSE if successful or not
 *
 *****************************************************************************/

static BOOL NBSetUpPage( HWND    hwndClient,
                         HWND    hwndNB,
                         HMODULE hModule,
                         PNBPAGE pPage,
                         PHWND   phwnd,
                         PVOID   pCreationParameters)
{
  ULONG ulPageId;
  BOOL  fSuccess = TRUE;
  HWND  hwndPage = NULLHANDLE;


  if (pPage == NULL)                                 /* ParameterÅberprÅfung */
    return (hwndPage);                                            /* failed !*/

  /* Insert a page into the notebook and store it in the array of page data. */
  /* Specify that it is to have status text and the window associated with   */
  /* each page will be automatically sized by the notebook according to the  */
  /* size of the page.                                                       */

  ulPageId = (ULONG) WinSendMsg( hwndNB,
                                 BKM_INSERTPAGE,
                                 NULL,
                                 MPFROM2SHORT( pPage->usTabType |
                                               pPage->fsPageStyles,
                                               BKA_LAST ) );

    if( ulPageId )
    {
        /* Insert a pointer to this page's info into the space available     */
        /* in each page (its PAGE DATA that is available to the application).*/
        fSuccess = (BOOL) WinSendMsg( hwndNB, BKM_SETPAGEDATA,
                                      MPFROMLONG( ulPageId ),
                                      MPFROMP( pPage ) );

                                       /* Set the text into the status line. */
        if( fSuccess )
        {
            if( pPage->fsPageStyles & BKA_STATUSTEXTON )
            {
                fSuccess = (BOOL) WinSendMsg( hwndNB, BKM_SETSTATUSLINETEXT,
                                  MPFROMP( ulPageId ),
                                  MPFROMP( pPage->szStatusLineText ) );
                if( !fSuccess )                              /* check errors */
                  ErrorMsg("BKM_SETSTATUSLINETEXT RC(%X)",
                           HWNDERR(hwndNB) );

            }
        }
        else
          ErrorMsg("BKM_SETPAGEDATA RC(%X)",
                   HWNDERR(hwndNB) );

        // Set the text into the tab for this page.
        if( fSuccess )
        {
          fSuccess = (BOOL) WinSendMsg(hwndNB,
                                       BKM_SETTABTEXT,
                                       MPFROMP(ulPageId),
                                       MPFROMP(pPage->szTabText) );
            if(!fSuccess)                                    /* check errors */
              ErrorMsg("BKM_SETTABTEXT RC(%X)",
                       HWNDERR( hwndNB ) );
        }


        // Create the dialog
        if (fSuccess)
        {
          hwndPage = (HWND) WinSendMsg( hwndNB, BKM_QUERYPAGEWINDOWHWND,
                                        MPFROMLONG( ulPageId ), NULL );
          if( hwndPage == (HWND) BOOKERR_INVALID_PARAMETERS )
          {
            hwndPage = NULLHANDLE;
            ErrorMsg( "NBSetUpPage BKM_QUERYPAGEWINDOWHWND Invalid page specified" );
          }
          else
            if( !hwndPage )
            {
              /* load dialog from resource */
              hwndPage = NBCreateDialogPage(hwndClient,
                                            hwndNB,
                                            hModule,
                                            ulPageId,
                                            pPage->ulID,
                                            pPage->wpDlgProc,
                                            pCreationParameters);
              if (phwnd != NULL)
                *phwnd = hwndPage;            /* RÅckgabeHWND liefern */

              if (hwndPage == NULLHANDLE)
              {
                fSuccess = FALSE;
                ErrorMsg( "pPage-pfncreate failed. RC(%X)", HWNDERR( hwndNB ) );
              }
            }
        }

    }
    else
        ErrorMsg( "BKM_INSERTPAGE RC(%X)", HWNDERR( hwndNB ) );

    return (fSuccess);
}


/**********************************************************************/
/*---------------------------- SetFramePos ---------------------------*/
/*                                                                    */
/*  SET THE FRAME ORIGIN AND SIZE.                                    */
/*                                                                    */
/*  INPUT: frame window handle                                        */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static BOOL NBSetFramePos( HWND hwndFrame )
{
    BOOL   fSuccess;
    POINTL aptl[ 2 ];

    // Convert origin and size from dialog units to pixels. We need to do this
    // because dialog boxes are automatically sized to the display. Since the
    // notebook contains these dialogs it must size itself accordingly so the
    // dialogs fit in the notebook.

/* @@@PH ??? */
#define FRAME_X               4    // In dialog units!
#define FRAME_Y               4    // In dialog units!
#define FRAME_CX              275  // In dialog units!
#define FRAME_CY              210  // In dialog units!

    aptl[ 0 ].x = FRAME_X;
    aptl[ 0 ].y = FRAME_Y;
    aptl[ 1 ].x = FRAME_CX;
    aptl[ 1 ].y = FRAME_CY;

    fSuccess = WinMapDlgPoints(HWND_DESKTOP,
                               aptl,
                               2,
                               TRUE );
    if( fSuccess )
    {
      fSuccess = WinSetWindowPos(hwndFrame,
                                 NULLHANDLE,
                                 aptl[ 0 ].x,
                                 aptl[ 0 ].y,
                                 aptl[ 1 ].x,
                                 aptl[ 1 ].y,
                                 SWP_SIZE |
                                 SWP_MOVE |
                                 SWP_SHOW |
                                 SWP_ACTIVATE );
      if( !fSuccess )
        ErrorMsg( "SetFramePos WinSetWindowPos RC(%X)",
                 HWNDERR( hwndFrame ) );
    }
    else
      ErrorMsg( "WinMapDlgPoints RC(%X)",
               HWNDERR( hwndFrame ) );

    return fSuccess;
}


/**********************************************************************/
/*------------------------- TurnToFirstPage --------------------------*/
/*                                                                    */
/*  TURN TO THE FIRST PAGE IN THE NOTEBOOK.                           */
/*                                                                    */
/*  INPUT: notebook window handle                                     */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static BOOL NBTurnToFirstPage( HWND hwndNB )
{
    ULONG ulFirstPage = 0;
    BOOL  fSuccess    = TRUE;

    ulFirstPage = (ULONG) WinSendMsg( hwndNB,
                                      BKM_QUERYPAGEID,
                                      NULL,
                                      (MPARAM)BKA_FIRST );
    if( ulFirstPage )
    {
      fSuccess = (ULONG) WinSendMsg(hwndNB,
                                    BKM_TURNTOPAGE,
                                    MPFROMLONG(ulFirstPage ),
                                    NULL );
        if( !fSuccess )
            ErrorMsg( "TurnToFirstPage BKM_TURNTOPAGE RC(%X)", HWNDERR( hwndNB ) );

    }
    else
    {
        fSuccess = FALSE;
        ErrorMsg( "TurnToFirstPage BKM_QUERYPAGEID RC(%X)", HWNDERR( hwndNB ) );
    }

    return fSuccess;
}


/**********************************************************************/
/*-------------------------- SetTabDimensions ------------------------*/
/*                                                                    */
/*  SET THE DIMENSIONS OF THE NOTEBOOK TABS.                          */
/*                                                                    */
/*  INPUT: window handle of notebook control                          */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static BOOL NBSetTabDimensions(HWND hwndNB)
{
  BOOL         fSuccess = TRUE;           /* success status of this function */
  HPS          hps      = WinGetPS( hwndNB );   /* presentation space handle */
  FONTMETRICS  fm;                                /* data about current font */
  INT          iSize,                                   /* width of tab text */
               iLongestText = 0;
  BOOKTEXT     Booktext;                  /* booktext structure for tab text */
  ULONG        ulPageId;                              /* id of notebook page */

  if( !hps )
  {
    ErrorMsg( "SetTabDimensions WinGetPS RC(%X)",
             HWNDERR( hwndNB ) );
    return FALSE;
  }

  (void) memset( &fm, 0, sizeof( FONTMETRICS ) );

  // Calculate the height of a tab as the height of an average font character
  // plus a margin value.

  if( GpiQueryFontMetrics( hps, sizeof( FONTMETRICS ), &fm ) )
      fm.lMaxBaselineExt += (TAB_HEIGHT_MARGIN * 2);
  else
  {
      fm.lMaxBaselineExt = DEFAULT_NB_TAB_HEIGHT + (TAB_HEIGHT_MARGIN * 2);

      ErrorMsg( "SetTabDimensions GpiQueryFontMetrics RC(%X)", HWNDERR( hwndNB ) );
    }

  // First setup the BOOKTEXT structure
  Booktext.pString = (PSZ)malloc(256);
  Booktext.textLen = 256;

  // disable window update
  WinEnableWindowUpdate (hwndNB,
                         FALSE);

  // Calculate the longest tab text for both the MAJOR and MINOR pages
  // this means to browse through all pages in the notebook and check
  // out the largest tab text size.
  ulPageId = (ULONG) WinSendMsg (hwndNB,
                                 BKM_QUERYPAGEID,
                                 NULL,
                                 (MPARAM)BKA_FIRST);
  while ( (ulPageId != NULLHANDLE) &&
          (ulPageId != (ULONG)BOOKERR_INVALID_PARAMETERS) )
  {
    // query pointer to tab text data
    WinSendMsg (hwndNB,
                BKM_QUERYTABTEXT,
                (MPARAM)ulPageId,
                (MPARAM)&Booktext);

    // determine largest tab text size
    iSize = WinGetStringSize(hps,
                             Booktext.pString );
    if( iSize > iLongestText )
        iLongestText = iSize;

    ulPageId = (ULONG) WinSendMsg (hwndNB,  /* query the handle for the next */
                                   BKM_QUERYPAGEID,         /* notebook page */
                                   MPFROMLONG( ulPageId ),
                                   (MPARAM)BKA_NEXT);
  }


  free (Booktext.pString);                       /* free allocated resources */
  WinReleasePS( hps );

  // Add a margin amount to the longest tab text

  if( iLongestText )
      iLongestText += TAB_WIDTH_MARGIN;

  // Set the tab dimensions for the MAJOR and MINOR pages. Note that the
  // docs as of this writing say to use BKA_MAJOR and BKA_MINOR in mp2 but
  // you really need BKA_MAJORTAB and BKA_MINORTAB.

  if( iLongestText )
  {
    fSuccess = (BOOL) WinSendMsg(hwndNB,
                                 BKM_SETDIMENSIONS,
                                 MPFROM2SHORT(iLongestText,
                                              (SHORT)fm.lMaxBaselineExt ),
                                 MPFROMSHORT( BKA_MAJORTAB ) );
      if( !fSuccess )
        ErrorMsg("BKM_SETDIMENSIONS(MAJOR) RC(%X)",
                 HWNDERR( hwndNB ) );
  }

  // enable window update
  WinEnableWindowUpdate (hwndNB,
                         TRUE);

  return fSuccess;
}


/**********************************************************************/
/*---------------------------- SetNBPage -----------------------------*/
/*                                                                    */
/*  SET THE TOP PAGE IN THE NOTEBOOK CONTROL.                         */
/*                                                                    */
/*  INPUT: client window handle,                                      */
/*         pointer to the PAGESELECTNOTIFY struct                     */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static VOID NBSetNBPage(HWND              hwndClient,
                        HMODULE           hModule,
                        PPAGESELECTNOTIFY ppsn,
                        PVOID             pCreationParameters)
{
  HWND hwndPage;

    // Get a pointer to the page information that is associated with this page.
    // It was stored in the page's PAGE DATA in the SetUpPage function.
  PNBPAGE pnbp = (PNBPAGE) WinSendMsg(ppsn->hwndBook,
                                      BKM_QUERYPAGEDATA,
                                      MPFROMLONG(ppsn->ulPageIdNew ),
                                      NULL );

    if( !pnbp )
      return;
    else
      if( (MRESULT)pnbp == (MRESULT)BOOKERR_INVALID_PARAMETERS )
      {
        ErrorMsg( "SetNBPage BKM_QUERYPAGEDATA Invalid page id" );
        return;
      }

    // If this is a BKA_MAJOR page and it is what this app terms a 'parent'
    // page, that means when the user selects this page we actually want to go
    // to its first MINOR page. So in effect the MAJOR page is just a dummy page
    // that has a tab that acts as a placeholder for its MINOR pages. If the
    // user is using the left arrow to scroll thru the pages and they hit this
    // dummy MAJOR page, that means they have already been to its MINOR pages in
    // reverse order. They would now expect to see the page before the dummy
    // MAJOR page, so we skip the dummy page. Otherwise the user is going the
    // other way and wants to see the first MINOR page associated with this
    // 'parent' page so we skip the dummy page and show its first MINOR page.

    if( pnbp->fParent )
    {
      ULONG ulPageFwd, ulPageNew;

      ulPageFwd = (ULONG) WinSendMsg(ppsn->hwndBook,
                                     BKM_QUERYPAGEID,
                                     MPFROMLONG( ppsn->ulPageIdNew ),
                                     MPFROM2SHORT(BKA_NEXT,
                                                  BKA_MINOR ) );

        // If this is true, the user is going in reverse order
        if( ulPageFwd == ppsn->ulPageIdCur )
          ulPageNew = (ULONG) WinSendMsg(ppsn->hwndBook,
                                         BKM_QUERYPAGEID,
                                         MPFROMLONG(ppsn->ulPageIdNew ),
                                         MPFROM2SHORT(BKA_PREV,
                                                      BKA_MAJOR) );
        else
            ulPageNew = ulPageFwd;
        if( ulPageNew == (ULONG) BOOKERR_INVALID_PARAMETERS )
            ErrorMsg( "SetNBPage BKM_QUERYPAGEID Invalid page specified" );
        else
          if( ulPageNew )
            if( !WinSendMsg(ppsn->hwndBook,
                            BKM_TURNTOPAGE,
                            MPFROMLONG( ulPageNew ),
                            NULL ) )
              ErrorMsg("BKM_TURNTOPAGE RC(%X)",
                       HWNDERR( ppsn->hwndBook ) );
    }
    else
    {
      hwndPage = (HWND) WinSendMsg(ppsn->hwndBook,
                                   BKM_QUERYPAGEWINDOWHWND,
                                   MPFROMLONG(ppsn->ulPageIdNew ),
                                   NULL );
      if( hwndPage == (HWND) BOOKERR_INVALID_PARAMETERS )
      {
          hwndPage = NULLHANDLE;
          ErrorMsg( "SetNBPage BKM_QUERYPAGEWINDOWHWND Invalid page specified" );
      }
      else
        if( !hwndPage )

            // It is time to load this dialog because the user has flipped pages
            // to a page that hasn't yet had the dialog associated with it.
          hwndPage = NBCreateDialogPage(hwndClient,
                                        ppsn->hwndBook,
                                        hModule,
                                        ppsn->ulPageIdNew,
                                        pnbp->ulID,
                                        pnbp->wpDlgProc,
                                        pCreationParameters);
    }

    // Set focus to the first control in the dialog. This is not automatically
    // done by the notebook.
    if( !pnbp->fParent && hwndPage )
      if(!WinSetFocus(HWND_DESKTOP,
                      pnbp->idFocus ?
                      WinWindowFromID(hwndPage,
                                      pnbp->idFocus ) : hwndPage ) )
      {
            // Bug in 2.0! Developers left some debug code in there!
        USHORT usErr = HWNDERR( ppsn->hwndBook );
          ErrorMsg("SetNBPage WinSetFocus RC(%X)",
                   usErr );
      }

  return;
}


/**********************************************************************/
/*------------------------ CreateDialogPage --------------------------*/
/*                                                                    */
/*  LOAD DIALOG AND ASSOCIATE IT WITH A NOTEBOOK PAGE                 */
/*                                                                    */
/*  INPUT: window handle of parent and owner,                         */
/*         notebook window handle,                                    */
/*         notebook page ID                                           */
/*         dialog resource ID                                         */
/*         dialog window procedure                                    */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: client window handle                                      */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static HWND NBCreateDialogPage(HWND    hwndParent,
                               HWND    hwndNB,
                               HMODULE hModule,
                               ULONG   ulPageID,
                               ULONG   idDlg,
                               FNWP    fpDlg,
                               PVOID   pCreationParameters)
{
    HWND hwndDlg;


    if (fpDlg == NULL)                        /* ParameterÅberprÅfung */
       return (NULLHANDLE);                   /* Fehler signalisieren */

    hwndDlg = WinLoadDlg( hwndParent,
                          hwndParent,
                          fpDlg,
                          hModule,
                          idDlg,
                          pCreationParameters);

    if( hwndDlg )                           /* check for valid window handle */
    {
      if( !WinSendMsg(hwndNB,                         /* attach hwnd to page */
                      BKM_SETPAGEWINDOWHWND,
                      MPFROMLONG( ulPageID ),
                      MPFROMLONG( hwndDlg ) ) )
        {
            WinDestroyWindow( hwndDlg );         /* destroy if attach failed */
            hwndDlg = NULLHANDLE;
            ErrorMsg( "NBCreateDialogPage SETPAGEWINDOWHWND RC(%X)",
                 HWNDERR( hwndNB ) );
        }
    }
    else
        ErrorMsg( "NBCreateDialogPage WinLoadDlg RC(%X)", HWNDERR( hwndNB ) );

    return hwndDlg;
}


/*****************************************************************************
 * Name      : BOOL NBSetTabText
 * Funktion  : Setzt Tabtext einer Seite.
 * Parameter : HWND hwndNB, ULONG ulPageId, PSZ pszTabtext
 * Variablen :
 * Ergebnis  : Erfolgscode
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 *****************************************************************************/

static BOOL NBSetTabText (HWND  hwndNB,
                          ULONG ulPageId,
                          PSZ   pszTabtext)
{
  BOOL fSuccess;

  if (pszTabtext == NULL)                            /* ParameterÅberprÅfung */
    return (FALSE);

  fSuccess = (BOOL) WinSendMsg(hwndNB,
                               BKM_SETTABTEXT,
                               MPFROMP( ulPageId ),
                               MPFROMP( pszTabtext ) );
  if( !fSuccess )
    ErrorMsg("NBSetTabText::BKM_SETTABTEXT RC(%X)",
             HWNDERR( hwndNB ) );

  return (fSuccess);                           /* RÅckgabewert liefern */
} /* BOOL NBSetTabText */


/***********************************************************************
 * Name      : BOOL NBSetStatuslineText
 * Funktion  : Setzt Statuslinetext einer Seite.
 * Parameter : HWND hwndNB, ULONG ulPageId, PSZ pszStatuslinetext
 * Variablen :
 * Ergebnis  : Erfolgscode
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static BOOL NBSetStatuslineText (HWND  hwndNB,
                                 ULONG ulPageId,
                                 PSZ   pszStatuslinetext)
{
  BOOL fSuccess;

  if (pszStatuslinetext == NULL)               /* ParameterÅberprÅfung */
    return (FALSE);

  fSuccess = (BOOL) WinSendMsg( hwndNB, BKM_SETSTATUSLINETEXT,
                                  MPFROMP( ulPageId ),
                                  MPFROMP( pszStatuslinetext ) );
  if( !fSuccess )
      ErrorMsg( "BKM_SETSTATUSLINETEXT RC(%X)", HWNDERR( hwndNB ) );
  return (fSuccess);                           /* RÅckgabewert liefern */
} /* BOOL NBSetStatuslineText */


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage1
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage1(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
  switch (ulMessage)
  {
    /*************************************************************************
     * user messages                                                         *
     *************************************************************************/

    case UM_PROPERTY_UNDO:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      ConPropPage1Set(hwnd,                                    /* set values */
                      pOpt);

      return ( (MPARAM) FALSE);
    }


    case UM_PROPERTY_APPLY:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      pOpt->fTerminateAutomatically = WinQueryButtonCheckstate(hwnd,
                                                 CB_CONSOLE_CLOSEWINDOWONEXIT);
      pOpt->fInsertMode             = WinQueryButtonCheckstate(hwnd,
                                                        CB_CONSOLE_INSERTMODE);
      pOpt->fQuickInsert            = WinQueryButtonCheckstate(hwnd,
                                                       CB_CONSOLE_QUICKINSERT);
      pOpt->fToolbarActive          = WinQueryButtonCheckstate(hwnd,
                                                           CB_CONSOLE_TOOLBAR);

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_TAB,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulTabSize),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE)
                        );

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_CURSORBLINK,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ucCursorDivisor),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE)
                       );

      return ( (MPARAM) FALSE);
    }
  }


  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage2
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage2(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
  switch (ulMessage)
  {
    case WM_COMMAND:
    {
      PICONSOLEOPTIONS pConsoleOptions;      /* the console options structure */
      HWND            hwndNotebook = WinQueryWindow(WinQueryWindow(hwnd,
                                                                   QW_PARENT),
                                                    QW_PARENT);

      pConsoleOptions = (PICONSOLEOPTIONS)          /* query pointer from wnd */
                        WinQueryWindowULong(hwndNotebook,
                                            QWL_USER);

      switch (SHORT1FROMMP(mp1))
      {
        case ID_BTN_SPEAKERTEST:
          /* test speaker */
          DosBeep(pConsoleOptions->ulSpeakerFrequency,
                  pConsoleOptions->ulSpeakerDuration);

          return ( (MPARAM) FALSE);
      }
    }


    /*************************************************************************
     * user messages                                                         *
     *************************************************************************/

    case UM_PROPERTY_UNDO:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      ConPropPage2Set(hwnd,                                    /* set values */
                      pOpt);

      return ( (MPARAM) FALSE);
    }


    case UM_PROPERTY_APPLY:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      pOpt->fSpeakerEnabled    = WinQueryButtonCheckstate(hwnd,
                                                          CB_CONSOLE_SPEAKERENABLE);
      pOpt->ulSpeakerDuration  = (ULONG)WinSendDlgItemMsg(hwnd,
                                                          SLB_CONSOLE_SPEAKERDURATION,
                                                          SLM_QUERYSLIDERINFO,
                                                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                                                       SMA_INCREMENTVALUE),
                                                          NULL);

      pOpt->ulSpeakerFrequency = (ULONG)WinSendDlgItemMsg(hwnd,
                                                          SLB_CONSOLE_SPEAKERFREQUENCY,
                                                          SLM_QUERYSLIDERINFO,
                                                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                                                       SMA_INCREMENTVALUE),
                                                          NULL);

      return ( (MPARAM) FALSE);
    }
  }


  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage3
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage3(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
  switch (ulMessage)
  {
    /*************************************************************************
     * user messages                                                         *
     *************************************************************************/

    case UM_PROPERTY_UNDO:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      ConPropPage3Set(hwnd,                                    /* set values */
                      pOpt);

      return ( (MPARAM) FALSE);
    }


    case UM_PROPERTY_APPLY:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

//      pOpt->fTerminateAutomatically = WinQueryButtonCheckstate(hwnd,
//                                                 CB_CONSOLE_CLOSEWINDOWONEXIT);

      return ( (MPARAM) FALSE);
    }
  }


  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage4
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage4(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
  switch (ulMessage)
  {
    /*************************************************************************
     * user messages                                                         *
     *************************************************************************/

    case UM_PROPERTY_UNDO:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      ConPropPage4Set(hwnd,                                    /* set values */
                      pOpt);

      return ( (MPARAM) FALSE);
    }


    case UM_PROPERTY_APPLY:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_X,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordDefaultPosition.X),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_Y,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordDefaultPosition.Y),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_DEFAULTWIDTH,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordDefaultSize.X),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_DEFAULTHEIGHT,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordDefaultSize.Y),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_BUFFERWIDTH,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordBufferSize.X),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_BUFFERHEIGHT,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->coordBufferSize.Y),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      return ( (MPARAM) FALSE);
    }
  }


  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage5
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage5(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
  switch (ulMessage)
  {
    /*************************************************************************
     * user messages                                                         *
     *************************************************************************/

    case UM_PROPERTY_UNDO:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      ConPropPage5Set(hwnd,                                    /* set values */
                      pOpt);

      return ( (MPARAM) FALSE);
    }


    case UM_PROPERTY_APPLY:
    {
      PICONSOLEOPTIONS pOpt = (PICONSOLEOPTIONS)mp1;            /* get pointer */

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_UPDATELIMIT,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulUpdateLimit),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_CONSOLEPRIORITY,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulConsoleThreadPriorityClass),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_CONSOLEPRIODELTA,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulConsoleThreadPriorityDelta),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_APPPRIORITY,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulAppThreadPriorityClass),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      WinSendDlgItemMsg(hwnd,                             /* query the value */
                        SPN_CONSOLE_APPPRIODELTA,
                        SPBM_QUERYVALUE,
                        MPFROMP(&pOpt->ulAppThreadPriorityDelta),
                        MPFROM2SHORT(0,SPBQ_ALWAYSUPDATE));

      return ( (MPARAM) FALSE);
    }
  }


  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/***********************************************************************
 * Name      : status MRESULT EXPENTRY wpDlgProcPage6
 * Funktion  : dialog window procedure
 * Parameter : HWND   hwnd
 *             ULONG  ulMessage
 *             MPARAM mp1
 *             MPARAM mp2
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [, 24.09.1995 02.57.00]
 ***********************************************************************/

static MRESULT EXPENTRY wpDlgProcPage6(HWND   hwnd,
                                       ULONG  ulMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
{
      /* since this dialog has no options, we're running the default message */
      /* processing only                                                     */
      /* we'll display a logo bitmap here shortly @@@PH                      */

  return WinDefDlgProc(hwnd,                   /* default message processing */
                       ulMessage,
                       mp1,
                       mp2);
}


/**********************************************************************/
/*-------------------------- GetStringSize ---------------------------*/
/*                                                                    */
/*  GET THE SIZE IN PIXELS OF A STRING.                               */
/*                                                                    */
/*  INPUT: presentation space handle,                                 */
/*         notebook window handle,                                    */
/*         pointer to string                                          */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

static INT WinGetStringSize(HPS hps,
                            PSZ szString )
{
  POINTL aptl[ TXTBOX_COUNT ];

  if (szString == NULL)                              /* ParameterÅberprÅfung */
    return 0;                          /* Im Fehlerfall wird 0 zurÅckgegeben */


                           /* Get the size, in pixels, of the string passed. */
  if( !GpiQueryTextBox( hps,
         strlen( szString ),
         szString,
         TXTBOX_COUNT,
         aptl ) )
    return 0;                          /* Im Fehlerfall wird 0 zurÅckgegeben */
  else
    return aptl[ TXTBOX_CONCAT ].x;
}


/*****************************************************************************
 * Name      : static APIRET ConPropPage1Set
 * Funktion  : setup dialog elements for page 1
 * Parameter :
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/04/02 19:00]
 *****************************************************************************/

static APIRET ConPropPage1Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions)
{
  if (pConsoleOptions == NULL)                           /* check parameters */
    return (ERROR_INVALID_PARAMETER);               /* raise error condition */

  WinCheckButton(hwndDlg,                                /* auto-termination */
                 CB_CONSOLE_CLOSEWINDOWONEXIT,
                 pConsoleOptions->fTerminateAutomatically);

  WinCheckButton(hwndDlg,                                     /* insert mode */
                 CB_CONSOLE_INSERTMODE,
                 pConsoleOptions->fInsertMode);

  WinCheckButton(hwndDlg,                               /* quick insert mode */
                 CB_CONSOLE_QUICKINSERT,
                 pConsoleOptions->fQuickInsert);

  WinCheckButton(hwndDlg,                                 /* console toolbar */
                 CB_CONSOLE_TOOLBAR,
                 pConsoleOptions->fToolbarActive);

                       /* set spin limits for the SPN_CONSOLE_TAB spinbutton */
  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_TAB,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(80),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg (hwndDlg,                           /* set current value */
                     SPN_CONSOLE_TAB,
                     SPBM_SETCURRENTVALUE,
                     MPFROMLONG(pConsoleOptions->ulTabSize),
                     0);

               /* set spin limits for the SPN_CONSOLE_CURSORBLINK spinbutton */
  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_CURSORBLINK,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(100),
                    MPFROMSHORT(1));

  WinSendDlgItemMsg (hwndDlg,                           /* set current value */
                     SPN_CONSOLE_CURSORBLINK,
                     SPBM_SETCURRENTVALUE,
                     MPFROMLONG(pConsoleOptions->ucCursorDivisor),
                     0);

  return (NO_ERROR);                                                   /* OK */
}

/*****************************************************************************
 * Name      : static APIRET ConPropPage2Set
 * Funktion  : setup dialog elements for page 2
 * Parameter :
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Jochen Schaeuble [1998/04/29 23:20]
 *****************************************************************************/

static APIRET ConPropPage2Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions)
{
  int       i;
  WNDPARAMS wp;
  SLDCDATA  slcd;

  if (pConsoleOptions == NULL)                           /* check parameters */
    return (ERROR_INVALID_PARAMETER);               /* raise error condition */

  WinCheckButton(hwndDlg,                                /* Enable speaker */
                 CB_CONSOLE_SPEAKERENABLE,
                 pConsoleOptions->fSpeakerEnabled);

  /* frequency slider */
  slcd.cbSize = sizeof(SLDCDATA);
  wp.pCtlData = &slcd;

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERFREQUENCY,
                    WM_QUERYWINDOWPARAMS,
                    (MPARAM)&wp,
                    NULL);

  slcd.usScale1Increments=9;
  slcd.usScale1Spacing=0;

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERFREQUENCY,
                    WM_SETWINDOWPARAMS,
                    (MPARAM)&wp,
                    NULL);

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERFREQUENCY,
                    SLM_SETTICKSIZE,
                    MPFROM2SHORT(SMA_SETALLTICKS, 40),
                    NULL);

  /* duration slider */
  slcd.cbSize = sizeof(SLDCDATA);
  wp.pCtlData = &slcd;

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERDURATION,
                    WM_QUERYWINDOWPARAMS,
                    (MPARAM)&wp,
                    NULL);

  slcd.usScale1Increments=9;
  slcd.usScale1Spacing=0;

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERDURATION,
                    WM_SETWINDOWPARAMS,
                    (MPARAM)&wp,
                    NULL);

  WinSendDlgItemMsg(hwndDlg,
                    SLB_CONSOLE_SPEAKERDURATION,
                    SLM_SETTICKSIZE,
                    MPFROM2SHORT(SMA_SETALLTICKS, 40),
                    NULL);


  return (NO_ERROR);                                                   /* OK */
}

/*****************************************************************************
 * Name      : static APIRET ConPropPage3Set
 * Funktion  : setup dialog elements for page 3
 * Parameter :
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Jochen Schaeuble [1998/04/29 23:20]
 *****************************************************************************/

static APIRET ConPropPage3Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions)
{
  if (pConsoleOptions == NULL)                           /* check parameters */
    return (ERROR_INVALID_PARAMETER);               /* raise error condition */

  /* setup check-button */


  WinCheckButton(hwndDlg,                                /* Override default */
                 CB_CONSOLE_WINDOWPOSITION,
                 pConsoleOptions->fSetWindowPosition);

  /* setup limits for spin-buttons */

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_X,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN)-1),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_Y,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN)-1),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_DEFAULTWIDTH,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN)-1),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_DEFAULTHEIGHT,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN)-1),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_BUFFERWIDTH,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(9999),
                    MPFROMSHORT(0));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_BUFFERHEIGHT,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(9999),
                    MPFROMSHORT(0));


  /* setup active values */

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_X,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordDefaultPosition.X),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_Y,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordDefaultPosition.Y),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_DEFAULTWIDTH,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordDefaultSize.X),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_DEFAULTHEIGHT,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordDefaultSize.Y),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_BUFFERWIDTH,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordBufferSize.X),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_BUFFERHEIGHT,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->coordBufferSize.Y),
                    0);


  return (NO_ERROR);                                                   /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConPropPage4Set
 * Funktion  : setup dialog elements for page 4
 * Parameter :
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

static APIRET ConPropPage4Set(HWND            hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions)
{
  ULONG ulColor;                              /* loop counter for the colors */
  ULONG ulRow;
  ULONG ulColumn;

#define MAX_TRANSCOLORS 16
  static const ULONG TabColorTranslation[MAX_TRANSCOLORS] =
  {          /*  RRGGBB */
    /* 1  */ 0x00000000,
    /* 2  */ 0x0000007f,
    /* 3  */ 0x00007f00,
    /* 4  */ 0x007f0000,
    /* 5  */ 0x00007f7f,
    /* 6  */ 0x007f007f,
    /* 7  */ 0x007f7f00,
    /* 8  */ 0x007f7f7f,
    /* 9  */ 0x00808080,
    /* 10 */ 0x008080ff,
    /* 11 */ 0x0080ff80,
    /* 12 */ 0x00ff8080,
    /* 13 */ 0x0080ffff,
    /* 14 */ 0x00ff80ff,
    /* 15 */ 0x00ffff80,
    /* 16 */ 0x00ffffff
  };


  if (pConsoleOptions == NULL)                           /* check parameters */
    return (ERROR_INVALID_PARAMETER);               /* raise error condition */

  /* setup the value sets */
  for (ulColor = 0;
       ulColor < MAX_TRANSCOLORS;
       ulColor++)
  {
    ulRow    = 1 + (ulColor & 0x01);                /* calculate field index */
    ulColumn = 1 + (ulColor >> 1);

                                                               /* foreground */
    WinSendDlgItemMsg (hwndDlg,
                       VS_CONSOLE_FOREGROUND,
                       VM_SETITEMATTR,
                       MPFROM2SHORT(ulRow, ulColumn),
                       MPFROM2SHORT(VIA_RGB, TRUE) );

    WinSendDlgItemMsg (hwndDlg,
                       VS_CONSOLE_FOREGROUND,
                       VM_SETITEM,
                       MPFROM2SHORT(ulRow, ulColumn),
                       (MPARAM) TabColorTranslation[ulColor] );

                                                               /* background */
    WinSendDlgItemMsg (hwndDlg,
                       VS_CONSOLE_BACKGROUND,
                       VM_SETITEMATTR,
                       MPFROM2SHORT(ulRow, ulColumn),
                       MPFROM2SHORT(VIA_RGB, TRUE) );

    WinSendDlgItemMsg (hwndDlg,
                       VS_CONSOLE_BACKGROUND,
                       VM_SETITEM,
                       MPFROM2SHORT(ulRow, ulColumn),
                       (MPARAM) TabColorTranslation[ulColor] );
  }


  /* @@@PH fonts */

  return (NO_ERROR);                                                   /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConPropPage5Set
 * Funktion  : setup dialog elements for page 5
 * Parameter :
 * Variablen :
 * Ergebnis  : MRESULT
 * Bemerkung :
 *
 * Autor     : Jochen Schaeuble [1998/04/29 23:20]
 *****************************************************************************/

static APIRET ConPropPage5Set(HWND             hwndDlg,
                              PICONSOLEOPTIONS pConsoleOptions)
{
  if (pConsoleOptions == NULL)                           /* check parameters */
    return (ERROR_INVALID_PARAMETER);               /* raise error condition */
//   #define PRTYC_NOCHANGE     0
//   #define PRTYC_IDLETIME     1
//   #define PRTYC_REGULAR      2
//   #define PRTYC_TIMECRITICAL 3
//   #define PRTYC_FOREGROUNDSERVER 4

//   /* Priority deltas */

//   #define PRTYD_MINIMUM     -31
//   #define PRTYD_MAXIMUM      31
  /* setup application-priorities */

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_APPPRIORITY,
                    SPBM_SETARRAY,
                    priorities,
                    MPFROMLONG(NUMPRIORITIES));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_APPPRIORITY,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->ulConsoleThreadPriorityClass-1),
                    NULL);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_APPPRIODELTA,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(PRTYD_MAXIMUM),
                    MPFROMSHORT(0));


  WinSendDlgItemMsg(hwndDlg,                           /* set current value */
                    SPN_CONSOLE_APPPRIODELTA,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(0),
                    0);

  /* setup console-priorities */

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_CONSOLEPRIORITY,
                    SPBM_SETARRAY,
                    priorities,
                    MPFROMLONG(NUMPRIORITIES));

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_CONSOLEPRIORITY,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(pConsoleOptions->ulConsoleThreadPriorityClass-1),
                    NULL);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_CONSOLEPRIODELTA,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(PRTYD_MAXIMUM),
                    MPFROMSHORT(0));


  WinSendDlgItemMsg(hwndDlg,                           /* set current value */
                    SPN_CONSOLE_CONSOLEPRIODELTA,
                    SPBM_SETCURRENTVALUE,
                    MPFROMLONG(0),
                    0);

  WinSendDlgItemMsg(hwndDlg,
                    SPN_CONSOLE_UPDATELIMIT,
                    SPBM_SETLIMITS,
                    MPFROMSHORT(50),
                    MPFROMSHORT(0));

  return (NO_ERROR);                                                   /* OK */
}


/*****************************************************************************
 * Name      : APIRET EXPENTRY ConsolePropertyApply
 * Funktion  : save properties from registry
 * Parameter : PICONSOLEOPTIONS pConsoleOptions
 * Variablen :
 * Ergebnis  : APIRET
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

DWORD ConsolePropertyApply(PICONSOLEOPTIONS pConsoleOptions)
{
  dprintf (("KERNEL32: Console:ConsolePropertyApply(%08xh) not implemented.\n",
            pConsoleOptions));

  return (NO_ERROR);
}


#if 0
DLGTEMPLATE DLG_CONSOLE_PAGE3
   CONTROL "Sample", SPN_CONSOLE_X, 49, 60, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_Y, 49, 45, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_DEFAULTWIDTH, 155, 60, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_DEFAULTHEIGHT, 155, 45, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "~Override default position and size", CB_CONSOLE_WINDOWPOSITION, 8, 80, 155, 10, WC_BUTTON, BS_AUTOCHECKBOX | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_BUFFERWIDTH, 49, 19, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_BUFFERHEIGHT, 155, 19, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Buffer allocates .. MB memory.", ST_CONSOLE_BUFFERMEMORY, 8, 6, 194, 8, WC_STATIC, SS_TEXT | DT_LEFT | DT_TOP | DT_MNEMONIC | WS_VISIBLE | WS_GROUP

DLGTEMPLATE DLG_CONSOLE_PAGE5
   CONTROL "Sample", SPN_CONSOLE_APPPRIORITY, 46, 52, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_READONLY | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_APPPRIODELTA, 154, 52, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_ALLCHARACTERS | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_CONSOLEPRIORITY, 46, 20, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_ALLCHARACTERS | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_CONSOLEPRIODELTA, 154, 20, 48, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_ALLCHARACTERS | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", SPN_CONSOLE_UPDATELIMIT, 113, 5, 89, 12, WC_SPINBUTTON, SPBS_MASTER | SPBS_ALLCHARACTERS | SPBS_JUSTLEFT | WS_VISIBLE | WS_GROUP | WS_TABSTOP

DLGTEMPLATE DLG_CONSOLE_PAGE4
   CONTROL "...", VS_CONSOLE_FOREGROUND, 6, 56, 95, 34, WC_VALUESET, VS_RGB | VS_BORDER | VS_ITEMBORDER | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "...", VS_CONSOLE_BACKGROUND, 103, 56, 99, 34, WC_VALUESET, VS_RGB | VS_BORDER | VS_ITEMBORDER | WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "Sample", LB_CONSOLE_FONTS, 6, 4, 95, 36, WC_LISTBOX, WS_VISIBLE | WS_GROUP | WS_TABSTOP
   CONTROL "00 x 00", ST_CONSOLE_WINDOWSIZE, 156, 16, 47, 8, WC_STATIC, SS_TEXT | DT_LEFT | DT_TOP | DT_MNEMONIC | WS_VISIBLE | WS_GROUP
   CONTROL "00 x 00", ST_CONSOLE_SCREENSIZE, 156, 4, 47, 8, WC_STATIC, SS_TEXT | DT_LEFT | DT_TOP | DT_MNEMONIC | WS_VISIBLE | WS_GROUP

#endif

