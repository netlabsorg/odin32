# $Id: setup.os2deblink.mk,v 1.1 2002-08-29 10:01:42 bird Exp $

#
# Include optional stuff.
#
!include $(PATH_MAKE)\setup.optional.link.mk

#
# The tool(s)
#
LINK            = link.exe
TOOL_STRIP      =

#
# The flags
#
LINK_FLAGS      = /nologo /map:full /linenumbers /codeview $(_LD_OPTIONAL)
LINK_FLAGS_EXE  = $(LINK_FLAGS) /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL  = $(LINK_FLAGS)
LINK_FLAGS_SYS  = $(LINK_FLAGS)
LINK_FLAGS_VDD  = false
LINK_FLAGS_IFS  = $(LINK_FLAGS_DLL)

LINK_CMD_EXE    = $(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL    = $(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS    = $(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD    = false
LINK_CMD_IFS    = $(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)

LINK_LNK1       = $(TARGET_OBJS: =+^
),
LINK_LNK2       = $(TARGET),
LINK_LNK3       = $(TARGET_MAP),
LINK_LNK4       = $(TARGET_LIBS: =+^
) $(_LD_LAST_LIB),
LINK_LNK5       = $(TARGET_DEF_LINK);

