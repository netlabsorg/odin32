/* $Id: mmsystem.h,v 1.1 1999-05-24 20:19:06 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Wrapper between OS/2 and Windows version of mmsystem.h
 *
 * Usage: Windows mmsystem.h: define WIN32OS2_USE_WIN_MMSYSTEM
 *        OS/2 mmsystem.h:  undefine WIN32OS2_USE_WIN_MMSYSTEM
 *
 * Note:  The OS/2 toolkit have to precede the Win32 SDK in the searchpath for includes.
 *        (SET INCLUDE enviorment and '-I' compiler option.)
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifdef WIN32SDK_USE_WIN_MMSYSTEM
    #include "win\mmsystem.h"
#else
    #error("This don't work! define WIN32SDK_USE_WIN_MMSYSTEM to include Win32SDK mmsystem.h.");
    #error("Use greate magic to include mmsystem.h from OS/2 toolkit.");
    //#include <mmsystem.h>
#endif
