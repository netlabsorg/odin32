# $Id: MkCallTab.mak,v 1.10 2002-08-22 02:57:24 bird Exp $

#
# MkCallTab - 16-bit source generator.
#
# Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config
#
!if [SET INCLUDE=]
!endif
BUILD_ENV_FORCE = MSCV6-16
PATH_ROOT  = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include ..\..\makefile.inc

#
# Target config
#
TARGET_NAME     = MkCallTab
TARGET_NEEDED   = 1
TARGET_PUBNAME  = $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
MAKEFILE        = $(TARGET_NAME).mak
CC_INCLUDES     = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES     = -DMKCALLTAB

TARGET_OBJS =\
$(PATH_TARGET)\MkCallTab.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\d16globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\

TARGET_LIBS =\
$(PATH_TOOLKIT)\lib\$(LIB_OS)\
$(PATH_MSC)\lib\$(LIB_C_OBJ)\

#
# Rule config
#
RULES_FORWARD = calltaba.asm ..\kKrnlLibImpLib.$(EXT_DEF) TstFakers.c tst
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)

#
# Rules For Source Generation.
#

# kKrnlLib Import library definition file.
..\kKrnlLibimplib.$(EXT_DEF): $(TARGET_PUBNAME)
    $(TARGET_PUBNAME) deffile > $@

# Generate calltaba.asm
calltaba.asm:           $(TARGET_PUBNAME)
    $(TARGET_PUBNAME) calltab > $@

# Generate TstFakers.c
TstFakers.c:            $(TARGET_PUBNAME)
    $(TARGET_PUBNAME) tstfakers > $@

!endif # !BUILD_FORWARDING

