# $Id: makefile,v 1.5 2000-12-02 23:27:01 bird Exp $

#
# Odin32 API
#
#       odininst.exe makefile
#


#
# Tell that we're producing an executable.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\odininst.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib \
$(RTLLIB) \
$(ODIN32_LIB)\kernel32.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = odininst


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

