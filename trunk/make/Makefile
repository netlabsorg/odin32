# $Id: Makefile,v 1.2 2002-09-02 20:23:29 bird Exp $

#
# Stub file in case someone wanna do the testcases
# by including make in the big make process.
#


#
# Load the build setup
#
PATH_ROOT=..
!include $(PATH_ROOT)\make\setup.mak

#
# Target config.
#
TARGET_MODE = EMPTY
TARGET_DIR  = $(PATH_PUB)\make


#
# Load the processing rules
#
RULE_PACKING    = packing
!include $(MAKE_INCLUDE_PROCESS)


!if !$(BUILD_FORWARDING)
#
# Packing rule - makes a release of the makesystem
#
$(RULE_PACKING):
    if not exist $(TARGET_DIR)\make\testcase  $(TOOL_CREATEPATH) $(TARGET_DIR)\make\testcase
    if not exist $(TARGET_DIR)\tools\bin      $(TOOL_CREATEPATH) $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) *.mk               $(TARGET_DIR)\make
    $(TOOL_COPY) *.mak              $(TARGET_DIR)\make
    $(TOOL_COPY) makefile           $(TARGET_DIR)\make
    $(TOOL_COPY) testcase\*.mak     $(TARGET_DIR)\make\testcase
    $(TOOL_COPY) testcase\makefile  $(TARGET_DIR)\make\testcase
    $(TOOL_COPY) testcase\*.c       $(TARGET_DIR)\make\testcase
    $(TOOL_COPY) testcase\*.cpp     $(TARGET_DIR)\make\testcase
    $(TOOL_COPY) testcase\*.def     $(TARGET_DIR)\make\testcase
    $(TOOL_COPY) $(PATH_TOOLS)\BuildEnv.cmd        $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\CmdQd.exe           $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\CreatePath.cmd      $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\DoDirs.cmd          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\DoMakes.cmd         $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\DoWithDirs.cmd      $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\ExecTestcase.cmd    $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\Exists.cmd          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\FastDep.exe         $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\kDef2Wat.exe        $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\lxLite.cfg          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\lxLite.cmd          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\lxLite.exe          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\MapSym.cmd          $(TARGET_DIR)\tools\bin
    $(TOOL_COPY) $(PATH_TOOLS)\rm.exe              $(TARGET_DIR)\tools\bin
!endif
