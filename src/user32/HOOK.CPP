/* $Id: HOOK.CPP,v 1.30 2001-10-26 09:10:12 phaller Exp $ */

/*
 * Windows hook functions
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 Port)
 *
 * Port of Wine code (windows\hook.c; dated 990920)
 * All 16 bits code removed
 *
 * Copyright 1994, 1995 Alexandre Julliard
 *                 1996 Andrew Lewycky
 *
 * Based on investigations by Alex Korobka
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

/*
 * Warning!
 * A HHOOK is a 32-bit handle for compatibility with Windows 3.0 where it was
 * a pointer to the next function. Now it is in fact composed of a USER heap
 * handle in the low 16 bits and of a HOOK_MAGIC value in the high 16 bits.
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>


#include <os2win.h>
#include "hook.h"
#include "queue.h"
#include "task.h"
#include "winproc.h"
#include "debugtools.h"
#include <misc.h>
#include <heapstring.h>
#include <vmutex.h>
#include <wprocess.h>
#include "menu.h"

#include "win32wbase.h"

#define DBG_LOCALLOG	DBG_hook
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-HOOK)
DEFAULT_DEBUG_CHANNEL(hook)

#include "pshpack1.h"


/*****************************************************************************
 * Definitions, structures, and variables                                    *
 *****************************************************************************/

/* Hook data (pointed to by a HHOOK) */
typedef struct
{
    HANDLE     next;               /* 00 Next hook in chain */
    HOOKPROC   proc;               /* 04 Hook procedure (original) */
    INT        id;                 /* 08 Hook id (WH_xxx) */
    DWORD      ownerThread;        /* 0C Owner thread (0 for system hook) */
    HMODULE    ownerModule;        /* 10 Owner module */
    DWORD      flags;              /* 14 flags */
    DWORD      magic;              /* 18 magic dword */
} HOOKDATA;

#include "poppack.h"

#define HOOK_MAGIC1  ((int)'H' | (int)'K' << 8)  /* 'HK' */
#define HOOK_MAGIC ((HOOK_MAGIC1<<16)|HOOK_MAGIC1) // 'HKHK'

#define CHECK_MAGIC(a) ((a != 0) && (((HOOKDATA *)a)->magic == HOOK_MAGIC))

//NOTE: This must be in the local data segment -> if a shared semaphore was
//      created by a different process, the handle returned by DosOpenMutexSem
//      will be returned in hGlobalHookMutex
static HMTX hGlobalHookMutex = 0;

//Global DLL Data
//SvL: Disabled global system hooks for now
//#define GLOBAL_HOOKS
#ifdef GLOBAL_HOOKS
#pragma data_seg(_GLOBALDATA)
#endif
static HANDLE HOOK_systemHooks[WH_NB_HOOKS] = { 0 };
#ifdef GLOBAL_HOOKS
static VMutex systemHookMutex(VMUTEX_SHARED, &hGlobalHookMutex);
#pragma data_seg()
#else
static CRITICAL_SECTION systemCritSect = {0};
#endif
static HANDLE HOOK_threadHooks[WH_NB_HOOKS] = { 0 };
static CRITICAL_SECTION threadCritSect = {0};
static BOOL fInitHook = FALSE;

#ifdef GLOBAL_HOOKS
#define SYSTEMHOOK_LOCK()	systemHookMutex.enter(VMUTEX_WAIT_FOREVER, &hGlobalHookMutex);
#define SYSTEMHOOK_UNLOCK()	systemHookMutex.leave(&hGlobalHookMutex);
#define INIT_SYSTEMHOOK()       aldkjfslfjsdlkfj
#else
#define SYSTEMHOOK_LOCK()	EnterCriticalSection(&systemCritSect);
#define SYSTEMHOOK_UNLOCK()	LeaveCriticalSection(&systemCritSect);
#define INIT_SYSTEMHOOK()       InitializeCriticalSection(&systemCritSect);
#endif

typedef VOID (*HOOK_MapFunc)(INT, INT, WPARAM *, LPARAM *);
typedef VOID (*HOOK_UnMapFunc)(INT, INT, WPARAM, LPARAM, WPARAM,
                               LPARAM);

/***********************************************************************
 *           HOOK_Map32ATo32W
 */
