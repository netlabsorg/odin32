/* $Id: common_x86.c,v 1.3 2000-05-23 20:40:25 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
 *
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 *  Check CPU capabilities & initialize optimized funtions for this particular
 *   processor.
 *
 *  Written by Holger Waechtler <holger@akaflieg.extern.tu-berlin.de>
 */

#include "glheader.h"
#include "common_x86asm.h"

#ifdef __WIN32OS2__
#include <cpuhlp.h>
#else
int gl_x86_cpu_features = 0;
#endif

static void message(const char *msg)
{
   if (getenv("MESA_DEBUG"))
      fprintf(stderr, "%s\n", msg);
}


void gl_init_all_x86_asm (void)
{
#ifdef USE_X86_ASM

#ifdef __WIN32OS2__
   /* Odin specific - use 'built in' cpuhlp lib */
   if(CPUFeatures & CPUID_FPU_PRESENT)
     {
       gl_init_x86_asm_transforms();
     }

#if defined(USE_3DNOW_ASM)
   if(CPUFeatures /*& CPUID_3DNOW*/)
     {
       gl_init_3dnow_asm_transforms();
     }
#endif                                 /* USE_3DNOW_ASM                    */

#else                                  /* __WIN32OS2__                     */
   gl_x86_cpu_features = gl_identify_x86_cpu_features ();
   gl_x86_cpu_features |= GL_CPU_AnyX86;

   if (getenv("MESA_NO_ASM") != 0)
      gl_x86_cpu_features = 0;

   if (gl_x86_cpu_features & GL_CPU_GenuineIntel) {
      message("GenuineIntel cpu detected.");
   }

   if (gl_x86_cpu_features) {
      gl_init_x86_asm_transforms ();
   }

#ifdef USE_MMX_ASM
   if (gl_x86_cpu_features & GL_CPU_MMX) {
      char *s = getenv( "MESA_NO_MMX" );
      if (s == NULL) {
         message("MMX cpu detected.");
      } else {
         gl_x86_cpu_features &= (~GL_CPU_MMX);
      }
   }
#endif                                 /* USE_MMX_ASM                      */


#ifdef USE_3DNOW_ASM
   if (gl_x86_cpu_features & GL_CPU_3Dnow) {
      char *s = getenv( "MESA_NO_3DNOW" );
      if (s == NULL) {
         message("3Dnow cpu detected.");
         gl_init_3dnow_asm_transforms ();
      } else {
         gl_x86_cpu_features &= (~GL_CPU_3Dnow);
      }
   }
#endif                                 /* USE_3DNOW_ASM                    */

#endif                                 /* __WIN32OS2__                     */

#endif                                 /* USE_X86_ASM                      */
}

