# $Id: setup.os2allipfc.mk,v 1.3 2003-06-30 13:13:21 bird Exp $

MAKE_INCLUDE_IPFC = $(PATH_MAKE)\setup.os2allipfc.mk

#
# The tool(s)
#
INFC            = ipfc.exe
HLPC            = ipfc.exe


#
# Options.
#
_INFC_COUNTRY   = /COUNTRY=001
_HLPC_COUNTRY   = /COUNTRY=001
_INFC_LANGUAGE  = /LANGUAGE=ENG
_HLPC_LANGUAGE  = /LANGUAGE=ENG
_INFC_CODEPAGE  = /CODEPAGE=0437,0850
_HLPC_CODEPAGE  = /CODEPAGE=0437,0850
_INFC_XFREF     =
_HLPC_XFREF     =

!if !defined(INFC_COUNTRY) && defined(ALL_COUNTRY)
INFC_COUNTRY = $(ALL_COUNTRY)
!endif
!if defined(INFC_COUNTRY)
! if "$(INFC_COUNTRY)" != ""
_INFC_COUNTRY = /COUNTRY=$(INFC_COUNTRY)
! else
_INFC_COUNTRY =
! endif
!endif

!if !defined(HLPC_COUNTRY) && defined(ALL_COUNTRY)
HLPC_COUNTRY = $(ALL_COUNTRY)
!endif
!if defined(HLPC_COUNTRY)
! if "$(HLPC_COUNTRY)" != ""
_HLPC_COUNTRY = /COUNTRY=$(HLPC_COUNTRY)
! else
_HLPC_COUNTRY =
! endif
!endif


!if !defined(INFC_LANGUAGE) && defined(ALL_LANGUAGE)
INFC_LANGUAGE = $(ALL_LANGUAGE)
!endif
!if defined(INFC_LANGUAGE)
! if "$(INFC_LANGUAGE)" != ""
_INFC_LANGUAGE = /LANGUAGE=$(INFC_LANGUAGE)
! else
_INFC_LANGUAGE =
! endif
!endif

!if !defined(HLPC_LANGUAGE) && defined(ALL_LANGUAGE)
HLPC_LANGUAGE = $(ALL_LANGUAGE)
!endif
!if defined(HLPC_LANGUAGE)
! if "$(HLPC_LANGUAGE)" != ""
_HLPC_LANGUAGE = /LANGUAGE=$(HLPC_LANGUAGE)
! else
_HLPC_LANGUAGE =
! endif
!endif


!if !defined(INFC_CODEPAGE) && defined(ALL_CODEPAGE)
INFC_CODEPAGE = $(ALL_CODEPAGE)
!endif
!if defined(INFC_CODEPAGE)
! if "$(INFC_CODEPAGE)" != ""
_INFC_CODEPAGE = /CODEPAGE=$(INFC_CODEPAGE)
! else
_INFC_CODEPAGE =
! endif
!endif

!if !defined(HLPC_CODEPAGE) && defined(ALL_CODEPAGE)
HLPC_CODEPAGE = $(ALL_CODEPAGE)
!endif
!if defined(HLPC_CODEPAGE)
! if "$(HLPC_CODEPAGE)" != ""
_HLPC_CODEPAGE = /CODEPAGE=$(HLPC_CODEPAGE)
! else
_HLPC_CODEPAGE =
! endif
!endif


!ifdef INFC_XFREF
_INFC_XFREF     = /x
!endif
!ifdef HLPC_XFREF
_HLPC_XFREF     = /x
!endif


_INFC_OPTIONS   = $(_INFC_CODEPAGE) $(_INFC_LANGUAGE) $(_INFC_COUNTRY) $(_INFC_XFREF)
_HLPC_OPTIONS   = $(_HLPC_CODEPAGE) $(_HLPC_LANGUAGE) $(_HLPC_COUNTRY) $(_HLPC_XFREF)


#
# The flags & cmdlines.
#
INFC_FLAGS      = /W3 $(_INFC_OPTIONS) /INF
HLPC_FLAGS      = /W3 $(_HLPC_OPTIONS)

INFC_CMD        = $(INFC) $(INFC_FLAGS) $(_SRC) $(_DST)
HLPC_CMD        = $(HLPC) $(HLPC_FLAGS) $(_SRC) $(_DST)

INFC_CMD_PRE    = $(CC) $(CC_FLAGS_EXE) $(CC_PC_2_STRIPPED_STDOUT) $(_SRC) > $(_DST).ipf && $(INFC) $(INFC_FLAGS) $(_DST).ipf $(_DST)
HLPC_CMD_PRE    = $(CC) $(CC_FLAGS_EXE) $(CC_PC_2_STRIPPED_STDOUT) $(_SRC) > $(_DST).ipf && $(HLPC) $(HLPC_FLAGS) $(_DST).ipf $(_DST)


