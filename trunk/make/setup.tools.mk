# $Id: setup.tools.mk,v 1.3 2002-04-13 04:40:01 bird Exp $

#
# Standard tools - may be overridden by compiler specific setupfiles.
#
TOOL_RM=rm.exe -f
TOOL_DOWITHDIRS=$(PATH_TOOLS)\dowithdirs.cmd
TOOL_DODIRS=$(PATH_TOOLS)\dodirs.cmd
TOOL_DOMAKES=$(PATH_TOOLS)\domakes.cmd
TOOL_MAKE=$(MAKE) -nologo
TOOL_DEP=$(PATH_TOOLS)\fastdep.exe
TOOL_DEP_FLAGS=$(CC_INCLUDES) $(CXX_INCLUDES) $(AS_INCLUDES) $(ALL_INCLUDES) -i$(PATH_INCLUDES:;= -i)
TOOL_DEP_FILES=*.c *.cpp *.asm *.h *.rc *.dlg *.orc *.inc *.ipf
TOOL_CREATEPATH=$(PATH_TOOLS)\CreatePath.cmd
TOOL_EXISTS=$(PATH_TOOLS)\Exists.cmd
TOOL_CMP=$(PATH_TOOLS)\cmp.exe
!if "$(BUILD_SHELL)" != "4OS2"
TOOL_COPY=copy
!else
TOOL_COPY=copy /Q
!endif
TOOL_DLLRNAME=dllrname /q


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
CLRFIL=[34;1m
CLRRST=[0m
!else
CLRTXT=
CLRERR=
CLRFIL=
CLRRST=
!endif
ECHO=@echo $(CLRTXT)


#
# Default Extensions.
#
EXT_OBJ = obj
EXT_LIB = lib
EXT_ILIB= lib
EXT_EXE = exe
EXT_DLL = dll
EXT_RES = res
EXT_SYS = sys
EXT_IFS = ifs
EXT_RES = res


#
# Depreciated (kso don't like these)
#
RM=$(TOOLS_RM)
TOOLS_DEL=@del      # use TOOLS_RM!
CD=cd
CD_ENTER=cd
CD_LEAVE=cd ..


