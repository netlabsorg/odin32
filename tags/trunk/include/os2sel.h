/* $Id: os2sel.h,v 1.7 2000-08-23 18:05:54 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SEL_H__
#define __OS2SEL_H__

#ifdef __WATCOMC__
unsigned short GetFS(void);
#pragma aux GetFS = \
   "mov  ax,fs" \
   value [ax] \
   parm nomemory;

void SetFS(unsigned short sel);
#pragma aux SetFS = \
   "mov  fs,ax" \
   parm [ax];

unsigned short RestoreOS2FS(void);
#pragma aux RestoreOS2FS = \
   "mov  bx, fs" \
   "mov  ax, 150bh" \
   "mov  fs, ax" \
   value [bx] \
   modify nomemory exact [ax];

#else
unsigned short SYSTEM GetFS       ();
unsigned short SYSTEM RestoreOS2FS();
void           SYSTEM SetFS       (unsigned short selector);
unsigned short SYSTEM SetReturnFS (unsigned short selector);
#endif

//SvL: Checks if thread FS & exception structure are valid
int            SYSTEM CheckCurFS();

#endif //__OS2SEL_H__
