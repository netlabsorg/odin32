/* $Id: Win32kCC.c,v 1.12.2.2 2001-02-16 23:23:05 bird Exp $
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
#define W32CCPG_INFO            0
#define W32CCPG_LOADERS         1
#define W32CCPG_LOGGING         2
#define W32CCPG_STATUS          3
#define W32CCPG_HEAPS           4
#define W32CCPG_LDRFIX          5
#define W32CCPG_PAGES           (W32CCPG_LDRFIX+1)



/*
 * Include defines
 */
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSRESOURCES
#define INCL_WINERRORS
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#define INCL_WINSTDSPIN
#define INCL_WINBUTTONS
#define INCL_WINWINDOWMGR
#define INCL_DOSMISC

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
    ULONG   ulPageId;
    HWND    hwnd;
} WIN32KCCPAGE, *PWIN32KCCPAGE;

typedef struct _Win32kCC
{
    HWND    hwnd;
    HAB     hab;
    BOOL    fDirty;

    K32OPTIONS Options;
    K32OPTIONS NewOptions;
    K32STATUS  Status;

    W32CCPG_PAGES   aPages[W32CCPG_PAGES];

} WIN32KCC, *PWIN32KCC;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL    fNotExit;                       /* Global variable used to stop WM_QUITS.
                                         * As long as this is true the message
                                         * loop will never exit, but ignore all
                                         * WM_QUITs.
                                         */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
MRESULT EXPENTRY    Win32kCCDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
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
     * Load the dialog.
     */
    hwnd = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP,
                      Win32kCCDlgProc,
                      NULLHANDLE,
                      DL_WIN32KCC,
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

#if 1
/**
 * Dialog procedure for the DL_WIN32KCC notebook dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY Win32kCCDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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
         * Sets the controls according to the data from win32k.
         *
         * mr:  Focus changed or not.
         * mp1: hwnd of dialog
         * mp2: (user data) (NULL)
         */
        case WM_INITDLG:
        {
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
            if (!WinSetWindowPtr(hwnd, QWL_USER, pThis))
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_SET_INSTACEDATA,
                         WinGetLastError(pThis->hab),
                         getLastErrorMsg(pThis->hab));
                WinDismissDlg(hwnd, 0);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                free(pThis);
                return FALSE;
            }

            /*
             * Insert notebooks pages.
             */



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
                    WinDismissDlg(hwnd, 0);
                    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
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
                    if (!pThis->NewOptions.fForcePreload)       strcat(szArgs, " -ForcePreload:Y"); /* default is disabled */
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
            break;


        /*
         * Window is destroyed (last message which ever should reach us!)
         *  -Free instance data
         *  -Set the instance data pointer to NULL (just in case).
         */
        case WM_DESTROY:
        {
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
            CHAR    szNumber[32];
            CHAR    szBuffer[100];


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
                WinDismissDlg(hwnd, 0);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                return NULL;
            }

            /*
             * Set the controls.
             */
            /* win32k */
            sprintf(szBuffer, "%d.%d", 0, pThis->Status.ulVersion);
            WinSetDlgItemText(hwnd, TX_W32K_VERSION_VAL,        szBuffer);
            sprintf(szBuffer, "%s %s", pThis->Status.szBuildTime, pThis->Status.szBuildDate);
            WinSetDlgItemText(hwnd, TX_W32K_BUILD_DATETIME_VAL, szBuffer);
            WinSetDlgItemText(hwnd, TX_W32K_SYMBOLFILE_VAL,     pThis->Status.szSymFile);
            sprintf(szBuffer, "%d - ", pThis->Status.ulBuild);
            if (GetFixpackDesc(pThis->Status.ulBuild, pThis->Status.fKernel, szBuffer + strlen(szBuffer)))
                sprintf(szBuffer, "%d", pThis->Status.ulBuild);
            WinSetDlgItemText(hwnd, TX_W32K_KERNELBUILD_VAL,    szBuffer);

            /* logging */
            WinSendDlgItemMsg(hwnd, CB_LOGGING_ENABLED,     BM_SETCHECK,    (MPARAM)(pThis->Options.fLogging),                  NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM1,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM2,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM3,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3e8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM4,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2e8),            NULL);

            /* loaders */
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

            /* OS/2 Loader Fixes */
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES,     BM_SETCHECK,    (MPARAM)(pThis->Options.fDllFixes),                 NULL);
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_SETCHECK,    (MPARAM)(pThis->Options.fForcePreload),             NULL);

            /* heaps */
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
            BOOL        fComplain = (BOOL)mp1;
            ULONG       ul;

            /*
             * Init temporary option struct.
             */
            memset(&pThis->NewOptions, 0, sizeof(K32OPTIONS));
            pThis->NewOptions.cb = sizeof(K32OPTIONS);

            /*
             * Logging.
             */
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

            /*
             * Loaders
             */
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

            /*
             * OS/2 Loader Fixes.
             */
            pThis->NewOptions.fDllFixes = WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES, BM_QUERYCHECK, NULL, NULL) != 0;
            pThis->NewOptions.fForcePreload = WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_QUERYCHECK, NULL, NULL) != 0;

            /*
             * Heaps
             */
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

            /*
             * Check if there is any change and set the fDirty flag accordingly.
             */
            pThis->fDirty = memcmp(&pThis->NewOptions, &pThis->Options, sizeof(K32OPTIONS)) != 0;
            return (MPARAM)TRUE;
        }


    }

    /*
     * Return thru the default dialog procedure.
     */
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


