# $Id: ntdlllib.mak,v 1.1 2001-09-25 18:25:14 sandervl Exp $

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
WRC_PREFIX_RESOURCE=1

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
$(OBJDIR)\ntdll.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\exception.obj \
$(OBJDIR)\nt.obj \
$(OBJDIR)\om.obj \
$(OBJDIR)\reg.obj \
##$(OBJDIR)\rtl.obj \
$(OBJDIR)\rtlstr.obj \
$(OBJDIR)\wcstring.obj \
$(OBJDIR)\sec.obj \
$(OBJDIR)\sync.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\regfunc.obj \
$(OBJDIR)\ntdllrsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = ntdllo


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

