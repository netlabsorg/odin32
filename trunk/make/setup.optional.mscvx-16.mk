# $Id: setup.optional.mscvx-16.mk,v 1.1 2002-08-24 22:33:11 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all mscvx-16 compilers.
#  Concidering the size of this, it really make sense putting it here.
#

#
# C Compiler flags.
#
_CC_SEG_TEXT     =
_CC_SEG_DATA     =
_CC_SEG_XCPT     =
_CC_DEFAULT_LIBS = /Zl
_CC_PACK         = /Zp
_CC_MODEL        = /Asfw
_OBJ_MODEL       = c

!ifdef ALL_SEG_TEXT
_CC_SEG_TEXT=/NT$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_TEXT
_CC_SEG_TEXT=/NT$(CC_SEG_TEXT)
!endif
!ifdef ALL_SEG_DATA
_CC_SEG_DATA=/ND$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_DATA
_CC_SEG_DATA=/ND$(CC_SEG_TEXT)
!endif
!if defined(CC_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CC_DEFAULT_LIBS =
!endif
!ifdef ALL_PACK
_CC_PACK        = /Zp$(ALL_PACK)
!endif
!ifdef CC_PACK
_CC_PACK        = /Zp$(CC_PACK)
!endif
# Model
!if !defined(CC_MODEL) && defined(ALL_MODEL)
CC_MODEL    = $(ALL_MODEL)
!endif
!ifdef CC_MODEL
_CC_MODEL   =
!endif
!if "$(CC_MODEL)" == "TINY"
_CC_MODEL   = /AT
_OBJ_MODEL  = s
!endif
!if "$(CC_MODEL)" == "SMALL"
_CC_MODEL   = /AS
_OBJ_MODEL  = s
!endif
!if "$(CC_MODEL)" == "COMPACT"
_CC_MODEL   = /AC
_OBJ_MODEL  = c
!endif
!if "$(CC_MODEL)" == "MEDIUM"
_CC_MODEL   = /AM
_OBJ_MODEL  = m
!endif
!if "$(CC_MODEL)" == "LARGE"
_CC_MODEL   = /AL
_OBJ_MODEL  = l
!endif
!if "$(CC_MODEL)" == "HUGE"
_CC_MODEL   = /AH
_OBJ_MODEL  = l
!endif
!if "$(_CC_MODEL)" == ""
! error Invalid MODEL. CC_MODEL=$(CC_MODEL)
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL)

