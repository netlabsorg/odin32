# $Id: setup.optional.watcom11x.mk,v 1.2 2002-04-30 19:48:59 bird Exp $

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
!ifdef ENV_16BIT
_CC_MODEL        = -ms
_OBJ_MODEL       = s
_CC_DEF_MODEL    = SMALL
!else
_CC_MODEL        = -mf
_CC_DEF_MODEL    = FLAT
!endif

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
# model stuff.
!if !defined(CC_MODEL) && defined(ALL_MODEL)
CC_MODEL    = $(ALL_MODEL)
!endif
!ifdef CC_MODEL
_CC_MODEL   =
!endif
! if "$(CC_MODEL)" == "TINY"
_CC_MODEL   = -ms
_OBJ_MODEL  = s
_CC_DEF_MODEL= SMALL
! error CC_MODEL=TINY is not supported by this compiler ($(ENV_NAME)).
! endif
!if "$(CC_MODEL)" == "SMALL"
_CC_MODEL   = -ms
_OBJ_MODEL  = s
_CC_DEF_MODEL= SMALL
!endif
!if "$(CC_MODEL)" == "COMPACT"
_CC_MODEL   = -mc
_OBJ_MODEL  = c
_CC_DEF_MODEL= COMPACT
!endif
!if "$(CC_MODEL)" == "MEDIUM"
_CC_MODEL   = -mm
_OBJ_MODEL  = m
_CC_DEF_MODEL= MEDIUM
!endif
!if "$(CC_MODEL)" == "LARGE"
_CC_MODEL   = -ml
_OBJ_MODEL  = l
_CC_DEF_MODEL= LARGE
!endif
!if "$(CC_MODEL)" == "HUGE"
! ifdef ENV_16BIT
_CC_MODEL   = -mh
_OBJ_MODEL  = h
_CC_DEF_MODEL= HUGH
! else
_CC_MODEL   = -ml
_OBJ_MODEL  = l
_CC_DEF_MODEL= LARGE
! endif
!endif
!if "$(CC_MODEL)" == "FLAT"
! ifdef ENV_16BIT
!  error CC_MODEL=FLAT is not supported by the 16-bit compiler. ($(ENV_NAME))
! else
_CC_MODEL   = -mf
_OBJ_MODEL  = f
_CC_DEF_MODEL= LARGE
! endif
!endif
!if "$(_CC_MODEL)" == ""
! error Invalid MODEL. CC_MODEL=$(CC_MODEL)
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL) -d$(_CC_DEF_MODEL)



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
!ifdef ENV_16BIT
_CXX_MODEL        = -ms
_OBJ_MODEL        = s
_CXX_DEF_MODEL    = SMALL
!else
_CXX_MODEL        = -mf
_CXX_DEF_MODEL    = FLAT
!endif

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
# Model
!if !defined(CXX_MODEL) && defined(ALL_MODEL)
CXX_MODEL    = $(ALL_MODEL)
!endif
!ifdef CXX_MODEL
_CXX_MODEL   =
!endif
! if "$(CXX_MODEL)" == "TINY"
_CXX_MODEL   = -ms
_OBJ_MODEL   = s
_CXX_DEF_MODEL= SMALL
! error CXX_MODEL=TINY is not supported by this compiler ($(ENV_NAME)).
! endif
!if "$(CXX_MODEL)" == "SMALL"
_CXX_MODEL   = -ms
_OBJ_MODEL   = s
_CXX_DEF_MODEL= SMALL
!endif
!if "$(CXX_MODEL)" == "COMPACT"
_CXX_MODEL   = -mc
_OBJ_MODEL   = c
_CXX_DEF_MODEL= COMPACT
!endif
!if "$(CXX_MODEL)" == "MEDIUM"
_CXX_MODEL   = -mm
_OBJ_MODEL   = m
_CXX_DEF_MODEL= MEDIUM
!endif
!if "$(CXX_MODEL)" == "LARGE"
_CXX_MODEL   = -ml
_OBJ_MODEL   = l
_CXX_DEF_MODEL= LARGE
!endif
!if "$(CXX_MODEL)" == "HUGE"
! ifdef ENV_16BIT
_CXX_MODEL   = -mh
_OBJ_MODEL   = h
_CXX_DEF_MODEL= HUGH
! else
_CXX_MODEL   = -ml
_OBJ_MODEL   = l
_CXX_DEF_MODEL= LARGE
! endif
!endif
!if "$(CXX_MODEL)" == "FLAT"
! ifdef ENV_16BIT
!  error CXX_MODEL=FLAT is not supported by the 16-bit compiler. ($(ENV_NAME))
! else
_CXX_MODEL   = -mf
_OBJ_MODEL   = f
_CXX_DEF_MODEL= LARGE
! endif
!endif
!if "$(_CXX_MODEL)" == ""
! error Invalid MODEL. CXX_MODEL=$(CXX_MODEL)
!endif

_CXX_OPTIONAL = $(_CXX_SEG_TEXT) $(_CXX_SEG_DATA) $(_CXX_SEG_XCPT) $(_CXX_DEFAULT_LIBS) $(_CXX_PACK) $(_CXX_XCPT) $(_CXX_MODEL) -d$(_CXX_DEF_MODEL)


