################################################################################
# $Id: Makefile,v 1.54 2000-09-26 09:07:05 bird Exp $
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
CC16        = clfix.exe $(MSCPATH)\binp\cl.exe
CC          = icc
!if "$(CCENV)" == "VAC36"
ILIB        = ilib /nofree /nobackup /nologo
!else
ILIB        = ilib /nobackup /nologo
!endif
LD          = ilink
AS          = alp
H2INC       = $(DDKPATH)\tools\h2inc.exe

CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CFLAGS16    = -c -W3 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Gx -Wall+ppt-ppc-inl-cnv-gnr-vft-
ASFLAGS     = -Sv:ALP -Mb -Li
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA
CDEFINESB   = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__
CDEFINESB16 = -DRING0
ASDEFINES   = -D:WIN32K

!if "$(CCENV)" == "VAC36"
VACLIB      = $(VACPATH)\lib\cpprni36.lib
!else
VACLIB      = $(VACPATH)\lib\cppon30.lib
!endif




!ifndef DEBUG
# RELEASE
CFLAGS      = $(CFLAGS)     -G5 -O+
CFLAGS16    = $(CFLAGS16)   -Ogeitln -Gs
CPPFLAGS    = $(CPPFLAGS)   -G5 -O+
LFLAGS      = $(LFLAGS)     /EXEPACK:2 /NODEBUG
CDEFINES    = $(CDEFINESB)  -DRELEASE
CDEFINES16  = $(CDEFINESB16) -DRELEASE
ASDEFINES   = $(ASDEFINES)  -D:RELEASE

!else

# DEBUG
CFLAGS      = $(CFLAGS)     -O+ -Os- -Ti+
CFLAGS16    = $(CFLAGS16)   -Zi -Od
CPPFLAGS    = $(CPPFLAGS)   -O+ -Os- -Ti+
LFLAGS      = $(LFLAGS)     /NOEXEPACK /DEBUG
ASFLAGS     = $(ASFLAGS)    -Od+
CDEFINES    = $(CDEFINESB)  -DDEBUG
CDEFINES16  = $(CDEFINESB16) -DDEBUG
ASDEFINES   = $(ASDEFINES)  -D:DEBUG
!endif

!ifdef NODEBUGINFO
LFLAGS      = $(LFLAGS)     /NODEB /EXEPACK:2
CFLAGS16    = $(CFLAGS16)   -Owis
CFLAGS      = $(CFLAGS)     -O+ -Os- -Oi-
CPPFLAGS    = $(CPPFLAGS)   -O+ -Os- -Oi-
!endif

CDEFINES_TST    = $(CDEFINES)   -DR3TST
CDEFINES16_TST  = $(CDEFINES16) -DR3TST


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

CINCLUDES16= -I$(WIN32KINCLUDE) -I. -I$(DDKPATH)\h -I$(MSCPATH)\include -I$(PDWIN32_INCLUDE) -I$(TOOLKIT)\h


AINCLUDES  = -Fdi:$(WIN32KINCLUDE) \
             -Fdi:$(DDKPATH)\inc


################################################################################
# SUFFIXES AND INFERENCE RULES
################################################################################
.SUFFIXES: .obj .cpp .c .asm .c .obj_e .inc

{dev16}.c.obj:
    @$(ECHO) compiling 16bit: $<
    $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) -Fo$(WIN32KOBJ)\$(*B).obj -Fa$(WIN32KLIST)\$(*B).s $<

{dev16}.c.obj_init:
    @$(ECHO) compiling 16bit: $<
    $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) /NTCODE16_INIT /NDDATA16_INIT -Fo$(WIN32KOBJ)\$(*B).obj_init -Fa$(WIN32KLIST)\$(*B).s $<
    $(CP) $@ $@.obj

{dev16}.c.obj_tst:
    @$(ECHO) compiling 16bit: $<
    $(CC16) -c $(CFLAGS16) $(CDEFINES16_TST) $(CINCLUDES16) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@B).obj_tst $<
    $(CP) $@ $@.obj
{dev16}.c.obj_tst_init:
    @$(ECHO) compiling 16bit: $<
    $(CC16) -c $(CFLAGS16) $(CDEFINES16_TST) $(CINCLUDES16) /NDDATA16_INIT /NTCODE16_INIT -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@B).obj_tst_init $<
    $(CP) $@ $@.obj


