/* $Id: os2sel.h,v 1.12 2003-04-02 11:02:34 sandervl Exp $ */
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
static inline unsigned short GetFS(void)
{
 asm volatile (
    "mov %fs, %ax	\n\t"
 );
}

static inline void SetFS(unsigned short sel)
{
 __asm__ __volatile__("movw %%ax,%%fs" : : "a" (sel));
}

static inline unsigned short RestoreOS2FS(void)
{
 asm volatile (
        "push    $0x0150b       \n\t"
        "movw    %fs,%ax	\n\t"
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
