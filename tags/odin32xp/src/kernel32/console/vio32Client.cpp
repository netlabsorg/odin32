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

#include "vio32Client.h"
#include "vio32Private.h"
#include "vio32Utils.h"
#include "vio32SharedData.h"
#include "vio32Errors.h"

extern "C" {
void __cdecl _ctordtorInit();
}

kbd32Client kbd32ClientInstance;

#define USE_EXCEPT 0

#if (USE_EXCEPT==1)
//*****************************************************************************
//
//      Event handler to act on ctrl-c ctrl-break or any other kill message
//
//*****************************************************************************

EXCEPTIONREGISTRATIONRECORD RegRec;

ULONG _System MyTermHandler(  PEXCEPTIONREPORTRECORD       p1,
                              PEXCEPTIONREGISTRATIONRECORD p2,
                              PCONTEXTRECORD               p3,
                              PVOID                        pv ){
    switch( p1->ExceptionNum) {
        case XCPT_PROCESS_TERMINATE:
        case XCPT_ASYNC_PROCESS_TERMINATE: {
            kbd32Close();
        }
        default:;
    }

   return XCPT_CONTINUE_SEARCH;          /* Exception not resolved... */
}

#else
static void APIENTRY vio32Cleanup(ULONG ulReason)
{
   DosBeep(200,100);
   vio32Close();
   DosExitList(EXLST_EXIT, vio32Cleanup);
}

#endif
//*****************************************************************************
//
//      Threads
//
//*****************************************************************************

VOID _Optlink kbdThread(PVOID pParameters){
    kbd32ClientInstance.kbdThread();
}

VOID _Optlink mouThread(PVOID pParameters){
    kbd32ClientInstance.mouThread();
}

VOID _Optlink kbdMonitorThread(void * pParameters){
    kbd32ClientInstance.kbdMonitorThread();
}

VOID _Optlink dataProcessingThread(void * pParameters){
    kbd32ClientInstance.dataProcessingThread();
}

//*****************************************************************************
//
//      kbd32Client::kbd32Client()
//
//*****************************************************************************

kbd32Client::kbd32Client(){
    SharedData              = NULL;
    patched                 = FALSE;
    mux                     = (HMTX)0;
    accessSemaphore         = (HMTX)0;
    primaryQ                = (HQUEUE)0;
    secondaryQ              = (HQUEUE)0;
    primaryQSem             = (HEV)0;
    secondaryQSem           = (HEV)0;
    mon                     = (HMONITOR)0;
    mou                     = (HMOU)0;
    kbdThreadId             = (TID)0;
    mouThreadId             = (TID)0;
    kbdMonitorThreadId      = (TID)0;
    dataProcessingThreadId  = (TID)0;
    threadsMustContinue     = TRUE;
    applicationType         = 0;
    applicationPid          = 0;
    hwndFrame               = 0;
    hwndClient              = 0;
    pos                     = &events[0];
    inputFilters            = NULL;
    outputFilters           = NULL;

    DosCreateMutexSem(NULL,&mux,0,0);

}


//*****************************************************************************
//
//      kbd32Client::~kbd32Client()
//
//*****************************************************************************

kbd32Client::~kbd32Client(){
    Close();
    if (mux != 0) DosCloseMutexSem(mux);

    // deallocate input filters
    while(inputFilters != NULL){
        vio32Filter *f = inputFilters;
        inputFilters = (vio32Filter *)inputFilters->next;
        free(f);
    }

    // deallocate output filters
    while(outputFilters != NULL){
        vio32Filter *f = outputFilters;
        outputFilters = (vio32Filter *)outputFilters->next;
        free(f);
    }
}

//*****************************************************************************
//
//      kbd32Client::init()
//
//*****************************************************************************

