# $Id: setup.optional.watcom11x.mk,v 1.7 2002-08-29 12:06:17 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all watcom11xx compilers.
#  Concidering the size of this, it really make sense putting it here.
#


#
# C Compiler flags.
#
_CC_SEG_TEXT    =
_CC_SEG_DATA    =
_CC_SEG_XCPT    =
_CC_SEG_TEXT_CLASS=
_CC_DEFAULT_LIBS= -zl
_CC_PACK        =
!ifdef ENV_16BIT
_CC_MODEL       = -ms
_OBJ_MODEL      = s
_CC_DEF_MODEL   = SMALL
!else
_CC_MODEL       = -mf
_CC_DEF_MODEL   = FLAT
!endif
_CC_OPT_R       =
_CC_O_INLINE_USER =
_CC_O_FRAMES=
_CC_O_O2    =
_CC_O_INLINE=
_CC_O_SIZE  =
_CC_O_TIME  =
_CC_O_DISABLE =
_CC_O_X     =
!ifdef ENV_16BIT
_CC_O_CPU   = -5
!else
_CC_O_CPU   = -5r
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
!ifdef ALL_SEG_TEXT_CLASS
_CC_SEG_TEXT_CLASS =-nc=$(ALL_SEG_TEXT_CLASS)
!endif
!ifdef CC_SEG_TEXT_CLASS
_CC_SEG_TEXT_CLASS =-nc=$(CC_SEG_TEXT_CLASS)
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

# watcom options.
!if defined(CC_SAVE_SEGS_ACCROSS_CALLS) || defined(ALL_SAVE_SEGS_ACCROSS_CALLS)
_CC_OPT_R = -r
!endif

# watcom optimizations.
!if !defined(ALL_NO_O) && !defined(CC_NO_O)
! if defined(ALL_O_INLINE_USER)
_CC_O_INLINE_USER = -e=$(ALL_O_INLINE_USER)
! endif
! if defined(CC_O_INLINE_USER)
_CC_O_INLINE_USER = -e=$(CC_O_INLINE_USER)
! endif
! if defined(CC_O_FRAMES) || defined(ALL_O_FRAMES)
_CC_O_FRAMES=f+
! endif
! if defined(CC_O_O2)     || defined(ALL_O_O2)
_CC_O_O2    =h
! endif
! if defined(CC_O_INLINE) || defined(ALL_O_INLINE)
_CC_O_INLINE=i
! endif
! if defined(CC_O_SIZE)   || defined(ALL_O_SIZE)
_CC_O_SIZE  =s
! endif
! if defined(CC_O_TIME)   || defined(ALL_O_TIME)
_CC_O_TIME  =t
! endif
! if defined(CC_O_X)   || defined(ALL_O_X)
_CC_O_X  =x
! endif
!else
_CC_O_DISABLE=d
!endif

_CC_OPTIM = -o$(_CC_O_DISABLE)$(_CC_O_O2)$(_CC_O_INLINE)$(_CC_O_SIZE)$(_CC_O_TIME)$(_CC_O_X)$(_CC_O_FRAMES)$(_CC_O_INLINE_USER)
!if "$(_CC_OPTIM)" == "-o"
_CC_OPTIM =
!endif

!if defined(ALL_O_CPU)
! ifdef ENV_16BIT
_CC_O_CPU   = -$(ALL_O_CPU)
! else
_CC_O_CPU   = -$(ALL_O_CPU)r
! endif
!endif
!if defined(CC_O_CPU)
! ifdef ENV_16BIT
_CC_O_CPU   = -$(CC_O_CPU)
! else
_CC_O_CPU   = -$(CC_O_CPU)r
! endif
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_SEG_TEXT_CLASS) $(_CC_DEFAULT_LIBS) \
               $(_CC_PACK) $(_CC_MODEL) -d$(_CC_DEF_MODEL) $(_CC_OPT_R) $(_CC_O_CPU) $(_CC_OPTIM)



