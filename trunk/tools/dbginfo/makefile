# $Id: makefile,v 1.3 2000-04-24 21:38:12 bird Exp $
#
# Makefile for debug info convertes - experimental.
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
EXETARGET=1
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CINCLUDES  = -I..\common

CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn -Tm-
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn -Tm-


# All rule - build objs, target dll, copies dll to bin and makes libs.
all: Sym2Hll.exe dbgLXDumper.exe


#
# Sym2Hll executable rule.
#
Sym2Hll:
Sym2Hll.exe: Sym2Hll.obj kHll.obj ..\common\commonicc.lib
    $(LD2) $(LD2FLAGS) /PMTYPE:vio /EXEC:$@ $** $(RTLLIB) os2386.lib


#
# LX debug info dumper (HLL only) executable rule.
#
dbgLXDumper.exe: dbgLXDumper.obj dummy.obj
    $(LD2) $(LD2FLAGS) /PMTYPE:vio /EXEC:$@ $** $(RTLLIB) os2386.lib

# Dummy source file rule
dummy.c:
    echo creating <<$@
/* dummy.c */
int idummy;
<<keep

dummy.obj: dummy.c
    icc -c  $**




# Dep rule - makes depenencies for C, C++ and Asm files.
# -o- removes the object directory - for the time being.
dep:
    $(DEPEND) -o- -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win; $(CINCLUDES) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post


