# $Id: setup.os2as.mk,v 1.3 2002-09-14 23:35:27 bird Exp $

#
# Assembler fanout.
#
!ifdef AS_USE_MASM5
# Alternate assembler: MASM v5.10
MAKE_INCLUDE_AS_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allmasm510.mk
! include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allmasm510.mk
!else
# Default assembler:   ALP
MAKE_INCLUDE_AS_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)allalp.mk
!endif
!include $(MAKE_INCLUDE_AS_SETUP)

