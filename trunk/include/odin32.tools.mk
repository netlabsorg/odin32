# $Id: odin32.tools.mk,v 1.11 2003-10-26 01:47:51 bird Exp $

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
DOMAKES     = $(ODIN32_TOOLS)\DoMakes.cmd
IMPDEF      = $(ODIN32_TOOLS)\impdef.exe
CREATEPATH  = $(ODIN32_TOOLS)\CreatePath.cmd
EXECEXCL    = $(ODIN32_TOOLS)\ExecExcl.cmd
BLDLEVELINF = $(ODIN32_TOOLS)\BldLevelInf.cmd
LXLITE      = $(ODIN32_TOOLS)\lxlite.exe
CMDQD       = $(ODIN32_TOOLS)\cmdqd.exe
PEBUILD		= $(ODIN32_TOOLS)\PEBuild.exe
DEPEND      = $(ODIN32_TOOLS)\fastdep.exe -o$$(OBJDIR) -cy+ -xdummy.c
!ifdef MAKEFLAGS
!if "$(MAKEVER)" != "5.0"
!if "$(MAKEFLAGS:A=z)" != "$(MAKEFLAGS)"
DEPEND      = $(ODIN32_TOOLS)\fastdep.exe -o$$(OBJDIR) -cy+ -xdummy.c -F+
!endif
!else
!if "$(MAKEFLAGS:a=z)" != "$(MAKEFLAGS)"
DEPEND      = $(ODIN32_TOOLS)\fastdep.exe -o$$(OBJDIR) -cy+ -xdummy.c -F+
!endif
!endif
!endif

!ifdef MULTIJOBS
!ifndef WAT
!ifndef NO_MULTIJOBS
CMDQD_SUB   = $(CMDQD) submit
CMDQD_WAIT  = $(CMDQD) wait
CMDQD_INIT  = $(CMDQD) kill & $(CMDQD) init $(MULTIJOBS)
!endif
!endif
!endif


#
# Include compiler dependent tools
#
!include $(ODIN32_INCLUDE)/odin32.tools.$(MKFILE).mk
