# $Id: setup.optional.mscvx-16.mk,v 1.2 2002-08-30 21:48:18 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all mscvx-16 compilers.
#  Concidering the size of this, it really make sense putting it here.
#

#
# C Compiler flags.
#
!if [set INCLUDE=]
!endif
_CC_SEG_TEXT    =
_CC_SEG_DATA    =
_CC_SEG_XCPT    =
_CC_DEFAULT_LIBS= /Zl
_CC_PACK        = /Zp
_CC_MODEL       = /Asfw
_OBJ_MODEL      = c
_CC_CPU         = 2
_CC_STACKPROB   = s
_CC_FASTCALL    =
!if "$(BUILD_MODE)" == "RELEASE"
_CC_OPTIM       = /Ogeitln
!else
_CC_OPTIM       = /Owis
!endif
_CC_O_INSTRICT  =# i
_CC_O_SPACE     =# s
_CC_O_TIME      =# t
_CC_O_O2        =# x
_CC_WAR         = /W0

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

!if defined(CC_STACKPROB) || defined(ALL_STACKPROB)
_CC_STACKPROB   =
!endif
!if defined(CC_NO_STACKPROB) || defined(ALL_NO_STACKPROB)
_CC_STACKPROB   = s
!endif

!if defined(CC_FASTCALL) || defined(ALL_FASTCALL)
_CC_FASTCALL    = r
!endif
!if defined(CC_NO_FASTCALL)
_CC_FASTCALL    =
!endif

!if defined(ALL_CPU)
_CC_CPU         = $(ALL_CPU)
!endif
!if defined(CC_CPU)
_CC_CPU         = $(CC_CPU)
!endif

!if !defined(ALL_NO_O) && !defined(CC_NO_O)
! if defined(CC_O_INSTRICT) || defined(ALL_O_INSTRICT)
_CC_O_INSTRICT  = i
! endif
! if defined(CC_O_SIZE)   || defined(ALL_O_SIZE)
_CC_O_SIZE      = s
! endif
! if defined(CC_O_TIME)   || defined(ALL_O_TIME)
_CC_O_TIME      = t
! endif
! if defined(CC_O_O2)     || defined(ALL_O_O2)
_CC_O_O2        = x
! endif
! if "$(_CC_C_INSTRICT)$(_CC_O_SIZE)$(_CC_O_TIME)" != ""
_CC_OPTIM       = /O$(_CC_C_INSTRICT)$(_CC_O_SIZE)$(_CC_O_TIME)
! endif
!else
_CC_OPTIM       = /Od
!endif

!if defined(CC_WALL) || defined(ALL_WALL)
_CC_WAR         =-W4
!endif


_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL) \
               /G$(_CC_FASTCALL)$(_CC_STACKPROB)$(_CC_CPU) $(_CC_OPTIM) $(_CC_WAR)

