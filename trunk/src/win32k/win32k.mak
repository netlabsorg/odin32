# $Id: win32k.mak,v 1.9 2001-03-11 16:46:08 bird Exp $

#
# Win32k.sys makefile.
#
# Copyright 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Include path definitions, common stuff and our own tools.
#
!include makefile.inc
!include ../../makefile.inc
!include win32k.tools.mk


#
# Main targetname
#
NAME = Win32k


#
# Main rules.
#
!if "$(DDKPATH)" == "" || "$(MSCPATH)" == ""  || "$(TOOLKIT)" == "" || "$(VACPATH)" == ""
all:    $(WIN32KBIN)\$(NAME).sys
!endif


#
# Win32k.sys objects and libraries - Don't mess with the order or objects and libraries!
#
OBJS  =\
    $(WIN32KOBJ)\devfirst.obj \
    $(WIN32KOBJ)\d16strat.obj \
    $(WIN32KOBJ)\d32CallGate.obj \
    $(WIN32KOBJ)\d32hlp.obj \
    $(WIN32KOBJ)\d32globals.obj \
    $(WIN32KOBJ)\d16globl.obj \
    $(WIN32KOBJ)\abort.obj \
    $(WIN32KOBJ)\asmutils.obj \
    $(WIN32KOBJ)\calltaba.obj \
    $(WIN32KOBJ)\locks.obj \
    $(WIN32KOBJ)\malloc.obj \
    $(WIN32KOBJ)\smalloc_avl.obj \
    $(WIN32KOBJ)\avl.obj \
    $(WIN32KOBJ)\rmalloc_avl.obj \
    $(WIN32KOBJ)\new.obj \
    $(WIN32KOBJ)\env.obj \
    $(WIN32KOBJ)\stricmp.obj \
    $(WIN32KOBJ)\yield.obj \
    $(WIN32KOBJ)\vsprintf.obj \
    $(WIN32KOBJ)\vprintf.obj \
    $(WIN32KOBJ)\ldr.obj \
    $(WIN32KOBJ)\myldrClose.obj \
    $(WIN32KOBJ)\myldrOpen.obj \
    $(WIN32KOBJ)\myldrRead.obj \
    $(WIN32KOBJ)\myLDRQAppType.obj \
    $(WIN32KOBJ)\myldrEnum32bitRelRecs.obj \
    $(WIN32KOBJ)\myldrOpenPath.obj \
    $(WIN32KOBJ)\myldrOpenPatha.obj \
    $(WIN32KOBJ)\myldrFindModule.obj \
    $(WIN32KOBJ)\myldrCheckInternalName.obj \
    $(WIN32KOBJ)\myldrGetFileName.obj \
    $(WIN32KOBJ)\d32Events.obj \
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\pe2lx.obj \
    $(WIN32KOBJ)\ModuleBase.obj \
    $(WIN32KOBJ)\d32ElfIOCtl.obj \
    $(WIN32KOBJ)\d32Win32kIOCtl.obj \
    $(WIN32KOBJ)\k32AllocMemEx.obj \
    $(WIN32KOBJ)\k32HandleSystemEvent.obj \
    $(WIN32KOBJ)\k32ProcessReadWrite.obj \
    $(WIN32KOBJ)\k32QueryCallGate.obj \
    $(WIN32KOBJ)\k32QueryOTEs.obj \
    $(WIN32KOBJ)\k32QueryOptionsStatus.obj \
    $(WIN32KOBJ)\k32QuerySystemMemInfo.obj \
    $(WIN32KOBJ)\k32SetOptions.obj \
    $(WIN32KOBJ)\mytkExecPgm.obj \
    $(WIN32KOBJ)\mytkStartProcess.obj \
    $(WIN32KOBJ)\vprntf16.obj \
    $(WIN32KOBJ)\d32init.obj \
    $(WIN32KOBJ)\d16init.obj_init \
    $(WIN32KOBJ)\ProbKrnl.obj_init \
    $(WIN32KOBJ)\SymDB.obj

LASTOBJ =\
    $(WIN32KOBJ)\devlast.obj

LIBS =\
    $(VACPATH)\lib\$(RTLLIB_NRE) \
    $(DDKPATH)\lib\os2386.lib \
    $(WIN32KOBJ)\devhelp.lib \
    $(WIN32KOBJ)\clib.lib


