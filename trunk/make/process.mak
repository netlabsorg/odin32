# $Id: process.mak,v 1.17 2002-08-20 04:05:40 bird Exp $

#
# Unix-like tools for OS/2
#
#   The common build process rules
#
# Note: this makefile is supposed to be included from the
# current source path.
#


# -----------------------------------------------------------------------------
# Assertions.
#   This makefile expects setup.mak and the specific setup to be included
#   already.
#   It also requires the TARGET_NAME to be specified in the makefile.
# -----------------------------------------------------------------------------
!if "$(MAKE_SETUP_INCLUDED)"  != "YES"
! ifndef MAKEVER
!  if [$(ECHO) $(CLRERR)Fatal error: You must include setup.mak before process.mak in the makefile.$(CLRRST)]
!  endif
!  error
!else
!  error $(CLRERR)Fatal error: You must include setup.mak before process.mak in the makefile.$(CLRRST)
!endif
!endif
!if "$(ENV_STATUS)" != "OK"
! ifndef MAKEVER
!  if [$(ECHO) $(CLRERR)Fatal error: The environment is not valid. Bad setup.mak?$(CLRRST)]
!  endif
!  error
! else
!  error $(CLRERR)Fatal error: The environment is not valid. Bad setup.mak?$(CLRRST)
! endif
!endif

!if "$(TARGET_NAME)" == "" && "$(TARGET_MODE)" != "EMPTY"
! ifndef MAKEVER
!  if [$(ECHO) $(CLRERR)Fatal error: TARGET_NAME is not defined! Should be set in the makefile.$(CLRRST)]
!  endif
!  error
! else
!  error $(CLRERR)Fatal error: TARGET_NAME is not defined! Should be set in the makefile.$(CLRRST)
! endif
!endif

!ifdef TARGET_MODE
# Executable target mode.
! if "$(TARGET_MODE)" != "EXE"
# Dynamic Load Library target mode.
!  if "$(TARGET_MODE)" != "DLL"
# Drive (/ system software) target mode.
!   if "$(TARGET_MODE)" != "SYS"
# Installable File System Drive target mode. (Also called FSD, File System Driver.)
!    if "$(TARGET_MODE)" != "IFS"
# Virtual Device Driver target mode.
!     if "$(TARGET_MODE)" != "VDD"
# Object Library target mode.
!      if "$(TARGET_MODE)" != "LIB"
# Object Library target mode - Special variant which is to be linked with a SYS target.
!       if "$(TARGET_MODE)" != "SYSLIB"
# Object Library target mode - Special variant which is to be linked with an IFS target.
!        if "$(TARGET_MODE)" != "IFSLIB"
# Dummy/Hub/TopLevel empty makefile. This has no target.
!         if "$(TARGET_MODE)" != "EMPTY"
# Dependency only makefile. (typical for include directories)
!          if "$(TARGET_MODE)" != "DEPEND"
# Testcase makefile.
!           if "$(TARGET_MODE)" != "TESTCASE"
# Bad TARGET_MODE complain.
!            ifndef MAKEVER
!             if [$(ECHO) $(CLRERR)Fatal Error: Bad TARGET_MODE="$(TARGET_MODE)". Valid ones are: EXE, DLL, SYS, IFS, VDD, LIB, SYSLIB, IFSLIB, TESTCASE and EMPTY.$(CLRRST)]
!             endif
!             error
!            else
!             error $(CLRERR)Fatal Error: Bad TARGET_MODE="$(TARGET_MODE)". Valid ones are: EXE, DLL, SYS, IFS, VDD, LIB, SYSLIB, IFSLIB, TESTCASE and EMPTY.$(CLRRST)
!            endif
!           endif
!          endif
!         endif
!        endif
!       endif
!      endif
!     endif
!    endif
!   endif
!  endif
! endif
!endif


# -----------------------------------------------------------------------------
# Provide overridable defaults
# -----------------------------------------------------------------------------

# Default target mode is executable.
!ifndef TARGET_MODE
TARGET_MODE = EXE
!endif

