################################################################################
# $Id: Makefile,v 1.41 2000-04-05 18:40:36 bird Exp $
#
# Win32k makefile.
#
# Copyright 1998-2000 knut st. osmundsen
#
# Project Odin Software License can be found in LICENSE.TXT
#
################################################################################


################################################################################
# Check that makefile.inc exists, If it doesn't we'll try make it.
# Check that makefile.inc is newer than configure.cmd.
#     If not newer this might be a fatal problem, so we'll fail.
################################################################################
!if [..\..\tools\bin\Exists.cmd makefile.inc] == 1
!   if [configure.cmd noninteractive]
!   endif
!endif
!if [Check.cmd $(MAKE) -nologo -q -f Check.mk makefile.inc] != 0
!   error makefile.inc is older than configure.cmd. Re-run configure.cmd!
!endif


################################################################################
# Include path definitions
################################################################################
!include makefile.inc
!include $(PDWIN32_INCLUDE)\pdwin32.tools

NAME = Win32k


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
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16
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
all: $(WIN32KBIN)\$(NAME).sys pe2lx.exe
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
        $(WIN32KOBJ)\devhelp.lib \
        $(WIN32KOBJ)\clib.lib


################################################################################
# SPECIAL RULES - Testing
################################################################################
Probkrnl.exe: $(WIN32KBIN)\ProbKrnl.exe
$(WIN32KBIN)\ProbKrnl.exe: dev16\ProbKrnl.c dev16\symdb.c dev16\vprntf16.c \
              $(WIN32KINCLUDE)\sym.h \
              $(WIN32KINCLUDE)\probKrnl.h
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B)_p.s     \
        -Fo$(WIN32KOBJ)\probkrnl_p.obj $(CINCLUDES16) dev16\probkrnl.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B)_p.s     \
        -Fo$(WIN32KOBJ)\symdb_p.obj $(CINCLUDES16) dev16\symdb.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fa -Zi -Od -Lp \
        -DDEBUG -DDEBUGR3 -Fa$(WIN32KLIST)\$(*B)_p.s     \
        -Fo$(WIN32KOBJ)\vprntf16_p.obj $(CINCLUDES16) dev16\vprntf16.c
    link /NOD /MAP:FULL /CO /pmtype:vio \
        $(WIN32KOBJ)\probkrnl_p.obj + $(WIN32KOBJ)\symdb_p.obj + $(WIN32KOBJ)\vprntf16_p.obj, \
        $@, $(WIN32KBIN)\probkrnl.map, \
        $(TOOLKIT)\lib\os2286.lib + $(MSCPATH)\lib\clibcep.lib;
    $(CP) $@

extract.exe: $(WIN32KBIN)\extract.exe
$(WIN32KBIN)\extract.exe: dev16\ProbKrnl.c dev16\vprntf16.c \
             $(WIN32KINCLUDE)\sym.h \
             $(WIN32KINCLUDE)\probKrnl.h
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp \
        -DDEBUG -DEXTRACT -Fa$(WIN32KLIST)\$(*B)_e.s     \
        -Fo$(WIN32KOBJ)\probkrnl_e.obj $(CINCLUDES16) dev16\probkrnl.c
    $(CC16) -c -W4 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp \
        -DDEBUG -DEXTRACT -Fa$(WIN32KLIST)\$(*B)_e.s     \
        -Fo$(WIN32KOBJ)\vprntf16_e.obj $(CINCLUDES16) dev16\vprntf16.c
    link /NOD /MAP:FULL /CO /pmtype:vio \
        $(WIN32KOBJ)\probkrnl_e.obj + $(WIN32KOBJ)\vprntf16_e.obj, \
        $@, $(WIN32KBIN)\$(*B).map, \
        $(TOOLKIT)\lib\os2286.lib + $(MSCPATH)\lib\clibcep.lib;
    $(CP) $@

d32init.exe: $(WIN32KBIN)\d32init.exe
$(WIN32KBIN)\d32init.exe: dev32\d32init.c
    icc -Ti+ -Ss /Gr+ $(CINCLUDES) -DRING3 -DDEBUGR3 -DDEBUG \
        -Fo$(WIN32KOBJ)\$(*B).r3obj -Fe$@ -Fm$(WIN32KBIN)\$(*B).map \
        dev32\$(*B).c
    $(CP) $@



