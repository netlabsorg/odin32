/* $Id: dbglocal.cpp,v 1.7 2001-07-20 15:34:15 sandervl Exp $ */

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

USHORT DbgEnabledUSER32[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2USER32[DBG_MAXFILES] = {0};

static char  *DbgFileNames[DBG_MAXFILES] =
{
"user32",
"loadres",
"dde",
"win32wndhandle",
"wsprintf",
"winmouse",
"icon",
"hook",
"winkeyboard",
"defwndproc",
"syscolor",
"char",
"initterm",
"uitools",
"unknown",
"spy",
"wndmsg",
"display",
"pmwindow",
"pmframe",
"win32class",
"win32wnd",
"win32dlg",
"msgbox",
"window",
"windowmsg",
"windowclass",
"win32wbase",
"windowword",
"gen_object",
"oslibwin",
"win32wndchild",
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
"clipboard",
"winicon",
"win32wmdiclient",
"oslibutil",
"oslibmsg",
"dib",
"oslibdos",
"win32wmdichild",
"wingdi",
"oslibgdi",
"winaccel",
"win32wbasepos",
"oslibres",
"dc",
"timer",
"caret",
"resource",
"winproc",
"text",
"oslibmsgtranslate",
"windlgmsg",
"windlg",
"win32wdesktop",
"win32wbasenonclient",
"win32wbaseprop",
"dcrgn"
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
        DbgEnabledUSER32[i] = 1;
    }

    if(envvar) {
    	dbgvar = strstr(envvar, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '-') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledUSER32[i] = 0;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledUSER32[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledUSER32[i] = 1;
		    }
		}
        }
    }
    if(envvar2) {
    	dbgvar = strstr(envvar2, "dll");
	if(dbgvar) {
	        if(*(dbgvar-1) == '+') {
	            for(i=0;i<DBG_MAXFILES;i++) {
	                DbgEnabledLvl2USER32[i] = 1;
	            }
	        }
	}
	for(i=0;i<DBG_MAXFILES;i++) {
	        dbgvar = strstr(envvar2, DbgFileNames[i]);
	        if(dbgvar) {
	            if(*(dbgvar-1) == '-') {
	                    DbgEnabledLvl2USER32[i] = 0;
	            }
	            else    
	            if(*(dbgvar-1) == '+') {
			DbgEnabledLvl2USER32[i] = 1;
		    }
		}
        }
    }
}
//******************************************************************************
//******************************************************************************

#endif
