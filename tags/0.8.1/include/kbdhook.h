/*
 * Keyboard hook definitions
 *
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __KBDHOOK_H
#define __KBDHOOK_H


/*
 * HK_PREACCEL:
 *      additional undocumented PM hook type
 */

#ifndef HK_PREACCEL
#define HK_PREACCEL             17
#endif

/**************************************************************/
/* Try to load the Presentation Manager Keyboard Hook module. */
/* If this fails, some hotkeys may not arrive properly at the */
/* targetted window, but no more harmful things will happen.  */
/**************************************************************/
#define STD_PMKBDHK_MODULE  "PMKBDHK";
#define PMKBDHK_HOOK_INIT   "_hookInit@8"
#define PMKBDHK_HOOK_TERM   "_hookKill@0"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL  (* WIN32API PFN_HOOKINIT)(ULONG hab, char *lpszWindowClass);
typedef BOOL  (* WIN32API PFN_HOOKTERM)(void);

BOOL WIN32API hookInit(ULONG hab);
BOOL WIN32API hookKill(ULONG hab);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __KBDHOOK_H