# Default extension corresponds to the target mode.
!ifndef TARGET_EXT
! if "$(TARGET_MODE)" == "DLL"
TARGET_EXT  = $(EXT_DLL)
! endif
! if "$(TARGET_MODE)" == "SYS"
TARGET_EXT  = $(EXT_SYS)
! endif
! if "$(TARGET_MODE)" == "IFS"
TARGET_EXT  = $(EXT_IFS)
! endif
! if "$(TARGET_MODE)" == "VDD"
TARGET_EXT  = $(EXT_VDD)
! endif
! if "$(TARGET_MODE)" == "EXE"
TARGET_EXT  = $(EXT_EXE)
! endif
! if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_EXT  = $(EXT_LIB)
! endif
! if "$(TARGET_MODE)" == "EMPTY"
TARGET_EXT  = empty
! endif
! if "$(TARGET_MODE)" == "DEPEND"
TARGET_EXT  = depend
! endif
! if "$(TARGET_MODE)" == "TESTCASE"
TARGET_EXT  = testcase
! endif
! ifndef TARGET_EXT
!  ifndef MAKEVER
!   if [$(ECHO) $(CLRERR)Internal Error: TARGET_EXT not set. Probably invalid TARGET_MODE. (TARGET_MODE="$(TARGET_MODE)")$(CLRRST)]
!   endif
!   error
!  else
!   error $(CLRERR)Internal Error: TARGET_EXT not set. Probably invalid TARGET_MODE. (TARGET_MODE="$(TARGET_MODE)")$(CLRRST)
!  endif
! endif
!endif

# Default target path. (where all the generated stuff for this target goes)
!ifndef PATH_TARGET
PATH_TARGET = $(PATH_OBJ)\$(TARGET_NAME).$(TARGET_EXT)
!endif

# Default target file. (output)
!ifndef TARGET
! if "$(TARGET_MODE)" != "EMPTY"
!  if "$(TARGET_MODE)" != "TESTCASE"
TARGET      = $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)
!  else
TARGET      = testcase
!  endif
! endif
!endif

# Default target .sym file. (output)
!ifndef TARGET_SYM
TARGET_SYM  = $(PATH_TARGET)\$(TARGET_NAME).$(EXT_SYM)
!endif

# Default object file. (output)
!ifndef TARGET_OBJS
TARGET_OBJS = $(PATH_TARGET)\$(TARGET_NAME).$(EXT_OBJ)
!endif

# Default libraries. (input)
!ifndef TARGET_LIBS
TARGET_LIBS = $(LIB_C_DLL) $(LIB_OS) $(LIB_C_RTDLL)
!endif

# Default definition file. (input)
!ifndef TARGET_DEF
TARGET_DEF  = $(MAKEDIR)\$(PATH_DEF)\$(TARGET_NAME).def
!endif

# Default modified definition filename. (output)
!ifndef TARGET_DEF_LINK
TARGET_DEF_LINK = $(PATH_TARGET)\$(TARGET_NAME)_link.def
!endif

# Default definition file for generating the import library. (input)
!ifndef TARGET_IDEF
TARGET_IDEF = $(TARGET_DEF)
!endif

# Default map file. (output)
!ifndef TARGET_MAP
TARGET_MAP  = $(PATH_TARGET)\$(TARGET_NAME).$(EXT_MAP)
!endif

# Default link file. (output)
!ifndef TARGET_LNK
TARGET_LNK  = $(PATH_TARGET)\$(TARGET_NAME).lnk
!endif

# Default import library file. (output)
!ifndef TARGET_ILIB
! if "$(TARGET_MODE)" == "DLL"
TARGET_ILIB =$(PATH_LIB)\$(TARGET_NAME).$(EXT_ILIB)
! endif
!endif

# Default public name. (output)
!ifndef TARGET_PUBNAME
TARGET_PUBNAME=
! ifdef TARGET_PUBLIC
!  if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_PUBNAME=$(PATH_LIB)\$(TARGET_NAME).$(TARGET_EXT)
!  endif
!  if "$(TARGET_MODE)" == "EXE"
TARGET_PUBNAME=$(PATH_EXE)\$(TARGET_NAME).$(TARGET_EXT)
!  endif
!  if "$(TARGET_MODE)" == "DLL"
TARGET_PUBNAME=$(PATH_DLL)\$(TARGET_NAME).$(TARGET_EXT)
!  endif
!  if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
TARGET_PUBNAME=$(PATH_SYS)\$(TARGET_NAME).$(TARGET_EXT)
!  endif
!  if "$(TARGET_MODE)" == "VDD"
TARGET_PUBNAME=$(PATH_VDD)\$(TARGET_NAME).$(TARGET_EXT)
!  endif
! endif
!endif

# Default depend filename.
!ifndef TARGET_DEPEND
TARGET_DEPEND = $(PATH_TARGET)\.depend
!endif

# Default makefile names.
!ifndef BUILD_MAKEFILE
BUILD_MAKEFILE = Makefile
!endif

# Default makefile name.
!ifndef MAKEFILE
MAKEFILE = $(BUILD_MAKEFILE)
!endif

