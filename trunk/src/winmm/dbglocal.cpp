/* $Id: dbglocal.cpp,v 1.1 2000-02-17 14:10:18 sandervl Exp $ */

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

USHORT DbgEnabled[DBG_MAXFILES];
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
"resource"
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

