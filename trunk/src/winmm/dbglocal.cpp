/* $Id: dbglocal.cpp,v 1.4 2001-03-19 19:28:37 sandervl Exp $ */

/*
 * debug logging functions for OS/2
 *
 *
 * Copyright 2000 Sander van Leeuwen
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifdef DEBUG

#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dbglocal.h"

USHORT DbgEnabled[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2[DBG_MAXFILES] = {0};

char  *DbgFileNames[DBG_MAXFILES] =
{
"os2timer",
"waveout",
"dwaveout",
"time",
"wavein",
"auxiliary",
"auxos2",
"mixer",
"midi",
"irtmidi",
"midistrm",
"initterm",
"mci",
"joy",
"mmio",
"driver",
"playsound",
"joyos2",
"resource",
"dwavein"
};
//******************************************************************************
//******************************************************************************
void ParseLogStatus()
{
 char *envvar = getenv(DBG_ENVNAME);
 char *envvar2= getenv(DBG_ENVNAME_LVL2);
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
    if(envvar2) {
    	dbgvar = strstr(envvar2, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '+') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledLvl2[i] = 1;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar2, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledLvl2[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledLvl2[i] = 1;
		    }
		}
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
