# $Id: kDump.mak,v 1.1 2000-11-21 04:37:23 bird Exp $

#
# Odin32 API
#
#       kDump.exe makefile
#


#
# Macro which defines the tools common directory.
#
PDWIN32_TCOMMON = .
PDWIN32_BIN = ..\bin


#
# Tell that we're producing an vio executable.
# Tell buildenvironment that we like to use static linked CRT.
# Tell that we're not to be copied to /bin.
# Tell that the makefile has another name.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1
MAKEFILE = kDump.mak


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk
!include $(PDWIN32_TCOMMON)/common.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kDump.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(COMMONLIB) \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = kDump


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post
!include $(PDWIN32_TCOMMON)/common.mk.post

