# $Id: common.mk,v 1.7 2002-02-23 23:27:18 bird Exp $
#
# Common definitions for tools.
#
# Requires that ODIN32_TCOMMON is defined to point to the
# /tools/common directory
#
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# We use exception, so we'll have to enable them.
#
!ifndef WMAKE
CXXFLAGS = $(CXXFLAGS) $(CXXEXCEPTIONS)
!else
CXXFLAGS += $(CXXEXCEPTIONS)
!endif


#
# Make a suggestion of which common.<a|lib> library to use.
# Not defined OMF indicates VAC or Watcom.
# OMF=0 indicates GCC+aout
# OMF=1 indicates GCC+OMF
#
!ifndef GCC
COMMONLIB = $(ODIN32_TCOMMON)\$(OBJDIR)\common.lib
!else
    !if $(OMF) == 0
COMMONLIB = $(ODIN32_TCOMMON)\$(OBJDIR)\common.a
    !else
COMMONLIB = $(ODIN32_TCOMMON)\$(OBJDIR)\common.lib
    !endif
!endif


#
# Add ODIN32_TCOMMON to the C and C++ includes.
#
CINCLUDES = -I$(ODIN32_TCOMMON) $(CINCLUDES)


#
# No internal libs cause we define our own internal libs.
#
NO_INTERNAL_LIBS = 1
INTLIBS = common_lib


#
# Macro for the post include.
#
ODIN32_TCOMMON_POST_INC = $(ODIN32_TCOMMON)/common.post.mk
