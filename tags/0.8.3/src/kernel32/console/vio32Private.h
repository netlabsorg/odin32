#if !defined (_KBD32_PRIVATE)
#define _KBD32_PRIVATE

#define INCL_DOS
#include <os2.h>
#include "vio32.h"

// the queue on which all the messages arrive
#define PRIMARY_QUEUE_NAME  "\\QUEUES\\VIO32\\PRIMARY%x"
// the queue on which all the messages arrive
#define SECONDARY_QUEUE_NAME"\\QUEUES\\VIO32\\SECONDARY%x"
// a sh mem containing global KBD32 info
#define SHMEM_NAME          "\\SHAREMEM\\VIO32HOOKX"
// a mux protecting the above mem
#define MUTEX_NAME          "\\SEM32\\VIO32HOOKX"
// max number of VIO32 apps
#define MAX_VIO32 256
// max number of messages on the primary mixing Q
#define MAX_PRIMARY_Q_LEN 4
// max number of messages on the secondary, data elaborated Q
#define MAX_SECONDARY_Q_LEN 64

// not for client use
extern "C" {
    //
    BOOL EXPENTRY InstallHook();
    //
    BOOL EXPENTRY DeInstallHook();
    //
    BOOL EXPENTRY InputHook(HAB hab, PQMSG pQmsg, ULONG fs);
    //
    VOID EXPENTRY SendMsgHook(HAB hab,PSMHSTRUCT psmh,BOOL fInterTask);

}


#endif
