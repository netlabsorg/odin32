# $Id: setup.tools.mk,v 1.7 2002-04-30 06:19:13 bird Exp $


#
# Colorful output.
# Define BUILD_NOCOLORS if you don't like it.. :-)
#
!ifdef SLKRUNS
BUILD_NOCOLORS = 1;
!endif
!ifndef BUILD_NOCOLORS
CLRTXT=[32;1m
CLRERR=[31;1m
CLRFIL=[33;1m
CLRRST=[0m
!else
CLRTXT=
CLRERR=
CLRFIL=
CLRRST=
!endif


#
# Standard tools - may be overridden by compiler specific setupfiles.
#
ECHO            = $(TOOL_ECHO)
TOOL_BLDLEVEL   = $(PATH_TOOLS)\BldLevelInf.cmd
TOOL_BUILDENV   = $(PATH_TOOLS)\BuildEnv.cmd
TOOL_CMP        = $(PATH_TOOLS)\cmp.exe
!if "$(BUILD_SHELL)" != "4OS2"
TOOL_COPY       = copy
!else
TOOL_COPY       = copy /Q
!endif
TOOL_CREATEPATH = $(PATH_TOOLS)\CreatePath.cmd
TOOL_DEP        = $(PATH_TOOLS)\fastdep.exe
TOOL_DEP_FILES  = *.c *.cpp *.asm *.h *.rc *.dlg *.orc *.inc *.ipf
TOOL_DEP_FLAGS  = $(CC_INCLUDES) $(CXX_INCLUDES) $(AS_INCLUDES) $(ALL_INCLUDES) -i$(PATH_INCLUDES:;= -i)
TOOL_DLLRNAME   = dllrname /q
TOOL_DODIRS     = $(PATH_TOOLS)\dodirs.cmd
TOOL_DOMAKES    = $(PATH_TOOLS)\domakes.cmd
TOOL_DOWITHDIRS = $(PATH_TOOLS)\dowithdirs.cmd
TOOL_ECHO       = @echo $(CLRTXT)
TOOL_EXISTS     = $(PATH_TOOLS)\Exists.cmd
TOOL_MAKE       = $(MAKE) -nologo
TOOL_MAPSYM     = $(PATH_TOOLS)\MapSym.cmd $(BUILD_ENV)
TOOL_RM         = rm.exe -f
TOOL_TYPE       = type


#
# Default Extensions.
#
EXT_DLL = dll
EXT_EXE = exe
EXT_IFS = ifs
EXT_ILIB= lib
EXT_LIB = lib
EXT_MAP = map
EXT_OBJ = obj
EXT_RES = res
EXT_RES = res
EXT_SYM = sym
EXT_SYS = sys
EXT_VDD = sys


