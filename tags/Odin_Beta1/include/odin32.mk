# $Id: odin32.mk,v 1.7 2001-10-01 01:27:54 bird Exp $

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
# Target directories.
# Both bin and lib directories are compiler dependent.
#
!ifndef ODIN32_BIN
!   ifdef DEBUG
!       ifndef PROFILE
ODIN32_BIN  = $(ODIN32_BIN_)\Debug$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Debug$(DIREXT)
!       else
ODIN32_BIN  = $(ODIN32_BIN_)\Profile$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Profile$(DIREXT)
!       endif
!   else
!       ifdef PROFILE
ODIN32_BIN  = $(ODIN32_BIN_)\Profile$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Profile$(DIREXT)
!       else
ODIN32_BIN  = $(ODIN32_BIN_)\Release$(DIREXT)
ODIN32_BIN__= $(ODIN32_BIN_)\Release$(DIREXT)
!       endif
!   endif
!endif

!ifndef ODIN32_LIB
!   ifdef DEBUG
!       ifndef PROFILE
ODIN32_LIB  = $(ODIN32_LIB_)\Debug$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Debug$(DIREXT)
!       else
ODIN32_LIB  = $(ODIN32_LIB_)\Profile$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Profile$(DIREXT)
!       endif
!   else
!       ifdef PROFILE
ODIN32_LIB  = $(ODIN32_LIB_)\Profile$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Profile$(DIREXT)
!       else
ODIN32_LIB  = $(ODIN32_LIB_)\Release$(DIREXT)
ODIN32_LIB__= $(ODIN32_LIB_)\Release$(DIREXT)
!       endif
!   endif
!endif

!ifndef OBJDIR
!   ifdef DEBUG
!       ifndef PROFILE
OBJDIR   = .\bin\Debug$(DIREXT)
!       else
OBJDIR   = .\bin\Profile$(DIREXT)
!       endif
!   else
!       ifdef PROFILE
OBJDIR   = .\bin\Profile$(DIREXT)
!       else
OBJDIR   = .\bin\Release$(DIREXT)
!       endif
!   endif
!endif


#
# Post include macro.
#
ODIN32_POST_INC = $(ODIN32_INCLUDE)/odin32.post.mk


#
# Common rules macro. (All makefiles should have these!)
# (Please don't change order of these rules!)
#
COMMONRULES = clean dep lib all nothing


#
# Altern configuration if we're making the custom build object library.
#
CUST =
!if "$(CUSTOMBUILD)" == "1"
!   ifndef LIBTARGET
!       ifndef PUBLICLIB
CUST        = o
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
# Include compiler environment.
#
!ifndef ONLY_TOOLS
!ifdef DEBUG
!   ifndef PROFILE
!       include $(ODIN32_INCLUDE)/odin32.dbg.$(MKFILE).mk
!   else
!       include $(ODIN32_INCLUDE)/odin32.profile.$(MKFILE).mk
!   endif
!else
!   ifdef PROFILE
!       include $(ODIN32_INCLUDE)/odin32.profile.$(MKFILE).mk
!   else
!       include $(ODIN32_INCLUDE)/odin32.rel.$(MKFILE).mk
!   endif
!endif
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