static void HOOK_Map32ATo32W(INT id, INT code, WPARAM *pwParam,
                             LPARAM *plParam)
{
    if (id == WH_CBT && code == HCBT_CREATEWND)
    {
        LPCBT_CREATEWNDA lpcbtcwA = (LPCBT_CREATEWNDA)*plParam;
        LPCBT_CREATEWNDW lpcbtcwW = (LPCBT_CREATEWNDW)HeapAlloc(GetProcessHeap(), 0,
                                                                sizeof(*lpcbtcwW) );
        lpcbtcwW->lpcs = (CREATESTRUCTW*)HeapAlloc( GetProcessHeap(), 0, sizeof(*lpcbtcwW->lpcs) );

        lpcbtcwW->hwndInsertAfter = lpcbtcwA->hwndInsertAfter;
        *lpcbtcwW->lpcs = *(LPCREATESTRUCTW)lpcbtcwA->lpcs;

        if (HIWORD(lpcbtcwA->lpcs->lpszName))
        {
            lpcbtcwW->lpcs->lpszName = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                                    lpcbtcwA->lpcs->lpszName );
        }
        else
          lpcbtcwW->lpcs->lpszName = (LPWSTR)lpcbtcwA->lpcs->lpszName;

        if (HIWORD(lpcbtcwA->lpcs->lpszClass))
        {
            lpcbtcwW->lpcs->lpszClass = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                                   lpcbtcwA->lpcs->lpszClass );
        }
        else
          lpcbtcwW->lpcs->lpszClass = (LPCWSTR)lpcbtcwA->lpcs->lpszClass;
        *plParam = (LPARAM)lpcbtcwW;
    }
    return;
}


/***********************************************************************
 *           HOOK_UnMap32ATo32W
 */
static void HOOK_UnMap32ATo32W(INT id, INT code, WPARAM wParamOrig,
                               LPARAM lParamOrig, WPARAM wParam,
                               LPARAM lParam)
{
    if (id == WH_CBT && code == HCBT_CREATEWND)
    {
        LPCBT_CREATEWNDW lpcbtcwW = (LPCBT_CREATEWNDW)lParam;
        if (HIWORD(lpcbtcwW->lpcs->lpszName))
            HeapFree( GetProcessHeap(), 0, (LPWSTR)lpcbtcwW->lpcs->lpszName );
        if (HIWORD(lpcbtcwW->lpcs->lpszClass))
            HeapFree( GetProcessHeap(), 0, (LPWSTR)lpcbtcwW->lpcs->lpszClass );
        HeapFree( GetProcessHeap(), 0, lpcbtcwW->lpcs );
        HeapFree( GetProcessHeap(), 0, lpcbtcwW );
    }
    return;
}


/***********************************************************************
 *           HOOK_Map32WTo32A
 */
static void HOOK_Map32WTo32A(INT id, INT code, WPARAM *pwParam,
                             LPARAM *plParam)
{
    if (id == WH_CBT && code == HCBT_CREATEWND)
    {
        LPCBT_CREATEWNDW lpcbtcwW = (LPCBT_CREATEWNDW)*plParam;
        LPCBT_CREATEWNDA lpcbtcwA = (LPCBT_CREATEWNDA)HeapAlloc(GetProcessHeap(), 0,
                                                                sizeof(*lpcbtcwA) );
        lpcbtcwA->lpcs = (CREATESTRUCTA*)HeapAlloc( GetProcessHeap(), 0, sizeof(*lpcbtcwA->lpcs) );

        lpcbtcwA->hwndInsertAfter = lpcbtcwW->hwndInsertAfter;
        *lpcbtcwA->lpcs = *(LPCREATESTRUCTA)lpcbtcwW->lpcs;

        if (HIWORD(lpcbtcwW->lpcs->lpszName))
          lpcbtcwA->lpcs->lpszName = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                                    lpcbtcwW->lpcs->lpszName );
        else
          lpcbtcwA->lpcs->lpszName = (LPSTR)lpcbtcwW->lpcs->lpszName;

        if (HIWORD(lpcbtcwW->lpcs->lpszClass))
          lpcbtcwA->lpcs->lpszClass = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                                   lpcbtcwW->lpcs->lpszClass );
        else
          lpcbtcwA->lpcs->lpszClass = (LPSTR)lpcbtcwW->lpcs->lpszClass;
        *plParam = (LPARAM)lpcbtcwA;
    }
    return;
}


