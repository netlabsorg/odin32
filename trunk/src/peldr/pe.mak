# $Id: pe.mak,v 1.5 2002-12-30 19:05:38 sandervl Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
CCENV = EMX
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
!if "$(VAC3)" == "1" || "$(VAC36)" == "1" || "$(WAT)" == "1"
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)
!else
CXXFLAGS    = -I$(ODIN32_INCLUDE)\emxruntime $(CXXFLAGS_ODINCRT) -I/compilers/gcc440/local452/include -I/os2tk45/h
LD2FLAGS    = $(LD2FLAGS_ODINCRT) -L/os2tk45/lib
!endif

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
"/os2tk45/lib/os2386.lib"


#
# Target name - name of the dll without extention and path.
#
TARGET  = pe


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

