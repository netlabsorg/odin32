/* $Id: glheader.h,v 1.1 2000-05-21 20:57:14 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
 *
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
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


#ifndef GLHEADER_H
#define GLHEADER_H


/*
 * This is the top-most include file of the Mesa sources.
 * It includes gl.h and all system headers which are needed.
 * Other Mesa source files should _not_ directly include any system
 * headers.  This allows Mesa to be integrated into XFree86 and
 * allows system-dependent hacks/work-arounds to be collected in one place.
 *
 * If you touch this file, everything gets recompiled!
 *
 * This file should be included before any other header in the .c files.
 */


#ifdef XFree86LOADER
#include "xf86_ansic.h"
#else
#ifndef __OS2_H__
#include <windows.h>
#endif
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(__linux__) && defined(__i386__)
#include <fpu_control.h>
#endif
#endif
#include <float.h>

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#include "gl.h"
#include "glext.h"


#ifdef __WIN32OS2__
#define PTR_DECL_IN_FRONT
#endif

/*
 * Put compiler/OS/assembly pragmas and macros here to avoid
 * cluttering other source files.
 */


/* Disable unreachable code warnings for Watcom C++ */
#ifdef  __WATCOMC__
#pragma disable_message(201)
#endif


/* Turn off macro checking systems used by other libraries */
#ifdef CHECK
#undef CHECK
#endif


/* Create a macro so that asm functions can be linked into compilers other
 * than GNU C
 */
#ifndef _ASMAPI
#if !defined( __GNUC__ ) && !defined( VMS )
#define _ASMAPI __cdecl
#else
#define _ASMAPI
#endif
#ifdef  PTR_DECL_IN_FRONT
#define _ASMAPIP * _ASMAPI
#else
#define _ASMAPIP _ASMAPI *
#endif
#endif

#ifdef USE_X86_ASM
#define _NORMAPI _ASMAPI
#define _NORMAPIP _ASMAPIP
#else
#define _NORMAPI
#define _NORMAPIP *
#endif



#endif /* GLHEADER_H */