/***********************************************************************
 *           HOOK_UnMap32WTo32A
 */
static void HOOK_UnMap32WTo32A(INT id, INT code, WPARAM wParamOrig,
                               LPARAM lParamOrig, WPARAM wParam,
                               LPARAM lParam)
{
    if (id == WH_CBT && code == HCBT_CREATEWND)
    {
        LPCBT_CREATEWNDA lpcbtcwA = (LPCBT_CREATEWNDA)lParam;
        if (HIWORD(lpcbtcwA->lpcs->lpszName))
            HeapFree( GetProcessHeap(), 0, (LPSTR)lpcbtcwA->lpcs->lpszName );
        if (HIWORD(lpcbtcwA->lpcs->lpszClass))
            HeapFree( GetProcessHeap(), 0, (LPSTR)lpcbtcwA->lpcs->lpszClass );
        HeapFree( GetProcessHeap(), 0, lpcbtcwA->lpcs );
        HeapFree( GetProcessHeap(), 0, lpcbtcwA );
    }
    return;
}


/***********************************************************************
 *           Map Function Tables
 */
static const HOOK_MapFunc HOOK_MapFuncs[3][3] =
{
    { NULL, NULL,             NULL },
    { NULL, NULL,             HOOK_Map32ATo32W },
    { NULL, HOOK_Map32WTo32A, NULL }
};

static const HOOK_UnMapFunc HOOK_UnMapFuncs[3][3] =
{
    { NULL, NULL,               NULL },
    { NULL, NULL,               HOOK_UnMap32ATo32W },
    { NULL, HOOK_UnMap32WTo32A, NULL }
};


/***********************************************************************
 *           Internal Functions
 */

/***********************************************************************
 *           HOOK_GetNextHook
 *
 * Get the next hook of a given hook.
 */
static HANDLE HOOK_GetNextHook( HANDLE hook )
{
    HOOKDATA *data = (HOOKDATA *)hook;

    if (!data || !hook) return 0;
    if (data->next) return data->next;
    if (!data->ownerThread) return 0;  /* Already system hook */

    /* Now start enumerating the system hooks */
    return HOOK_systemHooks[data->id - WH_MINHOOK];
}


/***********************************************************************
 *           HOOK_GetHook
 *
 * Get the first hook for a given type.
 */
static HANDLE HOOK_GetHook( INT id, DWORD threadId )
{
  MESSAGEQUEUE *queue;
  HANDLE hook = 0;
  TEB *teb;

    teb = GetTEBFromThreadId(threadId);
    if(teb) {
        hook = teb->o.odin.hooks[id - WH_MINHOOK];
    }
    if (!hook) hook = HOOK_systemHooks[id - WH_MINHOOK];

    return hook;
}


/***********************************************************************
 *           HOOK_SetHook
 *
 * Install a given hook.
 */
static HHOOK HOOK_SetHook( INT id, LPVOID proc, INT type,
                           HMODULE hModule, DWORD dwThreadId )
{
  HOOKDATA *data;
  TEB      *teb;

    if ((id < WH_MINHOOK) || (id > WH_MAXHOOK) || !proc )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(!fInitHook) {
        InitializeCriticalSection(&threadCritSect);
        INIT_SYSTEMHOOK();
        fInitHook = TRUE;
    }

    dprintf(("Setting hook %d: %08x %04x %08lx\n", id, (UINT)proc, hModule, dwThreadId ));

#ifndef __WIN32OS2__
    /* Create task queue if none present */
    GetFastQueue16();

    if (id == WH_JOURNALPLAYBACK) EnableHardwareInput16(FALSE);
#endif


    if (dwThreadId)  /* Task-specific hook */
    {
        if ((id == WH_JOURNALRECORD) || (id == WH_JOURNALPLAYBACK) ||
            (id == WH_SYSMSGFILTER)) {
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;  /* System-only hooks */
        }
    }

    /* Create the hook structure */

    data = (HOOKDATA *) HeapAlloc(GetProcessHeap(), 0, sizeof(HOOKDATA));
    data->proc        = (HOOKPROC)proc;
    data->id          = id;
    data->ownerThread = dwThreadId;
    data->ownerModule = hModule;
    data->flags       = type;
    data->magic       = HOOK_MAGIC;

    /* Insert it in the correct linked list */
    if(dwThreadId)
    {
        teb = GetTEBFromThreadId(dwThreadId);
        if(!teb) {
                dprintf(("HOOK_SetHook: can't find thread database for thread %x", dwThreadId));
                return 0;
        }
        EnterCriticalSection(&threadCritSect);
        data->next = teb->o.odin.hooks[id - WH_MINHOOK];
        teb->o.odin.hooks[id - WH_MINHOOK] = (DWORD)data;
        LeaveCriticalSection(&threadCritSect);
    }
    else
    {
	SYSTEMHOOK_LOCK();
        data->next = HOOK_systemHooks[id - WH_MINHOOK];
        HOOK_systemHooks[id - WH_MINHOOK] = (HANDLE)data;
	SYSTEMHOOK_UNLOCK();
    }

    return (HHOOK)data;
}


