#define INCL_WIN
#define INCL_DOS
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#define INCL_WINSWITCHLIST
#include <os2.h>
#include "DosQuerySysState2.h"
#include <stdio.h>
#include <stdlib.h>

#include "vio32Utils.h"

//
ULONG getParentPID(ULONG pid){
    union {
        struct {
            int extra[11];
            qsPrec_s process;
        } formatted;
        char raw[256];
        int intRaw[64];
    } data;
    APIRET rc = DosQuerySysState(QS_PROCESS, 0,pid, 0, &data, sizeof(data));
    if (rc != 0) return 0;
    return data.formatted.process.ppid;
}

// returns the window id of the client window of the VIO that is hosting the current program
HWND VIOWindowClient(){
    PPIB pib;
    PTIB tib;
    DosGetInfoBlocks(&tib, &pib);

    // now search for the window frame of this VIO window
    USHORT num = WinQuerySwitchList(0L, NULL, 0);
    if(!num) return 0;

    SWBLOCK *pSwb = (SWBLOCK *)malloc(sizeof(HSWITCH) + sizeof(SWENTRY) * num );
    num = WinQuerySwitchList(0L, pSwb, sizeof(HSWITCH) + sizeof(SWENTRY) * num );
    BOOL found = FALSE;
    ULONG pid = pib->pib_ulpid;
    ULONG level = 0;
    HWND hwndFrame;
    while (!found){
//printf ("searching pid %i\n",pid);
        for(USHORT i=0; i<num; i++){
            if (pSwb->aswentry[i].swctl.idProcess == pid){
//printf("LEVEL %i %04x %08x %s\n", level,pSwb->aswentry[i].swctl.idProcess,pSwb->aswentry[i].swctl.hwnd,pSwb->aswentry[i].swctl.szSwtitle);
                found = TRUE;
                hwndFrame = pSwb->aswentry[i].swctl.hwnd;
            }
        }
        level++;
        if (!found ) pid = getParentPID(pid);
        if (pid == 1) break;
        if (pid == 0) break;
    }
    free(pSwb);

    if (!found) return 0;

    // now get the client window id
    HWND hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);
//printf ("window frame %x client %x\n",hwndFrame,hwndClient);

    return hwndClient;
}


// returns the window id of the client window of the VIO that is hosting the current program
HWND VIOWindowFrame(){
    PPIB pib;
    PTIB tib;
    DosGetInfoBlocks(&tib, &pib);

    // now search for the window frame of this VIO window
    USHORT num = WinQuerySwitchList(0L, NULL, 0);
    if(!num) return 0;

    SWBLOCK *pSwb = (SWBLOCK *)malloc(sizeof(HSWITCH) + sizeof(SWENTRY) * num );
    num = WinQuerySwitchList(0L, pSwb, sizeof(HSWITCH) + sizeof(SWENTRY) * num );
    BOOL found = FALSE;
    ULONG pid = pib->pib_ulpid;
    ULONG level = 0;
    HWND hwndFrame;
    while (!found){
//printf ("searching pid %i\n",pid);
        for(USHORT i=0; i<num; i++){
            if (pSwb->aswentry[i].swctl.idProcess == pid){
//printf("LEVEL %i %04x %08x %s\n", level,pSwb->aswentry[i].swctl.idProcess,pSwb->aswentry[i].swctl.hwnd,pSwb->aswentry[i].swctl.szSwtitle);
                found = TRUE;
                hwndFrame = pSwb->aswentry[i].swctl.hwnd;
            }
        }
        level++;
        if (!found ) pid = getParentPID(pid);
        if (pid == 1) break;
        if (pid == 0) break;
    }
    free(pSwb);

    if (!found) return 0;

    return hwndFrame;
}

