# $Id: advapi32lib.mak,v 1.1 2001-09-25 18:26:27 sandervl Exp $

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
$(OBJDIR)\advapi32.obj \
$(OBJDIR)\advapi32rsrc.obj \
$(OBJDIR)\lsa.obj \
$(OBJDIR)\security.obj \
$(OBJDIR)\systemfunction.obj \
$(OBJDIR)\service.obj \
$(OBJDIR)\eventlog.obj \
$(OBJDIR)\crypt.obj \

#
# Target name - name of the library without extention and path.
#
TARGET  = advapi32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

