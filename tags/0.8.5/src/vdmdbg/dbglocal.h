/* $Id: dbglocal.h,v 1.1 2001-06-13 04:45:33 bird Exp $
 *
 * Debug logging functions for OS/2 - Overrides main dprintf macros.
 *
 * Copyright 2000 Sander van Leeuwen
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __DBGLOCAL_H__
    #define __DBGLOCAL_H__

    #ifdef DEBUG
        /* Parses environment variable for selective enabling/disabling of logging */
        void ParseLogStatus(void);

        #define DBG_ENVNAME        "dbg_vdmdbg"
        #define DBG_ENVNAME_LVL2   "dbg_vdmdbg_lvl2"

        #define DBG_vdmdbg         0
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