APIRET kbd32Client::init(){
    // get application pid and type
    PPIB pib;
    PTIB tib;
    APIRET ret;

    KBDINFO kInfo;
    KbdGetStatus(&kInfo,0);
    kInfo.fsMask |=  (KEYBOARD_BINARY_MODE | KEYBOARD_ASCII_MODE);
    kInfo.fsMask -=  ( KEYBOARD_ASCII_MODE);
    KbdSetStatus(&kInfo,0);

    // GET APPLICATION TYPE AND MORPH THREAD
    ret = DosGetInfoBlocks(&tib, &pib);
    if (ret != 0) return VIO32_GETAPPINFO | ret;
    applicationType = pib->pib_ultype;
    applicationPid  = pib->pib_ulpid;

    // check valid application type
    if ((applicationType != 2) && (applicationType != 3) && (applicationType != 0)) return VIO32_WRONGAPPTYPE;

    // allow PM calls
    if (pib->pib_ultype == 2) pib->pib_ultype = 3;

    // get window information
    if ((applicationType == 2) || (applicationType == 3)){
        hwndClient = VIOWindowClient();
        hwndFrame  = VIOWindowFrame();
    }

#if (USE_EXCEPT==1)
    // SETUP EXCEPTION HANDLER STRUCTURES
    RegRec.prev_structure = NULL;
    RegRec.ExceptionHandler = (ERR)MyTermHandler;
#endif

    // PREPARE PRIMARY QUEUE
    char qName[64];
    sprintf(qName,PRIMARY_QUEUE_NAME,applicationPid);
    ret = DosCreateQueue(&primaryQ,0,qName);
    if (ret != 0) return VIO32_CREATEQUEUE | ret;

    // PREPARE SECONDARY QUEUE
    sprintf(qName,SECONDARY_QUEUE_NAME,applicationPid);
    ret = DosCreateQueue(&secondaryQ,0,qName);
    if (ret != 0) return VIO32_CREATEQUEUE | ret;

    // PRIMARY EVENTS SEM CREATION: to wait for data
    ret = DosCreateEventSem(NULL,&primaryQSem,DC_SEM_SHARED,0);
    if (ret != 0) return VIO32_CREATEHEV | ret;

    // SECONDARY EVENTS SEM CREATION: to wait for data
    ret = DosCreateEventSem(NULL,&secondaryQSem,DC_SEM_SHARED,0);
    if (ret != 0) return VIO32_CREATEHEV | ret;

    // MUTEX SEM CREATION: protect shared mem
    char *  mxName = MUTEX_NAME;
    accessSemaphore = 0;
    ret = DosCreateMutexSem((PSZ)mxName,&accessSemaphore,DC_SEM_SHARED,1);
    if ((ret != 0) && (ret != ERROR_DUPLICATE_NAME)) return VIO32_CREATESHMUX | ret;

    // if it already existed let's get hold of it
    if (ret == ERROR_DUPLICATE_NAME){
        accessSemaphore = 0;
        ret = DosOpenMutexSem((PSZ)mxName,&accessSemaphore);
        if (ret != 0) return VIO32_OPENSHMUX | ret;

        ret = DosRequestMutexSem(accessSemaphore,SEM_INDEFINITE_WAIT);
        if (ret != 0) return VIO32_REQUESTSHMUX | ret;
    }

    // SHARED MEMORY: comunicate from PM
    // at this stage we have the semaphore and it is locked
    // now do the same with the shared memory
    PPVOID  pmem = (void **)&SharedData;
    char *  shName = SHMEM_NAME;
    BOOL    creator = FALSE;
    ret = DosAllocSharedMem(pmem,(PSZ)shName,(sizeof(kbd32SharedData)+4096)&0xFFFFF000,PAG_COMMIT | PAG_WRITE | PAG_READ );
    if ((ret != 0) && (ret != ERROR_ALREADY_EXISTS)){
        SharedUnlock();
        return VIO32_ALLOCSHMEM | ret;
    }

    // if it is already there then lets' simply attach to it
    if (ret == ERROR_ALREADY_EXISTS){
        ret = DosGetNamedSharedMem(pmem,(PSZ)shName,PAG_READ | PAG_WRITE);
        if (ret != 0){
            SharedUnlock();
            return VIO32_GETSHMEM | ret;
        }
    } else creator = TRUE;

    // FUNNY FAILURE
    if (SharedData == NULL){
        SharedUnlock();
        return VIO32_SHMEMNOTALLOCATED;
    }

    // now I have to initialize the memory: I get here only if I created it
    if (creator) SharedData->Init();

    SharedUnlock();

    // start KBD thread
    if (kbdThreadId == 0) kbdThreadId = _beginthread(::kbdThread,NULL,32768,NULL);

    // start KBD thread
    if (dataProcessingThreadId == 0) dataProcessingThreadId = _beginthread(::dataProcessingThread,NULL,32768,NULL);

    // now do application type specifics
    if (applicationType == 0){

        // PREPARE MONITOR
        ret = DosMonOpen("KBD$",&mon);
        if (ret != 0)  return VIO32_MONOPEN | ret;

        monin.cb = sizeof(monin);
        monout.cb = sizeof(monout);
        ret = DosMonReg(mon,(BYTE *)&monin,(BYTE *)&monout,0,-1);
        if (ret != 0) return VIO32_MONREG | ret;

        // now start mon thread
        if (kbdMonitorThreadId == 0) kbdMonitorThreadId = _beginthread(::kbdMonitorThread,NULL,32768,NULL);
    }

    // reset keyboard status
    KBD32_RESETKEYS(keyStatus);

    // add default filters
    ULONG where = VIO32_PRE_FIRST;
    AddFilter(&kbdMapper,where);

    // prepare mouse support
    // Get mouse handle
    ret = MouOpen(NULL, &mou);
    if (ret != 0)  return VIO32_MOUOPEN | ret;

    // Enable mouse
    ret = MouDrawPtr(mou);
    if (ret != 0)  return VIO32_MOUDRAWPTR | ret;

    // Enable all events fails on WSEB ??
//    USHORT mask = 0x7F;
//    ret = MouSetEventMask(&mask,mou);
//    if (ret != 0)  return VIO32_MOUSETEVENTMASK | ret;

    // now start mon thread
    if (mouThreadId == 0) mouThreadId = _beginthread(::mouThread,NULL,32768,NULL);

    // success
    return 0;
}

