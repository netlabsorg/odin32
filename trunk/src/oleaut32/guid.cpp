/* $Id: guid.cpp,v 1.2 1999-08-22 22:08:47 sandervl Exp $ */
#define INITGUID 1
/* 
 * GUIDs for OLEAUT32
 * 
 * Copyright 1999 Sander van Leeuwen 
 *
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

/* FIXME: we include all the header files containing GUIDs
 * so that the corresponding variables get defined. But they
 * don't even all belong to the same DLL !!!
 *
 * Therefore, as the builtin DLL's get teased apart (e.g. for elf-dlls)
 * then this file will have to be partitioned into per dll files.
 */
#include "oleaut32.h"
#include "wine\obj_oleaut.h"
#include "wine\obj_olefont.h"
#include "olectl.h"

/* and now for the one assumed GUID... */
DEFINE_GUID(GUID_NULL,   0,0,0,0,0,0,0,0,0,0,0);
