# $Id: makefile,v 1.1 2003-10-24 14:53:33 sandervl Exp $

#
# Odin32 API
#
#       Makefile for the ImpDef utility.
#
#


#
# Directory macros.
#
ODIN32_TCOMMON = ..\common
ODIN32_BIN     = $(ODIN32_TOOLS)


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
OBJS= \
##$(OBJDIR)\import.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\parser.obj \
##$(OBJDIR)\relay.obj \
##$(OBJDIR)\res32.obj \
$(OBJDIR)\spec32.obj \
$(OBJDIR)\utils.obj


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
TARGET = PEBuild


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

