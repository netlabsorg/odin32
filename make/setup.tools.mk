# $Id: setup.tools.mk,v 1.1 2002-04-06 20:25:06 bird Exp $

#
# Standard tools - may be overridden by compiler specific setupfiles.
#
TOOL_RM=rm.exe -f
TOOL_DOWITHDIRS=$(PATH_TOOLS)\dowithdirs.cmd
TOOL_DODIRS=$(PATH_TOOLS)\dodirs.cmd
TOOL_DOMAKES=$(PATH_TOOLS)\domakes.cmd
TOOL_MAKE=$(MAKE) -nologo
TOOL_DEP=$(PATH_TOOLS)\fastdep.exe
TOOL_DEP_FLAGS=-i $(PATH_INCLUDE) $(CC_INCLUDES) $(CXX_INCLUDES) $(AS_INCLUDES) $(ALL_INCLUDES)
TOOL_DEP_FILES=*.c *.cpp *.asm *.h *.rc *.dlg
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
# Depreciated (kso don't like these)
#
RM=$(TOOLS_RM)
TOOLS_DEL=@del      # use TOOLS_RM!
CD=cd
CD_ENTER=cd
CD_LEAVE=cd ..

