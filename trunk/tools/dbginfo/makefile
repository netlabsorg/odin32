# $Id: makefile,v 1.7 2000-12-02 23:26:59 bird Exp $

#
# Makefile for debug info convertes - experimental.
#   Don't bother to cleanup this mess yet.
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


# Directory macros.
ODIN32_TCOMMON = ..\common


# Compiler, tools, and interference rules.
EXETARGET=1
OLD_STYLE=1
!include ../../makefile.inc
!include $(ODIN32_TCOMMON)\common.mk


# Flag overloads and local macros.
CINCLUDES  = -I$(ODIN32_TCOMMON)
CFLAGS     = $(CFLAGS)   -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Ti+ -Tm-
CXXFLAGS   = $(CXXFLAGS) -Wall+ppt-ppc-inl-cnv-gnr-vft-             -Gm- -Ti+ -Tm- $(CXXEXCEPTIONS)


# All rule - build objs, target dll, copies dll to bin and makes libs.
all: kHll.exe # Sym2Hll.exe dbgLXDumper.exe


#
# Sym2Hll executable rule.
#
Sym2Hll:
Sym2Hll.exe: Sym2Hll.obj kHll.obj $(COMMONLIB)
    $(LD2) $(LD2FLAGS) /PMTYPE:vio /EXEC /OUT:$@ $** $(RTLLIB) os2386.lib

#
# New LX debug info dumper.
#
kHll.exe: kHll.obj kHllMain.obj $(COMMONLIB)
    $(LD2) $(LD2FLAGS) /PMTYPE:vio /EXEC /OUT:$@ $** $(RTLLIB) os2386.lib

#
# LX debug info dumper (HLL only) executable rule.
#
dbgLXDumper.exe: dbgLXDumper.obj dummy.obj
    $(LD2) $(LD2FLAGS) /PMTYPE:vio /EXEC /OUT:$@ $** $(RTLLIB) os2386.lib

# Dummy source file rule
dummy.c:
    echo creating <<$@
/* dummy.c */
int idummy;
<<keep

dummy.obj: dummy.c
    icc -c  $**


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

