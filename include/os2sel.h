/* $Id: os2sel.h,v 1.2 1999-06-10 16:21:48 achimha Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SEL_H__
#define __OS2SEL_H__

USHORT _System GetFS();
void   _System SetFS(USHORT selector);

#endif //__OS2SEL_H__