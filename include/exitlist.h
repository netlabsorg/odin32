// $Id: exitlist.h,v 1.2 2001-01-04 18:18:51 sandervl Exp $
/*
 * Win32 dll exit list invocation orders
 *
 * NOTE: Dlls should try to clean up their mess inside registered
 *       LibMain functions (RegisterLxDll) instead of exitlist handlers.
 *
 * Copyright 2000 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __EXITLIST_H__
#define __EXITLIST_H__

//Order of application dlls (i.e. Opera)
#define EXITLIST_APPDLL		     	0x00007000
#define EXITLIST_APPDLL_LVL2	     	0x00007100
#define EXITLIST_APPDLL_LVL3	     	0x00007200
#define EXITLIST_APPDLL_LVL4	     	0x00007300
#define EXITLIST_APPDLL_LVL5	     	0x00007400
#define EXITLIST_APPDLL_LVL6	     	0x00007500
#define EXITLIST_APPDLL_LVL7	     	0x00007600
#define EXITLIST_APPDLL_LVL8	     	0x00007700

//Order of dlls that are not part of the win32 core (example: glut & opengl)
#define EXITLIST_NONCOREDLL     	0x00008000
#define EXITLIST_NONCOREDLL_LVL2	0x00008100
#define EXITLIST_NONCOREDLL_LVL3	0x00008200
#define EXITLIST_NONCOREDLL_LVL4	0x00008300
#define EXITLIST_NONCOREDLL_LVL5	0x00008400
#define EXITLIST_NONCOREDLL_LVL6	0x00008500
#define EXITLIST_NONCOREDLL_LVL7	0x00008600
#define EXITLIST_NONCOREDLL_LVL8	0x00008700

//Order of non-critical system dlls (i.e. comctl32 & comdlg32)
#define EXITLIST_NONCRITDLL		0x00009000

//Order of USER32 & GDI32
#define EXITLIST_USER32			0x00009D00
#define EXITLIST_GDI32			0x00009D00

//Order of Kernel32
#define EXITLIST_KERNEL32		0x00009E00

//Order of WGSS50
#define EXITLIST_WGSS50			0x00009E01

//Order of odincrt.dll (should always be invoked last!)
#define EXITLIST_ODINCRT		0x00009F00

#endif //__EXITLIST_H__