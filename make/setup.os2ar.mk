# $Id: setup.os2ar.mk,v 1.1 2002-08-29 10:01:41 bird Exp $

#
# Archiver(/lib manager) fanout.
#
_AR_INCLUDED = 0
!ifdef AR_USE_WLIB
_AR_INCLUDED = 1
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allwlib.mk
!endif
!ifdef AR_USE_ILIB
_AR_INCLUDED = 1
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allilib.mk
!endif
!ifdef AR_USE_LIB
_AR_INCLUDED = 1
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)alllib.mk
!endif

# Use default one (for the given environment)
!if !$(_AR_INCLUDED)
! if "$(BUILD_ENV:WAT=xxx)" != "$(BUILD_ENV)" # (watcom check)
!  include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allwlib.mk
! else
!  include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allilib.mk
! endif
!endif

