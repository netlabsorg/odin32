# $Id: makefile,v 1.32 2003-02-28 10:32:50 sandervl Exp $

#
# Odin32 API
#
#       odincrt.dll makefile
#


#
# Tell the buildenvironment not to
#
NOTEXPDEF   = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)
!ifdef WITH_KLIB
CDEFINES    = $(CDEFINES_ODINCRT) -DWITH_KLIB
!else
CDEFINES    = $(CDEFINES_ODINCRT)
!endif
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

DBG=
!ifdef DEBUG
DBG=d
!endif

# Sorry, this looks like hell now.
!ifdef VAC3
! ifdef WITH_KLIB
DEFFILE_ORG = odincrt$(DBG).def
DEFFILE     = $(OBJDIR)\odincrt$(DBG)klib.def
! else
DEFFILE     = odincrt$(DBG).def
! endif
!endif
!ifdef VAC36
! ifdef WITH_KLIB
DEFFILE_ORG = odin36$(DBG).def
DEFFILE     = $(OBJDIR)\odin36$(DBG)klib.def
! else
DEFFILE     = odin36$(DBG).def
! endif
!endif
!ifdef WAT
! ifdef WITH_KLIB
DEFFILE_ORG = odinwat$(DBG).def
DEFFILE     = $(OBJDIR)\odinwat$(DBG)klib.def
! else
DEFFILE     = odinwat$(DBG).def
! endif
!endif
!ifndef DEFFILE
!error "not supported on this compiler yet."
!endif


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\odincrt.obj \
$(OBJDIR)\initterm.obj \
!ifndef WAT
!ifdef VAC3
$(OBJDIR)\math64.obj \
!endif
!ifdef WITH_KLIB
$(ODIN32_LIB)\kHeapDbgVACWrappersR3.lib \
!endif
$(OBJDIR)\malloc.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\critsect.obj \
$(OBJDIR)\interlock.obj \
!else
!ifdef NMAKE
$(WATCOM)\lib386\os2\$(RTLLIB_O)
!else
$(%WATCOM)\lib386\os2\$(RTLLIB_O)
!endif
!endif


#
# Libraries. One space before the '\'.
#
LIBS = \
!ifdef WITH_KLIB
$(ODIN32_LIB)\kLibR3.lib \
!endif
!ifdef WAT
$(RTLLIB_O)
!endif
!ifdef VAC36
$(SOMLIB)
!endif

#
# Target name - name of the dll without extention and path.
#
TARGET = $(ODINCRT)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)



!ifdef DEFFILE_ORG
# Add kLib export to the def-file.
$(DEFFILE): Makefile $(DEFFILE_ORG)
    $(CP) $(DEFFILE_ORG) $@
    $(ECHO)     kHeapDbgException           @1500 >> $@
!endif