# Ignore linker warnings for some target modes.
!ifndef TARGET_IGNORE_LINKER_WARNINGS
! if "$(TARGET_MODE)" == "DLL"
TARGET_IGNORE_LINKER_WARNINGS = 1
! endif
!endif


# Default stacksize
# If 16bit:      8KB
# Else (32bit): 64KB
!ifndef TARGET_STACKSIZE
! ifdef ENV_16BIT
TARGET_STACKSIZE=0x2000
! else
TARGET_STACKSIZE=0x10000
! endif
!endif



# -----------------------------------------------------------------------------
# Tell user what we're building.
# -----------------------------------------------------------------------------
!ifndef BUILD_QUIET
! ifndef MAKEVER
!  if [$(ECHO) Target is $(CLRFIL)$(TARGET)$(CLRRST)]
!  endif
! else
$(ECHO) Target is $(CLRFIL)$(TARGET)$(CLRRST)
! endif
!endif


# -----------------------------------------------------------------------------
# Ensure the platform-specific target path exists
# -----------------------------------------------------------------------------

!if "$(TARGET_MODE)" != "EMPTY"
! if "$(PATH_TARGET)" != ""
!  ifndef MAKEVER
!   if [$(TOOL_EXISTS) $(PATH_TARGET)] != 0
!    ifndef BUILD_QUIET
!     if [$(ECHO) Target path $(CLRFIL)$(PATH_TARGET)$(CLRTXT) does NOT exist. Creating. $(CLRRST)]
!     endif
!    endif
!    if [$(TOOL_CREATEPATH) $(PATH_TARGET)]
!     if [$(ECHO) $(CLRERR)Error: Could not create $(CLRFIL)$(PATH_TARGET)$(CLRRST)]
!     endif
!     error
!    endif
!   endif
!  else
!   if %exist($(PATH_TARGET)) == 0
!    ifndef BUILD_QUIET
$(ECHO) Target path $(CLRFIL)$(PATH_TARGET)$(CLRTXT) does NOT exist. Creating. $(CLRRST)
!    endif
!   else
!    if [$(TOOL_CREATEPATH) $(PATH_TARGET)]
!     error $(CLRERR)Error: Could not create $(CLRFIL)$(PATH_TARGET)$(CLRRST)
!    endif
!   endif
!  endif
! endif
!endif
# not 100% sure about the != EMPTY stuff, but this is way faster.



# -----------------------------------------------------------------------------
# Common inference rules
# -----------------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .c .cpp .asm .$(EXT_OBJ) .$(EXT_RES) .rc .ii .s

#
# A workaround for SlickEdits inability to find the buggy files..
# This makes the source filenames in the error listing have full path.
# See setup.mak for compile command line.
#
_SRC = $<
!ifdef SLKRUNS
_SRC = $(PATH_CURRENT)\$<
!endif


# Assembling assembly source.
.asm{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG) $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    $(TOOL_JOB_SUB) $(AS) $(AS_FLAGS_SYS) $(_SRC) $(AS_OBJ_OUT)$@
!else
    $(TOOL_JOB_SUB) $(AS) $(AS_FLAGS) $(_SRC) $(AS_OBJ_OUT)$@
!endif

.asm.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    $(AS) $(AS_FLAGS_SYS) $(_SRC) $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!else
    $(AS) $(AS_FLAGS) $(_SRC) $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!endif

# C++ Compiler base line
_CXX_BASELINE = $(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CXX_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "DLL"
        $(CXX_FLAGS_DLL) \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB"
        $(CXX_FLAGS_SYS) \
!endif
!if "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
        $(CXX_FLAGS_IFS) \
!endif

# Compiling C++ source.
.cpp{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) C++ Compiler $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_JOB_SUB) $(_CXX_BASELINE) \
!if "$(CXX_LST_OUT)" != ""
        $(CXX_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CXX_OBJ_OUT)$@ $(_SRC)

.cpp.$(EXT_OBJ):
    @$(ECHO) C++ Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CXX_BASELINE) \
!if "$(CXX_LST_OUT)" != ""
        $(CXX_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CXX_OBJ_OUT)$(PATH_TARGET)\$(@F) $(_SRC)


# Pre-Compiling C++ source.
.cpp.ii:
    @$(ECHO) C++ Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CXX_BASELINE) \
        $(CXX_PC_2_STDOUT) $(_SRC) > $@


