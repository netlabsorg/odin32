# $Id: win32k.tools.mk,v 1.10.2.8 2002-03-31 20:08:45 bird Exp $

#
# Win32k common tools makefile.
#
# Copyright 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
CC16        = $(WIN32KBIN)\clfix.exe $(MSCPATH)\binp\cl.exe
!ifdef DEBUG
CC          = $(WIN32KBIN)\ignore.exe -EDC0506 icc
CCPP        = icc
!else
CC          = $(WIN32KBIN)\ignore.exe -EDC0506 -EDC0435 -EDC0137 -szFileId icc
CCPP        = $(WIN32KBIN)\ignore.exe -EDC0435 -EDC0137 -szFileId icc
!endif
LD          = ilink
ILIB        = ilib /nobackup /nologo
!if "$(CCENV)" == "VAC36"
ILIB        = ilib /nofree /nobackup /nologo
!endif

!ifndef DEBUG
CFLAGS16    = -c -W0 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo -Ogeitln -Gs
CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Ti+ -G4 -Oc+ -Tm-
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-        -Gx+ -Ti+ -G4 -Oc+ -Tm-
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /EXEPACK:2 /NODEBUG
!else
CFLAGS16    = -c -W0 -Asfw /NTCODE16 /NDDATA16 -G2s -Zp -Zl -nologo -Zi -Owis
CFLAGS      = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Ti+ -Oc+ -Os- -Tm-
CPPFLAGS    = -q -Ge -Gs- -Gr+ -Rn -Ss+ -Mp -Wall+ppt-ppc-inl-cnv-gnr-vft-        -Gx+ -Ti+ -Oc+ -Os- -Tm-
!ifdef NODEBUGINFO
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /EXEPACK:2 /NODEBUG
!else
LFLAGS      = /nologo /MAP /NOI /NOE /NOD /A:16 /PACKCODE /PACKDATA /NOEXEPACK /DEBUG
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
AS      = @$(CMDQD_SUB) alp
AFLAGS  = -Sv:ALP -Mb -Li +Od
H2INC   = $(DDKPATH)\tools\h2inc.exe -c -w -f -d


#
# Defines
#
!ifdef DEBUG
CDEFINES    = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__ -DDEBUG $(CDEFINESEXTRA)
CDEFINES16  = -DRING0    -DDEBUG $(CDEFINESEXTRA16)
ADEFINES    = -D:WIN32K -D:RING0 -D:DEBUG $(ADEFINESEXTRA)
!else
CDEFINES    = -DWIN32K -DRING0 -D__WIN32OS2__ -D__WINE__ -DRELEASE $(CDEFINESEXTRA)
CDEFINES16  = -DRING0    -DRELEASE $(CDEFINESEXTRA16)
ADEFINES    = -D:WIN32K -D:RING0 -D:RELEASE $(ADEFINESEXTRA)
!endif

CDEFINES_TST    = $(CDEFINES)   -DR3TST
CDEFINES16_TST  = $(CDEFINES16) -DR3TST


#
# Includepaths
#
CINCLUDES  = -I$(WIN32KINCLUDE) \
             -I$(KKRNLLIBINCLUDE) \
             -I$(TOOLKIT)\h \
             -I$(VACPATH)\include \
             -I$(DDKPATH)\h \
             -I$(DDKPATH)\src\dev\dasd\diskh \
             -I. \
             -I$(ODIN32_INCLUDE) \
             -I$(ODIN32_INCLUDE)\win

CINCLUDES16= -I$(WIN32KINCLUDE) \
             -I$(KKRNLLIBINCLUDE) \
             -I. \
             -I$(DDKPATH)\h \
             -I$(MSCPATH)\include \
             -I$(ODIN32_INCLUDE) \
             -I$(TOOLKIT)\h

AINCLUDES  = -Fdi:$(WIN32KINCLUDE) \
             -Fdi:$(KKRNLLIBINCLUDE) \
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
ASMCOMPILE          = $(AS)     $(AFLAGS)    $(ADEFINES)   $(AINCLUDES) $< -Fl:$(WIN32KLIST)\$(@B).lst -Fo:$(WIN32KOBJ)\$(@F)

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
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE) & $(C16COMPILE)
{dev16}.c.obj_init:
    @$(ECHO) compiling 16bit dev16:  $(SOURCEFILE) & $(C16COMPILE_INIT) && $(CP) $@ $@.obj

{dev16}.c.obj_tst:
    @$(ECHO) compiling 16bit dev16 tst:  $(SOURCEFILE) & $(C16COMPILE_TST)
#    @$(CP) $@ $@.obj
{dev16}.c.obj_tst_init:
    @$(ECHO) compiling 16bit dev16 tst:  $(SOURCEFILE) & $(C16COMPILE_TST_INIT)
#    @$(CP) $@ $@.obj


{dev32}.asm{$(WIN32KOBJ)}.obj:
    $(CMDQD_SUB) $(ECHO) assembling dev32:       $(SOURCEFILE) & $(ASMCOMPILE)
{dev32}.asm.obj:
    $(CMDQD_SUB) $(ECHO) assembling dev32:       $(SOURCEFILE) & $(ASMCOMPILE)

{dev32}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE) & $(CCOMPILE)
{dev32}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE) & $(CCOMPILE)

{dev32}.c{$(WIN32KOBJ)}.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32 tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj
{dev32}.c.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32 tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj

{dev32}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE) & $(CCOMPILE)
{dev32}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit dev32:  $(SOURCEFILE) & $(CCOMPILE)


