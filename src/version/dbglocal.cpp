/* $Id: dbglocal.cpp,v 1.1 2003-10-24 15:07:59 sandervl Exp $ */

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
#include <ctype.h>
#include "dbglocal.h"

#ifdef DEBUG

USHORT DbgEnabledVERSION[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2VERSION[DBG_MAXFILES] = {0};

static const char *DbgFileNames[DBG_MAXFILES] =
{
    "info",
    "install",
    "resource"
};

//******************************************************************************
//******************************************************************************
BOOL CheckDebugStatus(int index)
{
    static int checked = 0;

    if(!checked) {
        checked = 1;
        ParseLogStatusVERSION();
    }
    return DbgEnabledVERSION[index];
}
//******************************************************************************
//******************************************************************************
void ParseLogStatusVERSION()
{
 char *envvar = getenv(DBG_ENVNAME);
 char *envvar2= getenv(DBG_ENVNAME_LVL2);
 char *dbgvar;
 int   i;

    for(i=0;i<DBG_MAXFILES;i++) {
        DbgEnabledVERSION[i] = 1;
    }

    if(envvar) {
    	dbgvar = strstr(envvar, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '-') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledVERSION[i] = 0;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
		int len = strlen(DbgFileNames[i]);
	        dbgvar = strstr(envvar, DbgFileNames[i]);
	        if(dbgvar && !islower(dbgvar[len])) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledVERSION[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledVERSION[i] = 1;
		    }
		}
        }
    }
    if(envvar2) {
    	dbgvar = strstr(envvar2, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '+') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledLvl2VERSION[i] = 1;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
		int len = strlen(DbgFileNames[i]);
	        dbgvar = strstr(envvar2, DbgFileNames[i]);
	        if(dbgvar && !islower(dbgvar[len])) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledLvl2VERSION[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledLvl2VERSION[i] = 1;
		    }
		}
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