# Compiler C++ source to assembly.
!if "$(CXX_AS_2_FILE)" != ""
.cpp.s:
    @$(ECHO) C++ To Assembly $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CXX_BASELINE) \
        $(CXX_AS_2_FILE)$@ $(_SRC)
!endif



# C Compiler base line
_CC_BASELINE = $(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CC_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "DLL"
        $(CC_FLAGS_DLL) \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB"
        $(CC_FLAGS_SYS) \
!endif
!if "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
        $(CC_FLAGS_IFS) \
!endif

# Compiling C source.
.c{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) C Compiler $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_JOB_SUB) $(_CC_BASELINE) \
!if "$(CC_LST_OUT)" != ""
        $(CC_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CC_OBJ_OUT)$@ $(_SRC)

.c.$(EXT_OBJ):
    @$(ECHO) C Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CC_BASELINE) \
!if "$(CC_LST_OUT)" != ""
        $(CC_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CC_OBJ_OUT)$(PATH_TARGET)\$(@F) $(_SRC)


# Pre-Compiling C source.
.c.ii:
    @$(ECHO) C PreCompiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CC_BASELINE) \
        $(CC_PC_2_STDOUT) $(_SRC) > $@


# Compiler C source to assembly.
!if "$(CC_AS_2_FILE)" != ""
.c.s:
    @$(ECHO) C To Assembly $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(_CC_BASELINE) \
        $(CC_AS_2_FILE)$@ $(_SRC)
!endif


# Compiling resources.
.rc{$(PATH_TARGET)}.res:
    @$(ECHO) RC Compiler $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_JOB_SUB) $(RC) $(RC_FLAGS) $(_SRC) $@

.rc.res:
    @$(ECHO) RC Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(RC) $(RC_FLAGS) $(_SRC) $(PATH_TARGET)\$(@F)





# -----------------------------------------------------------------------------
# The all rule - The default one, as it's the first rule in the file.
# -----------------------------------------------------------------------------
all: build



# -----------------------------------------------------------------------------
# The build rule - This runs all passes:
#   1. Make Dependencies
#   2. Make Libraries (all kinds)
#   3. Make Executables
#   4. Make Miscellaneous Targets
#   5. Make Install
# Note: In order to load dependencies we'll do a forwarding after making them.
# -----------------------------------------------------------------------------
build: _build
!if "$(MAKEFLAGS:I=_)" == "$(MAKEFLAGS)" # this is of course broken in nmake v5.0 for OS/2.
    @$(ECHO)$(CLRMAK)[Successfully Built Everything!] $(CLRRST)
!else
    @$(ECHO)$(CLRMAK)[Built Everything! (Ignore option specified)] $(CLRRST)
!endif

# internal rule shared by rebuild and build.
_build:  _build_banner_dep dep
!ifndef BUILD_QUIET
    @$(ECHO) Restarting $(CLRFIL)$(MAKEFILE)$(CLRTXT) with new dependencies. $(CLRRST)
!endif
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_MAKE) -f $(MAKEFILE) _build_new_dependencies_

# internal rule used to reload dependencies.
_build_new_dependencies_: \
        _build_banner_lib           lib \
        _build_banner_executable    executable \
        _build_banner_miscellaneous miscellaneous \
        _build_banner_install       install

# Banners for rebuild and build.
_build_banner_clean:
    @$(ECHO)$(CLRMAK)[Start Pass 0 - Make Clean] $(CLRRST)
    @SET _BUILD_PASS=0
_build_banner_dep:
    @$(ECHO)$(CLRMAK)[Start Pass 1 - Make Dependencies] $(CLRRST)
    @SET _BUILD_PASS=1
_build_banner_lib:
    @$(ECHO)$(CLRMAK)[Start Pass 2 - Make Libraries] $(CLRRST)
    @SET _BUILD_PASS=2
_build_banner_executable:
    @$(ECHO)$(CLRMAK)[Start Pass 3 - Make Executables] $(CLRRST)
    @SET _BUILD_PASS=3
_build_banner_miscellaneous:
    @$(ECHO)$(CLRMAK)[Start Pass 4 - Make Miscellaneous Targets] $(CLRRST)
    @SET _BUILD_PASS=4
_build_banner_install:
    @$(ECHO)$(CLRMAK)[Start Pass 5 - Make Install] $(CLRRST)
    @SET _BUILD_PASS=5



# -----------------------------------------------------------------------------
# The rebuild rule - Same as build but does a clean first (as Pass 0).
# -----------------------------------------------------------------------------
rebuild: \
        _build_banner_clean clean \
        _build
