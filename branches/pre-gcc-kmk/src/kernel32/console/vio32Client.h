#if !defined (KBD32_CLIENT_H)
#define KBD32_CLIENT_H

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

#include "vio32SharedData.h"
#include "vio32Errors.h"

//#define VIO32_PREFILTER   0x00000
//#define VIO32_POSTFILTER  0x10000

//
class kbd32Client{
    //      semaphore to allow muliple threads to use the structures of VIO32
    HMTX                mux;
    //      semaphore used to access shared memory
    HMTX                accessSemaphore;
    ///     the shared memory address.
    kbd32SharedData    *SharedData;
    ///     when the kbd32 is active = TRUE
    BOOL                patched;
    ///     the message queue used to collect all the messages
    HQUEUE              primaryQ;
    ///     the message queue where the processed messages are written to
    HQUEUE              secondaryQ;
    ///     a semaphore used to wait on the primary Q
    HEV                 primaryQSem;
    ///     a semaphore used to wait on the secondary Q
    HEV                 secondaryQSem;
    ///     the id of the thread listening to VIO kbd api
    TID                 kbdThreadId;
    ///     the id of the thread listening to a monitor
    TID                 kbdMonitorThreadId;
    ///     the id of the thread listening to the mou events
    TID                 mouThreadId;
    ///     the id of the thread elaborating data
    TID                 dataProcessingThreadId;
    ///     a flag to get the threads to exit.
    BOOL                threadsMustContinue;
    ///     the monitor handles
    HMONITOR            mon;
    ///     the monitor handles
    MONIN               monin;
    ///     the monitor handles
    MONOUT              monout;
    ///
    HMOU                mou;
    ///
    ULONG               applicationType;
    ///
    ULONG               applicationPid;
    ///
    HWND                hwndFrame;
    ///
    HWND                hwndClient;
    //
    VIO32EVENTINFO      events[MAX_SECONDARY_Q_LEN];
    //
    VIO32EVENTINFO     *pos;
    //      filters acting on the data stream as soon as it gets to the main Q
    vio32Filter        *inputFilters;
    //      filters acting on the data read
    vio32Filter        *outputFilters;
    //      the status of the keyboard + mouse buttons
    VIO32STATUSINFO    keyStatus;
    ///
    APIRET             _removeFilter(VOID *filterData,vio32Filter *&list);
public:

    ///
    VOID                kbdThread();
    ///
    VOID                mouThread();
    ///
    VOID                kbdMonitorThread();
    ///
    VOID                dataProcessingThread();
    ///     do all the general initialization needed for Open
    APIRET              init();
    ///     clean - up all the initialization done by init();
    VOID                finish();
    ///     gets a buffer
    VIO32EVENTINFO       *getBuffer(){
        if (pos >= (&events[MAX_PRIMARY_Q_LEN])) pos = &events[0];
        return pos;
    }
    ///     automatically updates the pointer
    void                advanceBuffer() { pos++; }

    ///
    VIO32STATUSINFO    &KeyStatus()     { return keyStatus; }

public:
    ///
    void                SharedLock()    { DosRequestMutexSem(accessSemaphore,SEM_INDEFINITE_WAIT); }
    ///
    void                SharedUnlock()  { DosReleaseMutexSem(accessSemaphore); }
    ///
    void                LocalLock()     { DosRequestMutexSem(mux,SEM_INDEFINITE_WAIT); }
    ///
    void                LocalUnlock()   { DosReleaseMutexSem(mux); }
    /// this initialization simply connects to the shared memory or creates it
                        kbd32Client();
    ///
                       ~kbd32Client();
    ///
    APIRET              Open();
    ///
    APIRET              Close();
    ///
    APIRET              GetKeyStroke(KBD32KEYINFO *key,ULONG msecTimeout);
    ///
    APIRET              GetMouse(VIO32MOUINFO *mou,ULONG msecTimeout);
    ///
    APIRET              GetEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG msecTimeout);
    ///
    APIRET              PeekEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG position);
    ///
    APIRET              FlushEventQ();
    ///
    APIRET              GetEventQSize(ULONG *size,ULONG msecTimeout);
    ///
    APIRET              WriteEvent(VIO32EVENTINFO *event, ULONG *eventType);
    ///     copies the kbd status to the user provided variable status. Will flush all the events..
    APIRET              GetKeyStatus(VIO32STATUSINFO *status);
    ///
    APIRET              SetMode(ULONG mode){
        if (patched==FALSE)         return VIO32_NOTOPENED;
        if (SharedData == NULL)     return VIO32_SHMEMNOTALLOCATED;
        return SharedData->SetMode(mode,applicationPid);
    }
    ///
    APIRET              GetMode(ULONG *mode){
        if (patched==FALSE)         return VIO32_NOTOPENED;
        if (SharedData == NULL)     return VIO32_SHMEMNOTALLOCATED;
        if (mode==NULL)             return VIO32_INVALIDPARAM;
        return SharedData->GetMode(*mode,applicationPid);
    }
    /// filter content is copied into a new allocated structure
    APIRET              AddFilter(vio32Filter *filter,ULONG &where);
    ///
    APIRET              RemoveFilter(VOID *filterData);

};

extern  kbd32Client kbd32ClientInstance;

// the filter that deals with KBD messages and MOU messages and FOCUS messages in order to produce the correct KBD32 messages and MOU32 messages
extern vio32Filter kbdMapper;
#define KBD32_REMAPPER_NAME "KbdMapper"

#endif
