# $Id: setup.optional.ilink.mk,v 1.3 2002-09-14 23:31:27 bird Exp $
MAKE_INCLUDE_CX_OPT = $(PATH_MAKE)\setup.optional.ilink.mk

#
# Helper file for all the option stuff for the ibm linker (ilink).
#
_LD_ALIGN       =
_LD_BASE        =
_LD_DEFAULT_LIBS= /NoD
_LD_EXTDIC      = /NoE
_LD_PACKCODE    = /Packcode
_LD_PACKDATA    = /Packdata
_LD_DBGPACK     = /NoDbgPack
_LD_IGNORECASE  = /NoI
_LD_OLDCPP      = /NoOldCPP
_LD_OPTFUNC     = /NoOptfunc

!if defined(LD_ALIGN)
_LD_ALIGN       = /ALIGNMENT:$(LD_ALIGN)
!endif
!if defined(LD_NO_BASE)
_LD_BASE        = /NoBase
!endif
!if defined(LD_BASE)
_LD_BASE        = /Base:$(LD_BASE)
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
_LD_PACKCODE    = /Packcode
!endif
!if defined(LD_NO_PACKDATA)
_LD_PACKDATA    = /NoPackdata
!endif
!if defined(LD_PACKDATA)
_LD_PACKDATA    = /Packdata
!endif
!if defined(LD_NO_DBGPACK)
_LD_DBGPACK     = /NoPackcode
!endif
!if defined(LD_DBGPACK)
_LD_DBGPACK     = /Packcode
!endif
!if defined(LD_IGNORECASE) || defined(ALL_IGNORECASE)
_LD_IGNORECASE  = /Ig
!endif
!if defined(LD_NO_IGNORECASE) || defined(ALL_NO_IGNORECASE)
_LD_IGNORECASE  = /NoI
!endif
!if defined(LD_OLDCPP)
_LD_OLDCPP      = /OldCPP
!endif
!if defined(LD_NO_OLDCPP)
_LD_OLDCPP      = /NoOldCPP
!endif
!if defined(LD_OPTFUNC)
_LD_OPTFUNC     = /Optfunc
!endif
!if defined(LD_NO_OPTFUNC)
_LD_OPTFUNC     = /NoOptfunc
!endif

_LD_OPTIONAL = $(_LD_ALIGN) $(_LD_BASE) $(_LD_DEFAULT_LIBS) $(_LD_EXTDIC) $(_LD_PACKCODE) $(_LD_PACKDATA) $(_LD_DBGPACK) $(_LD_IGNORECASE) $(_LD_OLDCPP) $(_LD_OPTFUNC)

#!if !$(BUILD_FORWARDING)
#! ifdef LD_FORMAT
#!  if "$(LD_FORMAT)" != "LX"
#!   error LD_FORMAT other than LX is not supported by this linker ($(ENV_NAME)).
#!  endif
#! endif
#!endif


# unsupported option checks.
!ifdef LD_NO_DOSSEG
! error LD_NO_DOSSEG is not supported by this linker ($(ENV_NAME)).
!endif
!ifdef LD_DOSSEG
! error LD_DOSSEG is not supported by this linker ($(ENV_NAME)).
!endif
!ifdef LD_SORT_BOTH
! error LD_SORT_BOTH is not supported by this linker ($(ENV_NAME)).
!endif
!ifdef LD_SORT_GLOBAL
! error LD_SORT_GLOBAL is not supported by this linker ($(ENV_NAME)).
!endif
!ifdef LD_SORT_ALPHABETICAL
! error LD_SORT_ALPHABETICAL is not supported by this linker ($(ENV_NAME)).
!endif


# assertions
!if defined(LD_DOSSEG_NO) || defined(LD_DOSSEG_YES)
! error LD_DOSSEG_NO/LD_DOSSEG_YES is renamed to LD_NO_DOSSEG / LD_DOSSEG!
!endif

