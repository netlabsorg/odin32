# $Id: win32k.tools.mk,v 1.2 2000-12-03 21:45:10 bird Exp $

#
# Win32k common tools makefile.
#
# Copyright 1998-2000 knut st. osmundsen
#
# Project Odin Software License can be found in LICENSE.TXT
#
#


#
# Sanity check
#
!ifndef WIN32KOBJ
!error  "Fatal error! WIN32KOBJ is not defined!"
!endif
!ifndef WIN32KLIST
!error  "Fatal error! WIN32KLIST is not defined!"
!endif


#
# Compiler and Linker.
#

!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
#
# IBM VisualAge for C++ v3.08 or IBM C/C++ Tools v3.6.5
#
CC16        = clfix.exe $(MSCPATH)\binp\cl.exe
CC          = icc
CCPP        = icc
LD          = ilink
ILIB        = ilib /nobackup /nologo \
!if "$(CCENV)" == "VAC36"
                   /nofree
!endif

!ifndef DEBUG
CFLAGS16    = -c -W3 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo -Ogeitln -Gs
CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Ti+ -G4 -Oc+
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-        -Gx+ -Ti+ -G4 -Oc+
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /EXEPACK:2 /NODEBUG
!else
CFLAGS16    = -c -W3 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo -Zi -Owis
CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Ti+ -Oc+ -Os-
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-        -Gx+ -Ti+ -Oc+ -Os-
!ifdef NODEBUGINFO
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /NOEXEPACK /DEBUG
!else
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /EXEPACK:2 /NODEBUG
!endif
!endif

!else

#
# Watcom C/C++ Compilers and linker.
#
!if "$(WAT)" == "1"
CC16        = wcc
CC          = wcc386
CCPP        = wpp386
LD          = wlink
ILIB        = wlib /nobackup /nologo \

!error "Compiler is not yet supported!"
!else
#
# Other compilers
#
!error "Compiler is not supported!"
!endif
!endif


#
# Assembler and other DDK tools.
#
AS      = alp
ASFLAGS = -Sv:ALP -Mb -Li +Od
H2INC   = $(DDKPATH)\tools\h2inc.exe


#
# Defines
#
!ifdef DEBUG
CDEFINES    = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__ -DDEBUG
CDEFINES16  = -DRING0    -DDEBUG
ASDEFINES   = -D:WIN32K -D:DEBUG
!else
CDEFINES    = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__ -DRELEASE
CDEFINES16  = -DRING0    -DRELEASE
ASDEFINES   = -D:WIN32K -D:RELEASE
!endif

CDEFINES_TST    = $(CDEFINES)   -DR3TST
CDEFINES16_TST  = $(CDEFINES16) -DR3TST


#
# Includepaths
#
CINCLUDES  = -I$(WIN32KINCLUDE) \
             -I$(TOOLKIT)\h \
             -I$(VACPATH)\include \
             -I$(DDKPATH)\h \
             -I$(DDKPATH)\src\dev\dasd\diskh \
             -I. \
             -I$(ODIN32_INCLUDE) \
             -I$(ODIN32_INCLUDE)\win

CINCLUDES16= -I$(WIN32KINCLUDE) \
             -I. \
             -I$(DDKPATH)\h \
             -I$(MSCPATH)\include \
             -I$(ODIN32_INCLUDE) \
             -I$(TOOLKIT)\h

AINCLUDES  = -Fdi:$(WIN32KINCLUDE) \
             -Fdi:$(DDKPATH)\inc


#
# Sanity check.
#
!ifndef RTLLIB_NRE
!error  "Fatal error RTLLIB_NRE is not defined.
!endif


#
# Suffixes and interference rules.
#
.SUFFIXES: .obj .cpp .c .asm .obj_e .obj_tst .obj_init .obj_tst_init .inc

C16COMPILE          = $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) -Fa$(WIN32KLIST)\$(@B).s -Fo$(WIN32KOBJ)\$(@F) $<
C16COMPILE_INIT     = $(CC16) -c $(CFLAGS16) $(CDEFINES16) $(CINCLUDES16) -Fa$(WIN32KLIST)\$(@B).s -Fo$(WIN32KOBJ)\$(@F) /NTCODE16_INIT /NDDATA16_INIT $<
CCOMPILE            = $(CC)   -c $(CFLAGS)   $(CDEFINES)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).s -Fo$(WIN32KOBJ)\$(@F) $<
CPPCOMPILE          = $(CCPP) -c $(CPPFLAGS) $(CDEFINES)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).s -Fo$(WIN32KOBJ)\$(@F) $<
ASMCOMPILE          = $(AS)     $(ASFLAGS)   $(ADEFINES)   $(AINCLUDES) $< -Fl:$(WIN32KLIST)\$(@B).lst -Fo:$(WIN32KOBJ)\$(@F)

