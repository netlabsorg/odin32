# $Id: setup.optional.link.mk,v 1.2 2002-09-14 23:29:13 bird Exp $
MAKE_INCLUDE_LD_OPT = $(PATH_MAKE)\setup.optional.link.mk

#
# Helper file for all the option stuff for the ibm linker (ilink).
#
_LD_ALIGN       =
_LD_DEFAULT_LIBS= /NoD
_LD_EXTDIC      = /NoE
_LD_PACKCODE    = /Packcode
_LD_PACKDATA    = /Packdata
_LD_IGNORECASE  = /NoI
_LD_DOSSEG      =
_LD_FAR         = /Far

!if defined(LD_ALIGN)
_LD_ALIGN       = /ALIGNMENT:$(LD_ALIGN)
!endif
!if defined(LD_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_LD_DEFAULT_LIBS = /Def
!endif
!if defined(LD_EXTDIC)
_LD_EXTDIC      = /Ext
!endif
!if defined(LD_NO_EXTDIC)
_LD_EXTDIC      = /NoE
!endif
!if defined(LD_NO_PACKCODE)
_LD_PACKCODE    = /NoPackcode
!endif
!if defined(LD_PACKCODE)
! if "$(LD_PACKCODE)" == ""
_LD_PACKCODE    = /Packcode
! else
_LD_PACKCODE    = /Packcode:$(LD_PACKCODE)
! endif
!endif
!if defined(LD_NO_PACKDATA)
_LD_PACKDATA    = /NoPackdata
!endif
!if defined(LD_PACKDATA)
! if "$(LD_PACKDATA)" == ""
_LD_PACKDATA    = /Packdata
! else
_LD_PACKDATA    = /Packdata:$(LD_PACKDATA)
! endif
!endif
!if defined(LD_IGNORECASE) || defined(ALL_IGNORECASE)
_LD_IGNORECASE  =
!endif
!if defined(LD_NO_IGNORECASE) || defined(ALL_NO_IGNORECASE)
_LD_IGNORECASE  = /noignorecase
!endif
!ifdef LD_NO_DOSSEG
_LD_DOSSEG      =
!endif
!ifdef LD_DOSSEG
_LD_DOSSEG      = /DOSSEG
!endif
!ifdef LD_NO_FAR
_LD_FAR         =
!endif
!ifdef LD_FAR
_LD_FAR         = /FAR
!endif


_LD_OPTIONAL = $(_LD_DEFAULT_LIBS) $(_LD_EXTDIC) $(_LD_IGNORECASE) $(_LD_DOSSEG) $(_LD_FAR) $(_LD_PACKCODE) $(_LD_PACKDATA) $(_LD_ALIGN)

#!if !$(BUILD_FORWARDING)
#! ifdef LD_FORMAT
#!  if "$(LD_FORMAT)" != "LX"
#!   error LD_FORMAT other than LX is not supported by this linker ($(ENV_NAME)).
#!  endif
#! endif
#!endif


# unsupported option checks.
!ifdef LD_SORT_BOTH
! error LD_SORT_BOTH is not supported by this linker (link).
!endif
!ifdef LD_SORT_GLOBAL
! error LD_SORT_GLOBAL is not supported by this linker (link).
!endif
!ifdef LD_SORT_ALPHABETICAL
! error LD_SORT_ALPHABETICAL is not supported by this linker (link).
!endif
!if defined(LD_NO_DBGPACK) || defined(LD_DBGPACK)
! error LD_NO_DBGPACK/LD_DBGPACK is not supported by this linker (link).
!endif
!if defined(LD_OPTFUNC) || defined(LD_NO_OPTFUNC)
! error LD_NO_OPTFUNC/LD_OPTFUNC is not supported by this linker (link).
!endif
!if defined(LD_NO_BASE) || defined(LD_BASE)
! error LD_BASE/LD_NO_BASE is not supported by this linker (link).
!endif
!if 0  # ignore this for the moment.
!if defined(LD_OLDCPP) || defined(LD_NO_OLDCPP)
! error LD_OLDCPP/LD_NO_OLDCPP is not supported by this linker (link).
!endif
!endif

# assertions
!if defined(LD_DOSSEG_NO) || defined(LD_DOSSEG_YES)
! error LD_DOSSEG_NO/LD_DOSSEG_YES is renamed to LD_NO_DOSSEG / LD_DOSSEG!
!endif

