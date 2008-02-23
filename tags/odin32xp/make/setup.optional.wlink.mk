# $Id: setup.optional.wlink.mk,v 1.6 2002-09-14 23:31:28 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all watcom11xx compilers.
#  Concidering the size of this, it really make sense putting it here.
#
MAKE_INCLUDE_LD_OPT = $(PATH_MAKE)\setup.optional.wlink.mk


#
# Linker/defconvert flags.
#
_LD_FORMAT      = $(BUILD_PLATFORM)
_LD_SORT        = Sort global
# Option
_LD_QUIET       =
_LD_DEFAULT_LIBS= ,nodefaultlibs
_LD_ALIGN       =
_LD_DOSSEG      = ,dosseg
_LD_BASE        =
_LD_PACKCODE    =
_LD_PACKDATA    =
_LD_IGNORECASE  = ,caseexact
_LD_CACHE       = ,cache



!ifdef LD_FORMAT
! if "$(LD_FORMAT)" == "NE"
_LD_FORMAT      = $(BUILD_PLATFORM)-16
TOOL_STRIP      =
! else
!  if "$(LD_FORMAT)" == "LX"
_LD_FORMAT      = os2
!  else
!   error error: LD_FORMAT other than NE and LX is not supported by this compiler ($(ENV_NAME)).
!  endif
! endif
!endif

!if defined(LD_SORT_GLOBAL)
_LD_SORT    = Sort global
!endif
!if defined(LD_SORT_ALPHABETICAL)
_LD_SORT    = Sort alphabetical
!endif
!if defined(LD_SORT_BOTH)
_LD_SORT    = Sort global alphabetical
!endif

# Option
!if defined(BUILD_QUIET)
_LD_QUIET   = ,quiet
!endif
!if defined(LD_ALIGN)
_LD_ALIGN   = ,alignment=$(LD_ALIGN)
!endif
!if defined(LD_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_LD_DEFAULT_LIBS =
!endif
!if defined(LD_NO_DOSSEG)
_LD_DOSSEG  =
!endif
!if defined(LD_DOSSEG)
_LD_DOSSEG  = ,dosseg
!endif
!if defined(LD_NO_BASE)
_LD_BASE        =
!endif
!if defined(LD_BASE)
_LD_BASE        = offset=$(LD_BASE)
!endif
!if defined(LD_NO_PACKCODE)
_LD_PACKCODE    =
!endif
!if defined(LD_PACKCODE)
_LD_PACKCODE    = ,packcode=$(LD_PACKCODE)
!endif
!if defined(LD_NO_PACKDATA)
_LD_PACKDATA    =
!endif
!if defined(LD_PACKDATA)
_LD_PACKDATA    = ,packdata=$(LD_PACKDATA)
!endif
!if defined(LD_IGNORECASE) || defined(ALL_IGNORECASE)
_LD_IGNORECASE  = ,nocaseexact
!endif
!if defined(LD_NO_IGNORECASE) || defined(ALL_NO_IGNORECASE)
_LD_IGNORECASE  = ,caseexact
!endif
!if defined(LD_CACHE)
_LD_CACHE   = ,cache
!endif
!if defined(LD_NO_CACHE)
_LD_CACHE   = ,nocache
!endif
!if defined(LD_DEF_CACHE)
_LD_CACHE   =
!endif


_LD_OPTIONAL = $(_LD_SORT) Option verbose $(_LD_QUIET) $(_LD_ALIGN) $(_LD_DOSSEG) $(_LD_BASE) $(_LD_PACKCODE) $(_LD_PACKDATA) $(_LD_CACHE) $(_LD_DEFAULT_LIBS)


# assert on nonsupported options.
!if defined(LD_EXTDIC) || defined(LD_NO_EXTDIC)
! error error: LD_EXTDIC/LD_NO_EXTDIC is not supported by the linker (wlink).
!endif
# don't complain!
#!if defined(LD_OLDCPP) || defined(LD_NO_OLDCPP)
#! error error: LD_OLDCPP/LD_NO_OLDCPP is not supported by the linker (wlink).
#!endif
!if defined(LD_OPTFUNC) || defined(LD_NO_OPTFUNC)
! error error: LD_DBGPACK/LD_NO_DBGPACK is not supported by the linker (wlink).
!endif

# assert on obsolete options.
!if defined(LD_DOSSEG_YES) || defined(LD_DOSSEG_NO)
! error error: LD_DOSSEG_YES/LD_DOSSEG_NO is obosolete. Use LD_DOSSEG and LD_NO_DOSSEG.
!endif

