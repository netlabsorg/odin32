# $Id: odin32.mk,v 1.3 2001-06-25 23:17:53 bird Exp $

#
# Odin32 API
#
#      Create:      cdmckill@novice.uwaterloo.ca, January 4th, 1998
#      Modified:    phaller@gmx.net, May 27th, 1999
#                   knut.stange.osmundsen@mynd.no, 2nd Dec. 2000
#

#
# defines
#    DEBUG - build a debug version instead of release version
#    PROFILE - build a profile debug version instead of release version
#    CCENV:
#      VAC3  - use IBM VisualAge for C++ 3 compiler environment (default).
#      VAC36 - use IBM VisualAge for C++ 3.6.5 compiler environment.
#      WAT   - use Watcom C/C++ v11.x compiler environment.
#      EMX   - use GNU/EMX compiler environment. (don't work!)
#

#
# Determin compiler environment
#
!ifndef __VERSION__
!   ifndef CCENV
CCENV   = VAC3
DIREXT  =
VAC3    = 1
!   else
!       if "$(CCENV)" == "VAC36"
CCENV   = VAC36
DIREXT  = .vac36
VAC36   = 1
!       else
!           if "$(CCENV)" == "EMX"
CCENV   = EMX
DIREXT  = .emx
EMXENV  = 1             # Can't use EMX. (SET EMX will show you why)
!           else
# default compiler
CCENV   = VAC3
DIREXT  =
VAC3    = 1
!           endif
!       endif
!   endif
!else
# (wmake and Watcom)
CCENV   = WAT
DIREXT  = .wat
WAT     = 1
!   if "$(%DEBUG)" != ""
DEBUG   = 1
!   endif
!endif


#
# Target directories.
# Both bin and lib directories are compiler dependent.
#
!ifndef ODIN32_BIN
!  ifdef DEBUG
ODIN32_BIN  = $(ODIN32_BIN_)\Debug$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Debug$(DIREXT)
!  else
!    ifdef PROFILE
ODIN32_BIN  = $(ODIN32_BIN_)\Debug$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Debug$(DIREXT)
!    else
ODIN32_BIN  = $(ODIN32_BIN_)\Release$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Release$(DIREXT)
!    endif
!  endif
!endif

!ifndef ODIN32_LIB
!  ifdef DEBUG
ODIN32_LIB  = $(ODIN32_LIB_)\Debug$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Debug$(DIREXT)
!  else
!    ifdef PROFILE
ODIN32_LIB  = $(ODIN32_LIB_)\Debug$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Debug$(DIREXT)
!    else
ODIN32_LIB  = $(ODIN32_LIB_)\Release$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Release$(DIREXT)
!    endif
!  endif
!endif

!ifndef OBJDIR
!  ifdef DEBUG
OBJDIR   = .\bin\Debug$(DIREXT)
!    else
!    ifdef PROFILE
OBJDIR   = .\bin\Debug$(DIREXT)
!    else
OBJDIR   = .\bin\Release$(DIREXT)
!    endif
!  endif
!endif


#
# Post include macro.
#
ODIN32_POST_INC = $(ODIN32_INCLUDE)/odin32.post.mk


#
# Common rules macro. (All makefiles should have these!)
#
COMMONRULES = clean dep lib all


#
# Include compiler environment.
#
!ifndef ONLY_TOOLS
!ifdef DEBUG
!  include $(ODIN32_INCLUDE)/odin32.dbg.$(CCENV).mk
!else
!  ifdef PROFILE
!    include $(ODIN32_INCLUDE)/odin32.profile.$(CCENV).mk
!  else
!    include $(ODIN32_INCLUDE)/odin32.rel.$(CCENV).mk
!  endif
!endif
!endif


#
# Include system tools
#
!include $(ODIN32_INCLUDE)/odin32.tools.mk

