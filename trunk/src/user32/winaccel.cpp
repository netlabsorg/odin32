/* $Id: winaccel.cpp,v 1.5 2000-01-18 20:11:04 sandervl Exp $ */
/*
 * Win32 accelerator key functions for OS/2
 *
 * Copyright 1999 Bart van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>
#include <winaccel.h>

/*****************************************************************************
 * Name      : HACCEL WIN32API LoadAcceleratorsA
 * Purpose   : Load accelerator resource and return handle to it
 * Parameters: HINSTANCE hinst, LPCSTR lpszAcc
 * Variables :
 * Result    : Global Handle of resource
 * Remark    : see code
 * Status    : OK
 *
 * Author    : Bart van Leeuwen  [Sun, 1998/12/26 17:01]
 *****************************************************************************/
HACCEL WIN32API LoadAcceleratorsA(HINSTANCE hinst, LPCSTR lpszAcc)
{
 HACCEL rc;

    rc = (HACCEL)FindResourceA(hinst, lpszAcc, RT_ACCELERATORA);

    dprintf(("LoadAcceleratorsA returned %d\n", rc));
    return(rc);
}
/*****************************************************************************
 * Name      : HACCEL WIN32API LoadAcceleratorsW
 * Purpose   : Load accelerator resource and return handle to it
 * Parameters: HINSTANCE hinst, LPCSTR lpszAcc
 * Variables :
 * Result    : Global Handle of resource
 * Remark    : see code
 * Status    : OK
 *
 * Author    : Bart van Leeuwen  [Sun, 1998/12/26 17:01]
 *****************************************************************************/

HACCEL WIN32API LoadAcceleratorsW(HINSTANCE hinst, LPCWSTR lpszAccel)
{
 HACCEL rc;

    rc = (HACCEL)FindResourceW(hinst, lpszAccel, RT_ACCELERATORW);


    dprintf(("LoadAcceleratorsW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyAcceleratorTable( HACCEL haccel)
{
 Win32Resource *winres;

    dprintf(("DestroyAcceleratorTable %x\n", haccel));
    winres = (Win32Resource *)haccel;
    delete winres;
    return TRUE;
}
/*****************************************************************************
 * Name      : int WIN32API TranslateAcceleratorA
 * Purpose   : Translate WM_*KEYDOWN messages to WM_COMMAND messages
 *             according to Accelerator table
 * Parameters: HWND hwnd, HACCEL haccel, LPMSG lpmsg
 * Variables :
 * Result    : int FALSE (no accelerator found) TRUE (accelerator found)
 * Remark    : if a accelerator is found it is not neccesarely executed
 *             depends on window stat
 * Status    : finished but not fully tested
 *
 * Author    : Bart van Leeuwen  [Sun, 1998/12/26 17:01]
 *****************************************************************************/
int WIN32API TranslateAcceleratorA(HWND hwnd, HACCEL haccel, LPMSG lpmsg)
{
 Win32BaseWindow *window,*parentWindow,*childWindow;
 LPWINACCEL lpAccelTbl;
 WINACCEL tmpAccel;
 HACCEL temp2;
 HGLOBAL GlobHandle;
 HWND hwndMenu,parent,child,msgHwnd;
 int i,keyMask;
 INT16 menuStat;
 BOOL sendMessage;


    //bvl: check if messages come from keyboard
    if ((lpmsg->message != WM_KEYDOWN &&
         lpmsg->message != WM_SYSKEYDOWN))
         {
           //dprintf(("TranslateAcceleratorA called by invalid message"));
           SetLastError(ERROR_SUCCESS);
           return FALSE;
         }

    window = Win32BaseWindow::GetWindowFromHandle(lpmsg->hwnd);

    if(!window)
    {
      dprintf(("TranslateAcceleratorA, window %x not found", hwnd));
      SetLastError(ERROR_INVALID_WINDOW_HANDLE);
      return FALSE;
    }

    //bvl: Get memory pointer here
    GlobHandle = LoadResource(NULL,haccel);
    lpAccelTbl = (LPWINACCEL) LockResource(GlobHandle);

    keyMask=0;
    if(GetKeyState(VK_SHIFT) & 0x8000) keyMask |= FSHIFT;
    if(GetKeyState(VK_CONTROL) & 0x8000) keyMask |= FCONTROL;
    if(GetKeyState(VK_MENU) & 0x8000) keyMask |= FALT;

    i=0;
    do
    {
       if ((lpAccelTbl[i].key == lpmsg->wParam)&&(keyMask == (lpAccelTbl[i].fVirt &
           (FSHIFT | FCONTROL | FALT))))
        {
         //bvl: Accelerator found
         dprintf(("Accelerator found for command: %X",lpAccelTbl[i].cmd));
         //bvl: this is not right, check control and win stat
         // SendMessageA(hwnd,WM_COMMAND,lpAccelTbl[i].cmd,0x00010000L);

         sendMessage = FALSE;

         if (window->isFrameWindow())
           {
            if ( !window->IsIconic() )
             {
              //bvl: this is a frame window so just use that menuhandle
              hwndMenu = window->GetMenu();
              if ( hwndMenu == NULL )
               {
                //bvl: window does not have a menu so command isn't a menu
                dprintf(("framenomenu"));
                sendMessage = TRUE;
               }
               else
               {
                //bvl: get menuitem  status
                menuStat = GetMenuState(hwndMenu,lpAccelTbl[i++].cmd,MF_BYCOMMAND);
                //bvl: -1 means no menu Item
                if( !menuStat == -1)
                 {
                  if( menuStat  & !(MF_DISABLED|MF_GRAYED) )
                   {
                     //bvl: its a menu item and its enabled
                     dprintf(("framemenu"));
                     sendMessage = TRUE;
                   }
                   else
                   {
                    //bvl: its a menu Item but disabled
                    sendMessage = FALSE;
                   }
                 }
                 else
                 {
                  //bvl: the message is probably not a menu msg so process it
                  dprintf(("framemenunoitem"));
                  sendMessage = TRUE;
                 }
               }
             }
           }
           else
           {
              //bvl: the message is probably not a menu msg so process it
              dprintf(("noframe"));
              sendMessage = TRUE;
           }

         if ( sendMessage )
         {
          //bvl: previous checks indicated that command can be send.
          SendMessageA(hwnd,WM_COMMAND,lpAccelTbl[i].cmd,0x00010000L);
          return TRUE;

         }
        }

    } while ((lpAccelTbl[i++].fVirt & 0x80) == 0);

    SetLastError(ERROR_SUCCESS);
    return FALSE;
}
/*****************************************************************************
 * Name      : int WIN32API TranslateAcceleratorA
 * Remark    : See TranslateAcceleratorA
 * Status    : finished but not fully tested
 *
 * Author    : Bart van Leeuwen  [Sun, 1998/12/26 17:01]
 *****************************************************************************/

int WIN32API TranslateAcceleratorW( HWND hwnd, HACCEL hAccel, LPMSG lpMsg)
{
    return TranslateAcceleratorA(hwnd, hAccel, lpMsg);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMDISysAccel( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateMDISysAccel\n");
#endif
    return O32_TranslateMDISysAccel(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableA( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableA\n");
#endif
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableW( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API CopyAcceleratorTableA(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableA, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//TODO:
//******************************************************************************
int WIN32API CopyAcceleratorTableW(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableW, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
