# $Id: Makefile,v 1.10 2001-09-23 06:44:14 bird Exp $

#
# kKrnlLib.sys makefile.
#
# Copyright 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Include path definitions, common stuff and our own tools.
#
!include makefile.inc
!include ../../../makefile.inc
!include ../win32k.tools.mk


#
# Main targetname
#
NAME = kKrnlLib


#
# Main rules.
#
!if "$(DDKPATH)" != "" && "$(MSCPATH)" != ""  && "$(TOOLKIT)" != "" && "$(VACPATH)" != ""
all:    $(WIN32KBIN)\$(NAME).sys \
        $(ODIN32_LIB)\$(NAME).lib
!endif


#
# kKrnlLib.sys objects and libraries - Don't mess with the order or objects and libraries!
#
OBJS  =\
    $(WIN32KOBJ)\devfirst.obj \
    $(WIN32KOBJ)\d16strat.obj \
    $(WIN32KOBJ)\d32hlp.obj \
    $(WIN32KOBJ)\d32globals.obj \
    $(WIN32KOBJ)\d16globl.obj \
    $(WIN32KOBJ)\calltaba.obj \
    $(WIN32KOBJ)\krnlInit.obj \
    $(WIN32KOBJ)\krnlLockedWrite.obj \
    $(WIN32KOBJ)\krnlOverloading.obj \
    $(WIN32KOBJ)\krnlPrologs.obj \
    $(WIN32KOBJ)\krnlStubs.obj \
    $(WIN32KOBJ)\asmutils.obj \
    $(WIN32KOBJ)\kAssert.obj \
    $(WIN32KOBJ)\kAVLPV.obj \
    $(WIN32KOBJ)\kAVLStr.obj \
    $(WIN32KOBJ)\kAVLStrI.obj \
    $(WIN32KOBJ)\kAVLULM.obj \
    $(WIN32KOBJ)\malloc.obj \
    $(WIN32KOBJ)\smalloc.obj \
    $(WIN32KOBJ)\rmalloc.obj \
    $(WIN32KOBJ)\new.obj \
    $(WIN32KOBJ)\stricmp.obj \
    $(WIN32KOBJ)\vsprintf.obj \
    $(WIN32KOBJ)\vprintf.obj \
    $(WIN32KOBJ)\abort.obj \
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\env.obj \
    $(WIN32KOBJ)\vprntf16.obj \
    $(WIN32KOBJ)\d32init.obj \
    $(WIN32KOBJ)\d16init.obj_init \
    $(WIN32KOBJ)\ProbKrnl.obj_init \
    $(WIN32KOBJ)\SymDB.obj

LASTOBJ =\
    $(WIN32KOBJ)\devlast.obj

LASTLIB = $(WIN32KOBJ)\lastlib.lib

LIBS =\
    $(VACPATH)\lib\$(RTLLIB_NRE) \
    $(DDKPATH)\lib\os2386.lib \
    $(WIN32KOBJ)\devhelp.lib \
    $(WIN32KOBJ)\clib.lib


#
# kKrnlLib.sys rule. (Don't mess with the order or objects and libraries!)
#
$(NAME).sys: $(WIN32KBIN)\$(NAME).sys

$(WIN32KBIN)\$(NAME).sys:   $(WIN32KBIN)\clfix.exe \
                            $(WIN32KINCLUDE)\options.inc \
                            $(OBJS) \
                            $(LIBS) \
                            $(WIN32KOBJ)\$(NAME)bldlevel.def \
                            $(LASTLIB) \
                            $(LIBSINIT) \
                            $(WIN32KOBJ)\$(@B).lnk \
                            Makefile makefile.inc ..\..\..\makefile.inc
    -@$(ECHO) recompiling d16globl to get correct build time and date:
    $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) -Fo$(WIN32KOBJ)\d16globl.obj \
        -Fa$(WIN32KLIST)\d16globl.s dev16\d16globl.c
    -@$(ECHO) linking: $@
