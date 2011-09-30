# $Id: makefile,v 1.16 2001-02-14 11:41:02 sandervl Exp $

#
# Odin32 API
#
#       Top-level makefile for glu32
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


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
$(OBJDIR)\glu32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
nurbs\interface\$(OBJDIR)\interface.lib \
nurbs\internals\$(OBJDIR)\internals.lib \
nurbs\nurbtess\$(OBJDIR)\nurbtess.lib \
tess\$(OBJDIR)\tess.lib \
util\$(OBJDIR)\util.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\opengl32.lib \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = glu32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

