# $Id: makefile,v 1.4 2000-12-02 23:27:02 bird Exp $

#
# Odin32 API
#
#       Very simple OMF/LIB dumper.
#

#
# Directory macros.
#
ODIN32_TCOMMON = ..\common
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're makeing an .exe.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc
!include $(ODIN32_TCOMMON)\common.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\omfdumper.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = omfdumper


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

