#define INCL_WIN
#define INCL_DOS
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#define INCL_WINSWITCHLIST
#include <os2.h>

#include <stdio.h>
#include "vio32Utils.h"

void main(){
    HWND hwndClient  = VIOWindowClient();
    HWND hwndFrame   = VIOWindowFrame();
    HWND hwndParent  = WinQueryWindow(hwndFrame,QW_PARENT);
    HWND hwndParent2 = WinQueryWindow(hwndParent,QW_PARENT);

    printf("client = %x, frame = %x, parent = %x, parent2 = %x\n",hwndClient,hwndFrame,hwndParent,hwndParent2);

    // now get the message Queue
    HMQ queue1 = WinQueryWindowULong(hwndClient, QWL_HMQ);
    // now get the message Queue
    HMQ queue2 = WinQueryWindowULong(hwndFrame, QWL_HMQ);
    // now get the message Queue
    HMQ queue3 = WinQueryWindowULong(hwndParent, QWL_HMQ);
    // now get the message Queue
    HMQ queue4 = WinQueryWindowULong(hwndParent2, QWL_HMQ);

    printf("Q client = %x, frame = %x, parent = %x, parent2 = %x\n",queue1,queue2,queue3,queue4);

    while(1) DosSleep(1000);
}