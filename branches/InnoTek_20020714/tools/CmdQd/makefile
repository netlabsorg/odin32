# $Id: makefile,v 1.8 2002-04-11 21:28:27 bird Exp $

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
# Tell buildenvironment that we no public cleaning - we do that our self.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1
NO_MULTIJOBS = 1
CLEAN2 = 1
LOCALCLEAN = 1


#
# include common definitions
#
!include ../../makefile.inc


#
# Addjust common definitions
#
OBJ_PROFILE =
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
$(OBJDIR)\CmdQd.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = CmdQd


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# We need all.
#
needed: all


#
# Extra clean rule to which removes the cmdqd.exe file from /tools/bin
# if it's not currently used.
#
clean2:
!ifndef MULTIJOBS
    -if exist $(ODIN32_TOOLS)\cmdqd.exe $(ODIN32_TOOLS)\cmdqd kill
    $(RM) $(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION) $(ODIN32_BIN)\$(TARGET).sym
!else
    @echo Info: Not removing $(ODIN32_TOOLS)\cmdqd.exe because MULTIJOBS is defined.
!endif


#
# If MULTIJOBS is defined make sure cmdqd is NOT while we make it running.
#
!ifdef MULTIJOBS
!   if [$(CMDQD) queryrunning] == 0
!       if [$(CMDQD) kill] == 0
!       endif
!   endif
!endif

