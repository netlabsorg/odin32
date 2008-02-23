/* $Id: hook.h,v 1.5 2001-10-09 05:18:39 phaller Exp $ */

/*
 * Windows hook definitions
 *
 * Copyright 1994 Alexandre Julliard
 */

#ifndef __WINE_HOOK_H
#define __WINE_HOOK_H

#define HOOK_WIN16		0x00
#define HOOK_WIN32A		0x01
#define HOOK_WIN32W		0x02
#define HOOK_INUSE		0x80
#define HOOK_DELAYED_DELETE 	0x100

/* hook type mask */
#define HOOK_MAPTYPE (HOOK_WIN16 | HOOK_WIN32A | HOOK_WIN32W)

extern BOOL HOOK_IsHooked( INT id );
extern LRESULT HOOK_CallHooksA( INT id, INT code, WPARAM wParam,
				  LPARAM lParam );
extern LRESULT HOOK_CallHooksW( INT id, INT code, WPARAM wParam,
				  LPARAM lParam );
extern void HOOK_FreeModuleHooks( HMODULE hModule );
extern void HOOK_FreeQueueHooks( DWORD threadId );
extern void HOOK_ResetQueueHooks( HQUEUE hQueue );
extern HOOKPROC HOOK_GetProc( HHOOK hook );

extern BOOL ProcessKbdHook(LPMSG msg, BOOL remove );
extern BOOL ProcessKbdHookLL(LPMSG msg, BOOL flagRemove, BOOL flagInjected );

#endif  /* __WINE_HOOK_H */
