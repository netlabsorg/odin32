# $Id: pe.mak,v 1.4 2000-12-16 15:40:53 sandervl Exp $

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
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\pe.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
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

