/* $Id: myldrGetResource.cpp,v 1.2 2000-01-28 15:28:43 bird Exp $
 *
 * Obsolete
 *
 * Copyright (c) 1998 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <ldr.h>


/*
    Notes: form DosGetResource parameters
        handle to module            hMod!
        idType                      idType?
        idName                      idName?
        pointer To resource         ppRes?


*/
static global_flag_resource = 0;


int LDRCALL myldrGetResource(USHORT idType, ULONG idName, USHORT hMod, PPVOID ppRes, ULONG ulP5)
{
    int rc;
    if ( ulP5 != 1 )
        kernel_printf(("_ldrGetResource: ulP5=%d (!=1)",ulP5 ));
    if ( global_flag_resource )
        kernel_printf(("_ldrGetResource:      idType:0x%x idName:0x%x hMod:0x%x *ppRes:0x%x p5:0x%x", idType, idName, hMod, *((PULONG)ppRes), ulP5));
    rc = _ldrGetResource(idType, idName, hMod, ppRes, ulP5);
    if ( global_flag_resource )
        kernel_printf(("_ldrGetResource-post: idType:0x%x idName:0x%x hMod:0x%x *ppRes:0x%x p5:0x%x", idType, idName, hMod, *((PULONG)ppRes), ulP5));

    return rc;
}

