# $Id: kDef2Wat.mak,v 1.2 2000-12-02 23:26:58 bird Exp $

#
# Odin32 API
#
#       kDef2Wat.exe makefile
#


#
# Macro which defines the tools common directory.
#
ODIN32_TCOMMON = .
ODIN32_BIN     = $(ODIN32_TOOLS)


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
MAKEFILE = kDef2Wat.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc
!include $(ODIN32_TCOMMON)/common.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kDef2Wat.obj


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
TARGET  = kDef2Wat


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

