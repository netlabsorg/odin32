/* $Id: hook.h,v 1.2 1999-12-16 00:09:32 sandervl Exp $ */

/*
 * Windows hook definitions
 *
 * Copyright 1994 Alexandre Julliard
 */

#ifndef __WINE_HOOK_H
#define __WINE_HOOK_H

#include "windef.h"

#define HOOK_WIN16		0x00
#define HOOK_WIN32A		0x01
#define HOOK_WIN32W		0x02
#define HOOK_INUSE		0x80
#define HOOK_DELAYED_DELETE 	0x100

/* hook type mask */
#define HOOK_MAPTYPE (HOOK_WIN16 | HOOK_WIN32A | HOOK_WIN32W)

extern HOOKPROC16 HOOK_GetProc16( HHOOK hhook );
extern BOOL HOOK_IsHooked( INT id );
extern LRESULT HOOK_CallHooks16( INT16 id, INT16 code, WPARAM16 wParam,
				 LPARAM lParam );
extern LRESULT HOOK_CallHooksA( INT id, INT code, WPARAM wParam,
				  LPARAM lParam );
extern LRESULT HOOK_CallHooksW( INT id, INT code, WPARAM wParam,
				  LPARAM lParam );
extern void HOOK_FreeModuleHooks( HMODULE hModule );
extern void HOOK_FreeQueueHooks( DWORD threadId );
extern void HOOK_ResetQueueHooks( HQUEUE hQueue );
extern HOOKPROC HOOK_GetProc( HHOOK hook );

#endif  /* __WINE_HOOK_H */
