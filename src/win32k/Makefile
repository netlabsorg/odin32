# $Id: Makefile,v 1.65 2000-12-16 23:07:25 bird Exp $

#
# Win32k makefile.
#
# Copyright 1998-2000 knut st. osmundsen
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Check that makefile.inc exists, If it doesn't we'll try make it.
# Check that makefile.inc is newer than configure.cmd.
#     If not newer this might be a fatal problem, so we'll fail.
#
!if [..\..\tools\bin\Exists.cmd makefile.inc] == 1
!   if [configure.cmd noninteractive]
!   endif
!endif
!if [Check.cmd $(MAKE) -nologo -q -f Check.mk makefile.inc] != 0
!   error makefile.inc is older than configure.cmd. Re-run configure.cmd!
!endif


#
# Include path definitions and check if they need updating.
# Include win32k tools and settings.
#
!include makefile.inc
!if [..\..\tools\bin\Exists.cmd $(ODIN32_INCLUDE)\win32k.h] == 1
!   if [configure.cmd noninteractive]
!   endif
!   include makefile.inc
!endif
!include ../../makefile.inc
!include win32k.tools.mk


#
# Subdirectories.
#
SUBDIRS = \
.\lib \
.\pe2lx \
.\rexx \
.\utils


#
# Targets.
#
all:
!if "$(DDKPATH)" == "" || "$(MSCPATH)" == ""  || "$(TOOLKIT)" == "" || "$(VACPATH)" == ""
    $(DODIRS) "lib pe2lx" $(MAKE_CMD)
!else
    $(MAKE_CMD) -f win32k.mak
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD)
!endif


#
# Lib rule - make the win32k.lib object library.
#
lib:
    $(DODIRS) ".\lib" $(MAKE_CMD) lib


#
# Cleanup rule.
#
clean:
    @$(ECHO) cleaning....
    $(RM) *.sys *.dll *.lib *.obj *.exe *.sym *.map *.pch *.err \
        $(ODIN32_BIN)\win32k.sys \
        $(ODIN32_BIN)\win32k.sym \
        $(ODIN32_BIN)\win32k.ddp
!if "$(WIN32KBIN)" != ""
    $(RM) $(WIN32KBIN)\*.*
!endif
!if "$(WIN32KOBJ)" != ""
    $(RM) $(WIN32KOBJ)\*obj* \
        $(WIN32KOBJ)\*lnk* \
        $(WIN32KOBJ)\*res* \
        $(WIN32KOBJ)\*lib* \
        $(WIN32KOBJ)\*def*
!endif
!if "$(WIN32KLIST)" != ""
    $(RM) $(WIN32KLIST)\*lst* \
        $(WIN32KLIST)\*.s* \
        $(WIN32KLIST)\*as*
!endif
!if "$(WIN32KBASE)" != ""
    $(RM) $(WIN32KBASE)\ldr\calltaba.asm $(WIN32KBASE)\test\TstFakers.c
!endif
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) clean


#
# Dependencies - all deps are autogenereated.
#
dep:
    $(ODIN32_TOOLS)\fastdep @<<
        -o$$(WIN32KOBJ) $(CINCLUDES) -xldr\calltaba.asm -xtest\TstFakers.c
        misc\*.c* ldr\*.c* dev32\*.c* pe2lx\*.c* k32\*.c* lib\*.c elf2lx\*.c* test\*.c*
        -objobj_tst
        misc\*.c* ldr\*.c* dev32\*.c* pe2lx\*.c* k32\*.c* lib\*.c elf2lx\*.c* test\*.c*
        $(WIN32KINCLUDE)\*.h
<<
    $(ODIN32_TOOLS)\fastdep @<<
        -a+ -o$$(WIN32KOBJ) -I$(WIN32KINCLUDE) -I$(DDKPATH)\inc
        -xldr\calltaba.asm -xtest\TstFakers.c
        dev16\*.asm ldr\*.asm misc\*.asm dev32\*.asm pe2lx\*.asm k32\*.asm lib\*.asm elf2lx\*.asm test\*.asm
        $(WIN32KINCLUDE)\*.inc
<<
    $(ODIN32_TOOLS)\fastdep @<<
        -a+ -o$$(WIN32KOBJ) $(CINCLUDES16)
        dev16\*.c*
        -objobj_e           $(CINCLUDES16) dev16\*.c*
        -objobj_c           $(CINCLUDES16) dev16\*.c*
        -objobj_tst         $(CINCLUDES16) dev16\*.c*
        -objobj_init        $(CINCLUDES16) dev16\*.c*
        -objobj_tst_init    $(CINCLUDES16) dev16\*.c*
<<
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) dep


#
# Generate Visual SlickEdit project file
#
win32k.vpj:
    ..\..\tools\vslick\genproject.cmd $@  $(CINCLUDES) -s -r


#
# Documentation generation - require SDS - http://www.ii.uib.no/~stig/
#  (Old SDS 0.33 - don't work anylonger.)
#
lxdoc:
    cpp2sds -o DocsA\ -i include\modulebase.h include\pe2lx.h ldr\modulebase.cpp \
        pe2lx\pe2lx.cpp
    cd DocsA
    exp sds2doc -ai *.sds

fulldoc:
    exp cpp2sds -o DocsA\ -i include\*.h *.cpp ldr\*.cpp misc\*.cpp pe2lx\*.cpp
    cd DocsA
    exp sds2doc -ai *.sds

