# $Id: MkCallTab.mak,v 1.5 2002-04-07 22:39:45 bird Exp $

#
# MkCallTab - 16-bit source generator.
#
#


#
# Setup.
#
!if [SET INCLUDE=]
!endif
BUILD_ENV  = MSCV6
PATH_ROOT  = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc
PATH_BIN   = $(PATH_TOOLS)



#
# Config.
#
TARGET_MODE = EXE
TARGET_NAME = MkCallTab
MAKEFILE    = $(TARGET_NAME).mak
CC_INCLUDES = -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DMKCALLTAB


#
# Targets.
#
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
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

#
# Install duplicate...
#
$(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT): $(TARGET)
    $(TOOL_COPY) $(TARGET) $(PATH_BIN)


#
# Rules For Source Generation.
#

# kKrnlLib Import library definition file.
kKrnlLib.def: $(PATH_TARGET)\kKrnlLibImpLib.def
..\kKrnlLibImpLib.def:      $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
    $(PATH_TOOLS)\MkCallTab.exe deffile > $@

# Generate calltaba.asm
calltaba.asm:               $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
    $(PATH_TOOLS)\MkCallTab.exe calltab > $@

# Generate TstFakers.c
TstFakers.c:                $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
    $(PATH_TOOLS)\MkCallTab.exe tstfakers > $@