!ifdef GREP
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk | $(GREP) -v LNK4001 | $(GREP) -v LNK4031
!else
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk
!endif
    cd $(WIN32KBIN)
    @mapsym $(*B).map > nul
    cd $(WIN32KBASE)
    if not exist $(ODIN32_BIN) $(CREATEPATH) $(ODIN32_BIN)
    $(CP) $@ $(ODIN32_BIN)
    $(CP) $*.sym $(ODIN32_BIN)
    $(CP) $(WIN32KBASE)\$(*B).ddp $(ODIN32_BIN)

# Linker file.
$(WIN32KOBJ)\$(NAME).lnk: Makefile makefile.inc ..\..\..\makefile.inc
    echo Creating linkerfile: @<<$(WIN32KOBJ)\$(NAME).lnk
/OUT:$(WIN32KBIN)\$(NAME).sys
/MAP:$(WIN32KBIN)\$(NAME).map
$(OBJS:  =^
)
/IG
$(LIBS:  =^
)
$(LASTLIB)
$(WIN32KOBJ)\$(NAME)bldlevel.def
<<KEEP

# Add bldlevel signature to kKrnlLib.def - creates temporary kKrnlLibbldlevel.def.
$(WIN32KOBJ)\$(NAME)bldlevel.def: $(NAME).def Makefile
    -$(ECHO) Creates $@ with bldlevel signature string.
    $(BLDLEVELINF) $(NAME).def $@ -R"$(NAME).def" \
        -V"#define=ODIN32_VERSION,$(ODIN32_INCLUDE)\odinbuild.h" \
        -M"#define=ODIN32_BUILD_NR,$(ODIN32_INCLUDE)\odinbuild.h"

# Last lib - needed to get LASTOBJ in after the importlibraries.
$(LASTLIB): $(LASTOBJ)
    -@$(RM) $@ > nul 2> nul
    $(ILIB) $@ $**;


#
# Win32ktst.exe object configuration.
#   (It should be as identical as possible to kKrnlLib.sys.)
#   (Don't mess with the order or objects and libraries!)
#
TSTOBJS =\
    $(WIN32KOBJ)\devfirst.obj \
    $(WIN32KOBJ)\d16strat.obj \
    $(WIN32KOBJ)\d32hlp.obj \
    $(WIN32KOBJ)\d32globals.obj \
    $(WIN32KOBJ)\d16globl.obj \
    $(WIN32KOBJ)\calltaba.obj \
    $(WIN32KOBJ)\krnlInit.obj_tst. \
    $(WIN32KOBJ)\krnlLockedWrite.obj \
    $(WIN32KOBJ)\krnlOverloading.obj \
    $(WIN32KOBJ)\krnlPrologs.obj \
    $(WIN32KOBJ)\krnlStubs.obj \
    $(WIN32KOBJ)\asmutils.obj \
    $(WIN32KOBJ)\kAssert.obj \
    $(WIN32KOBJ)\kAVLPV.obj \
    $(WIN32KOBJ)\kAVLStr.obj \
    $(WIN32KOBJ)\kAVLStrI.obj \
    $(WIN32KOBJ)\kAVLULM.obj \
    $(WIN32KOBJ)\malloc.obj_tst. \
    $(WIN32KOBJ)\smalloc.obj \
    $(WIN32KOBJ)\rmalloc.obj \
    $(WIN32KOBJ)\new.obj \
    $(WIN32KOBJ)\stricmp.obj \
    $(WIN32KOBJ)\vsprintf.obj \
    $(WIN32KOBJ)\vprintf.obj_tst. \
    $(WIN32KOBJ)\abort.obj \
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\env.obj \
    $(WIN32KOBJ)\$(NAME)tst.obj \
    $(WIN32KOBJ)\dh.obj \
    $(WIN32KOBJ)\dos.obj \
    $(WIN32KOBJ)\dosa.obj \
    $(WIN32KOBJ)\init.obj \
    $(WIN32KOBJ)\fake.obj \
    $(WIN32KOBJ)\fakea.obj \
    $(WIN32KOBJ)\TstFakers.obj \
    $(WIN32KOBJ)\vprntf16.obj_tst. \
    $(WIN32KOBJ)\d32init.obj_tst. \
    $(WIN32KOBJ)\d16init.obj_tst_init. \
    $(WIN32KOBJ)\ProbKrnl.obj_tst_init. \
    $(WIN32KOBJ)\SymDB.obj