//*****************************************************************************
//
//      kbd32Client::finish()
//
//*****************************************************************************

VOID kbd32Client::finish(){
    threadsMustContinue = FALSE;
    if (primaryQSem != 0)       DosCloseEventSem(primaryQSem);
    if (secondaryQSem != 0)     DosCloseEventSem(secondaryQSem);
    if (primaryQ != 0)          DosCloseQueue(primaryQ);
    if (secondaryQ != 0)        DosCloseQueue(secondaryQ);
    if (accessSemaphore != 0)   DosCloseMutexSem(accessSemaphore);
    if (mon != 0)               DosMonClose(mon);
    if (SharedData != NULL)     DosFreeMem(SharedData);
    if (mou != 0)               MouClose(mou);
#if (USE_EXCEPT==1)
    DosUnsetExceptionHandler(&RegRec);
#endif
}

//*****************************************************************************
//
//      kbd32Client::Open()
//
//*****************************************************************************

APIRET kbd32Client::Open(){
    if (patched) return VIO32_ALREADYOPENED;

    // serialize
    LocalLock();

    APIRET ret;

#if (USE_EXCEPT==1)
    // ACTIVATE EXCEPTION HANDLER
    ret = DosSetExceptionHandler(&RegRec);
    if (ret != 0){
        LocalUnlock();
        return ret  | VIO32_SETEXCHANDLER;
    }
#else
    APIRET rc = DosExitList(0x00008000|EXLST_ADD, vio32Cleanup);

#endif

    // INITIALIZE THE SYSTEM
    ret = init();
    if (ret != 0){
        finish();
        LocalUnlock();
        return ret;
    }

    // INSTALL APPLICATION
    ULONG nesting;
    DosEnterMustComplete(&nesting);
    SharedLock();
    ret = SharedData->InstallApplication(applicationPid,((applicationType == 2) || (applicationType == 3)));
    SharedUnlock();
    DosExitMustComplete(&nesting);

    if (ret != 0){
        finish();
        LocalUnlock();
        return ret;
    }

    patched = TRUE;

    LocalUnlock();
    return 0;
}


//*****************************************************************************
//
//      kbd32Client::Close()
//
//*****************************************************************************

APIRET kbd32Client::Close(){
    if (patched    == FALSE) return VIO32_NOTOPENED;
    if (SharedData == NULL)  return VIO32_SHMEMNOTALLOCATED;

    // remove settings on memory
    ULONG nesting;
    DosEnterMustComplete(&nesting);
    LocalLock();
    SharedLock();
    SharedData->UninstallApplication(applicationPid);
    SharedUnlock();
    LocalLock();
    DosExitMustComplete(&nesting);

    finish();
    patched = FALSE;

    return 0;
}

//*****************************************************************************
//
//      kbd32Client::AddFilter()
//
//*****************************************************************************