!if "$(MAKEFLAGS:i=_)" == "$(MAKEFLAGS)"
    @$(ECHO)$(CLRMAK)[Successfully Rebuilt Everything!] $(CLRRST)
!else
    @$(ECHO)$(CLRMAK)[Rebuilt Everything! (Ignore option specified)] $(CLRRST)
!endif



# -----------------------------------------------------------------------------
# Pass 0 - The clean rule - Clean up output files.
#   The current setup doesn't clean the installed ones.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" != "TESTCASE"
clean:
    @$(ECHO) Cleaning... $(CLRRST)
!if "$(PATH_TARGET)" != ""              # paranoia
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_RM) \
        $(PATH_TARGET)\*.$(EXT_OBJ) \
        $(PATH_TARGET)\*.$(EXT_ILIB) \
        $(PATH_TARGET)\*.$(EXT_EXE) \
        $(PATH_TARGET)\*.$(EXT_DLL) \
        $(PATH_TARGET)\*.$(EXT_RES)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_RM) \
        $(PATH_TARGET)\*.$(EXT_SYS) \
        $(PATH_TARGET)\*.$(EXT_LIB) \
        $(PATH_TARGET)\*.$(EXT_IFS) \
        $(PATH_TARGET)\*.$(EXT_MAP) \
        $(PATH_TARGET)\*.$(EXT_SYM)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_RM) \
        $(PATH_TARGET)\*.s \
        $(PATH_TARGET)\*.lst \
        $(PATH_TARGET)\*.lnk \
        $(PATH_TARGET)\*.ii \
        $(PATH_TARGET)\.depend
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_RM) \
        .\*.ii \
        .\*.err \
        .\.depend
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif
!endif #!TESTCASE



# -----------------------------------------------------------------------------
# Pass 1 - The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
    @$(ECHO) Making dependencies... $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
!if "$(TARGET_MODE)" != "EMPTY" && "$(TARGET_MODE)" != "TESTCASE"
    $(TOOL_DEP) $(TOOL_DEP_FLAGS) -o$$(PATH_TARGET) -d$(TARGET_DEPEND)\
! ifdef TARGET_NO_DEP
        -x$(TARGET_NO_DEP: =;)\
! endif
        $(TOOL_DEP_FILES)
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif



# -----------------------------------------------------------------------------
# Pass 2 - The lib rule - Make libraries.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_LIB = _subdir_lib
$(SUBDIRS_LIB):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) lib
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_LIB = _premakefiles_lib
$(PREMAKEFILES_LIB):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) lib
!endif

lib:    $(SUBDIRS_LIB) $(PREMAKEFILES_LIB) \
!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
        $(TARGET) $(TARGET_PUBNAME) \
!endif
        $(TARGET_ILIB)
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# Pass 3 - The executable rule - Build the executables.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_EXECUTABLE = _subdir_executable
$(SUBDIRS_EXECUTABLE):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) executable
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_EXECUTABLE = _premakefiles_executable
$(PREMAKEFILES_EXECUTABLE):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) executable
!endif

executable: \
!if "$(TARGET_MODE)" != "LIB" && "$(TARGET_MODE)" != "SYSLIB" && "$(TARGET_MODE)" != "IFSLIB"
        $(SUBDIRS_EXECUTABLE) $(PREMAKEFILES_EXECUTABLE) $(TARGET) $(TARGET_PUBNAME)
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET)$(CLRRST)
!else
        $(SUBDIRS_EXECUTABLE) $(PREMAKEFILES_EXECUTABLE)
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# Pass 4 - The miscellaneous rule - Makes other miscellaneous stuff like
#   documentations etc. This is experimental for the moment.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_MISC = _subdir_misc
$(SUBDIRS_MISC):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) miscellaneous
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_MISC = _premakefiles_misc
$(PREMAKEFILES_MISC):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) miscellaneous
!endif

miscellaneous:  $(SUBDIRS_MISC) $(PREMAKEFILES_MISC) \
                $(TARGET_DOCS) $(TARGET_MISC)
