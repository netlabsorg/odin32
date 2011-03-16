/*
 *  OS/2 System Tray support.
 *  A better implementaiton using the xsystray XCenter/eCenter widget API.
 *
 *  Author: Dmitriy Kuminov
 */

#define  INCL_WIN
#include <os2wrap.h>

#include <string.h>

#include <odin.h>
#include <winconst.h>

//@todo later
//#include <xsystray_api.h>

#include "systray_os2.h"

static BOOL SYSTRAY_Ex_ItemInit(SystrayItem *ptrayItem)
{
    //@todo later
    return TRUE;
}

static void SYSTRAY_Ex_ItemTerm(SystrayItem *ptrayItem)
{
    //@todo later

}

static void SYSTRAY_Ex_ItemSetMessage(SystrayItem *ptrayItem, ULONG uCallbackMessage)
{
    //@todo later

}

static void SYSTRAY_Ex_ItemSetIcon(SystrayItem *ptrayItem, HPOINTER hIcon)
{
    //@todo later

}

static void SYSTRAY_Ex_ItemSetTip(SystrayItem *ptrayItem, CHAR* szTip, int modify)
{
    //@todo later
}

BOOL SYSTRAY_Ex_Init(void)
{
    SYSTRAY_ItemInit = SYSTRAY_Ex_ItemInit;
    SYSTRAY_ItemTerm = SYSTRAY_Ex_ItemTerm;
    SYSTRAY_ItemSetMessage = SYSTRAY_Ex_ItemSetMessage;
    SYSTRAY_ItemSetIcon = SYSTRAY_Ex_ItemSetIcon;
    SYSTRAY_ItemSetTip = SYSTRAY_Ex_ItemSetTip;

    return TRUE;
}

