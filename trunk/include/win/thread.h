/*
 * Thread definitions
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_THREAD_H
#define __WINE_THREAD_H

#ifdef __WIN32OS2__
#define TLS_MINIMUM_AVAILABLE   64
 #ifndef __OS2_H__
  #include "config.h"
  #include "winbase.h"
  #include "winuser.h"
  #include <ntdef.h>
 #endif
 #include <ntdllsec.h> //PROCESSTHREAD_SECURITYINFO struct
#else
#include "config.h"
#include "winbase.h"
#include "selectors.h"  /* for SET_FS */
#endif

struct _PDB;

#ifndef OS2DEF_INCLUDED
   /* QMSG structure */
   typedef struct _QMSG    /* qmsg */
   {
      HWND    hwnd;
      ULONG   msg;
      ULONG   mp1;
      ULONG   mp2;
      ULONG   time;
      POINTL  ptl;
      ULONG   reserved;
   } QMSG;
#endif

/* Thread exception block

  flags in the comment:
  1-- win95 field
  d-- win95 debug version
  -2- nt field
  --3 wine special
  --n wine unused
  !-- or -!- likely or observed  collision
  more problems (collected from mailing list):
  psapi.dll 0x10/0x30 (expects nt fields)
  ie4       0x40
  PESHiELD  0x23/0x30 (win95)
*/

