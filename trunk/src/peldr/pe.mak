# $Id: pe.mak,v 1.5 2002-12-30 19:05:38 sandervl Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
STACKSIZE = 0x100000
EXETARGET = 1
MAKEFILE = pe.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\pe.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = pe


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

