# $Id: shlwapilib.mak,v 1.1 2001-07-20 15:37:53 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES    = $(CDEFINES) -DINVERT


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\shlwapi.obj \
$(OBJDIR)\shlstub.obj \
$(OBJDIR)\url.obj \
$(OBJDIR)\url_odin.obj \
$(OBJDIR)\reg_odin.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\regstream.obj \
$(OBJDIR)\pathcpp.obj \
$(OBJDIR)\path.obj \
$(OBJDIR)\string_odin.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\shlwapirsrc.obj \
$(OBJDIR)\ordinal.obj \
$(OBJDIR)\ordinal_odin.obj 

#
# Target name - name of the library without extention and path.
#
TARGET  = shlwapio


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

