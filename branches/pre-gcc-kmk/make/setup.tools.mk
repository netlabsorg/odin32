# $Id: setup.tools.mk,v 1.19 2003-09-14 18:53:03 bird Exp $


#
# Colorful output.
# Define BUILD_NOCOLORS if you don't like it.. :-)
#
!ifdef SLKRUNS
BUILD_NOCOLORS = 1
!endif
!ifndef BUILD_NOCOLORS
CLRTXT=[32;1m
CLRERR=[31;1m
CLRFIL=[33;1m
CLRMAK=[35;1m
CLRRST=[0m
!else
CLRTXT=
CLRERR=
CLRFIL=
CLRMAK=
CLRRST=
!endif


#
# Standard tools - may be overridden by compiler specific setupfiles.
#
TOOL_ECHO       = echo
TOOL_ECHOTXT    = echo $(CLRTXT)
!ifdef MAKEVER
ECHO            = %echo $(CLRTXT)
!else
ECHO            = $(TOOL_ECHOTXT)
!endif
TOOL_BLDLEVEL   = $(PATH_TOOLS)\BldLevelInf.cmd
TOOL_BUILDENV   = $(PATH_TOOLS)\BuildEnv.cmd
TOOL_CMP        = $(PATH_TOOLS)\cmp.exe
TOOL_CMDQD      = $(PATH_TOOLS)\cmdqd.exe
!if "$(COMSPEC:4OS2.EXE=x)" == "$(COMSPEC)"
TOOL_COPY       = copy
!else
TOOL_COPY       = copy /Q
!endif
TOOL_CREATEPATH = $(PATH_TOOLS)\CreatePath.cmd
TOOL_DEP        = $(PATH_TOOLS)\fastdep.exe
TOOL_DEP_FILES  = *.c *.cpp *.asm *.h *.rc *.dlg *.inc -r$(EXT_OBJ) *.orc *.ipf -obr- *.ipp
TOOL_DEP_FLAGS  = $(CC_INCLUDES) $(CXX_INCLUDES) $(AS_INCLUDES) $(ALL_INCLUDES) -i$(PATH_INCLUDES:;= -i)
TOOL_DLLRNAME   = dllrname /q
TOOL_DODIRS     = $(PATH_TOOLS)\dodirs.cmd
TOOL_DOMAKES    = $(PATH_TOOLS)\domakes.cmd
TOOL_DOWITHDIRS = $(PATH_TOOLS)\dowithdirs.cmd
TOOL_EXISTS     = $(PATH_TOOLS)\Exists.cmd
TOOL_IDEFCONV   = $(PATH_TOOLS)\ImpDef.exe
!if "$(BUILD_MULTIJOBS)" != ""
TOOL_JOB_SUB    = $(TOOL_CMDQD) submit
TOOL_JOB_WAIT   = $(TOOL_CMDQD) wait
TOOL_JOB_UP     = $(TOOL_CMDQD) queryrunning
TOOL_JOB_INIT   = $(TOOL_CMDQD) init
TOOL_JOB_WORKERS= 5
TOOL_JOB_SUB_MSG= (submitting job)
!endif
TOOL_MAKE       = $(MAKE:.exe=).exe -nologo
TOOL_MAPSYM     = $(PATH_TOOLS)\MapSym.cmd $(TOOL_MAPSYM_TYPE)
TOOL_RM         = $(PATH_TOOLS)\rm.exe -fF
TOOL_RMTREE     = $(PATH_TOOLS)\rm.exe -fFR
TOOL_RMDIR      = rmdir
TOOL_STRIP      = $(PATH_TOOLS)\lxlite.cmd
TOOL_TYPE       = type


#
# Check for all mode and modify some tool flags
#
!ifdef MAKEFLAGS
!if "$(MAKEVER)" != "5.0"
!if "$(MAKEFLAGS:A=z)" != "$(MAKEFLAGS)"
TOOL_DEP_FLAGS = $(TOOL_DEP_FLAGS) -F+
!endif
!else
!if "$(MAKEFLAGS:a=z)" != "$(MAKEFLAGS)"
TOOL_DEP_FLAGS += -F+
!endif
!endif
!endif


#
# Default Extensions.
#
EXT_DLL = dll
EXT_DEF = def
EXT_EXE = exe
EXT_HLP = hlp
EXT_IFS = ifs
EXT_ILIB= lib
EXT_INF = inf
EXT_LIB = lib
EXT_MAK = mak
EXT_MAP = map
EXT_OBJ = obj
EXT_RES = res
EXT_SYM = sym
EXT_SYS = sys
EXT_VDD = sys

