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

#include "vio32Private.h"
#include "vio32Utils.h"
#include "vio32SharedData.h"
#include "vio32Client.h"


APIRET EXPENTRY vio32Open(){
    return kbd32ClientInstance.Open();
}

APIRET EXPENTRY vio32Close(){
    return kbd32ClientInstance.Close();
}

APIRET EXPENTRY kbd32GetKey(KBD32KEYINFO *key,ULONG msecTimeout){
    return kbd32ClientInstance.GetKeyStroke(key,msecTimeout);
}

APIRET EXPENTRY vio32GetMouse(VIO32MOUINFO *mou,ULONG msecTimeout){
    return kbd32ClientInstance.GetMouse(mou,msecTimeout);
}

APIRET EXPENTRY vio32GetEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG msecTimeout){
    return kbd32ClientInstance.GetEvent(event,eventType,msecTimeout);
}

APIRET EXPENTRY vio32PeekEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG position){
    return kbd32ClientInstance.PeekEvent(event,eventType,position);
}

APIRET EXPENTRY vio32FlushEventQ(){
    return kbd32ClientInstance.FlushEventQ();
}

APIRET EXPENTRY vio32GetEventQSize(ULONG *size,ULONG msecTimeout){
    return kbd32ClientInstance.GetEventQSize(size,msecTimeout);
}

APIRET EXPENTRY vio32WriteEvent(VIO32EVENTINFO *event, ULONG *eventType){
    return kbd32ClientInstance.WriteEvent(event,eventType);
}

APIRET EXPENTRY kbd32GetKeyStatus(VIO32STATUSINFO *status){
    return kbd32ClientInstance.GetKeyStatus(status);
}

APIRET EXPENTRY kbd32SetMode(ULONG mode){
    return kbd32ClientInstance.SetMode(mode);
}

APIRET EXPENTRY kbd32GetMode(ULONG *mode){
    return kbd32ClientInstance.GetMode(mode);
}

APIRET kbd32AddFilter(vio32Filter *filter,ULONG *where){
    ULONG defaultPos=VIO32_PRE_FIRST;
    if (where == NULL) where = &defaultPos;
    return kbd32ClientInstance.AddFilter(filter,*where);
}

APIRET kbd32RemoveFilter(VOID *filterData){
    return kbd32ClientInstance.RemoveFilter(filterData);
}

// hook stuff
HMODULE module  = 0;
PFN     pfn1     = NULL;
PFN     pfn2     = NULL;

BOOL EXPENTRY InstallHook(){
    // already done!
    if (module != 0) return FALSE;

    // already done!
    if (pfn1 != NULL) return FALSE;

    // already done!
    if (pfn2 != NULL) return FALSE;

    // now morph into PM on this thread in acse it didn't happen before
    PPIB pib;
    PTIB tib;
    DosGetInfoBlocks(&tib, &pib);
    if (pib->pib_ultype == 2) pib->pib_ultype = 3;

    // this code can be run only from a VIO app
    if (pib->pib_ultype != 3) return FALSE;

    // get anchor block
    HAB hab = WinInitialize(0);

    // now get the message Queue
    HMQ queue = WinQueryWindowULong(VIOWindowClient(), QWL_HMQ);

    // now get my own module handle
    char buffer[256];
    APIRET rc = DosLoadModule(buffer,256,"vio32.dll",&module);
    if (rc != 0){
        printf("DosLoadModule rc is %i\n", rc);
        return FALSE;
    }

    // get function pointer
    rc = DosQueryProcAddr(module,1,NULL,&pfn1);
    if (rc != 0){
        printf("DosQueryProcAddr(2) rc is %i\n", rc);
        return FALSE;
    }

    BOOL ret = WinSetHook(hab,queue,HK_INPUT,pfn1,module);
    if (!ret){
        printf("WinSetHook(1) failed\n");
        return FALSE;
    }

    // get function pointer
    rc = DosQueryProcAddr(module,2,NULL,&pfn2);
    if (rc != 0){
        printf("DosQueryProcAddr(2) rc is %i\n", rc);
        return FALSE;
    }

    ret = WinSetHook(hab,queue,HK_SENDMSG,pfn2,module);
    if (!ret){
        printf("WinSetHook(2) failed\n");
        return FALSE;
    }

    return (rc == 0);
}

