/* $Id: common_x86asm.h,v 1.2 2000-05-21 20:20:44 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
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
 *  Changed by Andre Werthmann <wertmann@cs.uni-potsdam.de> for using the
 *  new Katmai functions
 */
#ifndef _common_3dnow_h
#define _common_3dnow_h

#define  GL_CPU_GenuineIntel     1
#define  GL_CPU_MMX              2
#define  GL_CPU_3Dnow            4
#define  GL_CPU_Katmai           8 /* set if the katmai-instructions are available */
#define  GL_CPU_AnyX86           16 /* set if x86 asm allowed */

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#ifdef USE_X86_ASM
#include "x86.h"
#ifdef USE_3DNOW_ASM
#include "3dnow.h"
#endif
#ifdef USE_KATMAI_ASM
#include "katmai.h"
#endif
#endif

extern int gl_x86_cpu_features;
extern void gl_init_all_x86_asm (void);
extern int gl_identify_x86_cpu_features(void);

#endif
