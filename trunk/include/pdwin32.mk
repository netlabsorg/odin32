# $Id: pdwin32.mk,v 1.9 2000-09-28 03:16:23 bird Exp $
#
# PD-Win32 Project
#
#
#      Create: cdmckill@novice.uwaterloo.ca, January 4th, 1998
#      Modified: phaller@gmx.net, May 27th, 1999
#
#

# defines
#    DEBUG - build a debug version instead of release version
#    CCENV:
#      VAC3  - use IBM VisualAge for C++ 3 compiler environment (default).
#      VAC36 - use IBM VisualAge for C++ 3.6.5 compiler environment.
#      WAT   - use Watcom C/C++ v11.x compiler environment.
#      EMX   - use GNU/EMX compiler environment.
#


# include compiler environment
!ifndef __VERSION__
!   ifndef CCENV
CCENV=VAC3
VAC3=1
!   else
!       if "$(CCENV)" == "VAC36"
CCENV=VAC36
VAC36=1
!       else

!       endif
!   endif
!else
# (wmake and Watcom)
CCENV=WAT
WAT=1
!   if "$(%DEBUG)" != ""
DEBUG=1
!   endif
!endif


# Set default directory paths if not set.
!ifndef PDWIN32_INCLUDE
PDWIN32_INCLUDE = ..\..\include
!endif
!ifndef PDWIN32_BIN
PDWIN32_BIN     = ..\..\$(OBJDIR)
!endif
!ifndef PDWIN32_LIB
PDWIN32_LIB     = ..\..\lib
!endif
!ifndef PDWIN32_TOOLS
PDWIN32_TOOLS   = ..\..\tools\bin
!endif


# Include compiler environment.
!ifdef DEBUG
!include $(PDWIN32_INCLUDE)/pdwin32.$(CCENV).dbg
!else
!include $(PDWIN32_INCLUDE)/pdwin32.$(CCENV).rel
!endif


# Include system tools
!include $(PDWIN32_INCLUDE)/pdwin32.tools

