################################################################################
# $Id: Makefile,v 1.31 2000-02-27 00:58:22 bird Exp $
#
# Copyright 1998-1999 knut st. osmundsen
#
# Project Odin Software License can be found in LICENSE.TXT
#
################################################################################

################################################################################
# Include path definitions
################################################################################
# Note! This crazy !ifdef stuff handles the case when makefile.inc don't exist
# and someone is trying to make dependencies. A makefile.inc is then generated
# and we'll try again making dependencies by the dep2 rule.
!ifdef NODEP
!   ifdef MAKEFILE_INC
!       include makefile.inc
!   else
PDWIN32_INCLUDE = ..\..\include
PDWIN32_TOOLS = ..\..\tools\bin
!   endif
!else
!   include makefile.inc
!endif
!include $(PDWIN32_INCLUDE)\pdwin32.tools

NAME = win32k

################################################################################
# COMPILER/LINKER/ASSEMBLER
################################################################################
CC16        = $(MSCPATH)\binp\cl
CC          = icc
!if "$(CCENV)" == "VAC36"
ILIB        = ilib /nofree
!else
ILIB        = ilib
!endif
LD          = ilink
AS          = alp

CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CFLAGS16    = -c -W4 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo -Fo$@ -Fa
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Gx -Wall+ppt-ppc-inl-cnv-gnr-vft-
ASFLAGS     = -Sv:ALP -Mb -Li
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /MAP /O:$@
CDEFINES    = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__
CDEFINES16  = -DRING0
ADEFINES    = -D:WIN32K




!ifndef DEBUG
# RELEASE
CFLAGS      = $(CFLAGS)     -G5 -O+
CFLAGS16    = $(CFLAGS16)   -Ogeitln -Gs
CPPFLAGS    = $(CPPFLAGS)   -G5 -O+
LFLAGS      = $(LFLAGS)     /EXEPACK:2 /NODEBUG /PACKC /PACKD
CDEFINES    = $(CDEFINES)   -DRELEASE
CDEFINES16  = $(CDEFINES16) -DRELEASE
ASDEFINES   = $(ASDEFINES)  -D:RELEASE

!else

# DEBUG
CFLAGS      = $(CFLAGS)     -O- -Ti+
CFLAGS16    = $(CFLAGS16)   -Zi -Od
CPPFLAGS    = $(CPPFLAGS)   -O- -Ti+
LFLAGS      = $(LFLAGS)     /NOEXEPACK /DEBUG  /NODEB
ASFLAGS     = $(ASFLAGS)    -Od+
CDEFINES    = $(CDEFINES)   -DDEBUG
CDEFINES16  = $(CDEFINES16) -DDEBUG
ASDEFINES   = $(ASDEFINES)  -D:DEBUG
!endif

!ifdef NODEBUGINFO
LFLAGS      = $(LFLAGS)     /NODEB /EXEPACK:2 /PACKD /PACKC
CFLAGS16    = $(CFLAGS16)   -Owis
CFLAGS      = $(CFLAGS)     -O+ -Oi-
CPPFLAGS    = $(CPPFLAGS)   -O+ -Oi-
!endif

################################################################################
# COMPILER/ASSEMBLER INCLUDEPATHS
################################################################################
CINCLUDES  = -I$(WIN32KINCLUDE) \
             -I$(TOOLKIT)\h \
             -I$(VACPATH)\include \
             -I$(DDKPATH)\h \
             -I$(DDKPATH)\src\dev\dasd\diskh \
             -I. \
             -I$(PDWIN32_INCLUDE) \
             -I$(PDWIN32_INCLUDE)\win

CINCLUDES16= -I$(WIN32KINCLUDE) \
             -I. \
             -I$(DDKPATH)\h \
             -I$(MSCPATH)\include \
             -I$(PDWIN32_INCLUDE)


AINCLUDES  = -Fdi:$(WIN32KINCLUDE) \
             -Fdi:$(DDKPATH)\inc


################################################################################
# SUFFIXES AND INFERENCE RULES
################################################################################
.SUFFIXES: .obj .cpp .c .asm

{dev16}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 16bit: $<
    @$(CC16) $(CFLAGS16) $(CDEFINES16) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES16) $<

