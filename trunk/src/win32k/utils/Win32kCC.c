/* $Id: Win32kCC.c,v 1.16 2001-10-04 12:30:29 bird Exp $
 *
 * Win32CC - Win32k Control Center.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * Private Window Messages
 */
#define  WM_SETCONTROLS         (WM_USER + 10)
#define  WM_QUERYCONTROLS       (WM_USER + 11)


/*
 * Notebook page constants.
 */
#define W32KCCPG_STATUS         0
#define W32KCCPG_LOADERS        1
#define W32KCCPG_LOGGING        2
#define W32KCCPG_HEAPS          3
#define W32KCCPG_LDRFIX         4
#define W32KCCPG_MEMINFO        5
#define W32KCCPG_PAGES          (W32KCCPG_MEMINFO+1)


/*
 * Include defines
 */
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSMISC

#define INCL_WINERRORS
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#define INCL_WINSTDSPIN
#define INCL_WINBUTTONS
#define INCL_WINWINDOWMGR
#define INCL_WINSTDBOOK
#define INCL_WINSYS
#define INCL_WINTIMER

#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR

#define INCL_GPIPRIMITIVES
#define INCL_GPILCIDS



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#include <Win32k.h>
#include <devSegDf.h>                   /* Win32k segment definitions. */
#include <Options.h>

#include "Win32kCC.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _Win32kCCPage
{
    ULONG   ulId;
    HWND    hwnd;
} WIN32KCCPAGE, *PWIN32KCCPAGE;

typedef struct _Win32kCC
{
    HWND            hwnd;
    HWND            hwndNtbk;
    HAB             hab;
    BOOL            fDirty;

    K32OPTIONS      Options;
    K32OPTIONS      NewOptions;
    K32STATUS       Status;

    ULONG           idMemTimer;         /* The Timer ID of the MemInfo Refresh Timer. */
    K32SYSTEMMEMINFO MemInfo;           /* Current displayed meminfo. */

    WIN32KCCPAGE    aPages[W32KCCPG_PAGES]; /* Array containing generic page info. */

} WIN32KCC, *PWIN32KCC;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL    fNotExit;                       /* Global variable used to stop WM_QUITS.
                                         * As long as this is true the message
                                         * loop will never exit, but ignore all
                                         * WM_QUITs.
                                         */
BOOL    fOldNtbk;                       /* Set if we must use the old notebook
                                         * style.
                                         */

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
MRESULT EXPENTRY    Win32kCCDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY    LoadersDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    LoggingDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    StatusDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    HeapsDlgProc    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    LdrFixDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    MemInfoDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY    NtbkDefPageDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

BOOL                SetDlgItemTextF(HWND hwndDlg, ULONG idItem, PSZ pszFormat, ...);
ULONG               ShowMessage(HWND hwndOwner, int id, ULONG flStyle);
BOOL                Complain(HWND hwndOwner, int id, ...);
PCSZ                getLastErrorMsg(HAB hab);
PSZ                 getMessage(ULONG id);
int                 GetFixpackDesc(ULONG ulBuild, ULONG flKernel, PSZ pszBuffer);
char *              stristr(const char *pszStr, const char *pszSubStr);



/**
 * Main function.
 * (No parameters)
 * @returns     -1  WinInitialize failed.
 *              -2  Failed to create message queue.
 *              -3  Failed to load dialog.
 *              0   Dialog was closed using cancel.
 *              1   Dialog was close using ok.
 */
int main(void)
{
    HAB     hab;
    HMQ     hmq;
    ULONG   rc = 0;
    HWND    hwnd;
    ULONG   aulVer[2];

    /*
     * Initialize PM.
     */
    hab = WinInitialize(0);
    if (hab == NULLHANDLE)
    {
        return -1;
    }

    /*
     * Create Message Queue.
     */
    hmq = WinCreateMsgQueue(hab, 0);
    if (hmq == NULLHANDLE)
    {
        WinTerminate(hab);
        return -2;
    }

    /*
     * Init Win32k library.
     */
    rc = libWin32kInit();
    if (rc != NO_ERROR)
    {
        switch (rc)
        {
            case ERROR_FILE_NOT_FOUND:
                Complain(HWND_DESKTOP, IDS_ERR_WIN32K_NOT_LOADED);
                break;

            default:
                Complain(HWND_DESKTOP, IDS_ERR_WIN32K_OPEN_FAILED, rc);
        }
    }

    /*
     * Check version and set fOldNtbk accordingly.
     */
    fOldNtbk = (DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR, &aulVer, sizeof(aulVer))
                || (aulVer[1] <= 20 && aulVer[0] < 40));

    /*
     * Load the dialog.
     */
    hwnd = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP,
                      Win32kCCDlgProc,
                      NULLHANDLE,
                      fOldNtbk ? DL_WIN32KCC_OLD : DL_WIN32KCC,
                      NULL);

    /*
     * Process the dialog.
     */
    if (hwnd != NULLHANDLE)
    {
        QMSG qmsg;
        do
        {
            fNotExit = FALSE;
            while(WinGetMsg(hab, &qmsg, NULLHANDLE, NULLHANDLE, NULLHANDLE))
                WinDispatchMsg(hab, &qmsg);
        } while (fNotExit);


    }
    else
    {
        Complain(HWND_DESKTOP,
                 IDS_ERR_DIALOGLOAD,
                 DL_WIN32KCC,
                 WinGetLastError(hab),
                 getLastErrorMsg(hab)
                 );
        rc = -3;
    }

    /*
     * Cleanup.
     */
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    libWin32kTerm();

    return rc;
}

#pragma info(noord) /*remove annoying (and possible incorrect) messages on the MP* macros */

