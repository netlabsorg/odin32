# $Id: makefile,v 1.7 2000-11-20 05:15:31 bird Exp $

#
# PD-Win32 API
#
# Makefile for the ImpDef utility.
#
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\bin
PDWIN32_TOOLS   = ..\bin
PDWIN32_TCOMMON = ..\common


#
# Tell buildenvironment that we're makeing an .exe.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)\pdwin32.mk
!include $(PDWIN32_TCOMMON)\common.mk


#
# Flag overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS   = $(CFLAGS)   -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gx-
!endif
!if "$(WAT)" == "1"
CXXFLAGS += -xs
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\ImpDef.obj


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
TARGET = ImpDef


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)\pdwin32.post
!include $(PDWIN32_TCOMMON)\common.mk.post

