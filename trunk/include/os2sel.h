/* $Id: os2sel.h,v 1.3 1999-06-26 18:24:22 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SEL_H__
#define __OS2SEL_H__

USHORT _System GetFS();
USHORT _System RestoreOS2FS();
void   _System SetFS(USHORT selector);

#endif //__OS2SEL_H__