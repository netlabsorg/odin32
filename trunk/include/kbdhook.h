/*
 * Keyboard hook definitions
 *
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __KBDHOOK_H
#define __KBDHOOK_H

/**************************************************************/
/* Try to load the Presentation Manager Keyboard Hook module. */
/* If this fails, some hotkeys may not arrive properly at the */
/* targetted window, but no more harmful things will happen.  */
/**************************************************************/
#define STD_PMKBDHK_MODULE  "PMKBDHK";
#define PMKBDHK_HOOK_INIT   "_hookInit@8"
#define PMKBDHK_HOOK_TERM   "_hookKill@0"


typedef BOOL  (* WIN32API PFN_HOOKINIT)(HAB hab, char *lpszWindowClass);
typedef BOOL  (* WIN32API PFN_HOOKTERM)(void);


#endif  // __KBDHOOK_H

