/* $Id: dbglocal.cpp,v 1.5 2001-07-20 15:41:04 sandervl Exp $ */

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

USHORT DbgEnabledWSOCK32[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2WSOCK32[DBG_MAXFILES] = {0};

static char  *DbgFileNames[DBG_MAXFILES] =
{
"initterm",
"wsastruct",
"wsock32",
"unknown",
"async",
"asyncthread",
"wsa"
};
//******************************************************************************
//******************************************************************************
void ParseLogStatusWSOCK32()
{
 char *envvar = getenv(DBG_ENVNAME);
 char *envvar2= getenv(DBG_ENVNAME_LVL2);
 char *dbgvar;
 int   i;

    for(i=0;i<DBG_MAXFILES;i++) {
        DbgEnabledWSOCK32[i] = 1;
    }

    if(!envvar)
        return;

    dbgvar = strstr(envvar, "dll");
    if(dbgvar) {
        if(*(dbgvar-1) == '-') {
            for(i=0;i<DBG_MAXFILES;i++) {
                DbgEnabledWSOCK32[i] = 0;
            }
        }
    }
    for(i=0;i<DBG_MAXFILES;i++) {
        dbgvar = strstr(envvar, DbgFileNames[i]);
        if(dbgvar) {
            if(*(dbgvar-1) == '-') {
                    DbgEnabledWSOCK32[i] = 0;
            }
            else    
            if(*(dbgvar-1) == '+') {
		DbgEnabledWSOCK32[i] = 1;
	    }
        }
    }
    if(envvar2) {
    	dbgvar = strstr(envvar2, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '+') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledLvl2WSOCK32[i] = 1;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar2, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledLvl2WSOCK32[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledLvl2WSOCK32[i] = 1;
		    }
		}
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
