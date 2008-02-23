# $Id: setup.os2debemxomfld.mk,v 1.1 2004-03-08 19:56:56 bird Exp $

#
# Include optional stuff.
#
!include $(PATH_MAKE)\setup.optional.emxomfld.mk

#
# The tool(s)
#
TARGET_IGNORE_LINKER_WARNINGS = 0
LINK            = $(PATH_IGCC)\bin\g++.exe -Zomf

#
# The flags
#
LINK_FLAGS      = -g -Zlinker -linenumbers $(_LD_OPTIONAL)
LINK_FLAGS_EXE  = $(LINK_FLAGS) -Zexe -Zlinker -STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL  = $(LINK_FLAGS) -Zdll
LINK_FLAGS_SYS  = $(LINK_FLAGS) -Zdll -Zlinker -PDD
LINK_FLAGS_VDD  = $(LINK_FLAGS) -Zdll -Zlinker -VDD
LINK_FLAGS_IFS  = $(LINK_FLAGS_DLL)

LINK_CMD_EXE    = $(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL    = $(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS    = $(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD    = $(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS    = $(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)

LINK_LNK1       = $(TARGET_OBJS: =^
)
LINK_LNK2       = -o^
$(TARGET)
LINK_LNK3       = -Zmap=$(TARGET_MAP)
LINK_LNK4       = $(TARGET_LIBS: =^
)
#$(_LD_LAST_LIB)
LINK_LNK5       = $(TARGET_DEF_LINK)