{elf2lx}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE) & $(CCOMPILE)
{elf2lx}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE) & $(CCOMPILE)

{elf2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE) & $(CPPCOMPILE)
{elf2lx}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit elf2lx: $(SOURCEFILE) & $(CPPCOMPILE)


{k32}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling k32:         $(SOURCEFILE) & $(ASMCOMPILE)
{k32}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling k32:         $(SOURCEFILE) & $(ASMCOMPILE)

{k32}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit k32:    $(SOURCEFILE) & $(CCOMPILE)
{k32}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit k32:    $(SOURCEFILE) & $(CCOMPILE)

{k32}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit k32:    $(SOURCEFILE) & $(CPPCOMPILE)
{k32}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit k32:    $(SOURCEFILE) & $(CPPCOMPILE)


{ldr}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling ldr:         $(SOURCEFILE) & $(ASMCOMPILE)
{ldr}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling ldr:         $(SOURCEFILE) & $(ASMCOMPILE)

{ldr}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE) & $(CCOMPILE)
{ldr}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE) & $(CCOMPILE)

{ldr}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE) & $(CPPCOMPILE)
{ldr}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit ldr:    $(SOURCEFILE) & $(CPPCOMPILE)


{api}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling api:         $(SOURCEFILE) & $(ASMCOMPILE)
{api}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling api:         $(SOURCEFILE) & $(ASMCOMPILE)

{api}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit api:    $(SOURCEFILE) & $(CCOMPILE)
{api}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit api:    $(SOURCEFILE) & $(CCOMPILE)

{api}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit api:    $(SOURCEFILE) & $(CPPCOMPILE)
{api}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit api:    $(SOURCEFILE) & $(CPPCOMPILE)


{misc}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling misc:        $(SOURCEFILE) & $(ASMCOMPILE)
{misc}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling misc:        $(SOURCEFILE) & $(ASMCOMPILE)

{misc}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc:   $(SOURCEFILE) & $(CCOMPILE)
{misc}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc:   $(SOURCEFILE) & $(CCOMPILE)

{misc}.c{$(WIN32KOBJ)}.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc tst: $(SOURCEFILE) & $(CCOMPILE_TST)
#    @$(CP) $@ $@.obj
{misc}.c.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc tst: $(SOURCEFILE) & $(CCOMPILE_TST)
#    @$(CP) $@ $@.obj

{misc}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc:   $(SOURCEFILE) & $(CPPCOMPILE)
{misc}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit misc:   $(SOURCEFILE) & $(CPPCOMPILE)


{Krnl}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling Krnl:       $(SOURCEFILE) & $(ASMCOMPILE)
{Krnl}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling Krnl:       $(SOURCEFILE) & $(ASMCOMPILE)

{Krnl}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)
{Krnl}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)

{Krnl}.c{$(WIN32KOBJ)}.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj
{Krnl}.c.obj_tst:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj

{Krnl}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)
{Krnl}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)


{clib}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling Krnl:       $(SOURCEFILE) & $(ASMCOMPILE)
{clib}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling Krnl:       $(SOURCEFILE) & $(ASMCOMPILE)

{clib}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)
{clib}.c.obj:
    @$(CMDQD_SUB) $(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)

{clib}.c{$(WIN32KOBJ)}.obj_tst:
    @$(CMDQD_SUB) $(ECHO) compiling 32bit Krnl tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj
{clib}.c.obj_tst:
    @$(CMDQD_SUB) $(ECHO) compiling 32bit Krnl tst: $(SOURCEFILE) & $(CCOMPILE_TST) && $(CP) $@ $@.obj

{clib}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)
{clib}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit Krnl:  $(SOURCEFILE) & $(CCOMPILE)




{pe2lx}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE) & $(CCOMPILE)
{pe2lx}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE) & $(CCOMPILE)

{pe2lx}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE) & $(CPPCOMPILE)
{pe2lx}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit pe2lx:  $(SOURCEFILE) & $(CPPCOMPILE)


{test}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling test:        $(SOURCEFILE) & $(ASMCOMPILE_TST)
{test}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling test:        $(SOURCEFILE) & $(ASMCOMPILE_TST)

{test}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit test:   $(SOURCEFILE) & $(CCOMPILE_TST_TEST)
{test}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit test:   $(SOURCEFILE) & $(CCOMPILE_TST_TEST)

{test}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit test:   $(SOURCEFILE) & $(CPPCOMPILE_TST_TEST)
{test}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit test:   $(SOURCEFILE) & $(CPPCOMPILE_TST_TEST)



{testcase}.asm{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling testcase:    $(SOURCEFILE) & @$(CMDQD_SUB) $(ASMCOMPILE_TST)
{testcase}.asm.obj:
    @$(CMDQD_SUB) @$(ECHO) assembling testcase:    $(SOURCEFILE) & $(ASMCOMPILE_TST)

{testcase}.c{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit testcase: $(SOURCEFILE) & $(CCOMPILE_TST_TEST)
{testcase}.c.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit testcase: $(SOURCEFILE) & $(CCOMPILE_TST_TEST)

{testcase}.cpp{$(WIN32KOBJ)}.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit testcase: $(SOURCEFILE) & $(CPPCOMPILE_TST_TEST)
{testcase}.cpp.obj:
    @$(CMDQD_SUB) @$(ECHO) compiling 32bit testcase: $(SOURCEFILE) & $(CPPCOMPILE_TST_TEST)