!if "$(TARGET_DOCS)$(TARGET_MISC)" != ""
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET_DOCS) $(TARGET_MISC)$(CLRRST)
!else
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# Pass 5 - The install rule - Copies target to main binary directory.
#   Installation order is not concidered vital, so subdirectories and
#   pre-makefiles are processed after this directory. This might be changed.
# -----------------------------------------------------------------------------
install:
!if "$(TARGET_PUBLIC)" == ""
! if "$(TARGET_MODE)" == "EXE"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_BIN)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist $(PATH_BIN) $(TOOL_CREATEPATH)       $(PATH_BIN)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_BIN)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_BIN)
! endif
! if "$(TARGET_MODE)" == "DLL"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_DLL)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist $(PATH_DLL) $(TOOL_CREATEPATH)       $(PATH_DLL)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_DLL)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_DLL)
! endif
! if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_SYS)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist $(PATH_SYS) $(TOOL_CREATEPATH)       $(PATH_SYS)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_SYS)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_SYS)
! endif
!if 0 # these targets are either TARGET_PUBLIC or all private.
!  if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_LIB)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist $(PATH_LIB) $(TOOL_CREATEPATH)       $(PATH_LIB)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_LIB)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_LIB)
!  endif
! endif
!endif
!if "$(TARGET_DOCS)" != ""
    $(TOOL_COPY) $(TARGET_DOCS) $(PATH_DOC)
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# Pass x - The testcase rule - Execute testcases when present.
#   Testcases are either a testcase.mak file or a testcase subdirectory.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" != "TESTCASE"
!ifndef BUILD_OWN_TESTCASE_RULE
!ifndef MAKEVER
_TESTCASE_TST1 = [$(TOOL_EXISTS) testcase] == 0
_TESTCASE_TST2 = [$(TOOL_EXISTS) testcase.mak] == 0
!else
_TESTCASE_TST1 = exists(testcase) != 0
_TESTCASE_TST2 = exists(testcase.mak) != 0
!endif

testcase:
    @$(ECHO) Executing testcases $(CLRRST)
!if $(_TESTCASE_TST1)
    @$(TOOL_DODIRS) "testcase" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!endif
!if $(_TESTCASE_TST2)
    @$(TOOL_DOMAKES) "testcase.mak" $(TOOL_MAKE) $@
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif
!endif
!endif #!TESTCASE



# -----------------------------------------------------------------------------
# The target rule - Build the target.
#   Note: This also builds libraries in subdirectories and submakefiles.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_TARGET = _subdir_target
$(SUBDIRS_TARGET):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) target
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_TARGET = _premakefiles_target
$(PREMAKEFILES_TARGET):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) target
!endif

target: $(SUBDIRS_TARGET) $(PREMAKEFILES_TARGET) $(TARGET) $(TARGET_ILIB) $(TARGET_PUBNAME)
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET) $(TARGET_ILIB)$(CLRRST)
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The shell rule - Setup the correcte shell environment and start a shell.
# -----------------------------------------------------------------------------
shell:
    @$(ECHO) Creating work shell $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    -$(TOOL_BUILDENV) $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(ENV_ENVS) \
        $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST) * $(COMSPEC)



# -----------------------------------------------------------------------------
# The nothing rule - Rule for testing the makefile structure.
# -----------------------------------------------------------------------------
nothing:
    @$(ECHO) Doing nothing in $(MAKEFILE).
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif
    @$(ECHO) Completed nothing in $(MAKEFILE).



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EXE, DLL, SYS and IFS targets
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
$(TARGET): $(TARGET_OBJS) $(TARGET_RES) $(TARGET_DEF_LINK) $(TARGET_LNK) $(TARGET_DEPS)
!if "$(TOOL_JOB_WAIT)" != ""
! ifndef BUILD_QUIET
    @$(ECHO) Waiting for jobs to complete $(CLRRST)
! endif
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_JOB_WAIT)
!endif
    @$(ECHO) Linking $(TARGET_MODE) $(CLRFIL)$@ $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
!ifdef TARGET_IGNORE_LINKER_WARNINGS
    -4 \
!endif
!if "$(TARGET_MODE)" == "EXE"
    $(LINK_CMD_EXE)
!endif
!if "$(TARGET_MODE)" == "DLL"
    $(LINK_CMD_DLL)
!endif
!if "$(TARGET_MODE)" == "SYS"
    $(LINK_CMD_SYS)
!endif
!if "$(TARGET_MODE)" == "IFS"
    $(LINK_CMD_IFS)
!endif
!if "$(TARGET_MODE)" == "VDD"
    $(LINK_CMD_VDD)
!endif
!if "$(TARGET_RES)" != "" && "$(RL)" != ""
    @$(ECHO) Linking Resources $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(RL) $(RL_FLAGS) $(TARGET_RES) $@
!endif
!if "$(TARGET_DLLRNAME)" != ""
    @$(ECHO) Dll Rename $(TARGET_DLLRNAME)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_DLLRNAME) $(TARGET) $(TARGET_DLLRNAME)
