# $Id: MkCallTab.mak,v 1.12 2002-12-16 00:46:47 bird Exp $

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
BUILD_ENV_FORCE = VAC308
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config
#
TARGET_NAME     = MkCallTab
TARGET_MODE     = EXE
TARGET_NEEDED   = 1
TARGET_PUB_DIR  = $(PATH_TOOLS)
MAKEFILE        = $(TARGET_NAME).mak
CC_INCLUDES     = -I../include -I../kLib/include
ALL_DEFINES     = -DMKCALLTAB

TARGET_OBJS     =\
$(PATH_TARGET)\MkCallTab.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\

TARGET_LIBS     =\
$(LIB_OS) \
$(LIB_C_OBJ) \

#
# Rule config
#
RULES_FORWARD = calltaba.asm ..\kKrnlLibImpLib.$(EXT_DEF) TstFakers.c
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

