# $Id: makefile,v 1.8 2001-04-14 09:09:23 bird Exp $

#
# Makefile for debug info convertes - experimental.
#
# Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Directory macros.
#
ODIN32_TCOMMON = ..\common
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're making an VIO .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1


#
# include common definitions
#
!include ../../makefile.inc
!include $(ODIN32_TCOMMON)\common.mk


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
OBJS = \
$(OBJDIR)\kHll.obj \
$(OBJDIR)\kHllMain.obj


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
TARGET = kHll


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