#else
/**
 * Dialog procedure for the DL_WIN32KCC dialog.
 * (See PMREF for the general specifications of this function.)
 */
MRESULT EXPENTRY Win32kCCDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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
         * Sets the controls according to the data from win32k.
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
             *  - Max length of the max heap size entry fields.
             */
            WinSendDlgItemMsg(hwnd, SB_LDR_PE_INFOLEVEL, SPBM_SETLIMITS, (MPARAM)4, (MPARAM)0);
            WinSendDlgItemMsg(hwnd, SB_LDR_ELF_INFOLEVEL, SPBM_SETLIMITS, (MPARAM)4, (MPARAM)0);

            WinSendDlgItemMsg(hwnd, SB_HEAP_RES_MAX, SPBM_SETLIMITS, (MPARAM)32678, (MPARAM)128);
            WinSendDlgItemMsg(hwnd, SB_HEAP_SWP_MAX, SPBM_SETLIMITS, (MPARAM)32678, (MPARAM)128);


            /*
             * Init and set instance data.
             */
            pThis = malloc(sizeof(*pThis));
            if (pThis == NULL)
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_MALLOC_FAILED, __FILE__, __LINE__, __FUNCTION__);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                return FALSE;
            }
            pThis->hwnd = hwnd;
            pThis->hab = WinQueryAnchorBlock(hwnd);
            if (!WinSetWindowPtr(hwnd, QWL_USER, pThis))
            {
                /* complain, dismiss and return. */
                Complain(hwnd, IDS_ERR_SET_INSTACEDATA,
                         WinGetLastError(pThis->hab),
                         getLastErrorMsg(pThis->hab));
                WinDismissDlg(hwnd, 0);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                free(pThis);
                return FALSE;
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
                    WinDismissDlg(hwnd, 0);
                    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
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
                    if (!pThis->NewOptions.fForcePreload)       strcat(szArgs, " -ForcePreload:Y"); /* default is disabled */
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
            break;


        /*
         * Window is destroyed (last message which ever should reach us!)
         *  -Free instance data
         *  -Set the instance data pointer to NULL (just in case).
         */
        case WM_DESTROY:
        {
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
            CHAR    szNumber[32];
            CHAR    szBuffer[100];


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
                WinDismissDlg(hwnd, 0);
                WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                return NULL;
            }

            /*
             * Set the controls.
             */
            /* win32k */
            sprintf(szBuffer, "%d.%d", 0, pThis->Status.ulVersion);
            WinSetDlgItemText(hwnd, TX_W32K_VERSION_VAL,        szBuffer);
            sprintf(szBuffer, "%s %s", pThis->Status.szBuildTime, pThis->Status.szBuildDate);
            WinSetDlgItemText(hwnd, TX_W32K_BUILD_DATETIME_VAL, szBuffer);
            WinSetDlgItemText(hwnd, TX_W32K_SYMBOLFILE_VAL,     pThis->Status.szSymFile);
            sprintf(szBuffer, "%d - ", pThis->Status.ulBuild);
            if (GetFixpackDesc(pThis->Status.ulBuild, pThis->Status.fKernel, szBuffer + strlen(szBuffer)))
                sprintf(szBuffer, "%d", pThis->Status.ulBuild);
            WinSetDlgItemText(hwnd, TX_W32K_KERNELBUILD_VAL,    szBuffer);

            /* logging */
            WinSendDlgItemMsg(hwnd, CB_LOGGING_ENABLED,     BM_SETCHECK,    (MPARAM)(pThis->Options.fLogging),                  NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM1,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM2,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2f8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM3,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x3e8),            NULL);
            WinSendDlgItemMsg(hwnd, RB_LOGGING_COM4,        BM_SETCHECK,    (MPARAM)(pThis->Options.usCom == 0x2e8),            NULL);

            /* loaders */
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

            /* OS/2 Loader Fixes */
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES,     BM_SETCHECK,    (MPARAM)(pThis->Options.fDllFixes),                 NULL);
            WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_SETCHECK,    (MPARAM)(pThis->Options.fForcePreload),             NULL);

            /* heaps */
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
            BOOL        fComplain = (BOOL)mp1;
            ULONG       ul;

            /*
             * Init temporary option struct.
             */
            memset(&pThis->NewOptions, 0, sizeof(K32OPTIONS));
            pThis->NewOptions.cb = sizeof(K32OPTIONS);

            /*
             * Logging.
             */
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

            /*
             * Loaders
             */
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

            /*
             * OS/2 Loader Fixes.
             */
            pThis->NewOptions.fDllFixes = WinSendDlgItemMsg(hwnd, CB_LDRFIX_DLLFIXES, BM_QUERYCHECK, NULL, NULL) != 0;
            pThis->NewOptions.fForcePreload = WinSendDlgItemMsg(hwnd, CB_LDRFIX_FORCEPRELOAD, BM_QUERYCHECK, NULL, NULL) != 0;

            /*
             * Heaps
             */
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

            /*
             * Check if there is any change and set the fDirty flag accordingly.
             */
            pThis->fDirty = memcmp(&pThis->NewOptions, &pThis->Options, sizeof(K32OPTIONS)) != 0;
            return (MPARAM)TRUE;
        }


    }

    /*
     * Return thru the default dialog procedure.
     */
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}
#endif


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
                pszAdd = (flKernel & KF_ALLSTRICT) ? "(Allstrict)" : "(Halfstrict)";
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

    return (char*)(*pszStr != '\0' ? (const char*)pszStr - 1 : (const char*)NULL);
}