#
# C++ Compiler flags.
#
_CXX_SEG_TEXT    =
_CXX_SEG_DATA    =
_CXX_SEG_XCPT    =
_CXX_SEG_TEXT_CLASS =
_CXX_DEFAULT_LIBS= -zl
_CXX_PACK        =
_CXX_XCPT        = -xd
_CXX_MODEL       = -mf
!ifdef ENV_16BIT
_CXX_MODEL       = -ms
_OBJ_MODEL       = s
_CXX_DEF_MODEL   = SMALL
!else
_CXX_MODEL       = -mf
_CXX_DEF_MODEL   = FLAT
!endif
_CXX_OPT_R       =
_CXX_O_INLINE_USER =
_CXX_O_FRAMES=
_CXX_O_O2    =
_CXX_O_INLINE=
_CXX_O_SIZE  =
_CXX_O_TIME  =
_CXX_O_DISABLE =
_CXX_O_X     =
_CXX_O_CPU   =
!ifdef ENV_16BIT
_CXX_O_CPU   = -5
!else
_CXX_O_CPU   = -5r
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
!ifdef ALL_SEG_TEXT_CLASS
_CXX_SEG_TEXT_CLASS =-nc=$(ALL_SEG_TEXT_CLASS)
!endif
!ifdef CXX_SEG_TEXT_CLASS
_CXX_SEG_TEXT_CLASS =-nc=$(CXX_SEG_TEXT_CLASS)
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
!if defined(CXX_SAVE_SEGS_ACCROSS_CALLS) || defined(ALL_SAVE_SEGS_ACCROSS_CALLS)
_CXX_OPT_R = -r
!endif


# watcom optimizations.
!if !defined(ALL_NO_O) && !defined(CXX_NO_O)
! if defined(ALL_O_INLINE_USER)
_CXX_O_INLINE_USER = -e=$(ALL_O_INLINE_USER)
! endif
! if defined(CXX_O_INLINE_USER)
_CXX_O_INLINE_USER = -e=$(CXX_O_INLINE_USER)
! endif
! if defined(CXX_O_FRAMES) || defined(ALL_O_FRAMES)
_CXX_O_FRAMES=f+
! endif
! if defined(CXX_O_O2)     || defined(ALL_O_O2)
_CXX_O_O2    =h
! endif
! if defined(CXX_O_INLINE) || defined(ALL_O_INLINE)
_CXX_O_INLINE=i
! endif
! if defined(CXX_O_SIZE)   || defined(ALL_O_SIZE)
_CXX_O_SIZE  =s
! endif
! if defined(CXX_O_TIME)   || defined(ALL_O_TIME)
_CXX_O_TIME  =t
! endif
! if defined(CXX_O_X)   || defined(ALL_O_X)
_CXX_O_X  =x
! endif
!else
_CXX_O_DISABLE=d
!endif

_CXX_OPTIM = -o$(_CXX_O_DISABLE)$(_CXX_O_O2)$(_CXX_O_INLINE)$(_CXX_O_SIZE)$(_CXX_O_TIME)$(_CXX_O_FRAMES)$(_CXX_O_X)$(_CXX_O_INLINE_USER)
!if "$(_CXX_OPTIM)" == "-o"
_CXX_OPTIM =
!endif

!if defined(ALL_O_CPU)
! ifdef ENV_16BIT
_CXX_O_CPU   = -$(ALL_O_CPU)
! else
_CXX_O_CPU   = -$(ALL_O_CPU)r
! endif
!endif
!if defined(CXX_O_CPU)
! ifdef ENV_16BIT
_CXX_O_CPU   = -$(CXX_O_CPU)
! else
_CXX_O_CPU   = -$(CXX_O_CPU)r
! endif
!endif

_CXX_OPTIONAL = $(_CXX_SEG_TEXT) $(_CXX_SEG_DATA) $(_CXX_SEG_XCPT) $(_CXX_SEG_TEXT_CLASS) $(_CXX_DEFAULT_LIBS) \
                $(_CXX_PACK) $(_CXX_XCPT) $(_CXX_MODEL) -d$(_CXX_DEF_MODEL) $(_CXX_OPT_R) $(_CXX_O_CPU) $(_CXX_OPTIM)

