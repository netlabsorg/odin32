# $Id: setup.os2as.mk,v 1.1 2002-08-29 10:01:41 bird Exp $

#
# Assembler fanout.
#
!ifdef AS_USE_MASM5
# Alternate assembler: MASM v5.10
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)masm510.mk
!else
# Default assembler:   ALP
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)alp.mk
!endif