TSTLASTOBJ = $(LASTOBJ)

TSTLASTLIB = $(WIN32KOBJ)\tstlastlib.lib

TSTLIBS = $(LIBS)


#
# kKrnlLibtst.exe rule.
#   This is a test executable; kKrnlLib.sys wrapped into an executable with emulation
#   code for most of the OS/2 kernel dependencies.
#
$(NAME)tst.exe: $(WIN32KBIN)\$(NAME)tst.exe
    $(CP) $(WIN32KBIN)\$@ $@
$(WIN32KBIN)\$(NAME)tst.exe:    $(WIN32KBIN)\clfix.exe \
                                testcase\TstFakers.c \
                                $(NAME)tst.def \
                                $(TSTOBJS:. = ) \
                                $(TSTLIBS) \
                                $(TSTLASTLIB) \
                                $(WIN32KOBJ)\$(@B).lnk \
                                Makefile makefile.inc ..\..\..\makefile.inc
    -@$(ECHO) linking: $@
!ifdef GREP
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk | $(GREP) -v LNK4001 | $(GREP) -v LNK4031
!else
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk
!endif

# Linker file.
$(WIN32KOBJ)\$(NAME)tst.lnk: Makefile makefile.inc ..\..\..\makefile.inc
    echo Creating linkerfile: @<<$(WIN32KOBJ)\$(NAME)tst.lnk
/DEBUG
/OUT:$(WIN32KBIN)\$(NAME)tst.exe
/MAP:$(WIN32KBIN)\$(NAME)tst.map
$(TSTOBJS:  =^
)
/IG
$(TSTLIBS:  =^
)
$(TSTLASTLIB)
$(NAME)tst.def
<<KEEP

# Last lib - needed to get LASTOBJ in after the importlibraries.
$(TSTLASTLIB): $(TSTLASTOBJ)
    -@$(RM) $@ > nul 2> nul
    $(ILIB) $@ $**;


#
#
#   L i b r a r i e s
#
#

#
# kKrnlLib Import library.
#
$(NAME).lib: $(ODIN32_LIB)\$(NAME).lib
$(ODIN32_LIB)\$(NAME).lib: $(WIN32KBIN)\MkCallTab.exe
    $(WIN32KBIN)\MkCallTab.exe deffile > $(WIN32KOBJ)\$(NAME)implib.def
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(WIN32KOBJ)\$(NAME)implib.def


#
# Libraries - segments are renamed for the 16-bit libraries.
#

# List of object files in the converted devhelp library.
DHLPOBJS =  +$(WIN32KOBJ)\dhcall5b.obj +$(WIN32KOBJ)\dhcal11a.obj \
            +$(WIN32KOBJ)\dhcal11g.obj +$(WIN32KOBJ)\dhret.obj \
            +$(WIN32KOBJ)\dhcall2e.obj

# Make corrected devhelp library with only the required object files.
$(WIN32KOBJ)\devhelp.lib: $(DDKPATH)\lib\dhcalls.lib $(WIN32KBIN)\libconv.exe $(DHLPOBJS:+=)
    @$(RM) $@
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $@ $(DHLPOBJS);
    @cd $(MAKEDIR)

# Convert devhelp library.
$(WIN32KOBJ)\devhelp_.lib: $(DDKPATH)\lib\dhcalls.lib
    $(WIN32KBIN)\libconv $** $@

# Extract required object files from the converted devhelp library.
$(DHLPOBJS:+=): $(WIN32KOBJ)\devhelp_.lib
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $** *$@;
    @cd $(MAKEDIR)


