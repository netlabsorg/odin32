/* $Id: os2sel.h,v 1.13 2003-04-10 10:26:23 sandervl Exp $ */
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

#ifndef __WATCOMC__
#ifdef  __EMX__
static inline unsigned short GetFS(void)
{
 int __res;
 __asm__ __volatile__("mov %%fs, %%eax" : "=a" (__res));
 return(__res);
}

static inline void SetFS(unsigned short sel)
{
 __asm__ __volatile__("mov %%eax,%%fs" : : "a" (sel));
}

static inline int RestoreOS2FS(void)
{
 int __res, xx = 0x0150b;

 __asm__ __volatile__(
   "mov   %%fs,%0      \n\t"
   "mov   %1,%%fs      \n\t"
   : "=&r" (__res) : "r" (xx));
 return(__res);
}


#else

unsigned short _System GetFS       (void);
unsigned short _System RestoreOS2FS(void);
void           _System SetFS       (unsigned short selector);
unsigned short _System SetReturnFS (unsigned short selector);
//SvL: Checks if thread FS & exception structure are valid
int            _System CheckCurFS(void);
#endif
#endif

  
  
#ifdef __cplusplus
}
#endif


#endif //__OS2SEL_H__
