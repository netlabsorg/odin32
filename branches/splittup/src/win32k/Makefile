# $Id: Makefile,v 1.71.2.1 2001-09-27 03:08:07 bird Exp $

#
# Win32k makefile.
#
# Copyright 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Check that makefile.inc exists, If it doesn't we'll try make it.
# Check that makefile.inc is newer than configure.cmd.
#     If not newer this might be a fatal problem, so we'll fail.
#
!if [..\..\tools\bin\Exists.cmd makefile.inc] == 1
!   if [echo No makefile.inc - Reconfiguring Win32k... & configure.cmd -noninteractive]
!   endif
!endif
!if [Check.cmd $(MAKE) -nologo -q -f Check.mk makefile.inc] != 0
!   error makefile.inc is older than configure.cmd. Re-run /src/win32k/configure.cmd!
!endif


#
# Include path definitions and check if they need updating.
# Include Win32k tools and settings.
#
!include makefile.inc
!if [..\..\tools\bin\Exists.cmd $(KKRNLLIBINCLUDE)\OS2Krnl.h] == 1
!   if [echo Bad makefile.inc - Reconfiguring Win32k... & configure.cmd -noninteractive]
!   endif
!   include makefile.inc
!endif
!include ../../makefile.inc
!include win32k.tools.mk


#
# Subdirectories.
#
SUBDIRS = \
.\kKrnlLib \
.\lib \
.\rexx \
.\utils


#
# Targets.
#
all:
!if "$(DDKPATH)" == "" || "$(MSCPATH)" == ""  || "$(TOOLKIT)" == "" || "$(VACPATH)" == ""
    $(DODIRS) "lib" $(MAKE_CMD)
    $(MAKE_CMD) -f xx2lx.mak
!else
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD)
    $(MAKE_CMD) -f win32k.mak
    $(MAKE_CMD) -f xx2lx.mak
!endif


#
# Individual Target Forwarder Rules.
#
$(WIN32KBIN)\win32k.sys win32k.sys win32k \
$(WIN32KBIN)\xx2lx.exe  xx2lx.exe  xx2lx: .force
    $(MAKE_CMD) -f $(@B).mak

$(WIN32KBIN)\win32ktst.exe win32ktst.exe win32ktst \
$(WIN32KBIN)\libconv.exe   libconv.exe   libconv \
clfix.exe: .force
    $(MAKE_CMD) -f win32k.mak $(@R).exe

$(WIN32KINCLUDE)\win32k.inc     win32k.inc \
$(WIN32KINCLUDE)\api.inc        api.inc \
$(WIN32KINCLUDE)\options.inc    options.inc: .force
    $(MAKE_CMD) -f win32k.mak $(WIN32KINCLUDE)\$(@B).inc

win32kcc.exe win32kcc:
    $(DODIRS) "utils" $(MAKE_CMD)


#
# Lib rule - make the win32k.lib object library.
#
lib:
    $(DODIRS) ".\KrnlLib .\lib" $(MAKE_CMD) lib


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
    $(RM) $(WIN32KBIN)\*
!endif
!if "$(WIN32KOBJ)" != ""
    $(RM) $(WIN32KOBJ)\*
!endif
!if "$(WIN32KLIST)" != ""
    $(RM) $(WIN32KLIST)\*
!endif
!if "$(WIN32KINCLUDE)" != ""
    $(RM) $(WIN32KBIN)\options.inc \
          $(WIN32KBIN)\api.inc \
          $(WIN32KBIN)\win32k.inc
!endif
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) clean


#
# Dependencies - all deps are autogenereated.
#
dep:
    $(RM) .depend
    $(ODIN32_TOOLS)\fastdep @<<
        -o$$(WIN32KOBJ) $(CINCLUDES)
        misc\*.c* ldr\*.c* dev32\*.c* pe2lx\*.c* k32\*.c* elf2lx\*.c* test\*.c*
        -objobj_tst
        misc\*.c* ldr\*.c* dev32\*.c* pe2lx\*.c* k32\*.c* elf2lx\*.c* test\*.c*
        $(WIN32KINCLUDE)\*.h
        $(KKRNLLIBINCLUDE)\*.h
<<
    $(ODIN32_TOOLS)\fastdep @<<
        -a+ -o$$(WIN32KOBJ) -I$(WIN32KINCLUDE) -I$(DDKPATH)\inc
        dev16\*.asm ldr\*.asm misc\*.asm dev32\*.asm pe2lx\*.asm k32\*.asm elf2lx\*.asm test\*.asm
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
# Api doc
#
StateUpd:
    ..\..\tools\bin\StateUpd.exe -Dll:Win32k dev16 dev32 misc ldr elf2lx mmf k32
    ..\..\tools\bin\StateUpd.exe -Dll:Pe2Lx Pe2Lx
    $(DODIRS) ".\kKrnlLib" $(MAKE_CMD) $@


#
# Dummy rule for force.
#
.force:


#
# Stub the rest of the rules.
#
$(COMMONRULES:clean dep lib all=):

