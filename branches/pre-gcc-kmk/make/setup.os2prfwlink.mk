# $Id: setup.os2prfwlink.mk,v 1.3 2002-08-29 12:06:18 bird Exp $

#
# Include optional stuff.
#
!include $(PATH_MAKE)\setup.optional.wlink.mk

#
# The tool(s)
#
LINK            = wlink.exe
TOOL_DEFCONV    = $(PATH_TOOLS)\kDef2Wat.exe $(_LD_FORMAT)

#
# The flags
#
LINK_FLAGS      = Option eliminate, manglednames, caseexact, verbose $(_LD_OPTIONAL) Debug codeview all
LINK_FLAGS_EXE  = $(LINK_FLAGS)
LINK_FLAGS_DLL  = $(LINK_FLAGS)
!if "$(LD_FORMAT)" == "NE"
LINK_FLAGS_SYS  = $(LINK_FLAGS) segment type code preload segment type data preload
!else
LINK_FLAGS_SYS  = $(LINK_FLAGS) segment type code preload segment type data preload Option internalrelocs, togglerelocs
!endif
LINK_FLAGS_VDD  = $(LINK_FLAGS_SYS)
LINK_FLAGS_IFS  = $(LINK_FLAGS) segment type code preload segment type data preload

LINK_CMD_EXE    = $(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL    = $(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS    = $(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD    = $(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS    = $(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)

LINK_LNK1       = file       $(TARGET_OBJS: =^
file       )
!ifdef _LD_LIBPATH
LINK_LNK2       = libpath    $(_LD_LIBPATH)
!endif
LINK_LNK3       = option map=$(TARGET_MAP)
LINK_LNK4       = library    $(TARGET_LIBS: =^, )
LINK_LNK5       = name       $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)

