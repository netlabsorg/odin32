# $Id: common.mk,v 1.3 2000-11-21 04:37:22 bird Exp $
#
# Common definitions for tools.
#
# Requires that PDWIN32_TCOMMON is defined to point to the
# /tools/common directory
#
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Make a suggestion of which common.<a|lib> library to use.
# Not defined OMF indicates VAC or Watcom.
# OMF=0 indicates GCC+aout
# OMF=1 indicates GCC+OMF
#
!ifndef GCC
COMMONLIB = $(PDWIN32_TCOMMON)\$(OBJDIR)\common.lib
!else
    !if $(OMF) == 0
COMMONLIB = $(PDWIN32_TCOMMON)\$(OBJDIR)\common.a
    !else
COMMONLIB = $(PDWIN32_TCOMMON)\$(OBJDIR)\common.lib
    !endif
!endif


#
# Add PDWIN32_TCOMMON to the C and C++ includes.
#
CINCLUDES = -I$(PDWIN32_TCOMMON) $(CINCLUDES)


#
# No internal libs cause we define our own internal libs.
#
NO_INTERNAL_LIBS = 1
INTLIBS = $(COMMONLIB)