BOOL EXPENTRY DeInstallHook(){
    // never installed
    if (module == 0) return FALSE;

    // never installed
    if (pfn1 == NULL) return FALSE;

    // never installed
    if (pfn2 == NULL) return FALSE;

    // now morph into PM on this thread in acse it didn't happen before
    PPIB pib;
    PTIB tib;
    DosGetInfoBlocks(&tib, &pib);
    if (pib->pib_ultype == 2) pib->pib_ultype = 3;

    // this code can be run only from a VIO app
    if (pib->pib_ultype != 3) return FALSE;

    // get anchor block
    HAB hab = WinInitialize(0);

    // now get the message Queue
    HMQ queue = WinQueryWindowULong(VIOWindowClient(), QWL_HMQ);

    // finally remove hook
    BOOL ret = WinReleaseHook(hab,queue,HK_INPUT,pfn1,module);

    // remove hook 2
    ret &= WinReleaseHook(hab,queue,HK_SENDMSG,pfn2,module);

    // deallocate the DLL
    DosFreeModule(module);
    module = NULL;
    pfn1   = NULL;
    pfn2   = NULL;

    return ret;
}


BOOL  EXPENTRY InputHook(HAB hab, PQMSG pQmsg, ULONG fs){

    // take only mouse and kbd messages
//    if ((pQmsg->msg < WM_MOUSEMOVE) || (pQmsg->msg > WM_CHAR)) return FALSE;
    if (pQmsg->msg != WM_CHAR) return FALSE;

    // now grab mutex sem
    APIRET ret;
    HMTX   hmtx = 0;
    BOOL   locked = FALSE;
    ret = DosOpenMutexSem((PSZ)MUTEX_NAME,&hmtx);
    if (ret == 0){
        ret = DosRequestMutexSem(hmtx,SEM_INDEFINITE_WAIT);
        if (ret == 0) locked = TRUE;
    }

    // grab shared mem
    kbd32SharedData *SharedData = NULL;
    BOOL haveMem = FALSE;
    if (ret == 0){
        ret = DosGetNamedSharedMem((PPVOID)&SharedData,(PSZ)SHMEM_NAME,PAG_READ | PAG_WRITE);
        // ok I have the pointer to the memory I am ok! let's release the memory
        if (ret == 0) haveMem = TRUE;
    }

    // now send the key stroke
    BOOL dataUsed = FALSE;
    if (ret == 0){
        // find if the application is registered
        PID pid;
        int index;
        HWND hwndFrame;
        if (SharedData->searchByHwndClient(pQmsg->hwnd,hwndFrame,pid,index)){
            // get mode of operation of given app
            ULONG mode = SharedData->getMode(index);

            // treat specific WM_CHAR msg
            if (pQmsg->msg == WM_CHAR){
                // trasnform qmsg in the specific msg
                CHRMSG *wmchar = (CHRMSG *)&pQmsg->mp1;
                // on special modes abort
                if (((mode & KBD32MODE_DISABLEBREAK)==0) && (wmchar->scancode == 110)) ret = 1;
            }
//            else // mouse move
//            if (pQmsg->msg == WM_MOUSEMOVE){
//                if ((mode & KBD32MODE_ENABLEMMOVE)==0) ret = 1;
//            } else // mouse clicks
//                if ((mode & KBD32MODE_ENABLEMOUSE)==0) ret = 1;


            HQUEUE keyQ = 0;

            // get the associated Q
            if (ret == 0){
                PID pid_;
                char qName[64];
                sprintf(qName,PRIMARY_QUEUE_NAME,pid);
                ret = DosOpenQueue(&pid_,&keyQ,qName);
            }

            // now check how many entries are busy
            ULONG entries = 0;
            if (ret == 0) ret = DosQueryQueue(keyQ, &entries);

            // if too many are used bleep
            if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
                ret = 1;
                DosBeep(1000,20);
            }
            // if all is good send data
            if (ret == 0){
                // allocate a Q object.
                VIO32EVENTINFO *key = SharedData->getBuffer(index);
                // copy data
                key->pmMsg.mp1 = pQmsg->mp1;
                key->pmMsg.mp2 = pQmsg->mp2;
                // send the data
                ret = DosWriteQueue(keyQ,pQmsg->msg,sizeof(VIO32EVENTINFO),key,0);
            }
            // now mark the buffer as used
            if (ret == 0){
                SharedData->advanceBuffer(index);
                dataUsed = TRUE;
            }
            // close Q
            if (keyQ != 0) DosCloseQueue(keyQ);
        }
    }

    // release memory
    if (haveMem) DosFreeMem((PVOID)SharedData);
    haveMem = FALSE;

    // relase mutex sem
    if (locked) DosReleaseMutexSem(hmtx);
    locked = FALSE;

    // let click do focus change
    if ((pQmsg->msg >= 0x71) && (pQmsg->msg <= 0x79)) return FALSE;
    return dataUsed;
}

