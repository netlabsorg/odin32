# $Id: setup.optional.igcc.mk,v 1.1 2004-03-08 19:56:43 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all Innotek GCC compilers.
#  Concidering the size of this, it really make sense putting it here.
#

MAKE_INCLUDE_CX_OPT = $(PATH_MAKE)\setup.optional.igcc.mk

#
# C Compiler flags.
#
_CC_SEG_TEXT     =
_CC_SEG_DATA     =
_CC_SEG_XCPT     =
_CC_DEFAULT_LIBS = -nodefaultlibs
_CC_PACK         =
_CC_MODEL        =

!if defined(ALL_SEG_TEXT) || defined(CC_SEG_TEXT) || defined(ALL_SEG_DATA) || defined(CC_SEG_DATA)
! error ALL_SEG_*/CC_SEG_* is not supported by this compiler, ($(ENV_NAME)).
!endif
!if defined(CC_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CC_DEFAULT_LIBS = -defaultlibs
!endif
!ifdef ALL_PACK
CC_PACK         = ALL_PACK
!endif
!ifdef CC_PACK
! if "$(CC_PACK)" == 1
_CC_PACK        = -fpack-struct
! else
! error ALL_PACK/CC_PACK different from 1 is not supported by this compiler, ($(ENV_NAME)).
! endif
!endif
!if defined(CC_MODEL) || defined(ALL_MODEL)
! if [$(ECHO) warning: CC_MODEL/ALL_MODEL isn't supported by this compiler$(CLRRST)]
! endif
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL)



#
# C++ Compiler flags.
#
_CXX_SEG_TEXT     =
_CXX_SEG_DATA     =
_CXX_SEG_XCPT     =
_CXX_DEFAULT_LIBS = -nodefaultlibs
_CXX_PACK         =
_CXX_XCPT         = -fno-exceptions
_CXX_MODEL        =
_CXX_RTTI         = -fno-rtti

!if defined(ALL_SEG_TEXT) || defined(CC_SEG_TEXT) || defined(ALL_SEG_DATA) || defined(CC_SEG_DATA) || defined(ALL_SEG_XCPT) || defined(CXX_SEG_XCPT)
! error ALL_SEG_*/CXX_SEG_* is not supported by this compiler, ($(ENV_NAME)).
!endif
!if defined(CC_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CXX_DEFAULT_LIBS = -defaultlibs
!endif
!ifdef ALL_PACK
CXX_PACK          = ALL_PACK
!endif
!ifdef CXX_PACK
! if "$(CXX_PACK)" == 1
_CXX_PACK         = -fpack-struct
! else
!  error ALL_PACK/CC_PACK different from 1 is not supported by this compiler, ($(ENV_NAME)).
! endif
!endif
!ifdef CXX_XCPT
_CXX_XCPT         = -fexceptions
!endif
!ifdef CXX_NO_XCPT
_CXX_XCPT         = -fno-exceptions
!endif
!if defined(CXX_MODEL) || defined(ALL_MODEL)
! if [$(ECHO) warning: CXX_MODEL/ALL_MODEL isn't supported by this compiler$(CLRRST)]
! endif
!endif

_CXX_OPTIONAL = $(_CXX_SEG_TEXT) $(_CXX_SEG_DATA) $(_CXX_SEG_XCPT) $(_CXX_DEFAULT_LIBS) $(_CXX_PACK) $(_CXX_XCPT) $(_CXX_MODEL)