#
# Win32k.sys rule. (Don't mess with the order or objects and libraries!)
#
$(NAME).sys: $(WIN32KBIN)\$(NAME).sys

$(WIN32KBIN)\$(NAME).sys:   clfix.exe \
                            $(WIN32KINCLUDE)\options.inc \
                            $(WIN32KINCLUDE)\api.inc \
                            $(WIN32KINCLUDE)\win32k.inc \
                            $(OBJS) \
                            $(LIBS) \
                            $(WIN32KOBJ)\$(NAME)bldlevel.def \
                            $(LASTOBJ) \
                            $(LIBSINIT) \
                            $(WIN32KOBJ)\$(@B).lnk \
                            win32k.mak makefile.inc ..\..\makefile.inc
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
$(WIN32KOBJ)\$(NAME).lnk: win32k.mak makefile.inc ..\..\makefile.inc
    echo Creating linkerfile: @<<$(WIN32KOBJ)\$(NAME).lnk
/OUT:$(WIN32KBIN)\$(NAME).sys
/MAP:$(WIN32KBIN)\$(NAME).map
$(OBJS:  =^
)
/IG
$(LIBS:  =^
)
$(LASTOBJ)
$(WIN32KOBJ)\$(NAME)bldlevel.def
<<KEEP

# Add bldlevel signature to win32k.def - creates temporary win32kbldlevel.def.
$(WIN32KOBJ)\$(NAME)bldlevel.def: $(NAME).def win32k.mak MakeDesc.cmd
    -$(ECHO) Creates $@ with bldlevel signature string.
    $(BLDLEVELINF) $(NAME).def $@ -R"$(NAME).def" \
        -V"#define=ODIN32_VERSION,$(ODIN32_INCLUDE)\odinbuild.h" \
        -M"#define=ODIN32_BUILD_NR,$(ODIN32_INCLUDE)\odinbuild.h"


#
# Win32ktst.exe object configuration.
#   (It should be as identical as possible to win32k.sys.)
#   (Don't mess with the order or objects and libraries!)
#
TSTOBJS =\
    $(WIN32KOBJ)\devfirst.obj \
    $(WIN32KOBJ)\$(NAME)tst.obj \
    $(WIN32KOBJ)\dh.obj \
    $(WIN32KOBJ)\dos.obj \
    $(WIN32KOBJ)\dosa.obj \
    $(WIN32KOBJ)\init.obj \
    $(WIN32KOBJ)\fake.obj \
    $(WIN32KOBJ)\fakea.obj \
    $(WIN32KOBJ)\TstFakers.obj \
    $(WIN32KOBJ)\d16strat.obj \
    $(WIN32KOBJ)\d32CallGate.obj \
    $(WIN32KOBJ)\d32hlp.obj \
    $(WIN32KOBJ)\d32globals.obj \
    $(WIN32KOBJ)\d16globl.obj \
    $(WIN32KOBJ)\abort.obj \
    $(WIN32KOBJ)\asmutils.obj \
    $(WIN32KOBJ)\calltaba.obj \
    $(WIN32KOBJ)\locks.obj \
    $(WIN32KOBJ)\malloc.obj_tst. \
    $(WIN32KOBJ)\smalloc_avl.obj \
    $(WIN32KOBJ)\avl.obj \
    $(WIN32KOBJ)\rmalloc_avl.obj \
    $(WIN32KOBJ)\new.obj \
    $(WIN32KOBJ)\env.obj \
    $(WIN32KOBJ)\stricmp.obj \
    $(WIN32KOBJ)\yield.obj \
    $(WIN32KOBJ)\vsprintf.obj \
    $(WIN32KOBJ)\vprintf.obj_tst. \
    $(WIN32KOBJ)\ldr.obj \
    $(WIN32KOBJ)\myldrClose.obj \
    $(WIN32KOBJ)\myldrOpen.obj \
    $(WIN32KOBJ)\myldrRead.obj \
    $(WIN32KOBJ)\myLDRQAppType.obj \
    $(WIN32KOBJ)\myldrEnum32bitRelRecs.obj \
    $(WIN32KOBJ)\myldrOpenPath.obj \
    $(WIN32KOBJ)\myldrOpenPatha.obj \
    $(WIN32KOBJ)\myldrFindModule.obj \
    $(WIN32KOBJ)\myldrCheckInternalName.obj \
    $(WIN32KOBJ)\myldrGetFileName.obj \
    $(WIN32KOBJ)\d32Events.obj \
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\pe2lx.obj \
    $(WIN32KOBJ)\ModuleBase.obj \
    $(WIN32KOBJ)\d32ElfIOCtl.obj \
    $(WIN32KOBJ)\d32Win32kIOCtl.obj \
    $(WIN32KOBJ)\k32AllocMemEx.obj \
    $(WIN32KOBJ)\k32HandleSystemEvent.obj \
    $(WIN32KOBJ)\k32ProcessReadWrite.obj \
    $(WIN32KOBJ)\k32QueryCallGate.obj \
    $(WIN32KOBJ)\k32QueryOTEs.obj \
    $(WIN32KOBJ)\k32QueryOptionsStatus.obj \
    $(WIN32KOBJ)\k32QuerySystemMemInfo.obj \
    $(WIN32KOBJ)\k32SetOptions.obj \
    $(WIN32KOBJ)\mytkExecPgm.obj \
    $(WIN32KOBJ)\mytkStartProcess.obj \
    $(WIN32KOBJ)\vprntf16.obj_tst. \
    $(WIN32KOBJ)\d32init.obj_tst. \
    $(WIN32KOBJ)\d16init.obj_tst_init. \
    $(WIN32KOBJ)\ProbKrnl.obj_tst_init. \
    $(WIN32KOBJ)\SymDB.obj