!endif
!if "$(TOOL_MAPSYM)" != "" && "$(TARGET_SYM)" != "" && "$(TARGET_MAP)" != ""
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_MAPSYM) $(TARGET_MAP) $(TARGET_SYM)
!endif


#
# Linker parameter file.
#
$(TARGET_LNK): $(MAKE_INCLUDE_PROCESS) $(MAKE_INCLUDE_SETUP) $(PATH_MAKE)\setup.mak $(MAKEFILE)
!ifndef TOOL_DEFCONV
    @$(TOOL_ECHO) Creating Linker Input File $(CLRRST)<<$@
$(LINK_LNK1)
$(LINK_LNK2)
$(LINK_LNK3)
$(LINK_LNK4)
$(LINK_LNK5)
<<KEEP
!else
    @$(ECHO) Creating Linker Input File $(CLRRST) $@
    @$(TOOL_RM) $@
    \
! ifdef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_DEFCONV) $(TARGET_DEF_LINK) $@ <<$(TARGET_LNK)2
#
# LINK_LNK[1-5]:
#
$(LINK_LNK1)
$(LINK_LNK2)
$(LINK_LNK3)
$(LINK_LNK4)
$(LINK_LNK5)
<<keep
!endif
!ifdef BUILD_VERBOSE
    @type $@
!endif


#
# Builddef modified definition file.
#
!if "$(TARGET_DEF_LINK)" != "$(TARGET_DEF)"
$(TARGET_DEF_LINK): $(TARGET_DEF)
! ifndef BUILD_QUIET
    @$(ECHO) Stamping deffile with build level info.$(CLRRST)
! endif
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BLDLEVEL) $(BUILD_BLDLEVEL_FLAGS) $(TARGET_BLDLEVEL_FLAGS) -R$** $** $@
!endif

!endif


# -----------------------------------------------------------------------------
# The $(TARGET) rule - For LIB, SYSLIB, and IFSLIB targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
$(TARGET): $(TARGET_OBJS) $(TARGET_LNK) $(TARGET_DEPS)
!if "$(TOOL_JOB_WAIT)" != ""
! ifndef BUILD_QUIET
   @$(ECHO) Waiting for jobs to complete $(CLRRST)
! endif
    \
! ifndef BUILD_VERBOSE
   @ \
! endif
   $(TOOL_JOB_WAIT)
!endif
    @$(ECHO) Creating Library $(CLRFIL)$@ $(CLRRST)
!ifndef BUILD_VERBOSE
    @$(TOOL_RM) $@
    @$(AR_CMD)
!else
    $(TOOL_RM) $@
    $(AR_CMD)
!endif


#
# Lib parameter file.
#
$(TARGET_LNK): $(MAKE_INCLUDE_PROCESS) $(MAKE_INCLUDE_SETUP) $(PATH_MAKE)\setup.mak $(MAKEFILE)
    @$(TOOL_ECHO) Creating Lib Input File $(CLRRST)<<$@
$(AR_LNK1)
$(AR_LNK2)
$(AR_LNK3)
$(AR_LNK4)
$(AR_LNK5)
<<KEEP
!ifdef BUILD_VERBOSE
    @type $@
!endif
!endif


# -----------------------------------------------------------------------------
# Copy rule for public targets.
#   Normally used for public libraries, but may be used for other purposes...
# -----------------------------------------------------------------------------
!if "$(TARGET_PUBNAME)" != ""
$(TARGET_PUBNAME): $(TARGET)
    @$(ECHO) Copying $(CLRFIL)$(TARGET)$(CLRTXT) to $(CLRFIL)$(@D)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @if not exist $(@D) $(ECHO) Target public path $(CLRFIL)$(@D)$(CLRTXT) does NOT exist. Creating. $(CLRRST)
!endif
    @if not exist $(@D) $(TOOL_CREATEPATH) $(@D)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_COPY) $** $@
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(@R).sym
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For DEPEND targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "DEPEND"
$(TARGET):
    @$(ECHO) .
!endif



# -----------------------------------------------------------------------------
# The $(TARGET_ILIB) rule - Make import library.
# -----------------------------------------------------------------------------
!ifdef TARGET_ILIB
$(TARGET_ILIB): $(TARGET_IDEF)
    @$(ECHO) Creating Import Library $(CLRFIL)$@ $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(TARGET_IDEF)
!endif



# -----------------------------------------------------------------------------
# The .force rule - Force a remake of something everytime.
# -----------------------------------------------------------------------------
.force:
!ifndef BUILD_VERBOSE
    @$(ECHO) .
!else
    @$(ECHO) . (force) .
!endif



