/* $Id: os2sel.h,v 1.6 2000-01-28 12:06:58 sandervl Exp $ */
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

//SvL: Checks if thread FS & exception structure are valid
int            _System CheckCurFS();

#endif //__OS2SEL_H__
