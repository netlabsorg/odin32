# $Id: makefile,v 1.21 2000-12-16 23:33:37 bird Exp $

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
!else
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
$(OBJDIR)\malloc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\file.obj
!else
$(%WATCOM)\lib386\os2\$(RTLLIB_O)
!endif


#
# Libraries. One space before the '\'.
#
LIBS = \
!ifdef WAT
$(RTLLIB_O)
!endif


#
# Target name - name of the dll without extention and path.
#
TARGET = $(ODINCRT)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

