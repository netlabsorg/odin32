/* $Id: os2sel.h,v 1.10 2003-01-06 13:24:20 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SEL_H__
#define __OS2SEL_H__


#ifdef __cplusplus
extern "C" {
#endif


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
#endif

#ifdef  __EMX__
extern inline unsigned short GetFS(void)
{
 asm volatile (
    "mov %eax, %fs	\n\t"
 );
}

extern inline void SetFS(unsigned short sel)
{
 asm volatile (
        "mov    %eax,%esp	\n\t"
        "mov    %fs, %eax	\n\t"
 );
}

extern inline unsigned short RestoreOS2FS(void)
{
 asm volatile (
        "push    0x150b		\n\t"
        "mov     %ax, %fs	\n\t"
        "pop     %fs"
 );
}


#else

unsigned short _System GetFS       (void);
unsigned short _System RestoreOS2FS(void);
void           _System SetFS       (unsigned short selector);
unsigned short _System SetReturnFS (unsigned short selector);
//SvL: Checks if thread FS & exception structure are valid
int            _System CheckCurFS(void);
#endif

  
  
#ifdef __cplusplus
}
#endif


#endif //__OS2SEL_H__