#pragma pack(1)
/* Thread exception block */
typedef struct _TEB
{
/* start of NT_TIB */
    void        *except; /* 12- 00 Head of exception handling chain */
    void        *stack_top;      /* 12-  04 Top of thread stack */
    void        *stack_low;      /* 12-  08 Stack low-water mark */
    HTASK16      htask16;        /* 1--  0c Win16 task handle */
    WORD         stack_sel;      /* 1--  0e 16-bit stack selector */
    DWORD        selman_list;    /* 1-n  10 Selector manager list */
    DWORD        user_ptr;       /* 12n  14 User pointer */
/* end of NT_TIB */  
    struct _TEB *self;           /* 12-  18 Pointer to this structure */
    WORD         tibflags;       /* 1!n  1c Flags (NT: EnvironmentPointer) */
    WORD         mutex_count;    /* 1-n  1e Win16 mutex count */
    void        *pid;            /* !2-  20 Process id (win95: debug context) */
    void        *tid;            /* -2-  24 Thread id */
    HQUEUE16     queue;          /* 1!-  28 Message queue (NT: DWORD ActiveRpcHandle)*/
    WORD         pad1;           /* --n  2a */
    LPVOID      *tls_ptr;        /* 2c Pointer to TLS array */
    struct _PDB *process;        /* 12-  30 owning process (win95: PDB; nt: NTPEB !!) */
    DWORD	 flags;	         /* 1-n  34 */
    DWORD        exit_code;      /* 1--  38 Termination status */
    WORD         teb_sel;        /* 1--  3c Selector to TEB */
    WORD         emu_sel;        /* 1-n  3e 80387 emulator selector */
    DWORD        unknown1;       /* --n  40 */
    DWORD        unknown2;       /* --n  44 */
    void       (*startup)(void); /* --3  48 Thread startup routine */
    int          thread_errno;   /* --3  4c Per-thread errno (was: ring0_thread) */
    int          thread_h_errno; /* --3  50 Per-thread h_errno (was: ptr to tdbx structure) */
    void        *stack_base;     /* 1--  54 Base of the stack */
    void        *signal_stack;   /* --3  58 Signal stack (was: exit_stack) */
    void        *emu_data;       /* --n  5c Related to 80387 emulation */
    DWORD        last_error;     /* 1--  60 Last error code */
    HANDLE       debug_cb;       /* 1-n  64 Debugger context block */
    DWORD        debug_thread;   /* 1-n  68 Thread debugging this one (?) */
    void        *pcontext;       /* 1-n  6c Thread register context */
    DWORD        cur_stack;      /* --3  70 Current stack (was: unknown) */
    DWORD        ThunkConnect;   /* 1-n  74 */
    DWORD        NegStackBase;   /* 1-n  78 */
    WORD         current_ss;     /* 1-n  7c Another 16-bit stack selector */
    WORD         pad2;           /* --n  7e */
    void        *ss_table;       /* --n  80 Pointer to info about 16-bit stack */
    WORD         thunk_ss;       /* --n  84 Yet another 16-bit stack selector */
    WORD         pad3;           /* --n  86 */
    DWORD        pad4[15];       /* --n  88 */
    ULONG        CurrentLocale;  /* -2-  C4 */
    DWORD        pad5[48];       /* --n  C8 */
    DWORD        delta_priority; /* 1-n 188 Priority delta */
    DWORD        unknown4[7];    /* d-n 18c Unknown */
    void        *create_data;    /* d-n 1a8 Pointer to creation structure */
    DWORD        suspend_count;  /* d-n 1ac SuspendThread() counter */
    void        *entry_point;    /* --3 1b0 Thread entry point (was: unknown) */
    void        *entry_arg;      /* --3 1b4 Entry point arg (was: unknown) */
    DWORD        unknown5[4];    /* --n 1b8 Unknown */
    DWORD        sys_count[4];   /* --3 1c8 Syslevel mutex entry counters */
    struct tagSYSLEVEL *sys_mutex[4];   /* --3 1d8 Syslevel mutex pointers */
    DWORD        unknown6[5];    /* --n 1e8 Unknown */

    union {
#ifdef __WIN32OS2__
          struct {
              struct _TEB   *next;
              DWORD          OrgTIBSel;      // Original OS/2 TIB selector (always the same, but let's not assume too much for future compatibility)
              ULONG          hmq;            // Thread message queue
              ULONG          hab;            // Thread Anchor block
              ULONG          hooks[WH_NB_HOOKS]; //list of hooks for this thread queue
              ULONG          threadId;       // Thread ID
              ULONG          hThread;        // thread handle
              ULONG          exceptFrame;    // address of os/2 exception handler frame
              ULONG          newWindow;      // Pointer to window object of window that was just created
              void*          pWsockData;     // Winsock data pointer (NULL if unused)
              DWORD          lParam;         // MessageExtraInfo
              WINDOWPOS      wp;             // Used by message translation for WM_WINDOWPOSCHANGED
              ULONG          nrOfMsgs;       // Usually 1; some PM messages can generated more than 1 win32 msg
              BOOL           fTranslated;    // WM_CHAR already translated or not
              ULONG          msgstate;       // odd -> dispatchmessage called, even -> not called
              QMSG           os2msg;         // original os2 msg (received with Get- or PeekMessage)
              MSG            winmsg;         // temporary storage for translated os2 msg (used in DispatchMessage)
              MSG            msg;            // Used by message translation to store translated PM message (sent to win32 window proc in pmwindow.cpp)
              MSG            msgWCHAR;       // Used to store extra WM_CHAR message generated by TranslateMessage
              DWORD          pidDebuggee;    // process id of debuggee
              DWORD          logfile;        // last FILE ptr used for logging (for exception handling in vfprintf)
              PROCESSTHREAD_SECURITYINFO threadinfo; //used to store security info for thread tokens
              DWORD          lcid;           // thread lcid
              BOOL           fIgnoreMsgs;    // set to true if waiting in WaitForSingleObject
          } odin;
#endif
          /* the following are nt specific fields */
          DWORD        pad6[633];                  /* --n 214 */
    } o;
    UNICODE_STRING StaticUnicodeString;      /* -2- bf8 used by advapi32 */
    USHORT       StaticUnicodeBuffer[261];   /* -2- c00 used by advapi32 */
    DWORD        pad7;                       /* --n e0c */
#ifdef __WIN32OS2__
    LPVOID         tls_array[TLS_MINIMUM_AVAILABLE];  /* -2- e10 Thread local storage */
#else
    LPVOID       tls_array[64];              /* -2- e10 Thread local storage */
#endif
    DWORD        pad8[3];                    /* --n f10 */
    PVOID        ReservedForNtRpc;           /* -2- f1c used by rpcrt4 */
    DWORD        pad9[24];                   /* --n f20 */
    PVOID	 ErrorInfo;                  /* -2- f80 used by ole32 (IErrorInfo*) */
} TEB;
#pragma pack()

/* Thread exception flags */
#define TEBF_WIN32  0x0001
#define TEBF_TRAP   0x0002


/* The pseudo handle value returned by GetCurrentThread */
#define CURRENT_THREAD_PSEUDOHANDLE 0xfffffffe

#endif  /* __WINE_THREAD_H */