/***********************************************************************
 *           HOOK_RemoveHook
 *
 * Remove a hook from the list.
 */
static BOOL HOOK_RemoveHook( HOOKDATA *data )
{
  HOOKDATA **prevHook;
  TEB       *teb;

    dprintf(("Removing hook %08x\n", data));

    if(!fInitHook) {
        InitializeCriticalSection(&threadCritSect);
        INIT_SYSTEMHOOK();
        fInitHook = TRUE;
    }

    if (data->flags & HOOK_INUSE)
    {
        /* Mark it for deletion later on */
        dprintf(("Hook still running, deletion delayed\n" ));
        data->flags |= HOOK_DELAYED_DELETE;
        return TRUE;
    }

#ifndef __WIN32OS2__
    if (data->id == WH_JOURNALPLAYBACK) EnableHardwareInput16(TRUE);
#endif

    /* Remove it from the linked list */

    if (data->ownerThread)
    {
        teb = GetTEBFromThreadId(data->ownerThread);
        if(!teb) {
                dprintf(("HOOK_RemoveHook: can't find thread database for thread %x", data->ownerThread));
                return FALSE;
        }
        EnterCriticalSection(&threadCritSect);
        prevHook = (HOOKDATA **)&teb->o.odin.hooks[data->id - WH_MINHOOK];
    }
    else {
       	SYSTEMHOOK_LOCK();
        prevHook = (HOOKDATA **)&HOOK_systemHooks[data->id - WH_MINHOOK];
    }
    while (*prevHook && *prevHook != data)
        prevHook = (HOOKDATA **)&(*prevHook)->next;

    if (!prevHook) {
	if (data->ownerThread) {
        	LeaveCriticalSection(&threadCritSect);
        }
	else   	SYSTEMHOOK_UNLOCK();

        return FALSE;
    }
    *prevHook = (HOOKDATA *)data->next;

    if (data->ownerThread) {
       	 LeaveCriticalSection(&threadCritSect);
    }
    else SYSTEMHOOK_UNLOCK();

    HeapFree(GetProcessHeap(), 0, (LPVOID)data );
    return TRUE;
}


/***********************************************************************
 *           HOOK_FindValidHook
 */
static HANDLE HOOK_FindValidHook( HANDLE hook )
{
    HOOKDATA *data;

    for (;;)
    {
        if (!(data = (HOOKDATA *)hook)) return 0;
        if (data->proc) return hook;
        hook = data->next;
    }
}


/***********************************************************************
 *           HOOK_CallHook
 *
 * Call a hook procedure.
 */
static LRESULT HOOK_CallHook( HANDLE hook, INT fromtype, INT code,
                              WPARAM wParam, LPARAM lParam )
{
    MESSAGEQUEUE *queue;
    HANDLE prevHook;
    HOOKDATA *data = (HOOKDATA *)hook;
    LRESULT ret;

    WPARAM wParamOrig = wParam;
    LPARAM lParamOrig = lParam;
    HOOK_MapFunc MapFunc;
    HOOK_UnMapFunc UnMapFunc;

    MapFunc = HOOK_MapFuncs[fromtype][data->flags & HOOK_MAPTYPE];
    UnMapFunc = HOOK_UnMapFuncs[fromtype][data->flags & HOOK_MAPTYPE];

    if (MapFunc)
      MapFunc( data->id, code, &wParam, &lParam );

    /* Now call it */

    data->flags |= HOOK_INUSE;

    dprintf2(("Calling hook %04x: %d %08x %08lx\n", hook, code, wParam, lParam ));

    ret = data->proc(code, wParam, lParam);

    data->flags &= ~HOOK_INUSE;

    if (UnMapFunc)
      UnMapFunc( data->id, code, wParamOrig, lParamOrig, wParam, lParam );

    if(data->flags & HOOK_DELAYED_DELETE) HOOK_RemoveHook( data );

    return ret;
}

