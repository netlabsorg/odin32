# $Id: makefile,v 1.19 2000-11-19 09:06:58 bird Exp $

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
!include ../../include/pdwin32.mk


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
!ifndef DEFFILE
!error "not supported on this compiler yet."
!endif


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\odincrt.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\malloc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\file.obj


#
# Libraries. One space before the '\'.
#
LIBS =


#
# Target name - name of the dll without extention and path.
#
TARGET = $(ODINCRT)


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

