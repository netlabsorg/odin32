#if !defined (KBD32_SHARED_DATA_H)
#define KBD32_SHARED_DATA_H

#include "vio32Private.h"
#include "vio32Utils.h"


// this object represents the shared memory
class kbd32SharedData {
    // size of the partially used buffer
    int             nOfWindows;
    // the hwnd client
    HWND            clientWindows[MAX_VIO32];
    // the hwnd frame
    HWND            frameWindows[MAX_VIO32];
    // the task to be served
    ULONG           processId[MAX_VIO32];
    // the task to be served
    ULONG           mode[MAX_VIO32];
    //
    VIO32EVENTINFO      events[MAX_VIO32*MAX_PRIMARY_Q_LEN];
    //
    VIO32EVENTINFO     *pos[MAX_VIO32];

public:
    ///
    void Init();
    /// installs a patch for the vio kbd stream for the current task
    APIRET InstallApplication(ULONG pid,BOOL windowed=TRUE);
    ///
    APIRET UninstallApplication(ULONG pid);
    ///
    APIRET SetMode(ULONG mode,ULONG pid);
    ///
    APIRET GetMode(ULONG &mode,ULONG pid);
    /// gets a buffer
    VIO32EVENTINFO *getBuffer(int ix);
    /// automatically updates the pointer
    void advanceBuffer(int ix){ pos[ix]++; }
    ///
    BOOL searchByHwndClient(HWND hwndClient,HWND &hwndFrame,PID &pid,int &ix);
    ///
    BOOL searchByHwndFrame(HWND hwndFrame,HWND &hwndClient,PID &pid,int &ix);
    ///
    BOOL searchByPid(PID pid,HWND &hwndClient,HWND &hwndFrame,int &ix);
    ///
    ULONG getMode(int ix){ return mode[ix]; }
};





#endif
