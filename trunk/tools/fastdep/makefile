# $Id: makefile,v 1.9 2001-03-14 20:15:59 bird Exp $

#
# Odin32 API
#
#   Makefile for the Quick-and-Dirty dependency utility. (FastDep)
#
#


#
# Directory macro.
#
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


#
# Addjust common definitions
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS   = $(CFLAGS) -W3 -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- \
!ifdef DEBUG
#        -O+ -Tm-
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
!include $(ODIN32_POST_INC)


#
# We need all.
#
needed: all


#
# NT version using Watcom C/C++.
#
fastdepnt.exe:
    wcl386 -bt=nt -l=nt -d2 /"option map" -DOS2FAKE=1 -I$(WATCOM)\h\nt fastdep.c avl.c os2fake-win.c /Fe=$@ kernel32.lib