/**
 * Dialog procedure for the DL_WIN32KCC notebook dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY Win32kCCDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;
    int         i;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Initialize the controls and trigger a setcontrol event.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (NULL)
         */
        case WM_INITDLG:
        {
            static struct _NtbkPage
            {
                PFNWP   pfnDlgProc;
                ULONG   idDlg;
                ULONG   idPage;
            }   aPages[W32KCCPG_PAGES] =
            {
                { StatusDlgProc ,  PG_WIN32K_INFO_PAGE, W32KCCPG_STATUS },
                { LoadersDlgProc,  DL_LOADERS_PAGE,     W32KCCPG_LOADERS},
                { LoggingDlgProc,  DL_LOGGING_PAGE,     W32KCCPG_LOGGING},
                { HeapsDlgProc  ,  DL_HEAPS_PAGE,       W32KCCPG_HEAPS  },
                { LdrFixDlgProc ,  DL_LDRFIX_PAGE,      W32KCCPG_LDRFIX },
                { MemInfoDlgProc , DL_MEMINFO_PAGE,     W32KCCPG_MEMINFO }
            };
            CHAR            szTabText[128];
            PDLGTEMPLATE    pdlgt;
            APIRET          rc;
            RECTL           rectl;
            SWP             swp;
            SWP             swp2;

            /*
             * Init and set instance data.
             */
            pThis = calloc(sizeof(*pThis), 1);
            if (pThis == NULL)
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_MALLOC_FAILED, __FILE__, __LINE__, __FUNCTION__);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                return FALSE;
            }
            pThis->hwnd = hwnd;
            pThis->hab = WinQueryAnchorBlock(hwnd);
            pThis->hwndNtbk = WinWindowFromID(hwnd, DL_WIN32KCC_NTBK);
            if (!WinSetWindowPtr(hwnd, QWL_USER, pThis))
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_SET_INSTANCEDATA,
                         WinGetLastError(pThis->hab),
                         getLastErrorMsg(pThis->hab));
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                WinDismissDlg(hwnd, 0);
                free(pThis);
                return FALSE;
            }


            /*
             * Load and set accellerator table.
             */
            WinSetAccelTable(pThis->hab, WinLoadAccelTable(pThis->hab, NULLHANDLE, DL_WIN32KCC), hwnd);


            /*
             * Insert notebooks pages.
             */
            for (i = 0; i < W32KCCPG_PAGES; i++)
            {
                ULONG ulErrId = 0;
                ULONG iPage = aPages[i].idPage;

                rc = DosGetResource(NULLHANDLE, RT_DIALOG, aPages[i].idDlg, (PPVOID)(void*)&pdlgt);
                if (rc)
                {
                    Complain(hwnd, IDS_ERR_FAILED_TO_LOAD_DLGT, aPages[i].idDlg, rc);
                    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                    WinSendMsg(hwnd, WM_DESTROY, NULL, NULL);
                    return FALSE;
                }

                pThis->aPages[iPage].hwnd =
                    (HWND)WinLoadDlg(hwnd, HWND_DESKTOP, aPages[i].pfnDlgProc,
                                     NULLHANDLE, aPages[i].idDlg, pThis);
                if (pThis->aPages[iPage].hwnd != NULLHANDLE)
                {
                    /*
                     * Resize the notebook according to the first page.
                     *  Get the size of the page we're inserting
                     *  Calc notebook size according to that page.
                     *  Resize the notebook controll.
                     *  Resize the dialog.
                     *  Recalc page rectangle.
                     */
                    if (i == 0)
                    {
                        WinQueryWindowPos(pThis->aPages[iPage].hwnd, &swp);
                        rectl.xLeft = rectl.yBottom = 0;
                        rectl.xRight = swp.cx;
                        rectl.yTop = swp.cy;
                        WinSendMsg(pThis->hwndNtbk, BKM_CALCPAGERECT, &rectl, (MPARAM)FALSE);

                        WinQueryWindowPos(pThis->hwndNtbk, &swp);
                        WinSetWindowPos(pThis->hwndNtbk, NULLHANDLE,
                                        0, 0,
                                        rectl.xRight - rectl.xLeft,
                                        rectl.yTop - rectl.yBottom,
                                        SWP_SIZE);

                        WinQueryWindowPos(hwnd, &swp2);
                        WinSetWindowPos(hwnd, NULLHANDLE,
                                        0, 0,
                                        swp2.cx + (rectl.xRight - rectl.xLeft) - swp.cx,
                                        swp2.cy + (rectl.yTop - rectl.yBottom) - swp.cy,
                                        SWP_SIZE);
                    }

                    /*
                     * Insert page.
                     */
                    pThis->aPages[iPage].ulId =
                        (ULONG)WinSendMsg(pThis->hwndNtbk, BKM_INSERTPAGE, NULL,
                                          MPFROM2SHORT(BKA_MAJOR | BKA_AUTOPAGESIZE, BKA_LAST));
                    if (pThis->aPages[iPage].ulId != 0)
                    {
                        /*
                         * Place the dialog into the page.
                         */
                        if (WinSendMsg(pThis->hwndNtbk, BKM_SETPAGEWINDOWHWND,
                                       (MPARAM)pThis->aPages[iPage].ulId, (MPARAM)pThis->aPages[iPage].hwnd))
                        {
                            /*
                             * Set tab text - use the title of the dialog.
                             */
                            szTabText[0] = '\0';
                            if (   pdlgt != NULL && pdlgt->adlgti[0].cchText != 0
                                && pdlgt->adlgti[0].offText != 0xFFFF && pdlgt->adlgti[0].offText != 0)
                                strncat(szTabText, (char*)((unsigned)(pdlgt) + pdlgt->adlgti[0].offText), pdlgt->adlgti[0].cchText);
                            WinSendMsg(pThis->hwndNtbk, BKM_SETTABTEXT, (MPARAM)pThis->aPages[iPage].ulId, &szTabText[0]);
                        }
                        else
                            ulErrId = IDS_ERR_ADD_NTBK_PAGE_SET;
                    }
                    else
                        ulErrId = IDS_ERR_ADD_NTBK_PAGE_INSERT;
                }
                else
                    ulErrId = IDS_ERR_ADD_NTBK_PAGE_LOAD;

                /* Check for error */
                if (ulErrId)
                {
                    Complain(hwnd, ulErrId, aPages[i].idDlg,  WinGetLastError(pThis->hab), getLastErrorMsg(pThis->hab));
                    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                    WinSendMsg(hwnd, WM_DESTROY, NULL, NULL);
                    return FALSE;
                }
            }

            if (fOldNtbk)
            {
                POINTL      ptl;
                FONTMETRICS fm;

                /*
                 * If it's an old style dialog we'll have to resize the tabs.
                 *      Hackish!!! Seems like I don't do this right!
                 */
                for (i = 0, ptl.x = 7, ptl.y = 7; i < W32KCCPG_PAGES; i++)
                {
                    BOOKTEXT    booktxt = {&szTabText[0], sizeof(szTabText)};
                    if (pThis->aPages[i].hwnd == NULLHANDLE) continue;

                    if (WinSendMsg(pThis->hwndNtbk, BKM_QUERYTABTEXT, (MPARAM)pThis->aPages[i].ulId, &booktxt))
                    {
                        POINTL aptl[TXTBOX_COUNT];
                        if (GpiQueryTextBox(WinGetPS(pThis->hwndNtbk), strlen(booktxt.pString)+1, booktxt.pString, TXTBOX_COUNT, aptl))
                        {
                            if (ptl.x < abs(aptl[TXTBOX_BOTTOMRIGHT].x - aptl[TXTBOX_BOTTOMLEFT].x))
                                ptl.x = abs(aptl[TXTBOX_BOTTOMRIGHT].x - aptl[TXTBOX_BOTTOMLEFT].x);
                        }
                    }
                }
                if (GpiQueryFontMetrics(WinGetPS(pThis->hwndNtbk), sizeof(fm), &fm))
                    ptl.y = fm.lXHeight + fm.lEmHeight;


                /*
                 * Before we resize anything, we'll have to get the size of a page.
                 * Change the tab size. This may effect the notebook page size.
                 * Recalc new notebook size using old page size.
                 * Addjust dialog window.
                 * Addjust notebook control.
                 */
                WinQueryWindowPos(pThis->aPages[aPages[0].idPage].hwnd, &swp);

                WinSendMsg(pThis->hwndNtbk, BKM_SETDIMENSIONS, MPFROM2SHORT(ptl.x, ptl.y), (MPARAM)BKA_MAJORTAB);

                rectl.xLeft = swp.x;
                rectl.yBottom = swp.y;
                rectl.xRight = swp.cx - swp.x;
                rectl.yTop = swp.cy - swp.y;
                WinSendMsg(pThis->hwndNtbk, BKM_CALCPAGERECT, &rectl, (MPARAM)FALSE);
                WinQueryWindowPos(hwnd, &swp);
                WinQueryWindowPos(pThis->hwndNtbk, &swp2);
                WinSetWindowPos(hwnd, NULLHANDLE, 0, 0,
                                swp.cx + rectl.xRight - rectl.xLeft - swp2.cx,
                                swp.cy + rectl.yTop - rectl.yBottom - swp2.cy,
                                SWP_SIZE);
                WinSetWindowPos(pThis->hwndNtbk, NULLHANDLE, 0, 0,
                                rectl.xRight - rectl.xLeft,
                                rectl.yTop - rectl.yBottom,
                                SWP_SIZE);

                /*
                 * Set Status text background color to dialog background color.
                 */
                WinSendMsg(pThis->hwndNtbk, BKM_SETNOTEBOOKCOLORS,
                           (MPARAM)SYSCLR_DIALOGBACKGROUND,
                           (MPARAM)BKA_BACKGROUNDPAGECOLORINDEX);
            }


            /*
             * Send a set controls message which gets data from
             * win32k and puts it into the controls.
             */
            WinSendMsg(hwnd, WM_SETCONTROLS, NULL, NULL);
            break;
        }


        /*
         * The user wants me to do something...
         */
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                /*
                 * The user pushed the "Apply" button.
                 */
                case PB_APPLY:
                {
                    /* Check and get data from the dialog. */
                    if (WinSendMsg(hwnd, WM_QUERYCONTROLS, (MPARAM)TRUE, NULL)
                        && pThis->fDirty
                        )
                    {
                        APIRET rc;
                        rc = libWin32kSetOptions(&pThis->NewOptions);
                        if (rc != NO_ERROR)
                            Complain(hwnd, IDS_ERR_SETPOPTIONS, rc);
                        WinSendMsg(hwnd, WM_SETCONTROLS, NULL, NULL);
                    }
                }
                break;


                /*
                 * User pushed the "Refresh" button.
                 */
                case PB_REFRESH:
                    WinSendMsg(hwnd, WM_SETCONTROLS, NULL, NULL);
                break;


                /*
                 * The user pushed the "Close" button.
                 */
                case DID_OK:
                    /* Check if data is dirty */
                    if (!WinSendMsg(hwnd, WM_QUERYCONTROLS, (MPARAM)FALSE, NULL) || pThis->fDirty)
                    {
                        if (ShowMessage(hwnd, IDM_INFO_DIRTY, MB_YESNO | MB_WARNING) != MBID_YES)
                        {
                            fNotExit = TRUE;
                            return NULL;
                        }
                    }
                    /* Close the dialog */
                    fNotExit = FALSE;
                    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                    WinDismissDlg(hwnd, 0);
                    break;

                /*
                 * The use requested update of config.sys.
                 */
                case PB_UPD_CONFIGSYS:
                {
                    ULONG   ulBootDrv;
                    FILE *  phConfigSys;
                    char *  pszConfigSys = "A:\\Config.sys";
                    char    szArgs[120];
                    int     cchArgs;

                    if (!WinSendMsg(hwnd, WM_QUERYCONTROLS, (MPARAM)TRUE, NULL))
                        break;
                    if (DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrv, sizeof(ulBootDrv)))
                        break;

                    /*
                     * Make argument list.
                     */
                    szArgs[0] = '\0';
                    if (pThis->NewOptions.fLogging)             strcat(szArgs, " -L:Y");
                    if (pThis->NewOptions.usCom == 0x3f8)       strcat(szArgs, " -C1");
                    /*if (pThis->NewOptions.usCom != 0x2f8)       strcat(szArgs, " -C2"); - default */
                    if (pThis->NewOptions.usCom == 0x3e8)       strcat(szArgs, " -C3");
                    if (pThis->NewOptions.usCom == 0x2e8)       strcat(szArgs, " -C4");
                    if (pThis->NewOptions.fPE == FLAGS_PE_PE2LX)strcat(szArgs, " -P:pe2lx");
                    /*if (pThis->NewOptions.fPE == FLAGS_PE_MIXED)strcat(szArgs, " -P:mixed"); - old default */
                    if (pThis->NewOptions.fPE == FLAGS_PE_MIXED)strcat(szArgs, " -P:mixed");
                    /* if (pThis->NewOptions.fPE == FLAGS_PE_PE)   strcat(szArgs, " -P:pe"); - default */
                    if (pThis->NewOptions.fPE == FLAGS_PE_NOT)  strcat(szArgs, " -P:not");
                    if (pThis->NewOptions.ulInfoLevel != 0) /* -W0 is default */
                        sprintf(szArgs + strlen(szArgs), " -W%d", pThis->NewOptions.ulInfoLevel); /* FIXME - to be changed */
                    if (pThis->NewOptions.fElf)                 strcat(szArgs, " -E:Y"); /* default is disabled */
                    if (!pThis->NewOptions.fUNIXScript)         strcat(szArgs, " -Script:N");
                    if (!pThis->NewOptions.fREXXScript)         strcat(szArgs, " -Rexx:N");
                    if (!pThis->NewOptions.fJava)               strcat(szArgs, " -Java:N");
                    if (pThis->NewOptions.fNoLoader)            strcat(szArgs, " -Noloader");
                    if (!pThis->NewOptions.fDllFixes)           strcat(szArgs, " -DllFixes:D"); /* default is enabled */
                    if (pThis->NewOptions.fForcePreload)        strcat(szArgs, " -ForcePreload:Y"); /* default is disabled */
                    if (pThis->NewOptions.cbSwpHeapMax != CB_SWP_MAX)
                        sprintf(szArgs + strlen(szArgs), " -HeapMax:%d", pThis->NewOptions.cbSwpHeapMax); /* FIXME - to be changed */
                    if (pThis->NewOptions.cbResHeapMax != CB_RES_MAX)
                        sprintf(szArgs + strlen(szArgs), " -ResHeapMax:%d", pThis->NewOptions.cbResHeapMax); /* FIXME - to be changed */
                    strcat(szArgs, "\n");
                    cchArgs = strlen(szArgs);

                    /*
                     * Update Config.sys.
                     */
                    *pszConfigSys = (char)(ulBootDrv - 1 + 'A');
                    phConfigSys = fopen(pszConfigSys, "r+");
                    if (phConfigSys)
                    {
                        ULONG   cbConfigSys;
                        if (    fseek(phConfigSys, 0, SEEK_END) == 0
                            &&  (cbConfigSys = ftell(phConfigSys)) > 0
                            &&  fseek(phConfigSys, 0, SEEK_SET) == 0
                            )
                        {
                            char *  pszConfigSys;

                            pszConfigSys = (char*)calloc(1, 2 * (cbConfigSys + 256)); /* Paranoia... */
                            if (pszConfigSys)
                            {
                                char *pszCurrent = pszConfigSys;

                                /* Read and modify config.sys */
                                while (fgets(pszCurrent, cbConfigSys + pszCurrent - pszConfigSys, phConfigSys))
                                {
                                    char *pszWin32k;
                                    /* NB! This statment will not only update the "device=" statements!
                                     * We'll call this a feature...
                                     */
                                    pszWin32k = stristr(pszCurrent, "win32k.sys");
                                    if (pszWin32k)
                                    {
                                        int cch;
                                        pszWin32k += 10;  /* skip "win32k.sys" */
                                        cch = strlen(pszWin32k);
                                        strcpy(pszWin32k, szArgs);
                                        if (cchArgs < cch)
                                        { /* fix which stops us from shrinking the file.. */
                                            memset(pszWin32k + cchArgs - 1, ' ', cch - cchArgs);
                                            pszWin32k[cch - 1] = '\n';
                                            pszWin32k[cch]     = '\0';
                                        }
                                    }

                                    /* next */
                                    pszCurrent += strlen(pszCurrent);
                                }
                                if (pszCurrent[-1] != '\n')
                                    *pszCurrent++ = '\n';

                                /* Write it back
                                 * One big question, how do we shrink a file?
                                 */
                                if (    fseek(phConfigSys, 0, SEEK_SET) == 0
                                    &&  fwrite(pszConfigSys, 1, pszCurrent - pszConfigSys, phConfigSys))
                                {
                                    ShowMessage(hwnd, IDM_CONFIGSYS_UPDATED, MB_OK);
                                }
                                else
                                    Complain(hwnd, IDS_FWRITE_FAILED, pszConfigSys);
                                free(pszConfigSys);
                            }
                            else
                                Complain(hwnd, IDS_MALLOC_FAILED, cbConfigSys + 256);
                        }
                        else
                            Complain(hwnd, IDS_FSIZE_FAILED, pszConfigSys);
                        fclose(phConfigSys);
                    }
                    else
                        Complain(hwnd, IDS_ERR_FOPEN_FAILED, pszConfigSys);
                    break;
                }
            }
            return NULL;


        /*
         * Close window. Typically sent when Alt-F4 pressed or system-menu-Close is selected.
         */
        case WM_CLOSE:
            fNotExit = TRUE;
            WinSendMsg(hwnd, WM_COMMAND,
                       MPFROMSHORT(DID_OK), MPFROM2SHORT(CMDSRC_MENU, FALSE));
            return NULL;


        /*
         * Window is destroyed (last message which ever should reach us!)
         *  -Free acceltable if present.
         *  -Free instance data
         *  -Set the instance data pointer to NULL (just in case).
         */
        case WM_DESTROY:
        {
            HACCEL haccel = WinQueryAccelTable(pThis->hab, hwnd);
            if (haccel)
                WinDestroyAccelTable(haccel);
            free(pThis);
            WinSetWindowPtr(hwnd, QWL_USER, NULL);
            break;
        }


        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            APIRET  rc;

            /*
             * Call Win32k.sys to get options and statuses.
             */
            memset(&pThis->Options, 0, sizeof(K32OPTIONS));
            pThis->Options.cb = sizeof(K32OPTIONS);
            memset(&pThis->Status, 0, sizeof(K32STATUS));
            pThis->Status.cb = sizeof(K32STATUS);
            rc = libWin32kQueryOptionsStatus(&pThis->Options, &pThis->Status);
            if (rc != NO_ERROR)
            {
                Complain(hwnd, IDS_ERR_QUERYOPTIONSTATUS, rc);
                fNotExit = FALSE;
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                WinDismissDlg(hwnd, 0);
                return NULL;
            }

            /*
             * Update the individual pages.
             */
            for (i = 0; i < W32KCCPG_PAGES; i++)
            {
                if (pThis->aPages[i].hwnd != NULLHANDLE)
                    WinSendMsg(pThis->aPages[i].hwnd, msg, mp1, mp2);
            }

            /* Since all fields are update now - we can't be dirty any longer. */
            pThis->fDirty = FALSE;
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            /*
             * Init temporary option struct.
             */
            memset(&pThis->NewOptions, 0, sizeof(K32OPTIONS));
            pThis->NewOptions.cb = sizeof(K32OPTIONS);

            /*
             * Query the individual pages.
             */
            for (i = 0; i < W32KCCPG_PAGES; i++)
            {
                if (pThis->aPages[i].hwnd != NULLHANDLE)
                if (!WinSendMsg(pThis->aPages[i].hwnd, msg, mp1, mp2))
                {
                    WinSendMsg(pThis->hwndNtbk, BKM_TURNTOPAGE, (MPARAM)pThis->aPages[i].ulId, NULL);
                    return (MPARAM)FALSE;
                }
            }

            /*
             * Check if there is any change and set the fDirty flag accordingly.
             */
            pThis->fDirty = memcmp(&pThis->NewOptions, &pThis->Options, sizeof(K32OPTIONS)) != 0;
            return (MPARAM)TRUE;
        }

        case WM_TRANSLATEACCEL:
        {
            break;
        }
    }

    /*
     * Return thru the default dialog procedure.
     */
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    LoadersDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Initialize controls.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (NULL)
         */
        case WM_INITDLG:
        {
            /*
             * Initiate controls (ie. behaviour not data).
             *  - Ranges of the info level spinbuttons.
             */
            WinSendDlgItemMsg(hwnd, SB_LDR_PE_INFOLEVEL, SPBM_SETLIMITS, (MPARAM)4, (MPARAM)0);
            WinSendDlgItemMsg(hwnd, SB_LDR_ELF_INFOLEVEL, SPBM_SETLIMITS, (MPARAM)4, (MPARAM)0);
            break;
        }


        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            CHAR    szNumber[32];

            WinSendDlgItemMsg(hwnd, CB_LDR_DISABLE_ALL,     BM_SETCHECK,    (MPARAM)(pThis->Options.fNoLoader),                  NULL);
            /* PE */
            WinSendDlgItemMsg(hwnd, RB_LDR_PE_PURE,         BM_SETCHECK,    (MPARAM)(pThis->Options.fPE == FLAGS_PE_PE2LX),     NULL);
            WinSendDlgItemMsg(hwnd, RB_LDR_PE_MIXED,        BM_SETCHECK,    (MPARAM)(pThis->Options.fPE == FLAGS_PE_MIXED),     NULL);
            WinSendDlgItemMsg(hwnd, RB_LDR_PE_PE,           BM_SETCHECK,    (MPARAM)(pThis->Options.fPE == FLAGS_PE_PE),        NULL);
            WinSendDlgItemMsg(hwnd, RB_LDR_PE_NOT,          BM_SETCHECK,    (MPARAM)(pThis->Options.fPE == FLAGS_PE_NOT),       NULL);
            WinSendDlgItemMsg(hwnd, CK_LDR_PE_ONEOBJECT,    BM_SETCHECK,    (MPARAM)(pThis->Options.fPEOneObject),              NULL);
            WinSendDlgItemMsg(hwnd, SB_LDR_PE_INFOLEVEL,    SPBM_SETCURRENTVALUE, (MPARAM)(pThis->Options.ulInfoLevel),         NULL); /* FIXME to be changed */
            sprintf(szNumber, "%d", pThis->Status.cPe2LxModules);
            WinSetDlgItemText(hwnd, TX_LDR_PE_MODULES_VAL, szNumber);
            /* Elf */
            WinSendDlgItemMsg(hwnd, CB_LDR_ELF_ENABLED,     BM_SETCHECK,    (MPARAM)(pThis->Options.fElf),                      NULL);
            WinSendDlgItemMsg(hwnd, SB_LDR_ELF_INFOLEVEL,   SPBM_SETCURRENTVALUE, (MPARAM)(pThis->Options.ulInfoLevel),         NULL); /* FIXME to be changed */
            sprintf(szNumber, "%d", pThis->Status.cElf2LxModules);
            WinSetDlgItemText(hwnd, TX_LDR_ELF_MODULES_VAL, szNumber);
            /* UNIX Shell Scripts */
            WinSendDlgItemMsg(hwnd, CB_LDR_SHELL_SCRIPTS,   BM_SETCHECK,    (MPARAM)(pThis->Options.fUNIXScript),               NULL);
            /* JAVA */
            WinSendDlgItemMsg(hwnd, CB_LDR_JAVA,            BM_SETCHECK,    (MPARAM)(pThis->Options.fJava),                     NULL);
            /* REXX Scripts */
            WinSendDlgItemMsg(hwnd, CB_LDR_REXX,            BM_SETCHECK,    (MPARAM)(pThis->Options.fREXXScript),               NULL);
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            BOOL    fComplain = (BOOL)mp1;
            ULONG   ul;

            pThis->NewOptions.fNoLoader = WinSendDlgItemMsg(hwnd, CB_LDR_DISABLE_ALL, BM_QUERYCHECK, NULL, NULL) != 0;
            /* PE */
            if (WinSendDlgItemMsg(hwnd, RB_LDR_PE_PURE, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.fPE = FLAGS_PE_PE2LX;
            else if (WinSendDlgItemMsg(hwnd, RB_LDR_PE_MIXED, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.fPE = FLAGS_PE_MIXED;
            else if (WinSendDlgItemMsg(hwnd, RB_LDR_PE_PE, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.fPE = FLAGS_PE_PE;
            else if (WinSendDlgItemMsg(hwnd, RB_LDR_PE_NOT, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.fPE = FLAGS_PE_NOT;
            else
            {
                if (fComplain)
                    Complain(hwnd, IDS_ERR_NO_PE_RADIOBUTTON);
                return (MPARAM)FALSE;
            }
            pThis->NewOptions.fPEOneObject = (ULONG)WinSendDlgItemMsg(hwnd, CK_LDR_PE_ONEOBJECT, BM_QUERYCHECK, NULL, NULL);
            if (pThis->NewOptions.fPEOneObject > 2)
            {
                if (fComplain)
                    Complain(hwnd, IDS_ERR_NO_PE_RADIOBUTTON);
                return (MPARAM)FALSE;
            }
            if (!WinSendDlgItemMsg(hwnd, SB_LDR_PE_INFOLEVEL, SPBM_QUERYVALUE, (MPARAM)&ul, MPFROM2SHORT(0, SPBQ_UPDATEIFVALID)))
            {
                if (fComplain)
                {
                    Complain(hwnd, IDS_ERR_INVALID_INFOLEVEL);
                    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, SB_LDR_PE_INFOLEVEL));
                }
                return (MPARAM)FALSE;
            }
            pThis->NewOptions.ulInfoLevel = ul; /* FIXME to be changed */

            /* Elf */
            pThis->NewOptions.fElf = WinSendDlgItemMsg(hwnd, CB_LDR_ELF_ENABLED, BM_QUERYCHECK, NULL, NULL) != 0;
            if (!WinSendDlgItemMsg(hwnd, SB_LDR_ELF_INFOLEVEL, SPBM_QUERYVALUE, (MPARAM)&ul, MPFROM2SHORT(0, SPBQ_UPDATEIFVALID)))
            {
                if (fComplain)
                {
                    Complain(hwnd, IDS_ERR_INVALID_INFOLEVEL);
                    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, SB_LDR_ELF_INFOLEVEL));
                }
                return (MPARAM)FALSE;
            }
            //pThis->NewOptions.ulInfoLevel = ul; /* FIXME to be changed */
            /* UNIX Shell Scripts */
            pThis->NewOptions.fUNIXScript = WinSendDlgItemMsg(hwnd, CB_LDR_SHELL_SCRIPTS, BM_QUERYCHECK, NULL, NULL) != 0;
            /* JAVA */
            pThis->NewOptions.fJava = WinSendDlgItemMsg(hwnd, CB_LDR_JAVA, BM_QUERYCHECK, NULL, NULL) != 0;
            /* REXX Scripts */
            pThis->NewOptions.fREXXScript = WinSendDlgItemMsg(hwnd, CB_LDR_REXX, BM_QUERYCHECK, NULL, NULL) != 0;
            return (MRESULT)TRUE;
        }
    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}


/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    LoggingDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            WinSendDlgItemMsg(hwnd, CB_LOGGING_ENABLED,     BM_SETCHECK,    (MPARAM)(pThis->Options.fLogging),                  NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM1,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM2,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM3,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3e8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM4,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2e8),            NULL);
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            BOOL    fComplain = (BOOL)mp1;

            pThis->NewOptions.fLogging = WinSendDlgItemMsg(hwnd, CB_LOGGING_ENABLED, BM_QUERYCHECK, NULL, NULL) != 0;
            if (WinSendDlgItemMsg(hwnd, RB_LOGGING_COM1, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.usCom = 0x3f8;
            else if (WinSendDlgItemMsg(hwnd, RB_LOGGING_COM2, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.usCom = 0x2f8;
            else if (WinSendDlgItemMsg(hwnd, RB_LOGGING_COM3, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.usCom = 0x3e8;
            else if (WinSendDlgItemMsg(hwnd, RB_LOGGING_COM4, BM_QUERYCHECK, NULL, NULL))
                pThis->NewOptions.usCom = 0x2e8;
            else
            {
                if (fComplain)
                    Complain(hwnd, IDS_ERR_NO_COM_RADIOBUTTON);
                return (MPARAM)FALSE;
            }
            return (MRESULT)TRUE;
        }
    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}


/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    StatusDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;

    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            CHAR    szBuffer[100];

            /*
             * Set the controls
             */
            sprintf(szBuffer, "%d.%d", 0, pThis->Status.ulVersion);
            WinSetDlgItemText(hwnd, TX_W32K_VERSION_VAL,        szBuffer);
            sprintf(szBuffer, "%s %s", pThis->Status.szBuildTime, pThis->Status.szBuildDate);
            WinSetDlgItemText(hwnd, TX_W32K_BUILD_DATETIME_VAL, szBuffer);
            WinSetDlgItemText(hwnd, TX_W32K_SYMBOLFILE_VAL,     pThis->Status.szSymFile);
            sprintf(szBuffer, "%d - ", pThis->Status.ulBuild);
            if (GetFixpackDesc(pThis->Status.ulBuild, pThis->Status.fKernel, szBuffer + strlen(szBuffer)))
                sprintf(szBuffer, "%d", pThis->Status.ulBuild);
            WinSetDlgItemText(hwnd, TX_W32K_KERNELBUILD_VAL,    szBuffer);
            return NULL;
        }
    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}

/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    HeapsDlgProc    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Initialize controls.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (NULL)
         */
        case WM_INITDLG:
        {
            /*
             * Initiate controls (ie. behaviour not data).
             *  - Max length of the max heap size entry fields.
             */
            WinSendDlgItemMsg(hwnd, SB_HEAP_RES_MAX, SPBM_SETLIMITS, (MPARAM)32678, (MPARAM)128);
            WinSendDlgItemMsg(hwnd, SB_HEAP_SWP_MAX, SPBM_SETLIMITS, (MPARAM)32678, (MPARAM)128);
            break;
        }


        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            CHAR    szNumber[32];

            /* Resident */
            WinSendDlgItemMsg(hwnd, SB_HEAP_RES_MAX,        SPBM_SETCURRENTVALUE, (MPARAM)(pThis->Options.cbResHeapMax / 1024), NULL);
            sprintf(szNumber, "%d", pThis->Status.cbResHeapInit / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_INIT_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbResHeapSize / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_SIZE_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbResHeapUsed / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_USED_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbResHeapFree / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_FREE_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cResBlocksUsed);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_USED_BLOCKS_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cResBlocksFree);
            WinSetDlgItemText(hwnd, TX_HEAP_RES_FREE_BLOCKS_VAL,   szNumber);
            /* Swappable */
            WinSendDlgItemMsg(hwnd, SB_HEAP_SWP_MAX,        SPBM_SETCURRENTVALUE, (MPARAM)(pThis->Options.cbSwpHeapMax / 1024), NULL);
            sprintf(szNumber, "%d", pThis->Status.cbSwpHeapInit / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_INIT_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbSwpHeapSize / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_SIZE_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbSwpHeapUsed / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_USED_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cbSwpHeapFree / 1024);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_FREE_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cSwpBlocksUsed);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_USED_BLOCKS_VAL,   szNumber);
            sprintf(szNumber, "%d", pThis->Status.cSwpBlocksFree);
            WinSetDlgItemText(hwnd, TX_HEAP_SWP_FREE_BLOCKS_VAL,   szNumber);
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            BOOL    fComplain = (BOOL)mp1;
            ULONG   ul;

            /* Resident */
            if (!WinSendDlgItemMsg(hwnd, SB_HEAP_RES_MAX, SPBM_QUERYVALUE, (MPARAM)&ul, MPFROM2SHORT(0, SPBQ_UPDATEIFVALID)))
            {
                if (fComplain)
                {
                    Complain(hwnd, IDS_ERR_INVALID_MAXHEAPSIZE);
                    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, SB_HEAP_RES_MAX));
                }
                return (MPARAM)FALSE;
            }
            pThis->NewOptions.cbResHeapMax = ul*1024;
            /* Swappable */
            if (!WinSendDlgItemMsg(hwnd, SB_HEAP_SWP_MAX, SPBM_QUERYVALUE, (MPARAM)&ul, MPFROM2SHORT(0, SPBQ_UPDATEIFVALID)))
            {
                if (fComplain)
                {
                    Complain(hwnd, IDS_ERR_INVALID_MAXHEAPSIZE);
                    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, SB_HEAP_SWP_MAX));
                }
                return (MPARAM)FALSE;
            }
            pThis->NewOptions.cbSwpHeapMax = ul*1024;
            return (MRESULT)TRUE;
        }
    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}


