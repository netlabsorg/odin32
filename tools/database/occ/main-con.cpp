/*
  Copyright (C) 1997-2001 Shigeru Chiba, Tokyo Institute of Technology.

  Permission to use, copy, distribute and modify this software and   
  its documentation for any purpose is hereby granted without fee,        
  provided that the above copyright notice appear in all copies and that 
  both that copyright notice and this permission notice appear in 
  supporting documentation.

  Shigeru Chiba makes no representations about the suitability of this 
  software for any purpose.  It is provided "as is" without express or
  implied warranty.
*/

/*
  Copyright (c) 1995, 1996 Xerox Corporation.
  All Rights Reserved.

  Use and copying of this software and preparation of derivative works
  based upon this software are permitted. Any copy of this software or
  of any derivative work must include the above copyright notice of
  Xerox Corporation, this paragraph and the one after it.  Any
  distribution of this software or derivative works must comply with all
  applicable United States export control laws.

  This software is made available AS IS, and XEROX CORPORATION DISCLAIMS
  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#include <iostream.h>

#if !defined(_MSC_VER)

#include "types.h"

#else

/*
  JLF: The GC_gc_no *MUST* be declared as below to be able to link...
  Unfortunately, this declaration collides with the one found in gc.h
  This is due to the fact that the _DLL macro is not defined when
  compiling occ.exe, thus driving to the extern declaration instead of
  __declspec(dllimport).  To bypass this problem, the Gc_gc_no is
  temporarily renamed before including gc.h (through types.h).
*/

#define GC_gc_no my_GC_gc_no
#include "types.h"
#undef GC_gc_no

#ifndef DONT_GC
extern "C" __declspec(dllimport) GC_word GC_gc_no;
#endif

#endif /* !defined(_MSC_VER) */

// these are defined in driver.cc
extern bool verboseMode;
extern void Compile(int, char**);

int main(int argc, char** argv)
{
    Compile(argc, argv);
    if(verboseMode) {
        cerr << "[done.]\n";
#ifndef DONT_GC
        cerr << "[GC happened " << GC_gc_no << " times.]\n";
#endif
    }

    return 0;
}