APIRET kbd32Client::AddFilter(vio32Filter *filter,ULONG &where){
    if (filter == NULL) return VIO32_INVALIDPARAM;

    LocalLock();

    // MAKE A COPY
    vio32Filter *f = (vio32Filter *)malloc(sizeof(vio32Filter));
    *f = *filter;
    f->next = NULL;

    vio32Filter **list = &inputFilters;
    // select list
    if (where & VIO32_POST_FIRST){
        list = &outputFilters;
    }

    // now look only at the position
    ULONG pos = where & 0x0000FFFF;

    if (((*list) == NULL)||(pos==0)) {
        pos = 0;
        f->next = (*list);
        (*list) = f;
    } else {
        ULONG index = 1;
        vio32Filter *q = (*list);
        while((pos > index)&&(q->next != NULL)){
            index++;
            q=(vio32Filter *)q->next;
        }
        f->next = q->next;
        q->next = f;
        pos = index;
    }

    where &= 0xFFFF0000;
    where += pos;

    LocalUnlock();
    return 0;
}

APIRET kbd32Client::_removeFilter(VOID *filterData,vio32Filter *&list){
    BOOL found = FALSE;

    if (list != NULL){
        vio32Filter *pf = list;
        if (list->filterData == filterData){
            list = (vio32Filter *)list->next;
            free(pf);
            found = TRUE;
        } else {
            while ((pf->next != NULL)&&(((vio32Filter *)pf->next)->filterData != filterData)) pf = (vio32Filter *)pf->next;
            if (pf->next != NULL){
                vio32Filter *pf2 = (vio32Filter *)pf->next;
                pf->next = pf2->next;
                free(pf2);
                found = TRUE;
            }
        }
    }

    if (!found) return VIO32_NOTFOUND;
    return 0;
}

//*****************************************************************************
//
//      kbd32Client::RemoveFilter()
//
//*****************************************************************************

APIRET kbd32Client::RemoveFilter(VOID *filterData){
    if (filterData == NULL) return VIO32_INVALIDPARAM;

    LocalLock();

    APIRET found = _removeFilter(filterData,inputFilters);
    if (found == VIO32_NOTFOUND)
    found = _removeFilter(filterData,outputFilters);

    LocalUnlock();

    return found;
}

//*****************************************************************************
//
//      kbd32Client::GetEvent()
//
//*****************************************************************************

APIRET kbd32Client::GetEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG msecTimeout){

    if (event == NULL)      return VIO32_INVALIDPARAM;
    if (eventType == NULL)  return VIO32_INVALIDPARAM;

    BOOL            dataUsed = TRUE;
    REQUESTDATA     request;
    ULONG           dataSize;
    VIO32EVENTINFO *data;
    BYTE            priority;
    while(dataUsed == TRUE){
        // exclusive access
        LocalLock();

        APIRET rc = DosReadQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,secondaryQSem);

        if ((rc != 0)&&(rc != ERROR_QUE_EMPTY)){
            LocalUnlock();
            return rc | VIO32_READQUEUE;
        }

        if (rc == ERROR_QUE_EMPTY){
            LocalUnlock();
            rc= DosWaitEventSem(secondaryQSem,msecTimeout);
            if (rc == ERROR_TIMEOUT) return rc;
            if (rc != 0) return rc | VIO32_WAITHEV;

            LocalLock();
            rc = DosReadQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,secondaryQSem);
            if (rc != 0){
                LocalUnlock();
                return rc | VIO32_READQUEUE;
            }
        }

        // PROCESS THE DATA here!
        vio32Filter *f = outputFilters;
        dataUsed = FALSE;
        while ((f != NULL)&&(!dataUsed)){
//FISA
//printf("output processing using %s\n",f->filterData);
            dataUsed = f->filter(&request.ulData,data,f->filterData);
            f = (vio32Filter *)f->next;
        }

        // if we are about to exit copy data
        if (!dataUsed){
            *eventType = request.ulData;
            *event = *data;
        }

        LocalUnlock();
    }

    return 0;
}

//*****************************************************************************
//
//      kbd32Client::PeekEvent()
//
//*****************************************************************************

