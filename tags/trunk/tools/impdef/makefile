# $Id: makefile,v 1.8 2000-11-21 04:37:53 bird Exp $

#
# PD-Win32 API
#
# Makefile for the ImpDef utility.
#
#


#
# Directory macros.
#
PDWIN32_BIN     = ..\bin
PDWIN32_TCOMMON = ..\common


#
# Tell buildenvironment that we're making an vio .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk
!include $(PDWIN32_TCOMMON)\common.mk


#
# Flag overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS   = $(CFLAGS)   -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Wall+ppt-ppc-inl-cnv-gnr-vft- $(CXXEXCEPTIONS)
!else
!if "$(WAT)" == "1"
CXXFLAGS += $(CXXEXCEPTIONS)
!else
CXXFLAGS = $(CXXFLAGS) $(CXXEXCEPTIONS)
!endif
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

