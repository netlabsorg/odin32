# $Id: makefile,v 1.27 2002-04-29 13:19:31 bird Exp $

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
CDEFINES    = $(CDEFINES_ODINCRT)
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

!ifdef DEBUG
DBG=d
!endif

!ifdef PROFILE
DBG=p
!endif

!ifdef RELEASE
DBG=
!endif

!ifdef VAC3
DEFFILE = odincrt$(DBG).def
!endif
!ifdef VAC36
DEFFILE = odin36$(DBG).def
!endif
!ifdef WAT
DEFFILE = odinwat$(DBG).def
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
$(OBJDIR)\string.obj \
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

