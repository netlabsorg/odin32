# $Id: makefile,v 1.6 2000-11-19 08:27:21 bird Exp $

#
# PD-Win32 API
#
# Makefile for the Quick-and-Dirty dependency utility. (FastDep)
#
#


#
# Tell buildenvironment that we're makeing an .exe.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
NO_MAIN_BIN_COPY = 1


#
# include common definitions
#
PDWIN32_INCLUDE = ..\..\include
PDWIN32_TOOLS   = ..\bin
PDWIN32_BIN     = ..\bin
PDWIN32_LIB     = ..\..\lib
!include $(PDWIN32_INCLUDE)\pdwin32.mk


#
# Addjust common definitions
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS   = $(CFLAGS) -W3 -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- \
!ifdef DEBUG
        -O+ -Tm-
!endif
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\fastdep.obj \
$(OBJDIR)\avl.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = FastDep


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# We need all.
#
needed: all