/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    LdrFixDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES,     BM_SETCHECK,    (MPARAM)(pThis->Options.fDllFixes),                 NULL);
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_SETCHECK,    (MPARAM)(pThis->Options.fForcePreload),             NULL);
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            pThis->NewOptions.fDllFixes = WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES, BM_QUERYCHECK, NULL, NULL) != 0;
            pThis->NewOptions.fForcePreload = WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_QUERYCHECK, NULL, NULL) != 0;
            pThis->NewOptions.fExeFixes = pThis->Options.fExeFixes;
            return (MRESULT)TRUE;
        }
    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}


/**
 * Dialog procedure for the DL_ dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    MemInfoDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PWIN32KCC   pThis;


    /*
     * Get instance data pointer (pThis).
     */
    if (msg != WM_INITDLG)
    {
        pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
        if (pThis == NULL)
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }


    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Start timer.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (pThis for notebook)
         */
        case WM_INITDLG:
        {
            pThis = (PWIN32KCC)mp2;
            WinEnableWindow(WinWindowFromID(hwnd, TX_MEMINFO_PAGE_ENABLED), FALSE);
            pThis->idMemTimer = WinStartTimer(pThis->hab, hwnd, 42, 1000);
            if (!pThis->idMemTimer)
                Complain(hwnd, IDS_ERR_TIMER_START, WinGetLastError(pThis->hab), getLastErrorMsg(pThis->hab));
            memset(&pThis->MemInfo, -1, sizeof(pThis->MemInfo)); /* Force update of everything. */
            break;                      /* not return, break thru the ntbk page default procedure. */
        }


        /*
         * We've started a timer for refreshing the memory data.
         *
         * mr:  reserved
         * mp1: Id of the timer which is ticking.
         * mp2: reserved
         */
        case WM_TIMER:
        {
            if ((ULONG)mp1 != pThis->idMemTimer)
                return NULL;

            /* intented fallthru to WM_SETCONTROLS */
        }


        /*
         * Save old data.
         * Gets system meminfo data from win32k.
         * Updated changed values.
         * Update pThis with new meminfo.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            K32SYSTEMMEMINFO    MemInfo;
            K32SYSTEMMEMINFO    OldMemInfo;
            APIRET              rc;

            OldMemInfo = pThis->MemInfo;
            MemInfo.cb = sizeof(MemInfo);
            MemInfo.flFlags = 0;
            rc = W32kQuerySystemMemInfo(&MemInfo);
            if (rc)
            {
                WinStopTimer(pThis->hab, hwnd, pThis->idMemTimer);
                break;
            }

            if (MemInfo.cbSwapFileSize != OldMemInfo.cbSwapFileSize)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_SIZE       , "%d", MemInfo.cbSwapFileSize / 1024);
            if (MemInfo.cbSwapFileAvail != OldMemInfo.cbSwapFileAvail)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_AVAIL      , "%d", MemInfo.cbSwapFileAvail / 1024);
            if (MemInfo.cbSwapFileUsed != OldMemInfo.cbSwapFileUsed)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_USED       , "%d", MemInfo.cbSwapFileUsed / 1024);
            if (MemInfo.cbSwapFileMinFree != OldMemInfo.cbSwapFileMinFree)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_MINFREE    , "%d", MemInfo.cbSwapFileMinFree / 1024);
            if (MemInfo.cbSwapFileCFGMinFree != OldMemInfo.cbSwapFileCFGMinFree)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_CFG_MINFREE, "%d", MemInfo.cbSwapFileCFGMinFree / 1024);
            if (MemInfo.cbSwapFileCFGSwapSize != OldMemInfo.cbSwapFileCFGSwapSize)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_CFG_SIZE   , "%d", MemInfo.cbSwapFileCFGSwapSize / 1024);
            if (MemInfo.cSwapFileBrokenDF != OldMemInfo.cSwapFileBrokenDF)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_BROKEN_DFS , "%d", MemInfo.cSwapFileBrokenDF);
            if (MemInfo.cSwapFileGrowFails != OldMemInfo.cSwapFileGrowFails)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_GROW_FAILS , "%d", MemInfo.cSwapFileGrowFails);
            if (MemInfo.cSwapFileInMemFile != OldMemInfo.cSwapFileInMemFile)
                SetDlgItemTextF(hwnd, TX_MEMINFO_SWAP_DFS_IN_MEMFILE, "%d", MemInfo.cSwapFileInMemFile);

            if (MemInfo.cbPhysSize != OldMemInfo.cbPhysSize)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PHYS_SIZE , "%d", MemInfo.cbPhysSize / 1024);
            if (MemInfo.cbPhysAvail != OldMemInfo.cbPhysAvail)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PHYS_AVAIL, "%d", MemInfo.cbPhysAvail / 1024);
            if (MemInfo.cbPhysUsed != OldMemInfo.cbPhysUsed)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PHYS_USED , "%d", MemInfo.cbPhysUsed / 1024);
            if (MemInfo.fPagingSwapEnabled != OldMemInfo.fPagingSwapEnabled)
                WinSendDlgItemMsg(hwnd, TX_MEMINFO_PAGE_ENABLED, BM_SETCHECK, (MPARAM)MemInfo.fPagingSwapEnabled, NULL);
            if (MemInfo.cPagingPageFaults != OldMemInfo.cPagingPageFaults)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_FAULTS   , "%d", MemInfo.cPagingPageFaults);
            if (MemInfo.cPagingPageFaultsActive != OldMemInfo.cPagingPageFaultsActive)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_FAULTS_ACTIVE, "%d", MemInfo.cPagingPageFaultsActive);
            if (MemInfo.cPagingPhysPages != OldMemInfo.cPagingPhysPages)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_PHYSPAGES, "%d", MemInfo.cPagingPhysPages);
            if (MemInfo.cPagingResidentPages != OldMemInfo.cPagingResidentPages)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_RESPAGES , "%d", MemInfo.cPagingResidentPages);
            if (MemInfo.cPagingSwappablePages != OldMemInfo.cPagingSwappablePages)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_SWAPPAGES, "%d", MemInfo.cPagingSwappablePages);
            if (MemInfo.cPagingDiscardablePages != OldMemInfo.cPagingDiscardablePages)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_DISCPAGES, "%d", MemInfo.cPagingDiscardablePages);
            if (MemInfo.cPagingDiscardableInmem != OldMemInfo.cPagingDiscardableInmem)
                SetDlgItemTextF(hwnd, TX_MEMINFO_PAGE_DISCINMEM, "%d", MemInfo.cPagingDiscardableInmem);

            if (MemInfo.ulAddressLimit != OldMemInfo.ulAddressLimit)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_ADDRESSLIMIT, "%08xh", MemInfo.ulAddressLimit);
            if (MemInfo.ulVMArenaSharedMin != OldMemInfo.ulVMArenaSharedMin)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SHARED_MIN  , "%08xh", MemInfo.ulVMArenaSharedMin);
            if (MemInfo.ulVMArenaSharedMax != OldMemInfo.ulVMArenaSharedMax)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SHARED_MAX  , "%08xh", MemInfo.ulVMArenaSharedMax);
            if (MemInfo.ulVMArenaPrivMax != OldMemInfo.ulVMArenaPrivMax)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_PRIVATE_MAX , "%08xh", MemInfo.ulVMArenaPrivMax);
            if (MemInfo.ulVMArenaSystemMin != OldMemInfo.ulVMArenaSystemMin)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SYSTEM_MIN  , "%08xh", MemInfo.ulVMArenaSystemMin);
            if (MemInfo.ulVMArenaSystemMax != OldMemInfo.ulVMArenaSystemMax)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SYSTEM_MAX  , "%08xh", MemInfo.ulVMArenaSystemMax);
            if (MemInfo.ulVMArenaHighPrivMax != OldMemInfo.ulVMArenaHighPrivMax)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_PRIVATE_HMAX, "%08xh", MemInfo.ulVMArenaHighPrivMax);
            if (MemInfo.ulVMArenaHighSharedMin != OldMemInfo.ulVMArenaHighSharedMin)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SHARED_HMIN , "%08xh", MemInfo.ulVMArenaHighSharedMin);
            if (MemInfo.ulVMArenaHighSharedMax != OldMemInfo.ulVMArenaHighSharedMax)
                SetDlgItemTextF(hwnd, TX_MEMINFO_VM_SHARED_HMAX , "%08xh", MemInfo.ulVMArenaHighSharedMax);

            pThis->MemInfo = MemInfo;
            return NULL;
        }

        /*
         * Cleanup.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_DESTROY:
        {
            WinStopTimer(pThis->hab, hwnd, pThis->idMemTimer);
            break;
        }

    }

    return NtbkDefPageDlgProc(hwnd, msg, mp1, mp2);
}



/**
 * Default notebook page dialog procedure.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY    NtbkDefPageDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    /*
     * Message switch.
     */
    switch (msg)
    {
        /*
         * Sets the controls according to the data from win32k.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (pThis for notebook)
         */
        case WM_INITDLG:
        {
            PWIN32KCC   pThis = (PWIN32KCC)mp2;

            if (!WinSetWindowPtr(hwnd, QWL_USER, mp2))
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_SET_INSTANCEDATA,
                         WinGetLastError(pThis->hab),
                         getLastErrorMsg(pThis->hab));
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                return FALSE;
            }

            /*
             * Install same acceltable as the notebook - Don't work.
             */
            WinSetAccelTable(pThis->hab, WinQueryAccelTable(pThis->hab, pThis->hwnd), hwnd);
            break;
        }


        /*
         * Gets data from win32k.
         * Sets the controls according to the data from win32k.
         *
         * mr:  reserved
         * mp1: reserved
         * mp2: reserved
         */
        case WM_SETCONTROLS:
        {
            return NULL;
        }


        /*
         * Validate data in the controls. Complains accoring to mp1.
         * Put the data into the win32k option struct.
         *
         * mr:  Valid indicator.
         *      TRUE:   Valid data.
         *      FALSE:  Not valid data.
         * mp1: BOOL fComplain.
         *      TRUE:   Do complain about errors. The pThis->Options struct
         *              is updated on successful return.
         *      FALSE:  Do not complain about errors, and don't update the
         *              pThis->Options struct.
         * mp2: reserved.
         */
        case WM_QUERYCONTROLS:
        {
            return (MRESULT)TRUE;
        }


        #if 0
        /*
         * Nice little hack to get global notebook accelerators to work.
         *
         * mr:
         *
         */
        case WM_TRANSLATEACCEL:
        {
            static BOOL fSem = FALSE;
            MRESULT     mr;
            PWIN32KCC   pThis = (PWIN32KCC)WinQueryWindowPtr(hwnd, QWL_USER);
            if (fSem || !pThis)
                return FALSE;
            fSem = TRUE;
            mr = WinSendMsg(pThis->hwnd, msg, mp1, mp2);
            fSem = FALSE;
            return mr;
        }
        #endif
    }

    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/**
 * Spirintf version of WinSetDlgItemText.
 * @returns Same as WinSetDlgItemText.
 * @param   hwndDlg     Dialog Window Handle.
 * @param   idItem      Control ID.
 * @param   pszFormat   Pointer to format string. (input to sprintf)
 * @param   ..          Additional parameters.
 * @status  completly implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL    SetDlgItemTextF(HWND hwndDlg, ULONG idItem, PSZ pszFormat, ...)
{
    BOOL    fRc;
    char    sz[64];
    va_list arg;

    #pragma info(none)
    va_start(arg, pszFormat);
    #pragma info(restore)
    vsprintf(sz, pszFormat, arg);
    va_end(arg);

    fRc = WinSetDlgItemText(hwndDlg, idItem, sz);
    #ifdef DEBUG
    if (!fRc)
        Complain(hwndDlg, IDS_ERR_ASSERT, __FILE__, __LINE__, __FUNCTION__);
    #endif

    return fRc;
}


/**
 * Pops up a message box displaying a message from the message table.
 * @returns Return from WinMessageBox
 * @param   hwndOwner   Handle to owner window.
 * @param   id          Message table id of the message.
 * @param   flStyle     Messagebox style.
 *                      If 0 the apply default style.
 */