VOID EXPENTRY SendMsgHook(HAB hab,PSMHSTRUCT psmh,BOOL fInterTask){
    if (fInterTask) return ;

    // filter messages
    BOOL toBeProcessed = FALSE;
    if (psmh->msg == WM_FOCUSCHANGE) toBeProcessed = TRUE;
    if (psmh->msg == WM_SIZE)        toBeProcessed = TRUE;
    if (psmh->msg == WM_MOVE)        toBeProcessed = TRUE;

    if (!toBeProcessed) return;

    // now grab mutex sem
    APIRET ret;
    HMTX   hmtx = 0;
    BOOL   locked = FALSE;
    ret = DosOpenMutexSem((PSZ)MUTEX_NAME,&hmtx);
    if (ret == 0){
        ret = DosRequestMutexSem(hmtx,SEM_INDEFINITE_WAIT);
        if (ret == 0) locked = TRUE;
    }

    // grab shared mem
    kbd32SharedData *SharedData = NULL;
    BOOL haveMem = FALSE;
    if (ret == 0){
        ret = DosGetNamedSharedMem((PPVOID)&SharedData,(PSZ)SHMEM_NAME,PAG_READ | PAG_WRITE);
        // ok I have the pointer to the memory I am ok! let's release the memory
        if (ret == 0) haveMem = TRUE;
    }

    // now send the key stroke
    BOOL dataUsed = FALSE;
    if (ret == 0){
        // find if the application is registered
        PID pid;
        int index;
        HWND hwndClient;
        if (SharedData->searchByHwndFrame(psmh->hwnd,hwndClient,pid,index)){

            HQUEUE keyQ = 0;

            // get the associated Q
            if (ret == 0){
                PID pid_;
                char qName[64];
                sprintf(qName,PRIMARY_QUEUE_NAME,pid);
                ret = DosOpenQueue(&pid_,&keyQ,qName);
            }

            // now check how many entries are busy
            ULONG entries = 0;
            if (ret == 0) ret = DosQueryQueue(keyQ, &entries);

            // if too many are used bleep
            if ((ret == 0) && (entries >= MAX_PRIMARY_Q_LEN)){
                ret = 1;
                DosBeep(1000,20);
            }
            // if all is good send data
            if (ret == 0){
                // allocate a Q object.
                VIO32EVENTINFO *key = SharedData->getBuffer(index);
                // copy content
                key->pmMsg.mp1 = psmh->mp1;
                key->pmMsg.mp2 = psmh->mp2;
                // send the data
                ret = DosWriteQueue(keyQ,psmh->msg,sizeof(VIO32EVENTINFO),key,0);
            }
            // now mark the buffer as used
            if (ret == 0){
                SharedData->advanceBuffer(index);
                dataUsed = TRUE;
            }
            // close Q
            if (keyQ != 0) DosCloseQueue(keyQ);
        }
    }

    // release memory
    if (haveMem) DosFreeMem((PVOID)SharedData);
    haveMem = FALSE;

    // relase mutex sem
    if (locked) DosReleaseMutexSem(hmtx);
    locked = FALSE;


    return ;
}




