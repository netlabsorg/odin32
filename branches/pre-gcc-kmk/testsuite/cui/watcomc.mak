# $Id: watcomc.mak,v 1.4 2002-06-25 01:21:19 bird Exp $

#
# This script contains the testcases for Doxygen version 1.2.16
#
# Note! We doesn't include any MAKE_INCLUDE_PROCESS file.
#

# Testsuite config for the process include.
PATH_ROOT = ..\..
TARGET_MODE = TESTCASE
TARGET_NAME = watcomc
MAKEFILE    = $(TARGET_NAME).mak

#
# Check if the system includes watcom.
# (We can't do these test without watcom...)
#
!if [$(PATH_ROOT)\tools\bin\BuildEnv.cmd watcomc11c?] == 0


BUILD_ENV_FORCE=WAT11C
# Include the setup.mak and testsuite.tools.mk.
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include $(PATH_MAKE)\testsuite.tools.mk


# Include the process rulez.
!include $(MAKE_INCLUDE_PROCESS)



#
# These are the files which should be installed in order to run watcomc.
#
WCL386 = $(WATCOM)\binnt\wcl386.exe
WCC386 = $(WATCOM)\binnt\wcc386.exe
WLINK  = $(WATCOM)\binnt\wlink.exe
WMAKE  = $(WATCOM)\binnt\wmake.exe

BIN_FILES = $(WCL386) $(WCC386) $(WLINK) $(WMAKE)


#
# Main rule, this runs all the other rulez.
#
testcase:   $(BIN_FILES)
    @$(ECHO) + Running testcases for WatCom C/C++ v11.0c(beta) $(CLRRST)
    $(TS_EXEC_1) "wcl386 help" $(TS_EXEC_2) testcase-wcl386-help
    $(TS_EXEC_1) "wcc386 compile 'hello world'" $(TS_EXEC_2) testcase-compile
    $(TS_EXEC_1) "wcc386+wlink compile, link and run 'hello world'" $(TS_EXEC_2) testcase-helloworld
    $(TS_EXEC_1) "wcl386 compile and link 'hello world'" $(TS_EXEC_2) testcase-compile-and-link
    @$(ECHO) + Completed WatCom C/C++ v11.0c(beta)! $(CLRRST)


#
# This testcase checks the help output.
# Broken: 'press any key to continue' Doesn't detect pipe.
#
testcase-wcl386-help: $(PATH_TARGET)\wcl386.help.stdout
    SET PATH=$(WATCOM)\winnt;$(PATH)
    -1 $(TS_PEC) $(WCL386) > $(PATH_TARGET)\stdout 2> $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_Z)   $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_NZ)  $(PATH_TARGET)\stdout
    $(TS_CMP) $(PATH_TARGET)\stdout $(PATH_TARGET)\wcl386.help.stdout

# This is the output of testcase-help
$(PATH_TARGET)\wcl386.help.stdout: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
Watcom C/C++32 Compile and Link Utility Version 11.0c
Copyright by Sybase, Inc., and its subsidiaries, 1988, 2000.
All rights reserved.  Watcom is a trademark of Sybase, Inc.
Usage:  wcl386 {option(s)} {file(s)}
Options:  ( - option is also accepted )
/c    compile only, no link
/cc   treat source files as C code
/cc++ treat source files as C++ code
/y    ignore the WCL386 environment variable
                             [Processor options]
/3r 386 register calling conventions  /5r Pentium register calling conv.
/3s 386 stack calling conventions     /5s Pentium stack calling conventions
/4r 486 register calling conventions  /6r Pentium Pro register call conven.
/4s 486 stack calling conventions     /6s Pentium Pro stack call conven.
                     [Floating-point processor options]
/fpc calls to floating-point library  /fp2 generate 287 floating-point code
/fpd enable Pentium FDIV check        /fp3 generate 387 floating-point code
/fpi   inline 80x87 with emulation    /fp5 optimize f-p for Pentium
/fpi87 inline 80x87                   /fp6 optimize f-p for Pentium Pro
/fpr use old floating-point conventions
                             [Compiler options]
