# $Id: setup.os2relilink.mk,v 1.1 2002-08-28 03:42:45 bird Exp $

#
# Include optional stuff.
#
!include $(PATH_MAKE)\setup.optional.ilink.mk

#
# The tool(s)
#
LINK            =   ilink.exe

#
# The flags
#
LINK_FLAGS      = /nofree /nologo /de /map /linenumbers /EXEPACK:2 $(_LD_OPTIONAL)
!if "$(_LD_BASE)" == ""
LINK_FLAGS_EXE  = $(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE) /BASE:0x10000
!else
LINK_FLAGS_EXE  = $(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
!endif
LINK_FLAGS_DLL  = $(LINK_FLAGS) /DLL
LINK_FLAGS_SYS  = $(LINK_FLAGS) /PDD
LINK_FLAGS_VDD  = $(LINK_FLAGS) /VDD
LINK_FLAGS_IFS  = $(LINK_FLAGS_DLL)

LINK_CMD_EXE    = $(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL    = $(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS    = $(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD    = $(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS    = $(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)

LINK_LNK1       = $(TARGET_OBJS: =+^
),
LINK_LNK2       = $(TARGET),
LINK_LNK3       = $(TARGET_MAP),
LINK_LNK4       = $(TARGET_LIBS: =+^
) $(_LD_LAST_LIB),
LINK_LNK5       = $(TARGET_DEF_LINK)

