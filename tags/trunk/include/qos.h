/* $Id: qos.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Wrapper between OS/2 and Windows version of mmsystem.h
 *
 * Usage: Windows qos.h: define WIN32OS2_USE_WIN_QOS
 *        OS/2 qos.h:  undefine WIN32OS2_USE_WIN_QOS
 *
 * Note:  The OS/2 toolkit have to precede the Win32 SDK in the searchpath for includes.
 *        (SET INCLUDE enviorment and '-I' compiler option.)
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifdef WIN32SDK_USE_WIN_QOS
    #include "win\qos.h"
#else
    #include <qos.h>
#endif
