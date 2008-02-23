/* $Id: dbglocal.h,v 1.1 2002-04-10 18:35:18 bird Exp $
 *
 * Debug logging functions for OS/2 - Overrides main dprintf macros.
 *
 * Copyright 2000 Sander van Leeuwen
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __DBGLOCAL_H__
    #define __DBGLOCAL_H__

    #ifdef DEBUG
        /* Parses environment variable for selective enabling/disabling of logging */
        void ParseLogStatus(void);

        #define DBG_ENVNAME        "dbg_uxtheme"
        #define DBG_ENVNAME_LVL2   "dbg_uxtheme_lvl2"

        #define DBG_uxtheme        0
        #define DBG_MAXFILES       1

        extern USHORT DbgEnabled[DBG_MAXFILES];
        extern USHORT DbgEnabledLvl2[DBG_MAXFILES];

        #ifdef dprintf
            #undef dprintf
        #endif

        #define dprintf(a)      if(DbgEnabled[DBG_LOCALLOG] == 1) WriteLog a

        #ifdef dprintf2
            #undef dprintf2
        #endif

        #define dprintf2(a)     if(DbgEnabledLvl2[DBG_LOCALLOG] == 1) WriteLog a

    #else

        #define ParseLogStatus()

    #endif //DEBUG

#endif