APIRET kbd32Client::PeekEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG position){

    if (event == NULL)      return VIO32_INVALIDPARAM;
    if (eventType == NULL)  return VIO32_INVALIDPARAM;

    REQUESTDATA     request;
    ULONG           dataSize;
    VIO32EVENTINFO *data;
    BYTE            priority;
    // exclusive access
    LocalLock();

    ULONG pos = 0;
    // start from the top
    APIRET rc = DosPeekQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,&pos,DCWW_NOWAIT, &priority,0);

    // move on if necessary
    while((rc==0)&&(pos != position)){
        pos = 1;
        rc = DosPeekQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,&pos,DCWW_NOWAIT, &priority,0);
    }

    if (rc != 0){
        LocalUnlock();
        return rc | VIO32_READQUEUE;
    }

    *eventType = request.ulData;
    *event = *data;
    LocalUnlock();

    return 0;
}

//*****************************************************************************
//
//      kbd32Client::FlushEventQ()
//
//*****************************************************************************

APIRET kbd32Client::FlushEventQ(){

    REQUESTDATA     request;
    ULONG           dataSize;
    VIO32EVENTINFO *data;
    BYTE            priority;
    // exclusive access
    LocalLock();

    ULONG pos = 0;
    // start from the top
    APIRET rc = DosReadQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,0);
    // move on if necessary
    while(rc==0) rc = DosReadQueue(secondaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,0);

    LocalUnlock();

    return 0;
}

//*****************************************************************************
//
//      kbd32Client::GetEventQSize()
//
//*****************************************************************************

APIRET kbd32Client::GetEventQSize(ULONG *size,ULONG msecTimeout){

    if (size == NULL)      return VIO32_INVALIDPARAM;

    // exclusive access
    LocalLock();

    APIRET rc = DosQueryQueue(secondaryQ,size);

    LocalUnlock();

    return rc;
}

//*****************************************************************************
//
//      kbd32Client::GetKeyStroke()
//
//*****************************************************************************

APIRET kbd32Client::GetKeyStroke(KBD32KEYINFO *key,ULONG msecTimeout){

    if (key == NULL) return VIO32_INVALIDPARAM;

    VIO32EVENTINFO event;
    ULONG eventType;
    APIRET ret;

    // flush all events until a key event is reached
    while(1){
        ret = GetEvent(&event,&eventType,0);
        // no more events in Q, wait for next event
        if (ret == ERROR_TIMEOUT) {
            ret = GetEvent(&event,&eventType,msecTimeout);
            if (ret != 0 ) return ret;
            // not a key event, must abort otw I will loop forever
            if (eventType != KBD32_CHAR) return VIO32_WRONGEVENTTYPE;
            // success!
            *key = event.vio32Kbd;
            return 0;
        }
        // some error, not a timeout
        if (ret != 0 ) return ret;
        if (eventType == KBD32_CHAR){
            *key = event.vio32Kbd;
            return 0;
        }
    }

    // will never each this code
    return 0;
}

//*****************************************************************************
//
//      kbd32Client::GetMouse()
//
//*****************************************************************************

APIRET kbd32Client::GetMouse(VIO32MOUINFO *mou,ULONG msecTimeout){

    if (mou == NULL) return VIO32_INVALIDPARAM;

    VIO32EVENTINFO event;
    ULONG eventType;
    APIRET ret;

    // flush all events until a key event is reached
    while(1){
        ret = GetEvent(&event,&eventType,0);
        // no more events in Q, wait for next event
        if (ret == ERROR_TIMEOUT) {
            ret = GetEvent(&event,&eventType,msecTimeout);
            if (ret != 0 ) return ret;
            // not a key event, must abort otw I will loop forever
            if (eventType != VIO32_MOU) return VIO32_WRONGEVENTTYPE;
            // success!
            *mou = event.vio32Mou;
            return 0;
        }
        // some error, not a timeout
        if (ret != 0 ) return ret;
        if (eventType == VIO32_MOU){
            *mou = event.vio32Mou;
            return 0;
        }
    }

    // will never each this code
    return 0;
}

//*****************************************************************************
//
//      kbd32Client::WriteEvent()
//
//*****************************************************************************