# List of object files in the converted crt library.
CLIBOBJS =  +$(WIN32KOBJ)\..\inp.obj       +$(WIN32KOBJ)\..\outp.obj \
            +$(WIN32KOBJ)\..\anfalmul.obj  +$(WIN32KOBJ)\..\anfaldiv.obj \
            +$(WIN32KOBJ)\..\anfalrem.obj  +$(WIN32KOBJ)\..\anfauldi.obj \
            +$(WIN32KOBJ)\..\anuldiv.obj   +$(WIN32KOBJ)\..\fmemcpy.obj \
            +$(WIN32KOBJ)\..\anulrem.obj   +$(WIN32KOBJ)\..\anlmul.obj \
            +$(WIN32KOBJ)\..\__AHINCR.obj  +$(WIN32KOBJ)\..\anlshl.obj

# Make corrected crt library with only the required object files.
$(WIN32KOBJ)\clib.lib: $(MSCPATH)\lib\clibcep.lib $(WIN32KBIN)\libconv.exe $(CLIBOBJS:+=)
    @$(RM) $@
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $@ @<<
$(CLIBOBJS);
<<
    @cd $(MAKEDIR)

# Convert devhelp library.
$(WIN32KOBJ)\clib_.lib: $(MSCPATH)\lib\clibcep.lib
    $(WIN32KBIN)\libconv $** $@ > nul

# Extract required object files from the converted crt library.
$(CLIBOBJS:+=): $(WIN32KOBJ)\clib_.lib
    @cd $(WIN32KOBJ)
    $(ILIB) $** *$@;
    @cd $(MAKEDIR)


#
# Make last library.
#
$(WIN32KOBJ)\last.lib: $(WIN32KOBJ)\devlast.obj
    -@$(RM) $@ > nul 2> nul
    $(ILIB) $@ $**;


#
#
#   S o u r c e
#
#

#
# Generate calltaba.asm
#
dev32\calltaba.asm \
$(WIN32KBASE)\dev32\calltaba.asm : dev16\probkrnl.c $(WIN32KBIN)\MkCallTab.exe
    $(WIN32KBIN)\MkCallTab.exe calltab > $@


#
# Generate TstFakers.c
#
testcase\TstFakers.c \
$(WIN32KBASE)\testcase\TstFakers.c: $(WIN32KBIN)\MkCallTab.exe
    $(WIN32KBIN)\MkCallTab.exe tstfakers > $@


#
# Make assembly version of options.h; options.inc
#
$(WIN32KINCLUDE)\options.inc:  $(WIN32KINCLUDE)\options.h
    @$(ECHO) H2Inc: $**
    $(H2INC) $** > $@


#
#
#   T o o l s
#
#

#
# Make the convert tool.
#
libconv.exe: $(WIN32KBIN)\libconv.exe
$(WIN32KBIN)\libconv.exe: Tools\libconv.c
    icc -Q+ -Ti+ -Fe$@ -Fo$(WIN32KOBJ)\$(*B).obj $**


#
# Make the MkCallTab.exe utillity.
#

# MkCallTab objects.
MKCALLTABOBJS = \
    $(WIN32KOBJ)\MkCallTab.obj_c \
    $(WIN32KOBJ)\probkrnl.obj_c \
    $(WIN32KOBJ)\d16globl.obj_c \
    $(WIN32KOBJ)\vprntf16.obj_c

# Rule to build MkCallTab.exe
MkCallTab.exe MkCallTab: $(WIN32KBIN)\MkCallTab.exe
$(WIN32KBIN)\MkCallTab.exe: $(WIN32KBIN)\clfix.exe $(MKCALLTABOBJS)
    $(LD) $(LFLAGS) /pmtype:vio @<<$(WIN32KOBJ)\$(@B).lnk
        /OUT:$@
        /MAP:$(WIN32KBIN)\$(@B).map
        $(MKCALLTABOBJS)
        $(TOOLKIT)\lib\os2286.lib
        $(MSCPATH)\lib\clibcep.lib
<<keep

# Inference rule to make the MkCallTab objects.
{dev16}.c.obj_c:
    @$(ECHO) compiling 16bit (MkCallTab): $<
    @$(CC16) -c -W3 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp -DDEBUG -DEXTRACT \
        -Fa$(WIN32KLIST)\$(*B)_c.s -Fo$(WIN32KOBJ)\$(@B).obj_c $(CINCLUDES16) $<


