# $Id: odin32.tools.mk,v 1.1 2000-12-02 23:50:49 bird Exp $

#
# Odin32 API
#
#       makefile for the build system tools and standard rules
#


#
# Check that CCENV is set; set defaults if not set.
#
!ifndef CCENV
!   ifndef __VERSION__
CCENV = VAC3
VAC3 = 1
!   else
CCENV = WAT
WAT = 1
!   endif
!endif


#
# Check that ODIN32_INCLUDE and ODIN32_TOOLS is set; set default if not set.
#
!ifndef ODIN32_INCLUDE
ODIN32_INCLUDE  = ..\..\include
!endif
!ifndef ODIN32_TOOLS
ODIN32_TOOLS    = ..\..\tools\bin
!endif


#
# System Utilites
#
RM          = rm -f
CP          = copy
MKDIR       = mkdir
ECHO        = echo
EXISTS      = $(ODIN32_TOOLS)\Exists.cmd
DODIRS      = $(ODIN32_TOOLS)\DoDirs.cmd
DOWITHDIRS  = $(ODIN32_TOOLS)\dowithdirs.cmd
IMPDEF      = $(ODIN32_TOOLS)\impdef.exe
DEPEND      = $(ODIN32_TOOLS)\fastdep.exe -o$$(OBJDIR) -cy+
CREATEPATH  = $(ODIN32_TOOLS)\CreatePath.cmd
EXECEXCL    = $(ODIN32_TOOLS)\ExecExcl.cmd


#
# Include compiler dependent tools
#
!include $(ODIN32_INCLUDE)/odin32.tools.$(CCENV).mk

