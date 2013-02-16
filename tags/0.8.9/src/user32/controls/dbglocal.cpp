/* $Id: dbglocal.cpp,v 1.1 2002-06-02 10:09:28 sandervl Exp $ */

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

USHORT DbgEnabledOdinCtrl[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2OdinCtrl[DBG_MAXFILES] = {0};

static char  *DbgFileNames[DBG_MAXFILES] =
{
"controls",
"button",
"static",
"scroll",
"listbox",
"menu",
"combo",
"edit",
"winswitch",
"icontitle",
"initterm",
"trace",
"dialog"
};
//******************************************************************************
//******************************************************************************
void ParseLogStatusUSER32()
{
 char *envvar = getenv(DBG_ENVNAME);
 char *envvar2= getenv(DBG_ENVNAME_LVL2);
 char *dbgvar;
 int   i;

    for(i=0;i<DBG_MAXFILES;i++) {
        DbgEnabledOdinCtrl[i] = 1;
    }

    if(envvar) {
    	dbgvar = strstr(envvar, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '-') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledOdinCtrl[i] = 0;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledOdinCtrl[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledOdinCtrl[i] = 1;
		    }
		}
        }
    }
    if(envvar2) {
    	dbgvar = strstr(envvar2, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '+') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledLvl2OdinCtrl[i] = 1;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar2, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledLvl2OdinCtrl[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledLvl2OdinCtrl[i] = 1;
		    }
		}
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
