# $Id: setup.optional.emxomfld.mk,v 1.1 2004-03-08 19:56:55 bird Exp $
MAKE_INCLUDE_CX_OPT = $(PATH_MAKE)\setup.optional.ilink.mk

#
# Helper file for all the option stuff for the ibm linker (ilink).
#
_LD_ALIGN       =
_LD_BASE        =
_LD_DEFAULT_LIBS= -Zlinker -NoD
_LD_EXTDIC      = -Zlinker -NoE
_LD_PACKCODE    = -Zlinker -Packcode
_LD_PACKDATA    = -Zlinker -Packdata
_LD_DBGPACK     = -Zlinker -NoDbgPack
_LD_IGNORECASE  = -Zlinker -NoI
_LD_OLDCPP      = -Zlinker -NoOldCPP
_LD_OPTFUNC     = -Zlinker -NoOptfunc

!if defined(LD_ALIGN)
_LD_ALIGN       = -Zlinker -ALIGNMENT:$(LD_ALIGN)
!endif
!if defined(LD_NO_BASE)
_LD_BASE        = -Zlinker -NoBase
!endif
!if defined(LD_BASE)
_LD_BASE        = -Zlinker -Base:$(LD_BASE)
!endif
!if defined(LD_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_LD_DEFAULT_LIBS = -Zlinker -Def
!endif
!if defined(LD_EXTDIC)
_LD_EXTDIC      = -Zlinker -Ext
!endif
!if defined(LD_NO_EXTDIC)
_LD_EXTDIC      = -Zlinker -NoE
!endif
!if defined(LD_NO_PACKCODE)
_LD_PACKCODE    = -Zlinker -NoPackcode
!endif
!if defined(LD_PACKCODE)
_LD_PACKCODE    = -Zlinker -Packcode
!endif
!if defined(LD_NO_PACKDATA)
_LD_PACKDATA    = -Zlinker -NoPackdata
!endif
!if defined(LD_PACKDATA)
_LD_PACKDATA    = -Zlinker -Packdata
!endif
!if defined(LD_NO_DBGPACK)
_LD_DBGPACK     = -Zlinker -NoPackcode
!endif
!if defined(LD_DBGPACK)
_LD_DBGPACK     = -Zlinker -Packcode
!endif
!if defined(LD_IGNORECASE) || defined(ALL_IGNORECASE)
_LD_IGNORECASE  = -Zlinker -Ig
!endif
!if defined(LD_NO_IGNORECASE) || defined(ALL_NO_IGNORECASE)
_LD_IGNORECASE  = -Zlinker -NoI
!endif
!if defined(LD_OLDCPP)
_LD_OLDCPP      = -Zlinker -OldCPP
!endif
!if defined(LD_NO_OLDCPP)
_LD_OLDCPP      = -Zlinker -NoOldCPP
!endif
!if defined(LD_OPTFUNC)
_LD_OPTFUNC     = -Zlinker -Optfunc
!endif
!if defined(LD_NO_OPTFUNC)
_LD_OPTFUNC     = -Zlinker -NoOptfunc
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