ULONG   ShowMessage(HWND hwndOwner, int id, ULONG flStyle)
{
    return WinMessageBox(HWND_DESKTOP,
                         hwndOwner,
                         getMessage(id),
                         "Win32k Control Center", 0,
                         (flStyle == 0
                            ? MB_OK | MB_INFORMATION
                            : flStyle)
                         | MB_MOVEABLE
                         );
}



/**
 * Pops up a message box displaying some complaint or error.
 * @returns     Success indicator.
 * @param       hwndOwner   Handle of owner window.
 * @param       id          String table id of the message.
 * @param       ...         Arguments passed on to vsprintf to format the message.
 */
BOOL Complain(HWND hwndOwner, int id, ...)
{
    ULONG   rc;
    char    szMsg[1024];
    char    szMsgOutput[4096];


    /*
     * Load the string and format it.
     */
    if (WinLoadString(WinQueryAnchorBlock(hwndOwner), 0, id, sizeof(szMsg), szMsg))
    {
        va_list args;
        #pragma info(none)
        va_start(args, id);
        #pragma info(restore)
        vsprintf(szMsgOutput, szMsg, args);
        va_end(args);
    }
    else
        sprintf(szMsgOutput, "Failed to load the message id %id.\n", id);


    /*
     * Show message.
     */
    rc = WinMessageBox(HWND_DESKTOP, hwndOwner,
                       szMsgOutput,
                       "Win32k Control Center - error",
                       0,
                       MB_APPLMODAL | MB_ICONHAND | MB_OK | MB_MOVEABLE);
    if (rc == (ULONG)MBID_ERROR)
    {
        rc = WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                           szMsgOutput,
                           "Win32k Control Center - error",
                           0,
                           MB_ICONHAND | MB_OK | MB_MOVEABLE);
    }


    /*
     * Return according to rc.
     */
    return rc != MBID_ERROR;
}


