################################################################################
# $Id: Makefile,v 1.25 2000-02-20 04:27:22 bird Exp $
#
# Copyright 1998-1999 knut st. osmundsen
#
# Project Odin Software License can be found in LICENSE.TXT
#
################################################################################
################################################################################
# Include path definitions
################################################################################
!include makefile.inc
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
CDEFINES16  =
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
LFLAGS      = $(LFLAGS)     /NOEXEPACK /DEBUG /NODEBUG
ASFLAGS     = $(ASFLAGS)    -Od+
CDEFINES    = $(CDEFINES)   -DDEBUG
CDEFINES16  = $(CDEFINES16) -DDEBUG
ASDEFINES   = $(ASDEFINES)  -D:DEBUG
!endif

!ifdef NODEBUGINFO
LFLAGS      = $(LFLAGS)     /NODEB
!endif

################################################################################
# COMPILER/ASSEMBLER INCLUDEPATHS
################################################################################
CINCLUDES  = -I$(WIN32KBASE)\include \
             -I$(TOOLKIT)\h \
             -I$(VACPATH)\include \
             -I$(DDKPATH)\h \
             -I$(DDKPATH)\src\dev\dasd\diskh \
             -I. \
             -I$(PDWIN32_INCLUDE) \
             -I$(PDWIN32_INCLUDE)\win

CINCLUDES16= -I$(WIN32KBASE)\include \
             -I. \
             -I$(DDKPATH)\h \
             -I$(MSCPATH)\include \
             -I$(PDWIN32_INCLUDE)


AINCLUDES  = -Fdi:$(WIN32KBASE)\include \
             -Fdi:$(DDKPATH)\inc


################################################################################
# SUFFIXES AND INFERENCE RULES
################################################################################
.SUFFIXES: .obj .cpp .c .asm

{dev16}.c{object}.obj:
    @$(ECHO) compiling 16bit: $<
    @$(CC16) $(CFLAGS16) $(CDEFINES16) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES16) $<

{dev32}.asm{object}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{dev32}.c{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{dev32}.cpp{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{k32}.asm{object}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{k32}.c{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{k32}.cpp{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{ldr}.asm{object}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{ldr}.c{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{ldr}.cpp{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{misc}.asm{object}.obj:
    @$(ECHO) assembling:      $<
    $(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{misc}.c{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{misc}.cpp{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<

{pe2lx}.c{object}.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{pe2lx}.cpp{object}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<



################################################################################
# TARGETS
################################################################################
#all: $(NAME).sys ProbKrnl.exe
all:$(NAME).sys



################################################################################
# OBJECTS and LIBRARIES - Don't mess with the order or objects and libraries!
################################################################################
OBJS  = object\devfirst.obj \
        object\d16strat.obj \
        object\d32hlp.obj \
        object\d32globals.obj \
        object\asmutils.obj \
        object\calltab.obj \
        object\malloc.obj \
        object\smalloc_avl.obj \
        object\avl.obj \
        object\rmalloc_avl.obj \
        object\new.obj \
        object\stricmp.obj \
        object\yield.obj \
        object\vsprintf.obj \
        object\vprintf.obj \
        object\ldr.obj \
        object\myldrClose.obj \
        object\myldrOpen.obj \
        object\myldrRead.obj \
        object\myLDRQAppType.obj \
        object\myldrEnum32bitRelRecs.obj \
        object\pe2lx.obj \
        object\ModuleBase.obj \
        object\d32ElfIOCtl.obj \
        object\d32Win32kIOCtl.obj \
        object\k32AllocMemEx.obj \
        object\buffer.obj \
        object\mytkExecPgm.obj


OBJSINIT = \
        object\d16init.obj \
        object\d32init.obj \
        object\probkrnl.obj

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
ProbKrnl.exe: dev16\ProbKrnl.c $(WIN32KBASE)\include\sym.h \
              $(WIN32KBASE)\include\probKrnl.h
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp \
        $(CDEFINES16) -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B).s     \
        -Foprobkrnl.obj $(CINCLUDES16) dev16\probkrnl.c
    link /NOD /MAP:FULL /CO /pmtype:vio probkrnl.obj, probkrnl.exe, probkrnl.map, $(TOOLKIT)\lib\os2286.lib + $(MSCPATH)\lib\clibcep.lib;



################################################################################
# Win32k.sys rule. Don't mess with the order or objects and libraries!
################################################################################
$(NAME).sys: $(OBJS) $(LIBS) last.lib $(OBJSINIT) $(LIBSINIT) $(NAME).def
    -@$(ECHO) linking: $@
    $(LD) $(LFLAGS) @<<
$(OBJS) $(LIBS) last.lib $(OBJSINIT) /IG $(LIBSINIT) $(NAME).def
<<
    @mapsym $*.map > nul
    $(CP) $@ $(PDWIN32_BIN)
    $(CP) $*.sym $(PDWIN32_BIN)
    $(CP) $*.ddp $(PDWIN32_BIN)

#    copy win32k.sym c:\temp
#    copy win32k.sys c:\temp


################################################################################
# Libraries - segments are renamed for the 16-bit libraries.
################################################################################
devhelp.lib: $(DDKPATH)\lib\dhcalls.lib libconv.exe
    libconv $(DDKPATH)\lib\dhcalls.lib $@
    $(ILIB) /nologo /nobackup /convformat $@;

clib.lib: $(MSCPATH)\lib\clibcep.lib libconv.exe
    libconv $(MSCPATH)\lib\clibcep.lib $@ > nul
    $(ILIB) /nologo /nobackup /convformat $@;

last.lib: object\devlast.obj
    -@del $@ > nul 2> nul
    $(ILIB) /nologo /nobackup $@ $**;

libconv.exe: libconv.c
    icc -Q+ libconv.c


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


################################################################################
# DEPENDENCIES - c and c++ deps are autogenereated.
#                asm deps must be updated by hand
################################################################################
dep:
    $(PDWIN32_TOOLS)\fastdep -oObject $(CINCLUDES) misc\*.c* ldr\*.c* dev32\*.c* \
        dev16\*.c* pe2lx\*.c* k32\*.c* lib\*.c include\*.h

object\asmutils.obj:            misc\asmutils.asm     include\devsegdf.inc
object\calltab.obj:             ldr\calltab.asm
object\d32hlp.obj:              dev32\d32hlp.asm      include\devsegdf.inc
object\devfirst.obj:            dev32\devfirst.asm    include\devsegdf.inc
object\devlast.obj:             dev32\devlast.asm     include\devsegdf.inc

!ifndef NODEP
!include .depend
!endif
