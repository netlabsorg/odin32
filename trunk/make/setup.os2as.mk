# $Id: setup.os2as.mk,v 1.2 2002-08-30 18:27:22 bird Exp $

#
# Assembler fanout.
#
!ifdef AS_USE_MASM5
# Alternate assembler: MASM v5.10
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allmasm510.mk
!else
# Default assembler:   ALP
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allalp.mk
!endif