/**
 * Gets the message string for the last error message.
 * @returns     Pointer to message string.
 * @param       hab     Handle of application anchor block.
 */
PCSZ getLastErrorMsg(HAB hab)
{
    char     *pszErrInfo;
    PERRINFO pErrInfo = WinGetErrorInfo(hab);

    if (pErrInfo != NULL && pErrInfo->cDetailLevel > 0)
    {
        pszErrInfo = (char*)(void*)pErrInfo;
        pszErrInfo += ((PUSHORT)(void*)(pszErrInfo + pErrInfo->offaoffszMsg))[pErrInfo->cDetailLevel-1];
    }
    else
        pszErrInfo = "<none>";

    return pszErrInfo;
}


/**
 * Gets a message from the executable resources.
 * @returns     Pointer to read-only string.
 *              NULL if not found/error.
 * @param       id      String Id.
 */
PSZ getMessage(ULONG id)
{
    PSZ psz;

    if (DosGetResource(NULLHANDLE, RT_MESSAGE, id / 16 + 1, (PPVOID)(void*)&psz) == NO_ERROR)
    {
        psz +=2;
        id %= 16;
        while (id-- > 0)
            psz += 1 + *psz;
        return psz+1;
    }
    else
        psz = NULL;

    return psz;
}


/**
 * Determin the fixpack+kernel description from build no. and kernel flags.
 * @returns     0 on success. Description i szBuffer.
 *              -1 on error.
 * @param       ulBuild     Kernel build no.
 * @param       flKernel    Win32k kernel flags.
 * @param       szBuffer    Pointer to buffer
 */