APIRET kbd32Client::WriteEvent(VIO32EVENTINFO *event, ULONG *eventType){

    if (event == NULL)      return VIO32_INVALIDPARAM;
    if (eventType == NULL)  return VIO32_INVALIDPARAM;

    ULONG nesting;
    DosEnterMustComplete(&nesting);
    SharedLock();

    HWND hwndClient;
    HWND hwndFrame;
    int index;
    APIRET ret = 0;
    if (SharedData->searchByPid(applicationPid,hwndClient,hwndFrame,index)){

        // now check how many entries are busy
        ULONG entries = 0;
        ret = DosQueryQueue(primaryQ, &entries);

        // if too many are used bleep
        if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
            ret = VIO32_QUEUEFULL;
            DosBeep(1000,20);
        }
        // if all is good send data
        if (ret == 0){
            // allocate a Q object.
            VIO32EVENTINFO *ev = SharedData->getBuffer(index);
            *ev = *event;
            // send the data
            ret = DosWriteQueue(primaryQ,*eventType,sizeof(VIO32EVENTINFO),ev,0);
            if (ret != 0) ret |= VIO32_WRITEQUEUE;
        }
        // now mark the buffer as used
        if (ret == 0){
            SharedData->advanceBuffer(index);
        }
    } else ret = VIO32_UNREGISTERED;

    SharedUnlock();
    DosExitMustComplete(&nesting);

    return ret;
};

//*****************************************************************************
//
//      kbd32Client::kbdThread()
//
//*****************************************************************************

APIRET kbd32Client::GetKeyStatus(VIO32STATUSINFO *status){
    if (status == NULL) return VIO32_INVALIDPARAM;
    APIRET ret = 0;
    VIO32EVENTINFO event;
    ULONG eventType;
    while(ret == 0) ret = GetEvent(&event,&eventType,0);
    if (ret == ERROR_TIMEOUT) ret = 0;
    LocalLock();
    *status = keyStatus;
    LocalUnlock();
    return ret;
}

//*****************************************************************************
//
//      kbd32Client::kbdThread()
//
//*****************************************************************************

VOID kbd32Client::kbdThread(){
    DosSetPriority(PRTYS_THREAD,PRTYC_FOREGROUNDSERVER,10,0);

    // finish together with the kbd monitor
    while(threadsMustContinue){
        KBDKEYINFO charData;
        APIRET ret = KbdCharIn(&charData,0,0);
        if (!threadsMustContinue) break;
        if (ret != 0) return;
        if (ret == 0){
            ULONG nesting;
            DosEnterMustComplete(&nesting);
            SharedLock();

            HWND hwndClient;
            HWND hwndFrame;
            int index;
            if (SharedData->searchByPid(applicationPid,hwndClient,hwndFrame,index)){

                // now check how many entries are busy
                ULONG entries = 0;
                ret = DosQueryQueue(primaryQ, &entries);

                // if too many are used bleep
                if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
                    ret = 1;
                    DosBeep(1000,20);
                }
                // if all is good send data
                if (ret == 0){
                    // allocate a Q object.
                    VIO32EVENTINFO *key = SharedData->getBuffer(index);
                    key->vioKbd = charData;
                    // send the data
                    ret = DosWriteQueue(primaryQ,VIO_CHAR,sizeof(VIO32EVENTINFO),key,0);
                }
                // now mark the buffer as used
                if (ret == 0){
                    SharedData->advanceBuffer(index);
                }
            }

            SharedUnlock();
            DosExitMustComplete(&nesting);
        }
    }
}

//*****************************************************************************
//
//      kbd32Client::mouThread()
//
//*****************************************************************************