/***********************************************************************
 *           Exported Functions & APIs
 */

/***********************************************************************
 *           HOOK_IsHooked
 *
 * Replacement for calling HOOK_GetHook from other modules.
 */
BOOL HOOK_IsHooked( INT id )
{
    /* Hmmm. Use GetThreadQueue(0) instead of GetFastQueue() here to
       avoid queue being created if someone wants to merely check ... */

    return HOOK_GetHook( id, GetCurrentThreadId() ) != 0;
}

/***********************************************************************
 *           HOOK_CallHooks32A
 *
 * Call a hook chain.
 */
LRESULT HOOK_CallHooksA( INT id, INT code, WPARAM wParam,
                           LPARAM lParam )
{
    HANDLE hook;

    if (!(hook = HOOK_GetHook( id, GetCurrentThreadId() ))) return 0;
    if (!(hook = HOOK_FindValidHook(hook))) return 0;
    return HOOK_CallHook( hook, HOOK_WIN32A, code, wParam, lParam );
}

/***********************************************************************
 *           HOOK_CallHooks32W
 *
 * Call a hook chain.
 */
LRESULT HOOK_CallHooksW( INT id, INT code, WPARAM wParam,
                           LPARAM lParam )
{
    HANDLE hook;

    if (!(hook = HOOK_GetHook( id, GetCurrentThreadId() ))) return 0;
    if (!(hook = HOOK_FindValidHook(hook))) return 0;
    return HOOK_CallHook( hook, HOOK_WIN32W, code, wParam,
                          lParam );
}


#if 0
/***********************************************************************
 *           HOOK_ResetQueueHooks
 */
void HOOK_ResetQueueHooks( HQUEUE hQueue )
{
    MESSAGEQUEUE *queue;

    if ((queue = (MESSAGEQUEUE *)QUEUE_Lock( hQueue )) != NULL)
    {
        HOOKDATA*       data;
        HHOOK           hook;
        int             id;
        for( id = WH_MINHOOK; id <= WH_MAXHOOK; id++ )
        {
            hook = queue->hooks[id - WH_MINHOOK];
            while( hook )
            {
                if( (data = (HOOKDATA *)hook) )
                {
                  data->ownerQueue = hQueue;
                  hook = data->next;
                } else break;
            }
        }

        QUEUE_Unlock( queue );
    }
}
#endif

/***********************************************************************
 *           HOOK_FreeModuleHooks
 */
void HOOK_FreeModuleHooks( HMODULE hModule )
{
 /* remove all system hooks registered by this module */

  HOOKDATA*     hptr;
  HHOOK         hook, next;
  int           id;

  for( id = WH_MINHOOK; id <= WH_MAXHOOK; id++ )
    {
       hook = HOOK_systemHooks[id - WH_MINHOOK];
       while( hook )
       {
          hptr = (HOOKDATA *)hook;
          if (hptr)
            {
              next = hptr->next;
              if( hptr->ownerModule == hModule )
                {
                  hptr->flags &= HOOK_MAPTYPE;
                  HOOK_RemoveHook(hptr);
                }
              hook = next;
            }
          else hook = 0;
       }
    }
}

/***********************************************************************
 *           HOOK_FreeQueueHooks
 */
void HOOK_FreeQueueHooks( DWORD threadId )
{
  /* remove all hooks registered by this queue */

  HOOKDATA*     hptr = NULL;
  HHOOK         hook, next;
  int           id;

  for( id = WH_MINHOOK; id <= WH_MAXHOOK; id++ )
    {
       hook = HOOK_GetHook( id, threadId );
       while( hook )
        {
          next = HOOK_GetNextHook(hook);

          hptr = (HOOKDATA *)hook;
          if( hptr && hptr->ownerThread == threadId )
            {
              hptr->flags &= HOOK_MAPTYPE;
              HOOK_RemoveHook(hptr);
            }
          hook = next;
        }
    }
}


/***********************************************************************
 *           SetWindowsHook32A   (USER32.525)
 */