{dev32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{dev32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{dev32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{k32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{k32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{k32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{ldr}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{ldr}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{ldr}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{misc}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    $(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{misc}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{misc}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{pe2lx}.c{$(WIN32KOBJ)}.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{pe2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{elf2lx}.c{$(WIN32KOBJ)}.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{elf2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<




################################################################################
# TARGETS
################################################################################
!if "$(DDKPATH)" == "" || "$(MSCPATH)" == ""  || "$(TOOLKIT)" == "" || "$(VACPATH)" == ""
all: pe2lx.exe
!else
all: $(NAME).sys pe2lx.exe
!endif


################################################################################
# OBJECTS and LIBRARIES - Don't mess with the order or objects and libraries!
################################################################################
OBJS  = $(WIN32KOBJ)\devfirst.obj \
        $(WIN32KOBJ)\d16strat.obj \
        $(WIN32KOBJ)\d32hlp.obj \
        $(WIN32KOBJ)\d32globals.obj \
        $(WIN32KOBJ)\d16globl.obj \
        $(WIN32KOBJ)\asmutils.obj \
        $(WIN32KOBJ)\calltab.obj \
        $(WIN32KOBJ)\malloc.obj \
        $(WIN32KOBJ)\smalloc_avl.obj \
        $(WIN32KOBJ)\avl.obj \
        $(WIN32KOBJ)\rmalloc_avl.obj \
        $(WIN32KOBJ)\new.obj \
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
        $(WIN32KOBJ)\pe2lx.obj \
        $(WIN32KOBJ)\ModuleBase.obj \
        $(WIN32KOBJ)\d32ElfIOCtl.obj \
        $(WIN32KOBJ)\d32Win32kIOCtl.obj \
        $(WIN32KOBJ)\k32AllocMemEx.obj \
        $(WIN32KOBJ)\buffer.obj \
        $(WIN32KOBJ)\mytkExecPgm.obj


OBJSINIT = \
        $(WIN32KOBJ)\d16init.obj \
        $(WIN32KOBJ)\d32init.obj \
        $(WIN32KOBJ)\ProbKrnl.obj \
        $(WIN32KOBJ)\SymDB.obj \
        $(WIN32KOBJ)\vprntf16.obj

LIBS = \
!if "$(CCENV)" == "VAC36"
        $(VACPATH)\lib\cpprni36.lib \
!else
        $(VACPATH)\lib\cppon30.lib  \
!endif
        $(DDKPATH)\lib\os2286.lib

LIBSINIT = \
        devhelp.lib \
        clib.lib


################################################################################
# SPECIAL RULES - Testing
################################################################################
ProbKrnl.exe: dev16\ProbKrnl.c dev16\symdb.c dev16\vprntf16.c \
              $(WIN32KINCLUDE)\sym.h \
              $(WIN32KINCLUDE)\probKrnl.h
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B).s     \
        -Fo$(WIN32KOBJ)\probkrnl_p.obj $(CINCLUDES16) dev16\probkrnl.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B).s     \
        -Fo$(WIN32KOBJ)\symdb_p.obj $(CINCLUDES16) dev16\symdb.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B).s     \
        -Fo$(WIN32KOBJ)\vprntf16_p.obj $(CINCLUDES16) dev16\vprntf16.c
    link /NOD /MAP:FULL /CO /pmtype:vio \
        $(WIN32KOBJ)\probkrnl_p.obj + $(WIN32KOBJ)\symdb_p.obj + $(WIN32KOBJ)\vprntf16_p.obj, \
        probkrnl.exe, probkrnl.map, $(TOOLKIT)\lib\os2286.lib + $(MSCPATH)\lib\clibcep.lib;

extract.exe: dev16\ProbKrnl.c dev16\vprntf16.c \
             $(WIN32KINCLUDE)\sym.h \
             $(WIN32KINCLUDE)\probKrnl.h
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp \
        -DDEBUG -DEXTRACT -Fa$(WIN32KLIST)\$(*B).s     \
        -Fo$(WIN32KOBJ)\probkrnl_e.obj $(CINCLUDES16) dev16\probkrnl.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp \
        -DDEBUG -DEXTRACT -Fa$(WIN32KLIST)\$(*B).s     \
        -Fo$(WIN32KOBJ)\vprntf16_e.obj $(CINCLUDES16) dev16\vprntf16.c
    link /NOD /MAP:FULL /CO /pmtype:vio \
        $(WIN32KOBJ)\probkrnl_e.obj + $(WIN32KOBJ)\vprntf16_e.obj, \
        $@, $(*B).map, $(TOOLKIT)\lib\os2286.lib + $(MSCPATH)\lib\clibcep.lib;

d32init.exe: dev32\d32init.c
    icc -Ss /Gr+ $(CINCLUDES) -DRING3 -DDEBUGR3 -DDEBUG \
        -Fo$(WIN32KOBJ)\$(*B).r3obj -Fe$@ dev32\$(*B).c



################################################################################
# Win32k.sys rule. (Don't mess with the order or objects and libraries!)
################################################################################
$(NAME).sys: $(OBJS) $(LIBS) last.lib init.lib $(LIBSINIT) $(NAME).def
    -@$(ECHO) linking: $@
    $(LD) $(LFLAGS) @<<
$(OBJS) $(LIBS) last.lib init.lib /IG $(LIBSINIT) $(NAME).def
<<
    @mapsym $*.map > nul
    $(CP) $@ $(PDWIN32_BIN)
    $(CP) $*.sym $(PDWIN32_BIN)
    $(CP) $*.ddp $(PDWIN32_BIN)


################################################################################
# Pe2Lx.exe rule
################################################################################
pe2lx.exe:
    cd pe2lx
    $(MAKE_CMD)
    cd ..

################################################################################
# Libraries - global libraries
################################################################################
lib:
    cd lib
    $(MAKE_CMD)
    cd ..


################################################################################
# Libraries - segments are renamed for the 16-bit libraries.
################################################################################
devhelp.lib: $(DDKPATH)\lib\dhcalls.lib libconv.exe
    libconv $(DDKPATH)\lib\dhcalls.lib $@
    $(ILIB) /nologo /nobackup /convformat $@;

clib.lib: $(MSCPATH)\lib\clibcep.lib libconv.exe
    libconv $(MSCPATH)\lib\clibcep.lib $@ > nul
    $(ILIB) /nologo /nobackup /convformat $@;

last.lib: $(WIN32KOBJ)\devlast.obj
    -@$(RM) $@ > nul 2> nul
    $(ILIB) /nologo /nobackup $@ $**;

init.lib: $(OBJSINIT)
    -@$(RM) $@ > nul 2> nul
    $(ILIB) /nologo /nobackup $@ $**;

libconv.exe: libconv.c
    icc -Q+ -Fe$@ -Fo$(WIN32KOBJ)\$(*B).obj libconv.c


#################################################################################
# Documentation generation - require SDS - http://www.ii.uib.no/~stig/
################################################################################
lxdoc:
   exp cpp2sds -o DocsA\ -i include\lx.h pe2lx\lx*.cpp
   cd Docs
   exp sds2doc -ai *.sds

fulldoc:
   exp cpp2sds -o DocsA\ -i include\*.h *.cpp ldr\*.cpp misc\*.cpp pe2lx\*.cpp
   cd Docs
   exp sds2doc -ai *.sds

#################################################################################
# Visual SlickEdit project file
################################################################################
win32k.vpj:
    genproject.cmd $@ $(CINCLUDES)

################################################################################
# Cleanup
################################################################################
clean:
    @$(ECHO) cleaning....
    $(RM) $(NAME).sys *.lib *.obj *.exe *.sym *.map \
        $(WIN32KOBJ)\*.obj $(WIN32KOBJ)\*.pe_obj \
        $(WIN32KLIST)\*.s $(WIN32KLIST)\*.asm $(WIN32KLIST)\*.map \
        $(WIN32KLIST)\*.lst \
        $(PDWIN32_BIN)\$(NAME).sys
    cd pe2lx
    $(MAKE_CMD) clean
    cd ..\lib
    $(MAKE_CMD) clean
    cd ..


################################################################################
# DEPENDENCIES - c and c++ deps are autogenereated.
#                asm deps must be updated by hand
################################################################################
# If makefile.inc doesn't exist we'll make a quit and dirty one.
# So when we are sure that makefile.inc exist we'll invoke nmake and
# ask it to make dependencies again using the dep2 rule.
dep:
    IF NOT EXIST makefile.inc configure.cmd noninteractive
    $(MAKE_CMD) NODEP=1 MAKEFILE_INC=1 dep2
    cd pe2lx
    $(MAKE_CMD) NODEP=1 dep
    cd ..\lib
    $(MAKE_CMD) NODEP=1 dep
    cd ..

dep2:
    $(PDWIN32_TOOLS)\fastdep -o$(WIN32KOBJ) $(CINCLUDES) misc\*.c* ldr\*.c* dev32\*.c* \
        dev16\*.c* pe2lx\*.c* k32\*.c* lib\*.c $(WIN32KINCLUDE)\*.h
    $(PDWIN32_TOOLS)\fastdep -a+ -o$(WIN32KOBJ) $(CINCLUDES16) dev16\*.c*


$(WIN32KOBJ)\asmutils.obj:      misc\asmutils.asm     include\devsegdf.inc
$(WIN32KOBJ)\buffer.obj:        misc\buffer.asm       include\devsegdf.inc
$(WIN32KOBJ)\calltab.obj:       ldr\calltab.asm       include\devsegdf.inc
$(WIN32KOBJ)\mytkExecPgm.obj:   ldr\mytkExecPgm.asm   include\devsegdf.inc
$(WIN32KOBJ)\d32hlp.obj:        dev32\d32hlp.asm      include\devsegdf.inc
$(WIN32KOBJ)\devfirst.obj:      dev32\devfirst.asm    include\devsegdf.inc
$(WIN32KOBJ)\devlast.obj:       dev32\devlast.asm     include\devsegdf.inc

!ifndef NODEP
!include .depend
!endif
