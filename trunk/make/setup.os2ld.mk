# $Id: setup.os2ld.mk,v 1.1 2002-08-29 10:01:43 bird Exp $

#
# LD (link editor) fanout.
#
_LD_INCLUDED = 0
!ifdef LD_USE_WLINK
_LD_INCLUDED = 1
TOOL_MAPSYM_TYPE = WLINK
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)wlink.mk
!endif
!ifdef LD_USE_ILINK
_LD_INCLUDED = 1
TOOL_MAPSYM_TYPE = ILINK
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)ilink.mk
!endif
!ifdef LD_USE_LINK
_LD_INCLUDED = 1
TOOL_MAPSYM_TYPE = LINK
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link.mk
!endif
!ifdef LD_USE_LINK386
_LD_INCLUDED = 1
TOOL_MAPSYM_TYPE = LINK386
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link386.mk
_TOOL_MAPSYM_ =
!endif


# Use default one (for the given environment)
!if !$(_LD_INCLUDED)
! if "$(BUILD_ENV:WAT=xxx)" != "$(BUILD_ENV)" # (watcom check)
TOOL_MAPSYM_TYPE = WLINK
!  include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)wlink.mk
! else
!  if "$(LD_FORMAT)" == "NE"
TOOL_MAPSYM_TYPE = LINK
!   include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link.mk
!  else
TOOL_MAPSYM_TYPE = ILINK
!   include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)ilink.mk
!  endif
! endif
!endif

