/* $Id: myldrCreateMte.cpp,v 1.2 2000-01-28 15:28:43 bird Exp $
 *
 * Obsolete
 *
 * Copyright (c) 1998 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <ldr.h>

int LDRCALL myldrCreateMte(ULONG p1, ULONG p2)
{

    //kernel_printf(("_ldrCreateMte: param1:0x%x  param2:0x%x", p1, p2));
//  if ( p1 != 0 )
//      kernel_printf(("_ldrCreateMte: workarea: %s",(PCHAR)p1));

    return _ldrCreateMte(p1,p2);
}
