# $Id: common.mk,v 1.1 2000-05-13 17:17:08 bird Exp $
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
# Make a suggestion of which common<gcc|icc>.<a|lib> library
# to use.
# not defined OMF indicates ICC
# OMF=0 indicates GCC+aout
# OMF=1 indicates GCC+OMF
#
!ifndef OMF
COMMONLIB = $(PDWIN32_TCOMMON)\commonicc.lib
!else
    !if $(OMF) == 0
COMMONLIB = $(PDWIN32_TCOMMON)\commongcc.a
    !else
COMMONLIB = $(PDWIN32_TCOMMON)\commongcc.lib
    !endif
!endif


