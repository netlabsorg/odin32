/* $Id: myLDRLoadExe.cpp,v 1.2 2000-01-28 15:28:42 bird Exp $
 *
 * Obsolete
 *
 * Copyright (c) 1998 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <ldr.h>

int LDRCALL myLDRLoadExe(PCHAR pExeName,PULONG pPTDA) /* ? */
{
    //kernel_printf(("_LDRLoadExe: %s - param2:0x%x", pExeName, p2));
    return _LDRLoadExe(pExeName,p2);
}