VOID kbd32Client::mouThread(){
    DosSetPriority(PRTYS_THREAD,PRTYC_FOREGROUNDSERVER,10,0);

    MOUEVENTINFO newMouEvent;
    MOUEVENTINFO outStandingMouEvent;
    BOOL hasOutStanding = FALSE;
    USHORT wait = 1;
    USHORT noWait = 0;
    // finish together with the kbd monitor
    while(threadsMustContinue){
        APIRET ret = 0;
        // check if there is a pending mouse event
        if (hasOutStanding == FALSE){
            // if not read a new and wait if it is not available
            ret = MouReadEventQue(&newMouEvent,&wait,mou);
            if (!threadsMustContinue) break;
        } else {
            // if an event was waiting use it as a new event
            newMouEvent = outStandingMouEvent;
        }
        hasOutStanding = FALSE;
        // now collapse all movements into a single event
        if (ret == 0){
            // stop on error or if a different event was encountered and stored
            while((ret == 0) && (hasOutStanding == FALSE)){
                // empty the Q
                ret = MouReadEventQue(&outStandingMouEvent,&noWait,mou);
                // an empty event does not produce error, so I generate one myself
                if (outStandingMouEvent.time == 0) ret = 1;
                if (ret == 0){
//printf("@%i %i %i %i\n",newMouEvent.time,newMouEvent.fs,newMouEvent.col,newMouEvent.row);
//printf("@%i %i %i %i\n",outStandingMouEvent.time,outStandingMouEvent.fs,outStandingMouEvent.col,outStandingMouEvent.row);
                    // bug fix for VIO window and spurius mouse move messages
                    if (((outStandingMouEvent.fs & 0x1)!= 0) &&
                        (newMouEvent.col == outStandingMouEvent.col) &&
                        (newMouEvent.row == outStandingMouEvent.row)) outStandingMouEvent.fs -= 0x1;
//printf("@%i %i %i %i\n",outStandingMouEvent.time,outStandingMouEvent.fs,outStandingMouEvent.col,outStandingMouEvent.row);

                    // if the status is different remember the event for later
                    if (outStandingMouEvent.fs != newMouEvent.fs) hasOutStanding = TRUE;
                    // just keep the last event
                    else newMouEvent = outStandingMouEvent;
                }
            }
            ret = 0;
        }


        // now Q the new event
        if (ret == 0){
            ULONG nesting;
            DosEnterMustComplete(&nesting);
            SharedLock();

            HWND hwndClient;
            HWND hwndFrame;
            int index;
            if (SharedData->searchByPid(applicationPid,hwndClient,hwndFrame,index)){

                // get mode of operation of given app
                ULONG mode = SharedData->getMode(index);

//printf("@%i %i %i %i\n",newMouEvent.time,newMouEvent.fs,newMouEvent.col,newMouEvent.row);
                // see whether to filter out messages
                static ULONG lastMouseStatus = 0;
                if ((mode & KBD32MODE_ENABLEMOUSE)==0) ret = 1;
                ULONG newMouseStatus = (newMouEvent.fs & 0x54) | ((newMouEvent.fs & 0x2A)<<1);
                if ((mode & KBD32MODE_ENABLEMMOVE)==0){
                    newMouEvent.fs = newMouseStatus;
                    if (newMouEvent.fs == lastMouseStatus) ret = 1;
                }
                lastMouseStatus = newMouseStatus;

                // now check how many entries are busy
                ULONG entries = 0;
                if (ret == 0)
                    ret = DosQueryQueue(primaryQ, &entries);

                // if too many are used bleep
                if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
                    ret = 1;
                    DosBeep(1000,20);
                }

                // if all is good send data
                if (ret == 0){
                    // allocate a Q object.
                    VIO32EVENTINFO *key = SharedData->getBuffer(index);
                    key->vioMou = newMouEvent;
                    // send the data
                    ret = DosWriteQueue(primaryQ,VIO_MOU,sizeof(VIO32EVENTINFO),key,0);
                }
                // now mark the buffer as used
                if (ret == 0){
                    SharedData->advanceBuffer(index);
                }
            }

            SharedUnlock();
            DosExitMustComplete(&nesting);
        }
    }
}

//*****************************************************************************
//
//      kbd32Client::kbdMonitorThread()
//
//*****************************************************************************


