# $Id: setup.os2ld.mk,v 1.2 2002-09-14 23:35:27 bird Exp $

#
# LD (link editor) fanout.
#
MAKE_INCLUDE_LD_SETUP =
!ifdef LD_USE_WLINK
TOOL_MAPSYM_TYPE        = WLINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)wlink.mk
!endif
!ifdef LD_USE_ILINK
TOOL_MAPSYM_TYPE        = ILINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)ilink.mk
!endif
!ifdef LD_USE_LINK
TOOL_MAPSYM_TYPE        = LINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link.mk
!endif
!ifdef LD_USE_LINK386
TOOL_MAPSYM_TYPE        = LINK386
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link386.mk
!endif

# Use default one (for the given environment)
!if "$(MAKE_INCLUDE_LD_SETUP)" == ""
! if "$(BUILD_ENV:WAT=xxx)" != "$(BUILD_ENV)" # (watcom check)
TOOL_MAPSYM_TYPE        = WLINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)wlink.mk
! else
!  if "$(LD_FORMAT)" == "NE"
TOOL_MAPSYM_TYPE        = LINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)link.mk
!  else
TOOL_MAPSYM_TYPE        = ILINK
MAKE_INCLUDE_LD_SETUP   = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)ilink.mk
!  endif
! endif
!endif

!include $(MAKE_INCLUDE_LD_SETUP)

