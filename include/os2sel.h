/* $Id: os2sel.h,v 1.5 1999-08-17 19:30:49 phaller Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SEL_H__
#define __OS2SEL_H__

unsigned short _System GetFS       ();
unsigned short _System RestoreOS2FS();
void           _System SetFS       (unsigned short selector);
unsigned short _System SetReturnFS (unsigned short selector);

#endif //__OS2SEL_H__