ODINFUNCTION2(HHOOK, SetWindowsHookA,
              INT, id, 
              HOOKPROC, proc )
{
    return SetWindowsHookExA( id, proc, 0, GetCurrentThreadId() );
}

/***********************************************************************
 *           SetWindowsHook32W   (USER32.528)
 */
ODINFUNCTION2(HHOOK, SetWindowsHookW,
              INT, id, 
              HOOKPROC, proc)
{
    return SetWindowsHookExW( id, proc, 0, GetCurrentThreadId() );
}

/***********************************************************************
 *           SetWindowsHookEx32A   (USER32.526)
 */
ODINFUNCTION4(HHOOK, SetWindowsHookExA,
              INT, id, 
              HOOKPROC, proc, 
              HINSTANCE, hInst,
              DWORD, dwThreadId )
{
    return HOOK_SetHook( id, proc, HOOK_WIN32A, hInst, dwThreadId );
}

/***********************************************************************
 *           SetWindowsHookEx32W   (USER32.527)
 */
ODINFUNCTION4(HHOOK, SetWindowsHookExW,
              INT, id, 
              HOOKPROC, proc, 
              HINSTANCE, hInst,
              DWORD, dwThreadId )
{
    return HOOK_SetHook( id, proc, HOOK_WIN32W, hInst, dwThreadId );
}

/***********************************************************************
 *           UnhookWindowsHook32   (USER32.557)
 */
ODINFUNCTION2(BOOL, UnhookWindowsHook,
              INT, id, 
              HOOKPROC, proc )
{
    HANDLE hook = HOOK_GetHook( id, GetCurrentThreadId() );

    dprintf(("UnhookWindowsHook: %d %08lx\n", id, (DWORD)proc ));

    while (hook)
    {
        HOOKDATA *data = (HOOKDATA *)hook;
        if (data->proc == proc) break;
        hook = HOOK_GetNextHook( hook );
    }
    if (!hook) return FALSE;
    return HOOK_RemoveHook( (HOOKDATA *)hook );
}


/***********************************************************************
 *           UnhookWindowHookEx32   (USER32.558)
 */
ODINFUNCTION1(BOOL, UnhookWindowsHookEx,
              HHOOK, hhook )
{
    if (CHECK_MAGIC(hhook) == FALSE) {
	dprintf(("ERROR: UnhookWindowsHookEx invalid hook %x", hhook));
        return FALSE;
    }

    return HOOK_RemoveHook( (HOOKDATA *)hhook );
}

/***********************************************************************
 *           CallNextHookEx32    (USER32.17)
 *
 * There aren't ANSI and UNICODE versions of this.
 */
LRESULT WINAPI CallNextHookEx(HHOOK hhook, INT code, WPARAM wParam, LPARAM lParam)
{
    HANDLE next;
    INT fromtype;       /* figure out Ansi/Unicode */
    HOOKDATA *oldhook;

    dprintf2(("CallNextHookEx %x %d %x %x", hhook, code, wParam, lParam));

    if (CHECK_MAGIC(hhook) == FALSE) {
	dprintf(("ERROR: CallNextHookEx invalid hook %x", hhook));
        return FALSE;
    }
    if (!(next = HOOK_GetNextHook( hhook ))) return 0;

    oldhook = (HOOKDATA *)hhook ;
    fromtype = oldhook->flags & HOOK_MAPTYPE;

    return HOOK_CallHook( next, fromtype, code, wParam, lParam );
}


/***********************************************************************
 *           CallMsgFilter32A   (USER32.15)
 */
/*
 * FIXME: There are ANSI and UNICODE versions of this, plus an unspecified
 * version, plus USER (the 16bit one) has a CallMsgFilter32 function.
 */
ODINFUNCTION2(BOOL, CallMsgFilterA,
              LPMSG, msg, 
              INT, code )
{
#if 0 //CB: not a Win32 API and unimplemented
    if (GetSysModalWindow()) return FALSE;      /* ??? */
#endif
    if (HOOK_CallHooksA( WH_SYSMSGFILTER, code, 0, (LPARAM)msg ))
      return TRUE;
    return HOOK_CallHooksA( WH_MSGFILTER, code, 0, (LPARAM)msg );
}


/***********************************************************************
 *           CallMsgFilter32W   (USER32.16)
 */
