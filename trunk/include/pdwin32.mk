# $Id: pdwin32.mk,v 1.4 1999-06-10 16:21:50 achimha Exp $
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
#      VAC3 - use IBM VisualAge for C++ 3 compiler environment (default)
#      EMX - use GNU/EMX compiler environment

# include compiler environment
!ifndef CCENV
CCENV=VAC3
!endif

!ifdef DEBUG
!include $(PDWIN32_INCLUDE)/pdwin32.$(CCENV).dbg
!else
!include $(PDWIN32_INCLUDE)/pdwin32.$(CCENV).rel
!endif

# include system tools
!include $(PDWIN32_INCLUDE)/pdwin32.tools