TSTLASTOBJ = $(LASTOBJ)

TSTLIBS = $(LIBS)


#
# Win32ktst.exe rule.
#   This is a test executable; win32k.sys wrapped into an executable with emulation
#   code for most of the OS/2 kernel dependencies.
#
$(NAME)tst.exe: $(WIN32KBIN)\$(NAME)tst.exe
    $(CP) $(WIN32KBIN)\$@ $@
$(WIN32KBIN)\$(NAME)tst.exe:    clfix.exe \
                                Test\TstFakers.c \
                                $(NAME)tst.def \
                                $(TSTOBJS:. = ) \
                                $(TSTLIBS) \
                                $(TSTLASTOBJ) \
                                $(WIN32KOBJ)\$(@B).lnk \
                                win32k.mak makefile.inc ..\..\makefile.inc
    -@$(ECHO) linking: $@
!ifdef GREP
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk | $(GREP) -v LNK4001 | $(GREP) -v LNK4031
!else
    -4 $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk
!endif

# Linker file.
$(WIN32KOBJ)\$(NAME)tst.lnk: win32k.mak makefile.inc ..\..\makefile.inc
    echo Creating linkerfile: @<<$(WIN32KOBJ)\$(NAME)tst.lnk
/DEBUG
/OUT:$(WIN32KBIN)\$(NAME)tst.exe
/MAP:$(WIN32KBIN)\$(NAME)tst.map
$(TSTOBJS:  =^
)
/IG
$(TSTLIBS:  =^
)
$(TSTLASTOBJ)
$(NAME)tst.def
<<KEEP



#
#
#   L i b r a r i e s
#
#

#
# Libraries - segments are renamed for the 16-bit libraries.
#

# List of object files in the converted devhelp library.
DHLPOBJS =  +$(WIN32KOBJ)\dhcall5b.obj +$(WIN32KOBJ)\dhcal11a.obj \
            +$(WIN32KOBJ)\dhcal11g.obj +$(WIN32KOBJ)\dhret.obj

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
CLIBOBJS =  +$(WIN32KOBJ)\inp.obj       +$(WIN32KOBJ)\outp.obj \
            +$(WIN32KOBJ)\anfalmul.obj  +$(WIN32KOBJ)\anfaldiv.obj \
            +$(WIN32KOBJ)\anfalrem.obj  +$(WIN32KOBJ)\anfauldi.obj \
            +$(WIN32KOBJ)\anuldiv.obj   +$(WIN32KOBJ)\fmemcpy.obj \
            +$(WIN32KOBJ)\anulrem.obj   +$(WIN32KOBJ)\anlmul.obj \
            +$(WIN32KOBJ)\__AHINCR.obj  +$(WIN32KOBJ)\anlshl.obj

# Make corrected crt library with only the required object files.
$(WIN32KOBJ)\clib.lib: $(MSCPATH)\lib\clibcep.lib $(WIN32KBIN)\libconv.exe $(CLIBOBJS:+=)
    @$(RM) $@
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $@ $(CLIBOBJS);
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
ldr\calltaba.asm \
$(WIN32KBASE)\ldr\calltaba.asm : dev16\probkrnl.c $(WIN32KBIN)\mkcalltab.exe
    $(WIN32KBIN)\mkcalltab.exe calltab > $@


