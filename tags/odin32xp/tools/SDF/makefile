# $Id: makefile,v 1.4 2000-12-02 23:26:57 bird Exp $

#
# Odin32 API
#
#       Makefile for the SDF utility.
#
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
# Flag overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS      = $(CFLAGS)        -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS    = $(CXXFLAGS) -Gx- -Wall+ppt-ppc-inl-cnv-gnr-vft-
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\sdf.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(COMMONLIB) \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = sdf


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

