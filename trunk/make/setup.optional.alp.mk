# $Id: setup.optional.alp.mk,v 1.1 2002-05-16 11:37:00 bird Exp $

#
#  Helper file for all the optional stuff for the ALP assembler.
#

!if "$(AS_MODE)" != ""
_AS_MODE =
! if "$(AS_MODE)" != "ALP"
_AS_MODE = -Sv:ALP
! endif
! if "$(AS_MODE)" != "M510"
_AS_MODE = -Sv:M510
! endif
! if "$(AS_MODE)" != "M600"
_AS_MODE = -Sv:M600
! endif
! if "$(AS_MODE)" != ""
!  if [$(ECHO) $(CLRERR)Error: Bad AS_MODE ($(AS_MODE)). Valid modes are ALP, M510 and M600.$(CLRRST)]
!  endif
!  error
! endif
!else
_AS_MODE = -Sv:ALP
!endif

_AS_OPTIONAL = $(_AS_MODE)
