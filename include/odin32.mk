# $Id: odin32.mk,v 1.12 2005-03-06 10:35:54 sao2l02 Exp $

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
#    PROFILE - build a profile release version instead of release version
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
NMAKE   = 1
!   ifndef CCENV
CCENV   = VAC3
MKFILE  = $(CCENV)
DIREXT  =
VAC3    = 1
!   else
!       if "$(CCENV)" == "VAC36"
CCENV   = VAC36
MKFILE  = $(CCENV)
DIREXT  = .vac36
VAC36   = 1
!       else
!           if "$(CCENV)" == "EMX"
CCENV   = EMX
MKFILE  = $(CCENV)
DIREXT  = .emx
EMXENV  = 1             # Can't use EMX. (SET EMX will show you why)
!           else
!               if "$(CCENV)" == "WAT"
# (nmake and Watcom)
CCENV   = WAT
MKFILE  = WATN
DIREXT  = .wat
WAT     = 1
!               else
# default compiler
CCENV   = VAC3
MKFILE  = $(CCENV)
DIREXT  =
VAC3    = 1
!               endif
!           endif
!       endif
!   endif
!else
# (wmake and Watcom)
WMAKE   = 1
CCENV   = WAT
MKFILE  = $(CCENV)
DIREXT  = .wat
WAT     = 1
!   if "$(%DEBUG)" != ""
DEBUG   = 1
!   endif
!endif

#
# Altern configuration if we're making the custom build object library.
#
CUST =
!if "$(CUSTOMBUILD)" == "1"
!   ifndef LIBTARGET
!       ifndef PUBLICLIB
CUST        = o
DIREXT2     = .cust
LIBTARGET   = 1
PUBLICLIB   = 1
WRC_PREFIX_RESOURCE=1
!       else
CUSTOMBUILD = 0
!       endif
!   else
CUSTOMBUILD = 0
!   endif
!endif


#
# Target directories.
# Both bin and lib directories are compiler dependent.
#
!ifndef BUILD_TYP
!  ifdef PROFILE
BUILD_TYP = profile
BUILD_DIR = Profile
!  else
!    ifdef DT
BUILD_TYP = dt
BUILD_DIR = Dt
!    else
!      ifdef DEBUG
BUILD_TYP = dbg
BUILD_DIR = Debug
!      else
BUILD_TYP = rel
BUILD_DIR = Release
!      endif
!    endif
!  endif
!endif
!ifndef ODIN32_BIN
ODIN32_BIN  = $(ODIN32_BIN_)\$(BUILD_DIR)
ODIN32_BIN__= $(ODIN32_BIN_)\$(BUILD_DIR)
!endif

!ifndef ODIN32_LIB
ODIN32_LIB  = $(ODIN32_LIB_)\$(BUILD_DIR)
ODIN32_LIB__= $(ODIN32_LIB_)\$(BUILD_DIR)
!endif

!ifndef OBJDIR
OBJDIR   = .\bin\$(BUILD_DIR)$(DIREXT)$(DIREXT2)
!endif


#
# Post include macro.
#
ODIN32_POST_INC = $(ODIN32_INCLUDE)/odin32.post.mk

#
# Common rules macro. (All makefiles should have these!)
# (Please don't change order of these rules!)
#
COMMONRULES = cleanall clean cleandlls dep lib all nothing


#
# Include compiler environment.
#
!ifndef ONLY_TOOLS
!include $(ODIN32_INCLUDE)/odin32.$(BUILD_TYP).$(MKFILE).mk
!endif


#
# Compiler environment modifications for custombuild.
#
!if "$(CUSTOMBUILD)" == "1"
DLLENTRY =
!endif


#
# Include system tools
#
!include $(ODIN32_INCLUDE)/odin32.tools.mk