/db generate browsing information     /s  remove stack overflow checks
/e=<n> set error limit number         /sg generate calls to grow the stack
/ee call epilogue hook routine        /st touch stack through SS first
/ef full paths in messages            /v  output func declarations to .def
/ei force enums to be type int        /vcap VC++ compat: alloca in arg lists
/em minimum base type for enum is int /w=<n>  set warning level number
/en emit routine names in the code    /wcd=<n> disable warning message <n>
/ep[=<n>] call prologue hook routine  /wce=<n> enable warning message <n>
/eq do not display error messages     /we treat all warnings as errors
/et P5 profiling                      /wx (C++) set warning level to max
/ez generate PharLap EZ-OMF object    /xr (C++) enable RTTI
/fh=<file> pre-compiled headers       /z{a,e} disable/enable extensions
/fhq[=<file>] fh without warnings     /zc place strings in CODE segment
/fhr  (C++) only read PCH             /zd{f,p} DS floats vs DS pegged to DGROUP
/fhw  (C++) only write PCH            /zdl load DS directly from DGROUP
/fhwe (C++) don't count PCH warnings  /zf{f,p} FS floats vs FS pegged to seg
/fi=<file> force include of file      /zg{f,p} GS floats vs GS pegged to seg
/fo=<file> set object file name       /zg function prototype using base type
/fr=<file> set error file name        /zk{0,0u,1,2,3,l} double-byte support
/ft (C++) check for 8.3 file names    /zku=<codepage> UNICODE support
/fx (C++) no check for 8.3 file names /zl  remove default library information
/g=<codegroup> set code group name    /zld remove file dependency information
/hc codeview debug format             /zm  place functions in separate segments
/hd dwarf debug format                /zmf (C++) zm with near calls allowed
/hw watcom debug format               /zp{1,2,4,8,16} struct packing align.
/j  change char default to signed     /zpw warning when padding a struct
/m{f,s,m,c,l}  memory model           /zq operate quietly
/nc=<name>  set CODE class name       /zs check syntax only
/nd=<name>  set data segment name     /zt<n> set data threshold
/nm=<module_name> set module name     /zu SS != DGROUP
/nt=<name>  set text segment name     /zv (C++) enable virt. fun. removal opt
/r  save/restore segregs across calls /zw  generate code for MS Windows
/ri promote function args/rets to int /zz remove @size from __stdcall func.
                             [Debugging options]
/d0 no debugging information          /d2t (C++) d2 but without type names
/d1{+} line number debugging info.    /d3 debug info with unref'd type names
/d2 full symbolic debugging info.     /d3i (C++) d3 + inlines as COMDATs
/d2i (C++) d2 + inlines as COMDATs    /d3s (C++) d3 + inlines as statics
/d2s (C++) d2 + inlines as statics
                           [Optimization options]
/oa relax alias checking              /ol+ ol with loop unrolling
/ob branch prediction                 /om generate inline math functions
/oc disable call/ret optimization     /on numerically unstable floating-point
/od disable optimizations             /oo continue compile when low on memory
/oe{=num} expand functions inline     /op improve floating-point consistency
/of{+} generate traceable stack frames/or re-order instructions to avoid stalls
/oh enable repeated optimizations     /os optimize for space
/oi inline intrinsic functions        /ot optimize for time
/oi+ (C++) oi with max inlining depth /ou ensure unique addresses for functions
/ok control flow entry/exit seq.      /ox maximum optimization (/obmilr /s)
/ol perform loop optimizations
                      [C++ exception handling options]
/xd  no exception handling            /xs  exception handling: balanced
/xds no exception handling: space     /xss exception handling: space
/xdt no exception handling            /xst exception handling: time
                           [Preprocessor options]
/d<name>[=text] define a macro        /u<name>  undefine macro name
/d+ extend syntax of /d option        /p{c,l,w=<n>} preprocess source file
/fo=<filename> set object file name      c     -> preserve comments
/i=<directory>  include directory        l     -> insert #line directives
/t=<n> (C++) # of spaces in tab stop     w=<n> -> wrap output at column n
/tp=<name> (C) set #pragma on( <name>
                              [Linker options]
