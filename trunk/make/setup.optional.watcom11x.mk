# $Id: setup.optional.watcom11x.mk,v 1.1 2002-04-30 06:11:23 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all watcom11xx compilers.
#  Concidering the size of this, it really make sense putting it here.
#


#
# C Compiler flags.
#
_CC_SEG_TEXT     =
_CC_SEG_DATA     =
_CC_SEG_XCPT     =
_CC_DEFAULT_LIBS = -zl
_CC_PACK         =
_CC_MODEL        = -mf

!ifdef ALL_SEG_TEXT
_CC_SEG_TEXT=-nt=$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_TEXT
_CC_SEG_TEXT=-nt=$(CC_SEG_TEXT)
!endif
!ifdef ALL_SEG_DATA
_CC_SEG_DATA=-nd=$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_DATA
_CC_SEG_DATA=-nd=$(CC_SEG_TEXT)
!endif
!if defined(CC_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CC_DEFAULT_LIBS =
!endif
!ifdef ALL_PACK
_CC_PACK        = -zp=$(ALL_PACK)
!endif
!ifdef CC_PACK
_CC_PACK        = -zp=$(CC_PACK)
!endif
!if !defined(CC_MODEL) && defined(ALL_MODEL)
CC_MODEL    = $(ALL_MODEL)
!endif
!ifdef CC_MODEL
_CC_MODEL   =
!endif
#!if "$(CC_MODEL)" == "TINY"
#_CC_MODEL   = -mt
#!endif
!if "$(CC_MODEL)" == "SMALL"
_CC_MODEL   = -ms
!endif
!if "$(CC_MODEL)" == "COMPACT"
_CC_MODEL   = -mc
!endif
!if "$(CC_MODEL)" == "MEDIUM"
_CC_MODEL   = -mm
!endif
!if "$(CC_MODEL)" == "LARGE"
_CC_MODEL   = -ml
!endif
!if "$(CC_MODEL)" == "HUGE"
_CC_MODEL   = -ml
!endif
!if "$(_CC_MODEL)" == ""
! error Invalid MODEL. CC_MODEL=$(CC_MODEL)
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL)



#
# C++ Compiler flags.
#
_CXX_SEG_TEXT     =
_CXX_SEG_DATA     =
_CXX_SEG_XCPT     =
_CXX_DEFAULT_LIBS = -zl
_CXX_PACK         =
_CXX_XCPT         = -xd
_CXX_MODEL        = -mf

!ifdef ALL_SEG_TEXT
_CXX_SEG_TEXT=-nt=$(ALL_SEG_TEXT)
!endif
!ifdef CXX_SEG_TEXT
_CXX_SEG_TEXT=-nt=$(CXX_SEG_TEXT)
!endif
!ifdef ALL_SEG_DATA
_CXX_SEG_DATA=-nd=$(ALL_SEG_TEXT)
!endif
!ifdef CXX_SEG_DATA
_CXX_SEG_DATA=-nd=$(CXX_SEG_TEXT)
!endif
!ifdef ALL_SEG_XCPT
_CXX_SEG_XCPT=
!endif
!ifdef CXX_SEG_XCPT
_CXX_SEG_XCPT=
!endif
!if defined(CXX_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CXX_DEFAULT_LIBS =
!endif
!ifdef ALL_PACK
_CXX_PACK         = -zp=$(ALL_PACK)
!endif
!ifdef CXX_PACK
_CXX_PACK         = -zp=$(CXX_PACK)
!endif
!ifdef CXX_XCPT
_CXX_XCPT         = -xs
!endif
!ifdef CXX_NO_XCPT
_CXX_XCPT         = -xd
!endif
!if !defined(CC_MODEL) && defined(ALL_MODEL)
CXX_MODEL    = $(ALL_MODEL)
!endif
!ifdef CXX_MODEL
_CXX_MODEL   =
!endif
#!if "$(CXX_MODEL)" == "TINY"
#_CXX_MODEL   = -mt
#!endif
!if "$(CXX_MODEL)" == "SMALL"
_CXX_MODEL   = -ms
!endif
!if "$(CXX_MODEL)" == "COMPACT"
_CXX_MODEL   = -mc
!endif
!if "$(CXX_MODEL)" == "MEDIUM"
_CXX_MODEL   = -mm
!endif
!if "$(CXX_MODEL)" == "LARGE"
_CXX_MODEL   = -ml
!endif
!if "$(CXX_MODEL)" == "HUGE"
_CXX_MODEL   = -ml
!endif
!if "$(_CXX_MODEL)" == ""
! error Invalid MODEL. CXX_MODEL=$(CXX_MODEL)
!endif

_CXX_OPTIONAL = $(_CXX_SEG_TEXT) $(_CXX_SEG_DATA) $(_CXX_SEG_XCPT) $(_CXX_DEFAULT_LIBS) $(_CXX_PACK) $(_CXX_XCPT) $(_CXX_MODEL)