ODINFUNCTION2(BOOL, CallMsgFilterW,
              LPMSG, msg, 
              INT, code )
{
#if 0 //CB: not a Win32 API and unimplemented
    if (GetSysModalWindow()) return FALSE;      /* ??? */
#endif
    if (HOOK_CallHooksW( WH_SYSMSGFILTER, code, 0, (LPARAM)msg ))
      return TRUE;
    return HOOK_CallHooksW( WH_MSGFILTER, code, 0, (LPARAM)msg );
}

BOOL ProcessKbdHook(LPMSG msg, BOOL remove )
{
  // call the low level hook first
  ProcessKbdHookLL(msg, remove, FALSE);
  
  // ALT-Key comes in with different scan code (0xa4 == VK_LMENU_W),
  // then gets translated to 0x12
  switch (msg->wParam)
  {
    case VK_LMENU:
    case VK_RMENU:
      msg->wParam = VK_MENU; // 0x12
      break;
    
    case VK_LCONTROL:
    case VK_CONTROL:
    msg->wParam = VK_CONTROL; // 0x11
      break;
  }
  
    /* Handle F1 key by sending out WM_HELP message */
    if ((msg->message == WM_KEYUP) && 
	(msg->wParam == VK_F1) &&
	remove &&
	(msg->hwnd != GetDesktopWindow()) &&
	!IsMenuActive())
    {   
	HELPINFO hi;
        Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(msg->hwnd);

	if (NULL != wnd)
	{
	    hi.cbSize = sizeof(HELPINFO);
	    hi.iContextType = HELPINFO_WINDOW;
	    hi.iCtrlId = wnd->getWindowId(); 
	    hi.hItemHandle = msg->hwnd;
	    hi.dwContextId = wnd->getWindowContextHelpId();
	    hi.MousePos = msg->pt;
            RELEASE_WNDOBJ(wnd);
	    SendMessageA(msg->hwnd, WM_HELP, 0, (LPARAM)&hi);
	}
    }
  
    // this undoc (don't care) bit isn't passed through to this hook
    DWORD lParam = msg->lParam & ~0x02000000;
  
    return (HOOK_CallHooksA( WH_KEYBOARD, remove ? HC_ACTION : HC_NOREMOVE,
                             LOWORD (msg->wParam), lParam )
                             ? TRUE : FALSE);
}



/***
 * ProcessKbdHookLL
 * this is a low-level version of the keyboard hook
 * present under NT/2000/XP.
 * It is supposed to be called for any keyboard event of the
 * system-input queue AFAIK.
 */
BOOL ProcessKbdHookLL(LPMSG msg, BOOL flagRemove, BOOL flagInjected )
{
  // @@@PH
  // format KBDLLHOOKSTRUCT and pass it in as msg->lParam
  KBDLLHOOKSTRUCT kbhs;
  
  // check if alt is currently pressed
  // Note: ALT seems to stick sometimes
  // BOOL fAltDown = GetKeyState(VK_LMENU);
  BOOL fAltDown = GetAsyncKeyState(VK_LMENU);
  
  kbhs.vkCode      = msg->wParam;
  kbhs.scanCode    = ( (msg->lParam & 0x03ff0000) >> 16);
  
  BOOL fKeyUp      = (msg->message == WM_KEYUP) ||
                     (msg->message == WM_SYSKEYUP);
  kbhs.flags       = fKeyUp ? LLKHF_UP : 0;
  
  // @@@PH seems not to work properly
  kbhs.flags      |= fAltDown ? LLKHF_ALTDOWN : 0;
  kbhs.flags      |= flagInjected ? LLKHF_INJECTED : 0;
  kbhs.flags      |= (msg->lParam & (1 << 24)) ? LLKHF_EXTENDED : 0;
  
/*
  kbhs.flags       = ( (msg->lParam & (1 << 24)) ? LLKHF_EXTENDED : 0 ) |
                     ( flagInjected ? LLKHF_INJECTED : 0) |
                     fAltDown ? LLKHF_ALTDOWN : 0 |
                     ( (msg->lParam & (1 << 31)) ? LLKHF_UP : 0);
                     */
  kbhs.time        = msg->time;
  kbhs.dwExtraInfo = 0; // @@@PH not supported?
  
  return (HOOK_CallHooksA(WH_KEYBOARD_LL,
                          flagRemove ? HC_ACTION : HC_NOREMOVE,
                          msg->message,
                          (LPARAM)&kbhs )
                          ? TRUE : FALSE);
}
