#define INCL_WIN
#include <os2wrap.h>
#include <odinwrap.h>
#include "os2DDWindow.h"

PFNWP pfnOrgClientProc = NULL;

MRESULT EXPENTRY DDOS2WindowProc(HWND hwnd, ULONG ulMsg, MPARAM mp1, MPARAM mp2);
extern VOID SwitchDisplay(HWND hwnd);

ODINFUNCTION1(BOOL , OS2DdSubClassWindow ,HWND, hwndClient)
{
  HWND hwndParent;

  // only allow subclassing once!

  if(NULL==pfnOrgClientProc)
  {
    hwndParent = WinQueryWindow(hwndClient,QW_PARENT);

    if(hwndParent!=HWND_DESKTOP)
      hwndFrame = hwndParent;
    else
      hwndFrame = hwndClient;

    pfnOrgClientProc = WinSubclassWindow(hwndFrame,DDOS2WindowProc);

    return TRUE;
  }

  return FALSE;
}


MRESULT EXPENTRY DDOS2WindowProc(HWND hwnd, ULONG ulMsg, MPARAM mp1, MPARAM mp2)
{
  USHORT usFlags;

  switch(ulMsg)
  {
    // capture the mouse
    case WM_ACTIVATE:
      if(mp1)
        WinSetCapture(HWND_DESKTOP,hwnd);
      else
        WinSetCapture(HWND_DESKTOP,NULLHANDLE);
      break;
    case WM_CHAR:
      // Check for CTRL+SHIFT+F12 to toggle between stretched (FS) / windowd mode
      usFlags = SHORT1FROMMP(mp1);
      if(!(usFlags & KC_KEYUP))
      {
        if( (usFlags & KC_VIRTUALKEY) && (usFlags & KC_SHIFT) && (usFlags & KC_CTRL))
        {
          if(VK_F12 == SHORT2FROMMP(mp2))
            SwitchDisplay(hwnd);
        }
      }
      break;
    default:
      break;
  }

  return pfnOrgClientProc(hwnd, ulMsg, mp1,mp2);
}

