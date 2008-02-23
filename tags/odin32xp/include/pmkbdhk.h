/* $Id: pmkbdhk.h,v 1.3 2003-03-25 12:16:56 sandervl Exp $ */
/*
 * OS/2 Presentation Manager Keyboard Hook
 *
 *
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 *
 */
 
#ifndef __PMKBDHK_H__
#define __PMKBDHK_H__

/*
 * Note: this won't interfere with any WM_USER value,
 * WM_PENFIRST is 0x04c0
 */
#define WM_CHAR_SPECIAL               0x04ba
#define WM_CHAR_SPECIAL_ALTGRCONTROL  0x04bb
#define WM_CHAR_SPECIAL_CONSOLE_BREAK 0x04bc


#endif //__PMKBDHK_H__