# -----------------------------------------------------------------------------
# Read Dependencies.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" != "TESTCASE" && "$(TARGET_MODE)" != "DEPEND"
!if "$(TARGET_MODE)" != "EMPTY" && "$(NODEP)" == ""

#
# Read dependency file for current directory
#
!ifndef MAKEVER
! if [$(TOOL_EXISTS) $(TARGET_DEPEND)] == 0
!  ifdef BUILD_VERBOSE
!   if [$(ECHO) Including dependency $(CLRFIL)$(TARGET_DEPEND)$(CLRRST)]
!   endif
!  endif
!  include $(TARGET_DEPEND)
! else
!  ifndef NODEP
!   if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(TARGET_DEPEND) is missing.$(CLRRST)]
!   endif
!  endif
! endif
!else
! if %exists($(TARGET_DEPEND)) != 0
!  ifdef BUILD_VERBOSE
$(ECHO) Including dependency $(CLRFIL)$(TARGET_DEPEND)$(CLRRST)
!  endif
!  include $(TARGET_DEPEND)
! else
!  ifndef NODEP
$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(TARGET_DEPEND) is missing.$(CLRRST)
!  endif
! endif
!endif


#
# Read global dependency files.
#
!ifdef BUILD_DEPEND1
! ifndef MAKEVER
!  if [$(TOOL_EXISTS) $(BUILD_DEPEND1)] == 0
!   ifdef BUILD_VERBOSE
!    if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND1)$(CLRRST)]
!    endif
!   endif
!   include $(BUILD_DEPEND1)
!  else
!   ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND1) is missing.$(CLRRST)]
!    endif
!   endif
!  endif
! else
!  if %exists($(BUILD_DEPEND1)) != 0
!   ifdef BUILD_VERBOSE
$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND1)$(CLRRST)
!   endif
!   include $(BUILD_DEPEND1)
!  else
!   ifndef NODEP
$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND1) is missing.$(CLRRST)
!   endif
!  endif
! endif
!endif


!ifdef BUILD_DEPEND2
! ifndef MAKEVER
!  if [$(TOOL_EXISTS) $(BUILD_DEPEND2)] == 0
!   ifdef BUILD_VERBOSE
!    if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND2)$(CLRRST)]
!    endif
!   endif
!   include $(BUILD_DEPEND2)
!  else
!   ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND2) is missing.$(CLRRST)]
!    endif
!   endif
!  endif
! else
!  if %exists($(BUILD_DEPEND2)) != 0
!   ifdef BUILD_VERBOSE
$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND2)$(CLRRST)
!   endif
!   include $(BUILD_DEPEND2)
!  else
!   ifndef NODEP
$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND2) is missing.$(CLRRST)
!   endif
!  endif
! endif
!endif


!ifdef BUILD_DEPEND3
! ifndef MAKEVER
!  if [$(TOOL_EXISTS) $(BUILD_DEPEND3)] == 0
!   ifdef BUILD_VERBOSE
!    if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND3)$(CLRRST)]
!    endif
!   endif
!   include $(BUILD_DEPEND3)
!  else
!   ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND3) is missing.$(CLRRST)]
!    endif
!   endif
!  endif
! else
!  if %exists($(BUILD_DEPEND3)) != 0
!   ifdef BUILD_VERBOSE
$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND3)$(CLRRST)
!   endif
!   include $(BUILD_DEPEND3)
!  else
!   ifndef NODEP
$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND3) is missing.$(CLRRST)
!   endif
!  endif
! endif
!endif


!endif


#
# If BUILD_MULTIJOBS is nonempty make sure the job daemon is up running.
#
!if "$(BUILD_MULTIJOBS)" != ""
! if [$(TOOL_JOB_UP)] != 0
!  if "$(BUILD_QUITE)" == ""
!   ifndef MAKEVER
!    if [$(ECHO) Starting Job Daemon With $(TOOL_JOB_WORKERS) Workers...$(CLRRST)]
!    endif
!   else
$(ECHO) Starting Job Daemon With $(TOOL_JOB_WORKERS) Workers...$(CLRRST)
!   endif
!  endif
!  if [$(TOOL_JOB_INIT) $(TOOL_JOB_WORKERS)] != 0
!   ifndef MAKEVER
!    if [$(ECHO) $(CLRERR)Fatal error: Failed to start job daemon.$(CLRRST)]
!    endif
!    error
!else
!    error $(CLRERR)Fatal error: Failed to start job daemon.$(CLRRST)
!endif
!  endif
! endif
!endif

!endif #!TESTCASE
