# $Id: setup.optional.alp.mk,v 1.3 2002-08-27 04:21:00 bird Exp $

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

_AS_DEBUG_TYPE = +Ods:IBM32
!ifdef AS_DEBUG_TYPE
! if "$(AS_DEBUG_TYPE)" == "Codeview"
_AS_DEBUG_TYPE = +Ods:MS16
! else
!  if "$(AS_DEBUG_TYPE)" != "HLL"
!   if [$(ECHO) $(CLRERR)Error: Bad AS_DEBUG_TYPE ($(AS_DEBUG_TYPE)). Valid types are Codeview and HLL.$(CLRRST)]
!   endif
!   error
!  endif
! endif
!endif

_AS_OPTIONAL = $(_AS_MODE) $(_AS_DEBUG_TYPE)
