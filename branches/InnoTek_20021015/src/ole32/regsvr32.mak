# $Id: regsvr32.mak,v 1.2 2000-12-02 23:40:43 bird Exp $

#
# Odin32 API
#
#       regsvr32.exe makefile
#

#
# Tell buildenvironment that we're makeing an executable,
# have a .def files and a non-default makefilename.
#
EXETARGET       = 1
DEFFILE         = $(TARGET).def
MAKEFILE        = regsvr32.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\regsvr32.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the exe without extention and path.
#
TARGET  = regsvr32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

