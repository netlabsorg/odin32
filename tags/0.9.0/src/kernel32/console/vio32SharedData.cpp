#define INCL_WIN
#define INCL_DOS
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#include "vio32private.h"
#include "vio32SharedData.h"
#include "vio32Errors.h"

void kbd32SharedData::Init(){
    nOfWindows = 0;
    for (int i=0;i<MAX_VIO32;i++) {
        clientWindows[i] = 0;
        frameWindows[i] = 0;
        processId[i] = 0;
        mode[i] = 0;
        pos[i] = &events[i * MAX_PRIMARY_Q_LEN];
    }
}


BOOL kbd32SharedData::searchByHwndClient(HWND hwndClient,HWND &hwndFrame,PID &pid,int &ix){
    ix = 0;
    for (;(ix<nOfWindows) && (clientWindows[ix] != hwndClient);ix++);
    if (ix < nOfWindows){
        pid = processId[ix];
        hwndFrame = frameWindows[ix];
        return TRUE;
    }
    return FALSE;
}

BOOL kbd32SharedData::searchByHwndFrame(HWND hwndFrame,HWND &hwndClient,PID &pid,int &ix){
    ix = 0;
    for (;(ix<nOfWindows) && (frameWindows[ix] != hwndFrame);ix++);
    if (ix < nOfWindows){
        pid = processId[ix];
        hwndClient = clientWindows[ix];
        return TRUE;
    }
    return FALSE;
}

BOOL kbd32SharedData::searchByPid(PID pid,HWND &hwndClient,HWND &hwndFrame,int &ix){
    ix = 0;
    for (;(ix<nOfWindows) && (processId[ix] != pid);ix++);
    if (ix < nOfWindows){
        hwndClient = clientWindows[ix];
        hwndFrame  = frameWindows[ix];
        return TRUE;
    }
    return FALSE;
}

VIO32EVENTINFO *kbd32SharedData::getBuffer(int ix){
    if (ix < nOfWindows){
        VIO32EVENTINFO *maxPos = &events[(ix+1)*MAX_PRIMARY_Q_LEN];
        if (pos[ix] >= maxPos) pos[ix] = &events[ix*MAX_PRIMARY_Q_LEN];
        return pos[ix];
    }
    return NULL;
}

APIRET kbd32SharedData::SetMode(ULONG mode,ULONG pid){
    for (int ix=0;(ix<nOfWindows) && (processId[ix] != pid);ix++);
    if (ix >= nOfWindows) return VIO32_NOTINSTALLED;
    this->mode[ix] = mode;
    return 0;
}

APIRET kbd32SharedData::GetMode(ULONG &mode,ULONG pid){
    for (int ix=0;(ix<nOfWindows) && (processId[ix] != pid);ix++);
    if (ix >= nOfWindows) return VIO32_NOTINSTALLED;
    mode = this->mode[ix];
    return 0;
}

APIRET kbd32SharedData::InstallApplication(ULONG pid,BOOL windowed){
    HWND hwndClient = -1;
    HWND hwndFrame = -1;
    if (windowed){
        hwndClient = VIOWindowClient();
        hwndFrame  = VIOWindowFrame();
        if (hwndClient == 0) return VIO32_NOTWINDOWED;
        if (hwndFrame == 0)  return VIO32_NOTWINDOWED;
    }

    APIRET ret = FALSE;
    for (int ix=0;(ix<nOfWindows) && (clientWindows[ix] != hwndClient);ix++);
    if (ix < nOfWindows) ret = VIO32_ALREADYINSTALLED;
    else {
        for (int ix=0;(ix<nOfWindows) && (clientWindows[ix] != 0);ix++);
        if (ix < nOfWindows){
            clientWindows[ix] = hwndClient;
            frameWindows[ix]  = hwndFrame;
            processId[ix] = pid;
            mode[ix] = 0;
            ret = 0;
        } else
        if (nOfWindows < MAX_VIO32){
            clientWindows[nOfWindows] = hwndClient;
            frameWindows[nOfWindows]  = hwndFrame;
            processId[nOfWindows] = pid;
            mode[nOfWindows] = 0;
            nOfWindows++;
            ret = 0;
        } else ret = VIO32_TOOMANYAPPS;
    }
    return ret;
}

BOOL kbd32SharedData::UninstallApplication(ULONG pid){

    APIRET ret;
    for (int ix=0;(ix<nOfWindows) && (processId[ix] != pid);ix++);
    if (ix < nOfWindows){
        ret = 0;
        clientWindows[ix] = 0;
        frameWindows[ix]  = 0;
        processId[ix] = 0;
        if (ix == (nOfWindows-1)) nOfWindows--;
    } else VIO32_NOTINSTALLED;
    return ret;
}