################################################################################
# Win32k.sys rule. (Don't mess with the order or objects and libraries!)
################################################################################
$(NAME).sys: $(WIN32KBIN)\$(NAME).sys
$(WIN32KBIN)\$(NAME).sys: $(OBJS) $(LIBS) $(NAME).def \
            $(WIN32KOBJ)\last.lib \
            $(WIN32KOBJ)\init.lib \
            $(LIBSINIT)
    -@$(ECHO) linking: $@
    $(LD) $(LFLAGS) @<<ole
/OUT:$@
/MAP:$(WIN32KBIN)\$(@B).map
$(OBJS)
$(LIBS)
$(WIN32KOBJ)\last.lib
$(WIN32KOBJ)\init.lib
/IG $(LIBSINIT)
$(NAME).def
<<keep
    cd $(WIN32KBIN)
    @mapsym $(*B).map > nul
    cd $(WIN32KBASE)
    $(CP) $@ $(PDWIN32_BIN)
    $(CP) $*.sym $(PDWIN32_BIN)
    $(CP) $(WIN32KBASE)\$(*B).ddp $(PDWIN32_BIN)


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
$(WIN32KOBJ)\devhelp.lib: $(DDKPATH)\lib\dhcalls.lib $(WIN32KBIN)\libconv.exe
    $(WIN32KBIN)\libconv $(DDKPATH)\lib\dhcalls.lib $@
    $(ILIB) /nologo /nobackup /convformat $@;

$(WIN32KOBJ)\clib.lib: $(MSCPATH)\lib\clibcep.lib $(WIN32KBIN)\libconv.exe
    $(WIN32KBIN)\libconv $(MSCPATH)\lib\clibcep.lib $@ > nul
    $(ILIB) /nologo /nobackup /convformat $@;

$(WIN32KOBJ)\last.lib: $(WIN32KOBJ)\devlast.obj
    -@$(RM) $@ > nul 2> nul
    $(ILIB) /nologo /nobackup $@ $**;

$(WIN32KOBJ)\init.lib: $(OBJSINIT)
    -@$(RM) $@ > nul 2> nul
    $(ILIB) /nologo /nobackup $@ $**;

$(WIN32KBIN)\libconv.exe: libconv.c
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
           $(PDWIN32_BIN)\$(NAME).sys \
           $(PDWIN32_BIN)\$(NAME).sym \
           $(PDWIN32_BIN)\$(NAME).ddp
!if "$(WIN32KBIN)" != ""
    $(RM) $(WIN32KBIN)\*sy* $(WIN32KBIN)\*map $(WIN32KBIN)\*exe
!endif
!if "$(WIN32KOBJ)" != ""
    $(RM) $(WIN32KOBJ)\*obj $(WIN32KOBJ)\*lib $(WIN32KOBJ)\*pch
!endif
!if "$(WIN32KLIST)" != ""
    $(RM) $(WIN32KLIST)\*a* $(WIN32KLIST)\*lst $(WIN32KLIST)\*.s*
!endif
    cd pe2lx
    $(MAKE_CMD) clean
    cd ..\lib
    $(MAKE_CMD) clean
    cd ..


################################################################################
# DEPENDENCIES - c and c++ deps are autogenereated.
#                asm deps must be updated by hand
################################################################################
dep:
    $(PDWIN32_TOOLS)\fastdep -o$$(WIN32KOBJ) $(CINCLUDES) misc\*.c* ldr\*.c* \
        dev32\*.c* pe2lx\*.c* k32\*.c* lib\*.c elf2lx\*.c* \
        $(WIN32KINCLUDE)\*.h
    $(PDWIN32_TOOLS)\fastdep -a+ -o$$(WIN32KOBJ) $(CINCLUDES16) dev16\*.c*
    $(PDWIN32_TOOLS)\fastdep -a+ -o$$(WIN32KOBJ) -I$(WIN32KINCLUDE) \
        -I$(DDKPATH)\inc dev32\*.asm ldr\*.asm misc\*.asm include\*.inc
    cd pe2lx
    $(MAKE_CMD) dep
    cd ..\lib
    $(MAKE_CMD) dep
    cd ..


################################################################################
# Include the .depend file.
################################################################################
!if [$(EXISTS) .depend] == 0
!   include .depend
!else
!   if [$(ECHO) .depend doesn't exist]
!   endif
!endif