{dev32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{dev32}.asm.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$(WIN32KOBJ)\$(@F) -Fl:$(WIN32KLIST)\$(*B).lst

{dev32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{dev32}.c.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{dev32}.c{$(WIN32KOBJ)}.obj_tst:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
    @$(CP) $@ $@.obj
{dev32}.c.obj_tst:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<
    @$(CP) $@ $@.obj

{dev32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{dev32}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{test}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{test}.asm.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$(WIN32KOBJ)\$(@F) -Fl:$(WIN32KLIST)\$(*B).lst

{test}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) -O- -Ge+ $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{test}.c.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) -O- -Ge+ $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{test}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) -O- -Ge+ $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{test}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) -O- -Ge+ $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{k32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{k32}.asm.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$(WIN32KOBJ)\$(@F) -Fl:$(WIN32KLIST)\$(*B).lst

{k32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{k32}.c.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{k32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{k32}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{ldr}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{ldr}.asm.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$(WIN32KOBJ)\$(@F) -Fl:$(WIN32KLIST)\$(*B).lst

{ldr}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{ldr}.c.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{ldr}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{ldr}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{misc}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling:      $<
    $(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst
{misc}.asm.obj:
    @$(ECHO) assembling:      $<
    $(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$(WIN32KOBJ)\$(@F) -Fl:$(WIN32KLIST)\$(*B).lst

{misc}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{misc}.c.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{misc}.c{$(WIN32KOBJ)}.obj_tst:
    @$(ECHO) compiling 32bit: $<
    $(CC) -c $(CFLAGS) $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
    @$(CP) $@ $@.obj
{misc}.c.obj_tst:
    @$(ECHO) compiling 32bit: $<
    $(CC) -c $(CFLAGS) $(CDEFINES_TST) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<
    @$(CP) $@ $@.obj

{misc}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{misc}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{pe2lx}.c{$(WIN32KOBJ)}.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{pe2lx}.c.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{pe2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{pe2lx}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<


{elf2lx}.c{$(WIN32KOBJ)}.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{elf2lx}.c.obj 32bit:
    @$(ECHO) compiling: $<
    @$(CC) -c $(CFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<

{elf2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$@ $(CINCLUDES) $<
{elf2lx}.cpp.obj:
    @$(ECHO) compiling 32bit: $<
    @$(CC) -c $(CPPFLAGS) $(CDEFINES) -Fa$(WIN32KLIST)\$(*B).s -Fo$(WIN32KOBJ)\$(@F) $(CINCLUDES) $<




################################################################################
# TARGETS
################################################################################
!if "$(DDKPATH)" == "" || "$(MSCPATH)" == ""  || "$(TOOLKIT)" == "" || "$(VACPATH)" == ""
all:    pe2lx.exe
!else
all:    clfix.exe \
        $(WIN32KINCLUDE)\options.inc \
        $(WIN32KBIN)\$(NAME).sys \
        pe2lx.exe \
        kRx.exe \
        lib \
        Win32kCC.exe
!endif


################################################################################
# OBJECTS and LIBRARIES - Don't mess with the order or objects and libraries!
################################################################################
OBJS  =\
    $(WIN32KOBJ)\devfirst.obj \
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
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\pe2lx.obj \
    $(WIN32KOBJ)\ModuleBase.obj \
    $(WIN32KOBJ)\d32ElfIOCtl.obj \
    $(WIN32KOBJ)\d32Win32kIOCtl.obj \
    $(WIN32KOBJ)\k32AllocMemEx.obj \
    $(WIN32KOBJ)\k32QueryOTEs.obj \
    $(WIN32KOBJ)\k32QueryOptionsStatus.obj \
    $(WIN32KOBJ)\k32SetOptions.obj \
    $(WIN32KOBJ)\mytkExecPgm.obj \
    $(WIN32KOBJ)\mytkStartProcess.obj \
    $(WIN32KOBJ)\vprntf16.obj \
    $(WIN32KOBJ)\d32init.obj \
    $(WIN32KOBJ)\SymDB.obj \
    $(WIN32KOBJ)\d16init.obj_init \
    $(WIN32KOBJ)\ProbKrnl.obj_init

LASTOBJ =\
    $(WIN32KOBJ)\devlast.obj

LIBS =\
    $(VACLIB) \
    $(DDKPATH)\lib\os2386.lib \
    $(WIN32KOBJ)\devhelp.lib \
    $(WIN32KOBJ)\clib.lib

#
# Win32ktst.exe object configuration
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
    $(WIN32KOBJ)\d16strat.obj \
    $(WIN32KOBJ)\d32hlp.obj \
    $(WIN32KOBJ)\d32globals.obj \
    $(WIN32KOBJ)\d16globl.obj \
    $(WIN32KOBJ)\asmutils.obj \
    $(WIN32KOBJ)\calltab.obj \
    $(WIN32KOBJ)\malloc.obj_tst \
    $(WIN32KOBJ)\smalloc_avl.obj \
    $(WIN32KOBJ)\avl.obj \
    $(WIN32KOBJ)\rmalloc_avl.obj \
    $(WIN32KOBJ)\new.obj \
    $(WIN32KOBJ)\env.obj \
    $(WIN32KOBJ)\stricmp.obj \
    $(WIN32KOBJ)\yield.obj \
    $(WIN32KOBJ)\vsprintf.obj \
    $(WIN32KOBJ)\vprintf.obj_tst \
    $(WIN32KOBJ)\ldr.obj \
    $(WIN32KOBJ)\myldrClose.obj \
    $(WIN32KOBJ)\myldrOpen.obj \
    $(WIN32KOBJ)\myldrRead.obj \
    $(WIN32KOBJ)\myLDRQAppType.obj \
    $(WIN32KOBJ)\myldrEnum32bitRelRecs.obj \
    $(WIN32KOBJ)\myldrOpenPath.obj \
    $(WIN32KOBJ)\OS2KTCB.obj \
    $(WIN32KOBJ)\OS2KPTDA.obj \
    $(WIN32KOBJ)\pe2lx.obj \
    $(WIN32KOBJ)\ModuleBase.obj \
    $(WIN32KOBJ)\d32ElfIOCtl.obj \
    $(WIN32KOBJ)\d32Win32kIOCtl.obj \
    $(WIN32KOBJ)\k32AllocMemEx.obj \
    $(WIN32KOBJ)\k32QueryOTEs.obj \
    $(WIN32KOBJ)\k32QueryOptionsStatus.obj \
    $(WIN32KOBJ)\k32SetOptions.obj \
    $(WIN32KOBJ)\mytkExecPgm.obj \
    $(WIN32KOBJ)\mytkStartProcess.obj \
    $(WIN32KOBJ)\vprntf16.obj_tst \
    $(WIN32KOBJ)\d32init.obj_tst \
    $(WIN32KOBJ)\SymDB.obj \
    $(WIN32KOBJ)\d16init.obj_tst_init \
    $(WIN32KOBJ)\ProbKrnl.obj_tst_init

TSTLASTOBJ = $(LASTOBJ)

TSTLIBS = $(LIBS)


################################################################################
# options.inc rule.
################################################################################
$(WIN32KINCLUDE)\options.inc:  $(WIN32KINCLUDE)\options.h
    @$(ECHO) H2Inc: $**
    $(H2INC) $** > $@


################################################################################
# clfix.exe rule.
################################################################################
clfix.exe: $(@B).c
    $(CC) $(@B).c


################################################################################
# extract.exe rule.
################################################################################
EXTRACTOBJS = \
$(WIN32KOBJ)\extract.obj_e \
$(WIN32KOBJ)\probkrnl.obj_e \
$(WIN32KOBJ)\d16globl.obj_e \
$(WIN32KOBJ)\vprntf16.obj_e

{dev16}.c.obj_e:
    @$(ECHO) compiling 16bit (extract): $<
    $(CC16) -c -W3 -Asfw -G2s -Zp -Zl -nologo -Fo$@ -Fa -Zi -Od -Lp -DDEBUG -DEXTRACT \
        -Fa$(WIN32KLIST)\$(*B)_e.s -Fo$(WIN32KOBJ)\$(@B).obj_e $(CINCLUDES16) $<

$(WIN32KBIN)\extract.exe: $(EXTRACTOBJS)
    $(LD) $(LFLAGS) /pmtype:vio @<<$(WIN32KOBJ)\$(@B).lnk
        /OUT:$@
        /MAP:$(WIN32KBIN)\$(@B).map
        $(EXTRACTOBJS)
        $(TOOLKIT)\lib\os2286.lib
        $(MSCPATH)\lib\clibcep.lib
<<keep

extract.exe: $(WIN32KBIN)\extract.exe
    $(CP) $** $@


################################################################################
# Win32k.sys rule. (Don't mess with the order or objects and libraries!)
################################################################################
$(NAME).sys: $(WIN32KBIN)\$(NAME).sys
$(WIN32KBIN)\$(NAME).sys: $(OBJS) $(LIBS) $(NAME).def $(LASTOBJ) $(LIBSINIT) makefile
    -@$(ECHO) recompiling d16globl to get correct build time and date:
    $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) -Fo$(WIN32KOBJ)\d16globl.obj \
        -Fa$(WIN32KLIST)\d16globl.s dev16\d16globl.c
    -@$(ECHO) linking: $@
    -4 $(LD) $(LFLAGS) @<<$(WIN32KOBJ)\$(@B).lnk
/OUT:$@
/MAP:$(WIN32KBIN)\$(@B).map
$(OBJS:  =^
)
/IG
$(LIBS:  =^
)
$(LASTOBJ)
$(NAME).def
<<KEEP
    cd $(WIN32KBIN)
    @mapsym $(*B).map > nul
    cd $(WIN32KBASE)
    $(CP) $@ $(PDWIN32_BIN)
    $(CP) $*.sym $(PDWIN32_BIN)
    $(CP) $(WIN32KBASE)\$(*B).ddp $(PDWIN32_BIN)


################################################################################
# Win32ktst.exe rule.
################################################################################
$(NAME)tst.exe: $(WIN32KBIN)\$(NAME)tst.exe
    $(CP) $(WIN32KBIN)\$@ $@

$(WIN32KBIN)\$(NAME)tst.exe: $(NAME)tst.def $(TSTOBJS) $(TSTLIBS) $(LASTOBJS) makefile
    -@$(ECHO) linking: $@
    -4 $(LD) $(LFLAGS) @<<$(WIN32KOBJ)\$(@B).lnk
/DEBUG
/OUT:$@
/MAP:$(WIN32KBIN)\$(@B).map
$(TSTOBJS:  =^
)
/IG
$(TSTLIBS:  =^
)
$(LASTOBJ)
$(NAME)tst.def
<<KEEP


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
# Win32k Control Center Rule.
################################################################################
Win32kCC.exe:
    cd utils
    $(MAKE_CMD)
    cd ..

################################################################################
# kRx.EXE - Pure Rexx interpreter
################################################################################
kRx.exe:
    cd rexx
    $(MAKE_CMD)
    cd ..

################################################################################
# Libraries - segments are renamed for the 16-bit libraries.
################################################################################
DHLPOBJS =  +$(WIN32KOBJ)\dhcall5b.obj +$(WIN32KOBJ)\dhcal11a.obj \
            +$(WIN32KOBJ)\dhcal11g.obj +$(WIN32KOBJ)\dhret.obj

$(WIN32KOBJ)\devhelp.lib: $(DDKPATH)\lib\dhcalls.lib $(WIN32KBIN)\libconv.exe $(DHLPOBJS:+=)
#!if "$(CCENV)" == "VAC36"
    @$(RM) $@
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $@ $(DHLPOBJS);
    @cd $(MAKEDIR)
#!else
#    $(WIN32KBIN)\libconv  $(DDKPATH)\lib\dhcalls.lib $@
#!endif

$(WIN32KOBJ)\devhelp_.lib: $(DDKPATH)\lib\dhcalls.lib
    $(WIN32KBIN)\libconv $** $@

$(DHLPOBJS:+=): $(WIN32KOBJ)\devhelp_.lib
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $** *$@;
    @cd $(MAKEDIR)


CLIBOBJS =  +$(WIN32KOBJ)\inp.obj       +$(WIN32KOBJ)\outp.obj \
            +$(WIN32KOBJ)\anfalmul.obj  +$(WIN32KOBJ)\anfaldiv.obj \
            +$(WIN32KOBJ)\anfalrem.obj  +$(WIN32KOBJ)\anfauldi.obj \
            +$(WIN32KOBJ)\anuldiv.obj   +$(WIN32KOBJ)\fmemcpy.obj \
            +$(WIN32KOBJ)\anulrem.obj   +$(WIN32KOBJ)\anlmul.obj \
            +$(WIN32KOBJ)\__AHINCR.obj  +$(WIN32KOBJ)\anlshl.obj

$(WIN32KOBJ)\clib.lib: $(MSCPATH)\lib\clibcep.lib $(WIN32KBIN)\libconv.exe $(CLIBOBJS:+=)
    @$(RM) $@
    @cd $(WIN32KOBJ)
    $(DDKPATH)\tools\lib /nologo $@ $(CLIBOBJS);
    @cd $(MAKEDIR)

$(WIN32KOBJ)\clib_.lib: $(MSCPATH)\lib\clibcep.lib
    $(WIN32KBIN)\libconv $** $@ > nul

$(CLIBOBJS:+=): $(WIN32KOBJ)\clib_.lib
    @cd $(WIN32KOBJ)
    $(ILIB) $** *$@;
    @cd $(MAKEDIR)


$(WIN32KOBJ)\last.lib: $(WIN32KOBJ)\devlast.obj
    -@$(RM) $@ > nul 2> nul
    $(ILIB) $@ $**;

$(WIN32KBIN)\libconv.exe: libconv.c
    icc -Q+ -Ti+ -Fe$@ -Fo$(WIN32KOBJ)\$(*B).obj libconv.c


#################################################################################
# Documentation generation - require SDS - http://www.ii.uib.no/~stig/
################################################################################
lxdoc:
    cpp2sds -o DocsA\ -i include\modulebase.h include\pe2lx.h ldr\modulebase.cpp \
        pe2lx\pe2lx.cpp
    cd DocsA
    exp sds2doc -ai *.sds

fulldoc:
    exp cpp2sds -o DocsA\ -i include\*.h *.cpp ldr\*.cpp misc\*.cpp pe2lx\*.cpp
    cd DocsA
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
    $(RM) $(WIN32KBIN)\*.*
!endif
!if "$(WIN32KOBJ)" != ""
    $(RM) $(WIN32KOBJ)\*obj* \
        $(WIN32KOBJ)\*lnk* \
        $(WIN32KOBJ)\*res*
!endif
!if "$(WIN32KLIST)" != ""
    $(RM) $(WIN32KLIST)\*lst* \
        $(WIN32KLIST)\*.s* \
        $(WIN32KLIST)\*as*
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
        dev32\*.c* pe2lx\*.c* k32\*.c* lib\*.c elf2lx\*.c* test\*.c* \
        $(WIN32KINCLUDE)\*.h
    $(PDWIN32_TOOLS)\fastdep -a+ -objobj_tst -o$$(WIN32KOBJ) $(CINCLUDES) misc\*.c* ldr\*.c* \
        dev32\*.c* pe2lx\*.c* k32\*.c* lib\*.c elf2lx\*.c* test\*.c*
    $(PDWIN32_TOOLS)\fastdep -a+ -o$$(WIN32KOBJ) -I$(WIN32KINCLUDE) \
        -I$(DDKPATH)\inc dev32\*.asm ldr\*.asm misc\*.asm test\*.asm include\*.inc
    $(PDWIN32_TOOLS)\fastdep -a+ -obr- $(CINCLUDES16) dev16\*.c*
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

