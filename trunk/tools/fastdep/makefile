# $Id: makefile,v 1.7 2000-11-21 04:35:37 bird Exp $

#
# PD-Win32 API
#
# Makefile for the Quick-and-Dirty dependency utility. (FastDep)
#
#


#
# Directory macro.
#
PDWIN32_BIN     = ..\bin


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
!include ../../include/pdwin32.mk


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

