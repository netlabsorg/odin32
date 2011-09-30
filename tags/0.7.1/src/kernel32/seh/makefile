# $Id: $

#
# Odin32 API
#
#       seh.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
PUBLICLIB = 1

CCENV = EMX


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


CDEFINES   =  -I$(ODIN32_INCLUDE)\emxruntime $(CDEFINES)

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\sehutil.obj


#
# Libraries. One space before the '\'.
#

# -liberty_s -lstdcxx -lExCRuntime -lExDllSupport -lc_alias -lgcc_eh
# -lgcc -lc -lc_dllso -lgcc_eh -lgcc -lc -lsys -los2 -lend

LIBS = 

#
# Target name - name of the dll without extention and path.
#
TARGET = seh

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

