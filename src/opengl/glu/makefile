# $Id: makefile,v 1.14 2000-11-19 08:58:13 bird Exp $

#
# Odin32 API
#
# Top-level makefile for glu32
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\tools\bin

#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk


#
# Overrides.
#
CINCLUDES   = $(CINCLUDES) -I..\mesa\GL


#
# Subdirectories to travers.
#
SUBDIRS = \
.\util \
.\tess \
.\nurbs \


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\glu32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
nurbs\interface\$(OBJDIR)\interface.lib \
nurbs\internals\$(OBJDIR)\internals.lib \
nurbs\nurbtess\$(OBJDIR)\nurbtess.lib \
tess\$(OBJDIR)\tess.lib \
util\$(OBJDIR)\util.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)\kernel32.lib \
$(PDWIN32_LIB)\opengl32.lib \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = glu32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