#
# Make the clfix.exe utillity - clfix.exe
#   This should fix some of the problems we are experiencing with the
#   MSC v6.0a compiler (cl.exe) (16-bit).
#
clfix.exe: $(WIN32KBIN)\clfix.exe
$(WIN32KBIN)\clfix.exe: Tools\clfix.c
    $(CC) -Fe$(WIN32KBIN)\$(@F) -Fo$(WIN32KBIN)\$(@B).obj $**


#
# Make the symbol extract utillity - Extract.exe
#

# Extract object files.
EXTRACTOBJS = \
    $(WIN32KOBJ)\Extract.obj_e \
    $(WIN32KOBJ)\Extract32.obj \
    $(WIN32KOBJ)\krnlPrologs.obj \
    $(WIN32KOBJ)\ProbKrnl.obj_e \
    $(WIN32KOBJ)\d16Globl.obj_e \
    $(WIN32KOBJ)\vprntf16.obj_e

# Rules to build Extract.exe.
$(WIN32KBIN)\Extract.exe: $(WIN32KBIN)\clfix.exe $(EXTRACTOBJS) Extract.def Makefile
    $(LD) $(LFLAGS) /pmtype:vio @<<$(WIN32KOBJ)\$(@B).lnk
        /OUT:$@
        /MAP:$(WIN32KBIN)\$(@B).map
        Extract.def
        $(EXTRACTOBJS)
        $(TOOLKIT)\lib\os2286.lib
        $(MSCPATH)\lib\clibcep.lib
        $(VACPATH)\lib\$(RTLLIB_NRE)
        $(DDKPATH)\lib\os2386.lib
<<keep

# Copy rule.
Extract.exe: $(WIN32KBIN)\Extract.exe
    $(CP) $** $@

# Inference rule to make the Extract objects.
{dev16}.c.obj_e:
    @$(ECHO) compiling 16bit (extract): $<
    @$(CC16) -c -W3 -Asfw -G2s -Zp -Zl -nologo -Zi -Od -Lp -DDEBUG -DEXTRACT \
        -Fa$(WIN32KLIST)\$(*B)_e.s -Fo$(WIN32KOBJ)\$(@B).obj_e $(CINCLUDES16) $<

{dev32}.c.obj_e:
    @$(ECHO) compiling 32bit (extract): $<
    $(CC) -c $(CFLAGS) -Gr -Rn -Ti- -ndBIG_ $(CDEFINES) $(CINCLUDES) -Fa$(WIN32KLIST)\$(@B)_e.s \
        -Fo$(WIN32KOBJ)\$(@F) $<


#
# Dependencies - all deps are autogenereated.
#
dep:
    $(RM) .depend
    $(ODIN32_TOOLS)\fastdep @<<
        -f+ -o$$(WIN32KOBJ) $(CINCLUDES) -xdev32\calltaba.asm -xtestcase\TstFakers.c
        dev32\*.c* clib\*.c* kern\*.c* testcase\*.c*
        -objobj_tst
        dev32\*.c* clib\*.c* kern\*.c* testcase\*.c*
        $(WIN32KINCLUDE)\*.h
<<
    $(ODIN32_TOOLS)\fastdep @<<
        -a+ -o$$(WIN32KOBJ) -I$(WIN32KINCLUDE) -I$(DDKPATH)\inc
        -xdev32\calltaba.asm -xtestcase\TstFakers.c
        dev16\*.asm dev32\*.asm clib\*.asm kern\*.asm testcase\*.asm
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


#
# Include the .depend file.
#
!if [$(EXISTS) .depend] == 0
!   include .depend
!else
!   if [$(ECHO) .depend doesn't exist]
!   endif
!endif


#
# Generate Visual SlickEdit project file
#
kKrnlLib.vpj:
    ..\..\..\tools\vslick\genproject.cmd $@  $(CINCLUDES) -s -r

