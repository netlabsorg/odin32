/* $Id: myldrGetMte.cpp,v 1.2 2000-01-28 15:28:43 bird Exp $
 *
 * Obsolete
 *
 * Copyright (c) 1998 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <ldr.h>

int LDRCALL myldrGetMte(ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5)
{
    //kernel_printf(("_ldrGetMte: p1:0x%x p2:0x%x p3:0x%x p4:0x%x p5:0x%x",p1,p2,p3,p4,p5));
    return _ldrGetMte(p1,p2,p3,p4,p5);
}