void kbd32Client::kbdMonitorThread(){
    DosSetPriority(PRTYS_THREAD,PRTYC_FOREGROUNDSERVER,10,0);
    //
    while(threadsMustContinue){
        MonRecord monData;
        USHORT bufSize = sizeof(monData);
        APIRET ret = DosMonRead((PBYTE)&monin,DCWW_WAIT,(PBYTE)&monData,&bufSize);
        if (!threadsMustContinue) break;

        BOOL dataUsed = (ret != 0);
        if (ret == 0){
            ULONG nesting;
            DosEnterMustComplete(&nesting);
            SharedLock();

            HWND hwndClient;
            HWND hwndFrame;
            int index;
            if (SharedData->searchByPid(applicationPid,hwndClient,hwndFrame,index)){

                // get mode of operation of given app
                ULONG mode = SharedData->getMode(index);

                // bypass certain keys
                ULONG specialKeys = monData.KbdDDFlagWord & 0x003F;
                if (specialKeys != 0){
                    ret = 1;
                    // pass all the CTRL/SHIFT/ALT/N-LOCK... events
                    if (specialKeys == 0x7) ret = 0;
                    if ((mode & KBD32MODE_DISABLEBREAK) && (specialKeys == 0x11)) ret = 0;
                }

                // now check how many entries are busy
                ULONG entries = 0;
                if (ret == 0){
                    ret = DosQueryQueue(primaryQ, &entries);
                }

                // if too many are used bleep
                if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
                    ret = 1;
                    DosBeep(1000,20);
                }

                // if all is good send data
                if (ret == 0){
                    // allocate a Q object.
                    VIO32EVENTINFO *key = SharedData->getBuffer(index);
                    key->monKbd = monData;
                    // send the data
                    ret = DosWriteQueue(primaryQ,MON_CHAR,sizeof(VIO32EVENTINFO),key,0);
                }
                // now mark the buffer as used
                if (ret == 0){
                    SharedData->advanceBuffer(index);
                    dataUsed = TRUE;
                }
            }
            SharedUnlock();
            DosExitMustComplete(&nesting);
        }
        if (dataUsed == FALSE){
/*
            if ((monData.KbdDDFlagWord & 0x7) == 0x7){
                monData.KbdDDFlagWord &= 0xFFF8;
                monData.Shift_State |= 0x2000; // means special char remapped -64
            }
            if (monData.KbdDDFlagWord & 0x0040){
                monData.KbdDDFlagWord &= 0xFFBF;
                monData.Shift_State |= 0x1000; // use  KBDSTF_SCROLLLOCK to say KEY-UP
            }
            CHAR scanCode = monData.MonFlagWord >> 8;
            monData.XlatedScan =  scanCode;
printf(">>%c %i 0x%x 0x%x 0x%x\n",monData.XlatedChar,monData.XlatedScan,
                        monData.MonFlagWord,monData.Shift_State,monData.KbdDDFlagWord);
*/
            DosMonWrite((PBYTE)&monout,(PBYTE)&monData,14);
        }
    }
}

//*****************************************************************************
//
//      kbd32Client::dataProcessingThread()
//
//*****************************************************************************


VOID kbd32Client::dataProcessingThread(){
    DosSetPriority(PRTYS_THREAD,PRTYC_FOREGROUNDSERVER,10,0);

    // the message read
    VIO32EVENTINFO queuedData;

    //
    while(threadsMustContinue){
        // exclusive access to primary Q
        SharedLock();

        // READ DATA OFF the primary Q
        REQUESTDATA request;
        ULONG dataSize;
        VIO32EVENTINFO *data;
        BYTE priority;
        APIRET rc = DosReadQueue(primaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,primaryQSem);
        if (!threadsMustContinue) break;
        if (rc == ERROR_QUE_EMPTY){
            SharedUnlock();
            rc= DosWaitEventSem(primaryQSem,-1);
            SharedLock();

            if (rc == 0)
                rc = DosReadQueue(primaryQ,&request,&dataSize,(PPVOID)&data,0,DCWW_NOWAIT, &priority,primaryQSem);
        }

        // copy data before releasing the semaphore
        if (rc == 0) queuedData = *data;

        // relinquish primary Q
        SharedUnlock();


        // exclusive access to secondary Q
        LocalLock();

        // now check how many entries are busy
        ULONG entries = 0;
        if (rc == 0) rc = DosQueryQueue(secondaryQ, &entries);

        // if too many are used bleep
        if ((rc == 0) && (entries >= MAX_SECONDARY_Q_LEN)){
            rc = 1;
            DosBeep(2000,20);
        }

        // now allocate a space in the secondary Q
        VIO32EVENTINFO *key;
        if (rc == 0){
            // allocate a Q object.
            key = getBuffer();
            //
            if (key == NULL) rc = 1;
        }

        // PROCESS THE DATA here!
        vio32Filter *f = inputFilters;
        BOOL dataUsed = FALSE;
        if (rc==0)
            while ((f != NULL)&&(!dataUsed)){
//FISA
//printf("INPUT processing with %s\n",f->filterData);
                dataUsed = f->filter(&request.ulData,&queuedData,f->filterData);
                f = (vio32Filter *)f->next;
            }

        // if all is right and the data was not consumed send it forward
        if ((rc == 0) && (!dataUsed)){
            // copy content
            *key = queuedData;
            // send the data
            rc = DosWriteQueue(secondaryQ,request.ulData,sizeof(VIO32EVENTINFO),key,0);

            // now mark the buffer as used
            if (rc == 0) advanceBuffer();
        }

        // free secondary system
        LocalUnlock();

        if ((rc != 0) && (rc != ERROR_TIMEOUT)) return;
    }
}