/bd  build Dynamic link library       /fm[=<map_file>]  generate map file
/bm  build Multi-thread application   /k<stack_size> set stack size
/br  build with dll run-time library  /l=<system> link for the specified system
/bw  build default Windowing app.     /x  make names case sensitive
/bt=<os> build target OS.             @<directive_file> include file
/fd[=<directive_file>] directive file /"linker directives"
/fe=<executable> name executable file
<<KEEP


#
# Testcase where we try to run compile some simple source.
#
testcase-compile: $(PATH_TARGET)\helloworld.c
    SET PATH=$(WATCOM)\winnt;$(PATH)
    $(TS_PEC) $(WCC386) -I$(WATCOM)\h\nt -I$(WATCOM)\h -Fo=$(PATH_TARGET)\compile.obj \
        $(PATH_TARGET)\helloworld.c
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\compile.obj


#
# Testcase where we try to run compile some simple source.
#
testcase-compile-and-link: $(PATH_TARGET)\helloworld.c
    SET PATH=$(WATCOM)\winnt;$(PATH)
    $(TS_PEC) $(WCL386) -bt=tnt -l=tnt -I$(WATCOM)\h\nt -I$(WATCOM)\h \
        -Fo=$(PATH_TARGET)\compileandlink.obj -Fe=$(PATH_TARGET)\compileandlink.exe \
        $(PATH_TARGET)\helloworld.c
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\compileandlink.obj
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\compileandlink.exe


#
# Testcase where we try to run doxygen on tools/common/kFileFormatBase.[cpp|h].
#
testcase-helloworld: \
            $(PATH_TARGET)\helloworld.c \
            $(PATH_TARGET)\helloworld.stdout
    SET PATH=$(WATCOM)\winnt;$(PATH)
    $(TS_PEC) $(WCC386) -I$(WATCOM)\h\nt -I$(WATCOM)\h -Fo=$(PATH_TARGET)\helloworld.obj \
        $(PATH_TARGET)\helloworld.c
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\helloworld.obj
    SET LIB=$(WATCOM)\lib386\nt;$(WATCOM)\lib386;
    $(TS_PEC) $(WLINK) \
        FORMAT Window NT \
        FILE $(PATH_TARGET)\helloworld.obj \
        NAME $(PATH_TARGET)\helloworld.exe \
        LIBPATH $(WATCOM)\lib386\nt;$(WATCOM)\lib386
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\helloworld.exe
    $(TOOL_COPY) $(PATH_TARGET)\helloworld.exe .\helloworld.exe
    $(TS_PEC) .\helloworld.exe > $(PATH_TARGET)\stdout 2> $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\stdout
    $(TS_CMP) $(PATH_TARGET)\stderr <<
Hello stderr
<<
    $(TS_SED) "s/=$(MAKEDIR:\=\\)\\HELLOWORLD\.EXE/=HelloWorld\.ExE/I" $(PATH_TARGET)\stdout | $(TS_CMP) $(PATH_TARGET)\helloworld.stdout -


# generate some C source code to be compiled.
$(PATH_TARGET)\helloworld.c: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
#include <stdio.h>                      /* Needed for [f]printf prototype. */
#include <sys/types.h>                  /* just to see if it's found. */

int main(int argc, char **argv)
{
    printf("Hello World\n");
    printf("argc=%d argv0=%s\n", argc, argv[0]);
    fprintf(stderr, "Hello stderr\n");
    return 0;
}
<<KEEP

# stdout from helloworld.exe
$(PATH_TARGET)\helloworld.stdout: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
Hello World
argc=1 argv0=HelloWorld.ExE
<<KEEP


#
# Clean rule
#
!if "$(MAKE_INCLUDE_PROCESS:process.forwarder=)" == "$(MAKE_INCLUDE_PROCESS)"
clean:
    $(TOOL_RM) *.log $(PATH_TARGET)\* .\helloworld.exe
!else
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif


!else

# No watcom - Do nothing!
TARGET_MODE = EMPTY
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include $(MAKE_INCLUDE_PROCESS)

!endif