#
# Generate TstFakers.c
#
test\TstFakers.c \
$(WIN32KBASE)\test\TstFakers.c: dev16\probkrnl.c include\probkrnl.h $(WIN32KBIN)\mkcalltab.exe
    $(WIN32KBIN)\mkcalltab.exe tstfakers > $@


#
# Make assembly version of options.h; options.inc
#
$(WIN32KINCLUDE)\options.inc:  $(WIN32KINCLUDE)\options.h
    @$(ECHO) H2Inc: $**
    $(H2INC) $** > $@


#
# Make assembly version of api.h; api.inc
#
$(WIN32KINCLUDE)\api.inc:  $(WIN32KINCLUDE)\api.h
    @$(ECHO) H2Inc: $**
    $(H2INC) $** > $@

#
# Make assembly version of win32k.h; win32k.inc
#
$(WIN32KINCLUDE)\win32k.inc:  $(ODIN32_INCLUDE)\win32k.h
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
$(WIN32KBIN)\libconv.exe: libconv.c
    icc -Q+ -Ti+ -Fe$@ -Fo$(WIN32KOBJ)\$(*B).obj libconv.c


#
# Make the MkCallTab.exe utillity.
#

# MkCallTab objects.
MKCALLTABOBJS = \
    $(WIN32KOBJ)\mkcalltab.obj_c \
    $(WIN32KOBJ)\probkrnl.obj_c \
    $(WIN32KOBJ)\d16globl.obj_c \
    $(WIN32KOBJ)\vprntf16.obj_c

# Rule to build MkCallTab.exe
$(WIN32KBIN)\mkcalltab.exe: clfix.exe $(MKCALLTABOBJS)
    $(LD) $(LFLAGS) /pmtype:vio @<<$(WIN32KOBJ)\$(@B).lnk
        /OUT:$@
        /MAP:$(WIN32KBIN)\$(@B).map
        $(MKCALLTABOBJS)
        $(TOOLKIT)\lib\os2286.lib
        $(MSCPATH)\lib\clibcep.lib
<<keep

# Interference rule to make the MkCallTab objects.
{dev16}.c.obj_c:
    @$(ECHO) compiling 16bit (mkcalltab): $<
    @$(CC16) -c -W3 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp -DDEBUG -DEXTRACT \
        -Fa$(WIN32KLIST)\$(*B)_c.s -Fo$(WIN32KOBJ)\$(@B).obj_c $(CINCLUDES16) $<


#
# Make the clfix.exe utillity - clfix.exe
#   This should fix some of the problems we are experiencing with the
#   MSC v6.0a compiler (cl.exe) (16-bit).
#
clfix.exe: clfix.c
    $(CC) $(@B).c


#
# Make the symbol extract utillity - Extract.exe
#

# Extract object files.
EXTRACTOBJS = \
    $(WIN32KOBJ)\extract.obj_e \
    $(WIN32KOBJ)\probkrnl.obj_e \
    $(WIN32KOBJ)\d16globl.obj_e \
    $(WIN32KOBJ)\vprntf16.obj_e

# Rules to build Extract.exe.
$(WIN32KBIN)\extract.exe: clfix.exe $(EXTRACTOBJS)
    $(LD) $(LFLAGS) /pmtype:vio @<<$(WIN32KOBJ)\$(@B).lnk
        /OUT:$@
        /MAP:$(WIN32KBIN)\$(@B).map
        $(EXTRACTOBJS)
        $(TOOLKIT)\lib\os2286.lib
        $(MSCPATH)\lib\clibcep.lib
<<keep

# Copy rule.
extract.exe: $(WIN32KBIN)\extract.exe
    $(CP) $** $@

# Interference rule to make the Extract objects.
{dev16}.c.obj_e:
    @$(ECHO) compiling 16bit (extract): $<
    @$(CC16) -c -W3 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp -DDEBUG -DEXTRACT \
        -Fa$(WIN32KLIST)\$(*B)_e.s -Fo$(WIN32KOBJ)\$(@B).obj_e $(CINCLUDES16) $<




#
# Include the .depend file.
#
!if [$(EXISTS) .depend] == 0
!   include .depend
!else
!   if [$(ECHO) .depend doesn't exist]
!   endif
!endif