int GetFixpackDesc(ULONG ulBuild, ULONG flKernel, PSZ pszBuffer)
{

    pszBuffer[0] = '\0';
    if (ulBuild == 9023)
        strcpy(pszBuffer, "Warp 4 GA");
    else if (ulBuild > 9023 && ulBuild <= 9036)
        sprintf(pszBuffer, "Warp 4 FP %d", ulBuild - 9024);
    else if (ulBuild == 14039)
        strcpy(pszBuffer, "WS4eB GA");
    else if (ulBuild == 14040)
        strcpy(pszBuffer, flKernel & KF_W4 ? "Warp 4 FP13" : "WS4eB FP1");
    else if (ulBuild >= 14041 /*&& ulBuild <= 1406x*/)
        strcpy(pszBuffer, "Warp 4 FP14");
    /*
    else if (ulBuild >= 14048)
    {
        if (flKernel & KF_W4)
            sprintf(pszBuffer, "Warp 4 FP%d", ulBuild - 14049 + 15); //???
        else
            sprintf(pszBuffer, "WS4eB FP%d", ulBuild - 14048 + 2); //???
    }
    */
    else if (ulBuild >= 8255 && ulBuild <= 8270)
        sprintf(pszBuffer, "Warp 3 FP%d", ulBuild - 8255 + 32);
    else
        return -1;

    /*
     * Check type.
     */
    if (pszBuffer[0] != '\0')
    {
        char *pszAdd;

        if (flKernel & KF_SMP)
            pszAdd = "SMP ";
        else
            pszAdd = " ";
        strcpy(pszBuffer + strlen(pszBuffer), pszAdd);

        if (flKernel & KF_DEBUG)
        {
            if (flKernel & KF_HAS_DEBUGTYPE)
                pszAdd = (flKernel & (KF_ALLSTRICT | KF_HALFSTRICT)) == KF_ALLSTRICT
                          ? "(Allstrict)" : "(Halfstrict)";
            else
                pszAdd = "(Debug)";
        }
        else
            pszAdd = "(Retail)";
        strcpy(pszBuffer + strlen(pszBuffer), pszAdd);
    }

    return 0;
}


/**
 * Upcases a char.
 * @returns   Upper case of the char given in ch.
 * @param     ch  Char to capitalize.
 */
#define  upcase(ch) ((ch) >= 'a' && (ch) <= 'z' ?  (char)((ch) - ('a' - 'A')) : (ch))


/**
 * Searches for a substring in a string.
 * @returns   Pointer to start of substring when found, NULL when not found.
 * @param     pszStr     String to be searched.
 * @param     pszSubStr  String to be searched.
 * @remark    Depends on the upcase function.
 */
static char *stristr(const char *pszStr, const char *pszSubStr)
{
    int cchSubStr = strlen(pszSubStr);
    int i = 0;

    while (*pszStr != '\0' && i < cchSubStr)
    {
        i = 0;
        while (i < cchSubStr && pszStr[i] != '\0' &&
               (upcase(pszStr[i]) == upcase(pszSubStr[i])))
            i++;
        pszStr++;
    }

    #pragma info(none)
    return (char*)(*pszStr != '\0' ? (const char*)pszStr - 1 : (const char*)NULL);
    #pragma info(restore)
}

