# $Id: setup.os2ar.mk,v 1.2 2002-09-14 23:35:27 bird Exp $

#
# Archiver(/lib manager) fanout.
#
MAKE_INCLUDE_AR_SETUP =
!ifdef AR_USE_WLIB
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allwlib.mk
MAKE_INCLUDE_AR_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allwlib.mk
!endif
!ifdef AR_USE_ILIB
MAKE_INCLUDE_AR_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allilib.mk
!endif
!ifdef AR_USE_LIB
MAKE_INCLUDE_AR_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)alllib.mk
!endif

# Use default one (for the given environment)
!if "$(MAKE_INCLUDE_AR_SETUP)" == ""
! if "$(BUILD_ENV:WAT=xxx)" != "$(BUILD_ENV)" # (watcom check)
MAKE_INCLUDE_AR_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allwlib.mk
! else
MAKE_INCLUDE_AR_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allilib.mk
! endif
!endif

!include $(MAKE_INCLUDE_AR_SETUP)