C16COMPILE_TST      = $(CC16) -c $(CFLAGS16) $(CDEFINES16_TST) $(CINCLUDES16) -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) $<
C16COMPILE_TST_INIT = $(CC16) -c $(CFLAGS16) $(CDEFINES16_TST) $(CINCLUDES16) -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) /NTCODE16_INIT /NDDATA16_INIT $<
CCOMPILE_TST        = $(CC)   -c $(CFLAGS)   $(CDEFINES_TST)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) $<
CCOMPILE_TST_TEST   = $(CC)   -c $(CFLAGS)   $(CDEFINES_TST)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) $<
CPPCOMPILE_TST      = $(CCPP) -c $(CPPFLAGS) $(CDEFINES_TST)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) $<
CPPCOMPILE_TST_TEST = $(CCPP) -c $(CPPFLAGS) $(CDEFINES_TST)   $(CINCLUDES)   -Fa$(WIN32KLIST)\$(@B).tst.s -Fo$(WIN32KOBJ)\$(@F) $<
ASMCOMPILE_TST      = $(ASMCOMPILE)


!ifndef __VERSION__
SOURCEFILE  = $<
!else
SOURCEFILE  = $^.
!endif


{dev16}.c.obj:
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE)
    $(C16COMPILE)
{dev16}.c.obj_init:
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE)
    $(C16COMPILE_INIT)
    @$(CP) $@ $@.obj

{dev16}.c.obj_tst:
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE)
    @$(C16COMPILE_TST)
    @$(CP) $@ $@.obj
{dev16}.c.obj_tst_init:
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE)
    @$(C16COMPILE_TST_INIT)
    @$(CP) $@ $@.obj


{dev32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling dev32:       $(SOURCEFILE)
    @$(ASMCOMPILE)
{dev32}.asm.obj:
    @$(ECHO) assembling dev32:       $(SOURCEFILE)
    @$(ASMCOMPILE)

{dev32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)
{dev32}.c.obj:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)

{dev32}.c{$(WIN32KOBJ)}.obj_tst:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)
    @$(CP) $@ $@.obj
{dev32}.c.obj_tst:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)
    @$(CP) $@ $@.obj

{dev32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)
{dev32}.cpp.obj:
    @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE)
    @$(CCOMPILE)


{elf2lx}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE)
    @$(CCOMPILE)
{elf2lx}.c.obj:
    @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE)
    @$(CCOMPILE)

{elf2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE)
    @$(CPPCOMPILE)
{elf2lx}.cpp.obj:
    @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE)
    @$(CPPCOMPILE)


{k32}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling k32:         $(SOURCEFILE)
    @$(ASMCOMPILE)
{k32}.asm.obj:
    @$(ECHO) assembling k32:         $(SOURCEFILE)
    @$(ASMCOMPILE)

{k32}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit k32:    $(SOURCEFILE)
    @$(CCOMPILE)
{k32}.c.obj:
    @$(ECHO) compiling 32bit k32:    $(SOURCEFILE)
    @$(CCOMPILE)

{k32}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit k32:    $(SOURCEFILE)
    @$(CPPCOMPILE)
{k32}.cpp.obj:
    @$(ECHO) compiling 32bit k32:    $(SOURCEFILE)
    @$(CPPCOMPILE)


{ldr}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling ldr:         $(SOURCEFILE)
    @$(ASMCOMPILE)
{ldr}.asm.obj:
    @$(ECHO) assembling ldr:         $(SOURCEFILE)
    @$(ASMCOMPILE)

{ldr}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE)
    @$(CCOMPILE)
{ldr}.c.obj:
    @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE)
    @$(CCOMPILE)

{ldr}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE)
    @$(CPPCOMPILE)
{ldr}.cpp.obj:
    @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE)
    @$(CPPCOMPILE)


{misc}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling misc:        $(SOURCEFILE)
    @$(ASMCOMPILE)
{misc}.asm.obj:
    @$(ECHO) assembling misc:        $(SOURCEFILE)
    @$(ASMCOMPILE)

{misc}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CCOMPILE)
{misc}.c.obj:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CCOMPILE)

{misc}.c{$(WIN32KOBJ)}.obj_tst:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CCOMPILE_TST)
    @$(CP) $@ $@.obj
{misc}.c.obj_tst:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CCOMPILE_TST)
    @$(CP) $@ $@.obj

{misc}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CPPCOMPILE)
{misc}.cpp.obj:
    @$(ECHO) compiling 32bit misc:   $(SOURCEFILE)
    @$(CPPCOMPILE)


{pe2lx}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE)
    @$(CCOMPILE)
{pe2lx}.c.obj:
    @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE)
    @$(CCOMPILE)

{pe2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE)
    @$(CPPCOMPILE)
{pe2lx}.cpp.obj:
    @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE)
    @$(CPPCOMPILE)


{test}.asm{$(WIN32KOBJ)}.obj:
    @$(ECHO) assembling test:        $(SOURCEFILE)
    @$(ASMCOMPILE_TST)
{test}.asm.obj:
    @$(ECHO) assembling test:        $(SOURCEFILE)
    @$(ASMCOMPILE_TST)

{test}.c{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit test:   $(SOURCEFILE)
    @$(CCOMPILE_TST_TEST)
{test}.c.obj:
    @$(ECHO) compiling 32bit test:   $(SOURCEFILE)
    @$(CCOMPILE_TST_TEST)

{test}.cpp{$(WIN32KOBJ)}.obj:
    @$(ECHO) compiling 32bit test:   $(SOURCEFILE)
    @$(CPPCOMPILE_TST_TEST)
{test}.cpp.obj:
    @$(ECHO) compiling 32bit test:   $(SOURCEFILE)
    @$(CPPCOMPILE_TST_TEST)


