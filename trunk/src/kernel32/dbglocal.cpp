/* $Id: dbglocal.cpp,v 1.5 2000-03-23 19:23:44 sandervl Exp $ */

/*
 * debug logging functions for OS/2
 *
 *
 * Copyright 2000 Sander van Leeuwen
 * Project Odin Software License can be found in LICENSE.TXT
 */
#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dbglocal.h"

#ifdef DEBUG

USHORT DbgEnabled[DBG_MAXFILES];
char  *DbgFileNames[DBG_MAXFILES] =
{
"kernel32",
"kobjects",
"console",
"conin",
"conbuffer",
"conout",
"unicode",
"network",
"hmdevio",
"profile",
"thread",
"virtual",
"thunk",
"obsolete",
"comm",
"message",
"resource",
"exceptions",
"heapshared",
"cpuhlp",
"heapcode",
"lfile",
"npipe",
"oslibdos",
"oslibmisc",
"misc",
"exceptutil",
"lang",
"iccio",
"map",
"win32util",
"heap",
"heapstring",
"os2heap",
"vmutex",
"initterm",
"handlemanager",
"environ",
"initsystem",
"hmdevice",
"hmopen32",
"hmobjects",
"hmevent",
"hmmutex",
"hmcomm",
"semaphore",
"wprocess",
"conprop",
"conprop2",
"winimagelx",
"winimagebase",
"windllbase",
"winexebase",
"time",
"mmap",
"winimagepe2lx",
"winimagepeldr",
"windllpe2lx",
"windlllx",
"windllpeldr",
"winexepe2lx",
"winexelx",
"winexepeldr",
"winres",
"critsection",
"pefile",
"winimgres",
"wintls",
"async",
"fileio",
"hmtoken",
"kernelrsrc",
"atom",
"disk",
"directory",
"cvtbitmap",
"hmmmap",
"winfakepeldr",
"cvtaccel",
"cvticon",
"cvticongrp",
"oslibexcept",
"cpu",
"process",
"cvtcursor",
"cvtcursorgrp",
"stubs",
"interlock",
"toolhelp",
"codepage",
"debug",
"oslibdebug",
"registry",
"queue",
"hmthread",
"hmprocess",
"vsemaphore"
};
//******************************************************************************
//******************************************************************************
void ParseLogStatus()
{
 char *envvar = getenv(DBG_ENVNAME);
 char *dbgvar;
 int   i;

    for(i=0;i<DBG_MAXFILES;i++) {
        DbgEnabled[i] = 1;
    }

    if(!envvar)
        return;

    dbgvar = strstr(envvar, "dll");
    if(dbgvar) {
        if(*(dbgvar-1) == '-') {
            for(i=0;i<DBG_MAXFILES;i++) {
                DbgEnabled[i] = 0;
            }
        }
    }
    for(i=0;i<DBG_MAXFILES;i++) {
        dbgvar = strstr(envvar, DbgFileNames[i]);
        if(dbgvar) {
            if(*(dbgvar-1) == '-') {
                    DbgEnabled[i] = 0;
            }
            else    
            if(*(dbgvar-1) == '+') {
		DbgEnabled[i] = 1;
	    }
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
