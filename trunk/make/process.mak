# $Id: process.mak,v 1.39 2004-02-02 22:26:57 bird Exp $
#
# Generic Buildsystem
#
#   The common build process rules
#
# Copyright (c) 2001-2002 knut st. osmundsen <bird@anduin.net>
# Copyright (c) 2001 Patrick Haller
#
# This file is part of Generic Buildsystem.
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
!             if [$(ECHO) $(CLRERR)Fatal Error: Bad TARGET_MODE="$(TARGET_MODE)". Valid ones are: EXE, DLL, SYS, IFS, VDD, LIB, SYSLIB, IFSLIB, DEPEND, TESTCASE and EMPTY.$(CLRRST)]
!             endif
!             error
!            else
!             error $(CLRERR)Fatal Error: Bad TARGET_MODE="$(TARGET_MODE)". Valid ones are: EXE, DLL, SYS, IFS, VDD, LIB, SYSLIB, IFSLIB, DEPEND, TESTCASE and EMPTY.$(CLRRST)
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

# stuff which has changed behavior.
!if 1

! ifdef TARGET_PUBNAME
!  if [$(ECHO) $(CLRERR)Fatal error: TARGET_PUBNAME isn't supported. The pubname stuff has changed!$(CLRRST)]
!  endif
!  error
! endif

! ifdef PATH_INCLUDE
!  if [$(ECHO) $(CLRERR)Fatal error: PATH_INCLUDE has changed name to PATH_INCLUDES!$(CLRRST)]
!  endif
!  error
! endif

!endif



# -----------------------------------------------------------------------------
# Provide overridable defaults
# -----------------------------------------------------------------------------

# Default target mode is executable.
!ifndef TARGET_MODE
! if "$(_BUILD_PROJECT)" != "Odin32"
TARGET_MODE = EXE
! else
# Odin32 defaults.
!  ifndef CUSTOMBUILD
TARGET_MODE = DLL
!  else
TARGET_MODE = LIB
!  endif
! endif
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
!if "$(TARGET_SUB)" != ""
PATH_TARGET = $(PATH_OBJ)\$(TARGET_SUB)\$(TARGET_NAME).$(TARGET_EXT)
!else
PATH_TARGET = $(PATH_OBJ)\$(TARGET_NAME).$(TARGET_EXT)
!endif
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
! if "$(_BUILD_PROJECT)" != "Odin32"
TARGET_IDEF = $(TARGET_DEF)
! else
!  if defined(TARGET_IDEF_CONV) || "$(TARGET_IDEF_ORG)" != ""
TARGET_IDEF = $(PATH_TARGET)\$(TARGET_NAME)_implib.def
!   ifndef TARGET_IDEF_ORG
TARGET_IDEF_ORG = $(TARGET_DEF)
!   endif
!  else
TARGET_IDEF = $(TARGET_DEF)
TARGET_IDEF_ORG = $(TARGET_DEF)
!  endif
! endif

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
! if "$(TARGET_MODE)" == "DLL" || "$(TARGET_ILIB_YES)" != ""
TARGET_ILIB = $(PATH_LIB)\$(TARGET_NAME).$(EXT_ILIB)
! endif
!endif

# Default public base directory. (publish)
!ifndef TARGET_PUB_BASE
! if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_PUB_BASE = $(PATH_LIB)
! else
TARGET_PUB_BASE = $(PATH_PUB)
!endif
!endif

# Default public base directory for unstripped release version.
!ifndef TARGET_PUB_BASE_DEB
! if "$(BUILD_MODE)" == "RELEASE" && "$(TARGET_MODE:LIB=...)" == "$(TARGET_MODE)"
!  if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
TARGET_PUB_BASE_DEB = $(PATH_PUB_DEB)
!  endif
! endif
!endif

# Default sub directory.
!ifndef TARGET_PUB_SUB
! if "$(TARGET_SUB)" != ""
TARGET_PUB_SUB  = $(TARGET_SUB)
! else
!  if "$(TARGET_MODE)" == "EXE"
TARGET_PUB_SUB  = $(PATH_SUB_BIN)
!  endif
!  if "$(TARGET_MODE)" == "DLL"
TARGET_PUB_SUB  = $(PATH_SUB_DLL)
!  endif
!  if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_PUB_SUB  = .
!  endif
!  if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
TARGET_PUB_SUB  = $(PATH_SUB_SYS)
!  endif
!  if "$(TARGET_MODE)" == "VDD"
TARGET_PUB_SUB  = $(PATH_SUB_VDD)
!  endif
! endif
!endif

# Default public directory.
!ifndef TARGET_PUB_DIR
TARGET_PUB_DIR  = $(TARGET_PUB_BASE)\$(TARGET_PUB_SUB)
!endif

# Default unstripped public directory.
!if !defined(TARGET_PUB_DIR_DEB) && "$(TARGET_PUB_BASE_DEB)" != ""
TARGET_PUB_DIR_DEB  = $(TARGET_PUB_BASE_DEB)\$(TARGET_PUB_SUB)
!endif

# Default public name.
!ifndef TARGET_PUB_NAME
TARGET_PUB_NAME = $(TARGET_NAME)
!endif

# Default public ext.
!ifndef TARGET_PUB_EXT
TARGET_PUB_EXT  = $(TARGET_EXT)
!endif

# Default public full name.
!ifndef TARGET_PUB
TARGET_PUB      = $(TARGET_PUB_DIR)\$(TARGET_PUB_NAME).$(TARGET_PUB_EXT)
!endif

# Default public full name of unstripped version.
!if !defined(TARGET_PUB_DEB) && "$(TARGET_PUB_DIR_DEB)" != ""
TARGET_PUB_DEB  = $(TARGET_PUB_DIR_DEB)\$(TARGET_PUB_NAME).$(TARGET_PUB_EXT)
!endif

# Target publish no.2

!if defined(TARGET_PUB2_BASE) || defined(TARGET_PUB2_BASE_DEB) || defined(TARGET_PUB2_SUB) \
 || defined(TARGET_PUB2_DIR) || defined(TARGET_PUB2_DIR_DEB) || defined(TARGET_PUB2_NAME) \
 || defined(TARGET_PUB2_EXT) || defined(TARGET_PUB2)

# Default public base directory. (publish)
! ifndef TARGET_PUB2_BASE
!  if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_PUB2_BASE = $(PATH_LIB)
!  else
TARGET_PUB2_BASE = $(PATH_PUB)
!  endif
! endif

# Default public base directory for unstripped release version.
! ifndef TARGET_PUB2_BASE_DEB
!  if "$(BUILD_MODE)" == "RELEASE" && "$(TARGET_MODE:LIB=...)" == "$(TARGET_MODE)"
!   if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
TARGET_PUB2_BASE_DEB = $(PATH_PUB2_DEB)
!   endif
!  endif
! endif

# Default sub directory.
! ifndef TARGET_PUB2_SUB
!  if "$(TARGET_SUB)" ! = ""
TARGET_PUB2_SUB  = $(TARGET_SUB)
!  else
!   if "$(TARGET_MODE)" == "EXE"
TARGET_PUB2_SUB  = $(PATH_SUB_BIN)
!   endif
!   if "$(TARGET_MODE)" == "DLL"
TARGET_PUB2_SUB  = $(PATH_SUB_DLL)
!   endif
!   if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
TARGET_PUB2_SUB  = .
!   endif
!   if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
TARGET_PUB2_SUB  = $(PATH_SUB_SYS)
!   endif
!   if "$(TARGET_MODE)" == "VDD"
TARGET_PUB2_SUB  = $(PATH_SUB_VDD)
!   endif
!  endif
! endif

# Default public directory.
! ifndef TARGET_PUB2_DIR
TARGET_PUB2_DIR  = $(TARGET_PUB2_BASE)\$(TARGET_PUB2_SUB)
! endif

# Default unstripped public directory.
! if ! defined(TARGET_PUB2_DIR_DEB) && "$(TARGET_PUB2_BASE_DEB)" != ""
TARGET_PUB2_DIR_DEB  = $(TARGET_PUB2_BASE_DEB)\$(TARGET_PUB2_SUB)
! endif

# Default public name.
! ifndef TARGET_PUB2_NAME
TARGET_PUB2_NAME = $(TARGET_NAME)
! endif

# Default public ext.
! ifndef TARGET_PUB2_EXT
TARGET_PUB2_EXT  = $(TARGET_EXT)
! endif

# Default public full name.
! ifndef TARGET_PUB2
TARGET_PUB2      = $(TARGET_PUB2_DIR)\$(TARGET_PUB2_NAME).$(TARGET_PUB2_EXT)
! endif

# Default public full name of unstripped version.
! if !defined(TARGET_PUB2_DEB) && "$(TARGET_PUB2_DIR_DEB)" != ""
TARGET_PUB2_DEB  = $(TARGET_PUB2_DIR_DEB)\$(TARGET_PUB2_NAME).$(TARGET_PUB2_EXT)
! endif

!endif # pub2

# Default public book dirs
!ifndef TARGET_BOOK_PUB_BASE
TARGET_BOOK_PUB_BASE = $(PATH_PUB)
!endif
!ifndef TARGET_BOOK_PUB_SUB
TARGET_BOOK_PUB_SUB = $(PATH_SUB_BOOK)
!endif
!ifndef TARGET_BOOK_PUB_DIR
TARGET_BOOK_PUB_DIR = $(TARGET_BOOK_PUB_BASE)\$(TARGET_BOOK_PUB_SUB)
!endif

# Default public help dirs
!ifndef TARGET_HELP_PUB_BASE
TARGET_HELP_PUB_BASE = $(PATH_PUB)
!endif
!ifndef TARGET_HELP_PUB_SUB
TARGET_HELP_PUB_SUB = $(PATH_SUB_HELP)
!endif
!ifndef TARGET_HELP_PUB_DIR
TARGET_HELP_PUB_DIR = $(TARGET_HELP_PUB_BASE)\$(TARGET_HELP_PUB_SUB)
!endif

# Default public docs dirs
!ifndef TARGET_DOC_PUB_BASE
TARGET_DOC_PUB_BASE = $(PATH_PUB)
!endif
!ifndef TARGET_DOC_PUB_SUB
TARGET_DOC_PUB_SUB = $(PATH_SUB_DOC)
!endif
!ifndef TARGET_DOC_PUB_DIR
TARGET_DOC_PUB_DIR = $(TARGET_DOC_PUB_BASE)\$(TARGET_DOC_PUB_SUB)
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
# Internal helper macros - don't mess!
# -----------------------------------------------------------------------------

# Helper for doing early publish of a target.
!if defined(TARGET_NEEDED) || defined(TARGET_PUBLIC)
_TARGET_EARLY_PUBLISH = publish_target
!else
_TARGET_EARLY_PUBLISH =
!endif

# The unstripped release name (internal!)
_TARGET_DEB = $(TARGET).unstripped



# -----------------------------------------------------------------------------
# Tell user what we're building.
# -----------------------------------------------------------------------------
!ifndef BUILD_QUIET
! if "$(TARGET)" != ""
!  ifndef MAKEVER
!   if [$(ECHO) Target is $(CLRFIL)$(TARGET)$(CLRRST)]
!   endif
!  else
$(ECHO) Target is $(CLRFIL)$(TARGET)$(CLRRST)
!  endif
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
# Make all targets 'unprecious'. (works only on nmake5)
# -----------------------------------------------------------------------------
.PRECIOUS: .force


# -----------------------------------------------------------------------------
# Common inference rules
# -----------------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .c .cpp .orc .asm .$(EXT_OBJ) .$(EXT_RES) .rc .ii .s .ipp .ipf .$(EXT_INF) .$(EXT_HLP)

#
# A workaround for SlickEdits inability to find the buggy files..
# This makes the source filenames in the error listing have full path.
# See setup.mak for compile command line.
#
#_SRC = $<
#!ifdef SLKRUNS
_SRC = $(PATH_CURRENT)\$(<F)
#!endif
_DST = $(PATH_TARGET)\$(@F)


# Assembling assembly source.
.asm{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG) $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
! ifdef AS_CMD_SYS
    $(TOOL_JOB_SUB) $(AS_CMD_SYS)
! else
    $(TOOL_JOB_SUB) $(AS) $(AS_FLAGS_SYS) $(_SRC) $(AS_OBJ_OUT)$(_DST)
! endif
!else
! ifdef AS_CMD
    $(TOOL_JOB_SUB) $(AS_CMD)
! else
    $(TOOL_JOB_SUB) $(AS) $(AS_FLAGS) $(_SRC) $(AS_OBJ_OUT)$(_DST)
! endif
!endif

.asm.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
! ifdef AS_CMD_SYS
    $(AS_CMD_SYS)
! else
    $(AS) $(AS_FLAGS_SYS) $(_SRC) $(AS_OBJ_OUT)$(_DST)
! endif
!else
! ifdef AS_CMD
    $(AS_CMD)
! else
    $(AS) $(AS_FLAGS) $(_SRC) $(AS_OBJ_OUT)$(_DST)
! endif
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
        $(CXX_OBJ_OUT)$(_DST) $(_SRC)

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
        $(CXX_OBJ_OUT)$(_DST) $(_SRC)


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
!if "$(TARGET_MODE)" == "VDD"
        $(CC_FLAGS_VDD) \
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
        $(CC_OBJ_OUT)$(_DST) $(_SRC)

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
        $(CC_OBJ_OUT)$(_DST) $(_SRC)


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
    $(TOOL_JOB_SUB) $(RC) $(RC_FLAGS) $(_SRC) $(RC_RES_OUT)$(_DST)

.rc.res:
    @$(ECHO) RC Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(RC) $(RC_FLAGS) $(_SRC) $(RC_RES_OUT)$(_DST)


# Compiling Odin32 resources.
.orc{$(PATH_TARGET)}.obj:
    @$(ECHO) ORC Compiler $(CLRFIL)$(_SRC) $(CLRTXT)$(TOOL_JOB_SUB_MSG)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(ORC) $(ORC_FLAGS) $(ORC_ASM_OUT)$(PATH_TARGET)\$(@B).asm $(_SRC)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(AS) $(AS_FLAGS) $(PATH_TARGET)\$(@B).asm $(AS_OBJ_OUT)$(_DST)

.orc.obj:
    @$(ECHO) ORC Compiler $(CLRFIL)$(_SRC) $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(ORC) $(ORC_FLAGS) $(ORC_ASM_OUT)$(PATH_TARGET)\$(@B).asm $(_SRC)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(AS) $(AS_FLAGS) $(PATH_TARGET)\$(@B).asm $(AS_OBJ_OUT)$(_DST)


# Compiling INFs.
.ipf{$(PATH_TARGET)}.$(EXT_INF):
    @$(ECHO) BOOK Compiler $(CLRFIL)$(_SRC)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(INFC_CMD)

.ipf.inf:
    @$(ECHO) BOOK Compiler $(CLRFIL)$(_SRC)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(INFC_CMD)


# Compiling INFs with pre-compiling
.ipp{$(PATH_TARGET)}.$(EXT_INF):
    @$(ECHO) CC Pre + BOOK Compiler $(CLRFIL)$(_SRC)$(CLRRST)
!ifndef CC_PC_2_STRIPPED_STDOUT
    $(TOOL_ECHO) $(CLRERR)fatal error: CC_PC_2_STRIPPED_STDOUT isn't supported on this compiler. $(CLRRST) && false
!endif
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(INFC_CMD_PRE)

.ipp.inf:
    @$(ECHO) CC Pre + BOOK Compiler $(CLRFIL)$(_SRC)$(CLRRST)
!ifndef CC_PC_2_STRIPPED_STDOUT
    $(TOOL_ECHO) $(CLRERR)fatal error: CC_PC_2_STRIPPED_STDOUT isn't supported on this compiler. $(CLRRST) && false
!endif
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(INFC_CMD_PRE)


# Compiling HLPs
.ipf{$(PATH_TARGET)}.$(EXT_HLP):
    @$(ECHO) HELP Compiler $(CLRFIL)$(_SRC)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(HLPC_CMD)

.ipf.hlp:
    @$(ECHO) HELP Compiler $(CLRFIL)$(_SRC)$(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(HLPC_CMD)


# Compiling HLPs with pre-compiling
.ipp{$(PATH_TARGET)}.$(EXT_HLP):
    @$(ECHO) CC Pre + HELP Compiler $(CLRFIL)$(_SRC)$(CLRRST)
!ifndef CC_PC_2_STRIPPED_STDOUT
    $(TOOL_ECHO) $(CLRERR)fatal error: CC_PC_2_STRIPPED_STDOUT isn't supported on this compiler. $(CLRRST) && false
!endif
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(HLPC_CMD_PRE)

.ipp.hlp:
    @$(ECHO) CC Pre + HELP Compiler $(CLRFIL)$(_SRC)$(CLRRST)
!ifndef CC_PC_2_STRIPPED_STDOUT
    $(TOOL_ECHO) $(CLRERR)fatal error: CC_PC_2_STRIPPED_STDOUT isn't supported on this compiler. $(CLRRST) && false
!endif
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(HLPC_CMD_PRE)



# -----------------------------------------------------------------------------
# The all rule - The default one, as it's the first rule in the file.
# -----------------------------------------------------------------------------
all: build



# -----------------------------------------------------------------------------
# The build rule - This runs all passes:
#   1. Make Dependencies
#   2. Make Needed Tools (stuff required in the next steps)
#   3. Make Libraries (all kinds)
#   4. Make Binaries (ie. executable files)
#   5. Make Miscellaneous Targets
#   6. Make Install
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
        _build_banner_needed        needed \
        _build_banner_lib           lib \
        _build_banner_binary        binary \
        _build_banner_misc          misc \
        _build_banner_publish       publish


# Banners for rebuild and build.
_build_banner_clean:
    @$(ECHO)$(CLRMAK)[Start Pass 0 - Make Clean] $(CLRRST)
    @SET _BUILD_PASS=0
_build_banner_dep:
    @$(ECHO)$(CLRMAK)[Start Pass 1 - Make Dependencies] $(CLRRST)
    @SET _BUILD_PASS=1
_build_banner_needed:
    @$(ECHO)$(CLRMAK)[Start Pass 2 - Make Needed Tools] $(CLRRST)
    @SET _BUILD_PASS=2
_build_banner_lib:
    @$(ECHO)$(CLRMAK)[Start Pass 3 - Make Libraries] $(CLRRST)
    @SET _BUILD_PASS=3
_build_banner_binary:
    @$(ECHO)$(CLRMAK)[Start Pass 4 - Make Binaries] $(CLRRST)
    @SET _BUILD_PASS=4
_build_banner_misc:
    @$(ECHO)$(CLRMAK)[Start Pass 5 - Make Miscellaneous Targets] $(CLRRST)
    @SET _BUILD_PASS=5
_build_banner_publish:
    @$(ECHO)$(CLRMAK)[Start Pass 6 - Make Public (i.e. Publish)] $(CLRRST)
    @SET _BUILD_PASS=6



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
# A number of shortcuts for lazy programmers.
# -----------------------------------------------------------------------------
pass1:  build

pass2:  _build_new_dependencies_

pass3:  _build_banner_lib           lib \
        _build_banner_binary        binary \
        _build_banner_misc          misc \
        _build_banner_publish       publish

pass4:  _build_banner_binary        binary \
        _build_banner_misc          misc \
        _build_banner_publish       publish

pass5:  _build_banner_misc          misc \
        _build_banner_publish       publish

pass6:  _build_banner_publish       publish

quick:  _build_banner_lib           lib \
        _build_banner_binary        binary \
        _build_banner_publish       publish



# -----------------------------------------------------------------------------
# Pass 0 - The clean rule - Clean up output files.
#   The current setup doesn't clean the published ones.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" != "TESTCASE"
clean:
    @$(ECHO) Cleaning... $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_RM) @<<
"$(PATH_TARGET)\*.$(EXT_OBJ)"
"$(PATH_TARGET)\*.$(EXT_ILIB)"
"$(PATH_TARGET)\*.$(EXT_EXE)"
"$(PATH_TARGET)\*.$(EXT_DLL)"
"$(PATH_TARGET)\*.$(EXT_RES)"
"$(PATH_TARGET)\*.$(EXT_SYS)"
"$(PATH_TARGET)\*.$(EXT_LIB)"
"$(PATH_TARGET)\*.$(EXT_IFS)"
"$(PATH_TARGET)\*.$(EXT_MAP)"
"$(PATH_TARGET)\*.$(EXT_SYM)"
"$(PATH_TARGET)\*.$(EXT_DEF)"
"$(PATH_TARGET)\*.s"
"$(PATH_TARGET)\*.lst"
"$(PATH_TARGET)\*.lnk"
"$(PATH_TARGET)\*.ii"
"$(PATH_TARGET)\.depend"
"$(TARGET_DEPEND)"
.\*.ii
.\*.err
.\.depend
<<
!ifdef SUBDIRS_CLEAN
    @$(TOOL_DODIRS) "$(SUBDIRS_CLEAN)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
!else
! if "$(SUBDIRS)$(SUBDIRS_NEEDED)$(SUBDIRS_LIB)$(SUBDIRS_BINARY)$(SUBDIRS_MISC)" != ""
    @$(TOOL_DODIRS) "$(SUBDIRS) $(SUBDIRS_NEEDED) $(SUBDIRS_LIB) $(SUBDIRS_BINARY) $(SUBDIRS_MISC)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
! endif
!endif
!ifdef PREMAKEFILES_CLEAN
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_CLEAN)" $(TOOL_MAKE) NODEP=1 $@
!else
! if "$(PREMAKEFILES)$(PREMAKEFILES_NEEDED)$(PREMAKEFILES_LIB)$(PREMAKEFILES_BINARY)$(PREMAKEFILES_MISC)" != ""
    @$(TOOL_DOMAKES) "$(PREMAKEFILES) $(PREMAKEFILES_NEEDED) $(PREMAKEFILES_LIB) $(PREMAKEFILES_BINARY) $(PREMAKEFILES_MISC)" $(TOOL_MAKE) NODEP=1 $@
! endif
!endif
!ifdef POSTMAKEFILES_CLEAN
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_CLEAN)" $(TOOL_MAKE) NODEP=1 $@
!else
! if "$(POSTMAKEFILES)$(POSTMAKEFILES_NEEDED)$(POSTMAKEFILES_LIB)$(POSTMAKEFILES_BINARY)$(POSTMAKEFILES_MISC)" != ""
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES) $(POSTMAKEFILES_NEEDED) $(POSTMAKEFILES_LIB) $(POSTMAKEFILES_BINARY) $(POSTMAKEFILES_MISC)" $(TOOL_MAKE) NODEP=1 $@
! endif
!endif

!endif #!TESTCASE



# -----------------------------------------------------------------------------
# Pass 1 - The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
!if "$(TARGET_MODE)" != "EMPTY" && "$(TARGET_MODE)" != "TESTCASE"
    @$(ECHO) Making dependencies... $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_DEP) \
! ifdef TARGET_NO_DEP
        -x$(TARGET_NO_DEP: =;)\
! endif
        @<<$(TARGET_DEPEND).rsp
$(TOOL_DEP_FLAGS)
-o$$(PATH_TARGET)
-d$(TARGET_DEPEND)
-srcadd AS:$$(MAKE_INCLUDE_AS_SETUP)
-srcadd AS:$$(MAKE_INCLUDE_AS_OPT)
-srcadd RC:$$(MAKE_INCLUDE_RC_SETUP)
-srcadd ORC:$$(MAKE_INCLUDE_ORC_SETUP)
-srcadd CX:$$(MAKE_INCLUDE_SETUP)
-srcadd CX:$$(MAKE_INCLUDE_CX_OPT)
-srcadd IPF:$$(MAKE_INCLUDE_IPFC)
-srcadd IPP:$$(MAKE_INCLUDE_IPFC)
-srcadd IPP:$$(MAKE_INCLUDE_SETUP)
-srcadd IPP:$$(MAKE_INCLUDE_CX_OPT)
$(TOOL_DEP_FILES)
<<KEEP
!ifdef BUILD_VERBOSE
	@type $(TARGET_DEPEND).rsp
!endif
!ifdef BUILD_VERBOSE2
    $(ECHO) Dependcy file: $(CLRRST)
    @type $(TARGET_DEPEND)
!endif
!endif
!ifdef SUBDIRS_DEP
    @$(TOOL_DODIRS) "$(SUBDIRS_DEP)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
!else
! if "$(SUBDIRS)$(SUBDIRS_NEEDED)$(SUBDIRS_LIB)$(SUBDIRS_BINARY)$(SUBDIRS_MISC)" != ""
    @$(TOOL_DODIRS) "$(SUBDIRS) $(SUBDIRS_NEEDED) $(SUBDIRS_LIB) $(SUBDIRS_BINARY) $(SUBDIRS_MISC)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) NODEP=1 $@
! endif
!endif
!ifdef PREMAKEFILES_DEP
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_DEP)" $(TOOL_MAKE) NODEP=1 $@
!else
! if "$(PREMAKEFILES)$(PREMAKEFILES_NEEDED)$(PREMAKEFILES_LIB)$(PREMAKEFILES_BINARY)$(PREMAKEFILES_MISC)" != ""
    @$(TOOL_DOMAKES) "$(PREMAKEFILES) $(PREMAKEFILES_NEEDED) $(PREMAKEFILES_LIB) $(PREMAKEFILES_BINARY) $(PREMAKEFILES_MISC)" $(TOOL_MAKE) NODEP=1 $@
! endif
!endif
!ifdef POSTMAKEFILES_DEP
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_DEP)" $(TOOL_MAKE) NODEP=1 $@
!else
! if "$(POSTMAKEFILES)$(POSTMAKEFILES_NEEDED)$(POSTMAKEFILES_LIB)$(POSTMAKEFILES_BINARY)$(POSTMAKEFILES_MISC)" != ""
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES) $(POSTMAKEFILES_NEEDED) $(POSTMAKEFILES_LIB) $(POSTMAKEFILES_BINARY) $(POSTMAKEFILES_MISC)" $(TOOL_MAKE) NODEP=1 $@
! endif
!endif



# -----------------------------------------------------------------------------
# Pass 2 - The needed rule - Make need tools.
#   That is tools and stuff that is required for the next passes.
#   WARNING! These tools and stuff shouldn't rely on libraries and other
#            results of later passes.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS_NEEDED
_SUBDIRS_NEEDED = _subdir_needed
$(_SUBDIRS_NEEDED):
    @$(TOOL_DODIRS) "$(SUBDIRS_NEEDED)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) needed
!else
! ifdef SUBDIRS
_SUBDIRS_NEEDED = _subdir_needed
$(_SUBDIRS_NEEDED):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) needed
! endif
!endif

!ifdef PREMAKEFILES_NEEDED
_PREMAKEFILES_NEEDED = _premakefiles_needed
$(_PREMAKEFILES_NEEDED):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_NEEDED)" $(TOOL_MAKE) needed
!else
! ifdef PREMAKEFILES
_PREMAKEFILES_NEEDED = _premakefiles_needed
$(_PREMAKEFILES_NEEDED):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) needed
! endif
!endif

!ifdef TARGET_NEEDED
needed:    $(_SUBDIRS_NEEDED) $(_PREMAKEFILES_NEEDED) target
!else
needed:    $(_SUBDIRS_NEEDED) $(_PREMAKEFILES_NEEDED)
!endif
!ifdef POSTMAKEFILES_NEEDED
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_NEEDED)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif



# -----------------------------------------------------------------------------
# Pass 3 - The lib rule - Make libraries.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS_LIB
_SUBDIRS_LIB = _subdir_lib
$(_SUBDIRS_LIB):
    @$(TOOL_DODIRS) "$(SUBDIRS_LIB)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) lib
!else
! ifdef SUBDIRS
_SUBDIRS_LIB = _subdir_lib
$(_SUBDIRS_LIB):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) lib
! endif
!endif

!ifdef PREMAKEFILES_LIB
_PREMAKEFILES_LIB = _premakefiles_lib
$(_PREMAKEFILES_LIB):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_LIB)" $(TOOL_MAKE) lib
!else
! ifdef PREMAKEFILES
_PREMAKEFILES_LIB = _premakefiles_lib
$(_PREMAKEFILES_LIB):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) lib
! endif
!endif

lib:    $(_SUBDIRS_LIB) $(_PREMAKEFILES_LIB) \
!if !defined(TARGET_NEEDED) && ("$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB")
        $(TARGET) $(_TARGET_EARLY_PUBLISH) \
!endif
        $(TARGET_ILIB)
!ifdef POSTMAKEFILES_LIB
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_LIB)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif



# -----------------------------------------------------------------------------
# Pass 4 - The binary rule - Build the binary.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS_BINARY
_SUBDIRS_BINARY = _subdir_binary
$(_SUBDIRS_BINARY):
    @$(TOOL_DODIRS) "$(SUBDIRS_BINARY)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) binary
!else
! ifdef SUBDIRS
_SUBDIRS_BINARY = _subdir_binary
$(_SUBDIRS_BINARY):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) binary
! endif
!endif

!ifdef PREMAKEFILES_BINARY
_PREMAKEFILES_BINARY = _premakefiles_binary
$(_PREMAKEFILES_BINARY):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_BINARY)" $(TOOL_MAKE) binary
!else
! ifdef PREMAKEFILES
_PREMAKEFILES_BINARY = _premakefiles_binary
$(_PREMAKEFILES_BINARY):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) binary
! endif
!endif

binary executable: \
!if "$(TARGET_MODE)" != "LIB" && "$(TARGET_MODE)" != "SYSLIB" && "$(TARGET_MODE)" != "IFSLIB" && !defined(TARGET_NEEDED)
        $(_SUBDIRS_BINARY) $(_PREMAKEFILES_BINARY) $(TARGET) $(_TARGET_EARLY_PUBLISH) $(OTHERS)
!else
        $(_SUBDIRS_BINARY) $(_PREMAKEFILES_BINARY)
!endif
!ifdef POSTMAKEFILES_BINARY
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_BINARY)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif



# -----------------------------------------------------------------------------
# Pass 5 - The misc(ellaneous) rule - Makes miscellaneous stuff like
#   help, documentations etc. This is experimental for the moment.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS_MISC
_SUBDIRS_MISC = _subdir_misc
$(_SUBDIRS_MISC):
    @$(TOOL_DODIRS) "$(SUBDIRS_MISC)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) misc
!else
! ifdef SUBDIRS
_SUBDIRS_MISC = _subdir_misc
$(_SUBDIRS_MISC):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) misc
! endif
!endif

!ifdef PREMAKEFILES_MISC
_PREMAKEFILES_MISC = _premakefiles_misc
$(PREMAKEFILES_MISC):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_MISC)" $(TOOL_MAKE) misc
!else
! ifdef PREMAKEFILES
_PREMAKEFILES_MISC = _premakefiles_misc
$(_PREMAKEFILES_MISC):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) misc
! endif
!endif

misc miscellaneous: $(_SUBDIRS_MISC) $(_PREMAKEFILES_MISC) \
                    $(TARGET_BOOKS) $(TARGET_HELPS) $(TARGET_DOCS) $(TARGET_MISC)
!if "$(TARGET_BOOKS)$(TARGET_HELPS)$(TARGET_DOCS)$(TARGET_MISC)" != ""
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET_BOOKS) $(TARGET_HELPS) $(TARGET_DOCS) $(TARGET_MISC)$(CLRRST)
!endif
!ifdef POSTMAKEFILES_MISC
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_MISC)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif



# -----------------------------------------------------------------------------
# Pass 6 - The publish rule - Copies target to main binary directory.
#   Installation order is not concidered vital, so subdirectories and
#   pre-makefiles are processed after this directory.
#   However, this order might be changed!
# -----------------------------------------------------------------------------
!if "$(_TARGET_EARLY_PUBLISH)" != ""
publish:
!else
publish: publish_target
!endif
!ifdef SUBDIRS_PUBLISH
    @$(TOOL_DODIRS) "$(SUBDIRS_PUBLISH)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!else
! ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
! endif
!endif
!ifdef PREMAKEFILES_PUBLISH
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_PUBLISH)" $(TOOL_MAKE) $@
!else
! ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif
!ifdef POSTMAKEFILES_PUBLISH
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_PUBLISH)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif


#
# Publish rule which doesn't go down into subdirs and submakes.
# This one is invoked when a target is 'needed' or early published.
#
publish_target:
!if ( "$(_TARGET_EARLY_PUBLISH)" != "" || "$(TARGET_MODE:LIB=cute)" == "$(TARGET_MODE)" ) && "$(TARGET_PRIVATE)" == ""
! if "$(TARGET_MODE)" != "EMPTY" && "$(TARGET_MODE)" != "TESTCASE" && "$(TARGET_MODE)" != "DEPEND"
    @$(ECHO) Publishing $(CLRFIL)$(TARGET)$(CLRTXT) to directory $(CLRFIL)$(TARGET_PUB_DIR)$(CLRRST)
    @if not exist "$(TARGET)"           $(TOOL_ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist "$(TARGET_PUB_DIR)"   $(TOOL_CREATEPATH) $(TARGET_PUB_DIR)
    @if exist "$(TARGET)"               $(TOOL_COPY) "$(TARGET)"      "$(TARGET_PUB)"
    @if exist "$(TARGET_SYM)"           $(TOOL_COPY) "$(TARGET_SYM)"  "$(TARGET_PUB_DIR)"
!  ifdef PUBLISH_MAP
    @if exist "$(TARGET_MAP)"           $(TOOL_COPY) "$(TARGET_MAP)"  "$(TARGET_PUB_DIR)"
!  endif
!  if "$(TARGET_PUB_DEB)" != ""
    @$(ECHO) Publishing unstripped $(CLRFIL)$(TARGET)$(CLRTXT) to directory $(CLRFIL)$(TARGET_PUB_DIR_DEB)$(CLRRST)
    @if not exist "$(_TARGET_DEB)"      $(TOOL_ECHO) $(CLRERR)WARNING: $(CLRFIL)$(_TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist "$(TARGET_PUB_DIR_DEB)" $(TOOL_CREATEPATH)  $(TARGET_PUB_DIR_DEB)
    @if exist "$(_TARGET_DEB)"          $(TOOL_COPY) "$(_TARGET_DEB)" "$(TARGET_PUB_DEB)"
    @if exist "$(TARGET_SYM)"           $(TOOL_COPY) "$(TARGET_SYM)"  "$(TARGET_PUB_DIR_DEB)"
!   ifdef PUBLISH_MAP
    @if exist "$(TARGET_MAP)"           $(TOOL_COPY) "$(TARGET_MAP)"  "$(TARGET_PUB_DIR_DEB)"
!   endif
!  endif
!  if "$(TARGET_PUB2)" != ""
    @$(ECHO) Publishing $(CLRFIL)$(TARGET)$(CLRTXT) to directory $(CLRFIL)$(TARGET_PUB2_DIR)$(CLRRST)
    @if not exist "$(TARGET)"           $(TOOL_ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist "$(TARGET_PUB2_DIR)"  $(TOOL_CREATEPATH) $(TARGET_PUB2_DIR)
    @if exist "$(TARGET)"               $(TOOL_COPY) "$(TARGET)"      "$(TARGET_PUB2)"
    @if exist "$(TARGET_SYM)"           $(TOOL_COPY) "$(TARGET_SYM)"  "$(TARGET_PUB2_DIR)"
!   ifdef PUBLISH_MAP
    @if exist "$(TARGET_MAP)"           $(TOOL_COPY) "$(TARGET_MAP)"  "$(TARGET_PUB2_DIR)"
!   endif
!   if "$(TARGET_PUB2_DEB)" != ""
    @$(ECHO) Publishing unstripped $(CLRFIL)$(TARGET)$(CLRTXT) to directory $(CLRFIL)$(TARGET_PUB2_DIR_DEB)$(CLRRST)
    @if not exist "$(_TARGET_DEB)"      $(TOOL_ECHO) $(CLRERR)WARNING: $(CLRFIL)$(_TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if not exist "$(TARGET_PUB2_DIR_DEB)" $(TOOL_CREATEPATH)  $(TARGET_PUB_DIR_DEB)
    @if exist "$(_TARGET_DEB)"          $(TOOL_COPY) "$(_TARGET_DEB)" "$(TARGET_PUB2_DEB)"
    @if exist "$(TARGET_SYM)"           $(TOOL_COPY) "$(TARGET_SYM)"  "$(TARGET_PUB2_DIR_DEB)"
!    ifdef PUBLISH_MAP
    @if exist "$(TARGET_MAP)"           $(TOOL_COPY) "$(TARGET_MAP)"  "$(TARGET_PUB2_DIR_DEB)"
!    endif
!   endif
!  endif
! endif
!endif
!if "$(TARGET_BOOKS)" != ""
    @$(ECHO) Publishing Book(s) $(CLRFIL)$(TARGET_BOOKS)$(CLRTXT) to directory $(CLRFIL)$(TARGET_BOOK_PUB_DIR)$(CLRRST)
    @if not exist "$(TARGET_BOOK_PUB_DIR)" $(TOOL_CREATEPATH) $(TARGET_BOOK_PUB_DIR)
    @$(TOOL_COPY) $(TARGET_BOOKS) $(TARGET_BOOK_PUB_DIR)
!endif
!if "$(TARGET_HELPS)" != ""
    @$(ECHO) Publishing Help file(s) $(CLRFIL)$(TARGET_HELPS)$(CLRTXT) to directory $(CLRFIL)$(TARGET_HELP_PUB_DIR)$(CLRRST)
    @if not exist "$(TARGET_HELP_PUB_DIR)" $(TOOL_CREATEPATH) $(TARGET_HELP_PUB_DIR)
    @$(TOOL_COPY) $(TARGET_HELPS) $(TARGET_HELP_PUB_DIR)
!endif
!if "$(TARGET_DOCS)" != ""
    @$(ECHO) Publishing Doc(s) $(CLRFIL)$(TARGET_DOCS)$(CLRTXT) to directory $(CLRFIL)$(TARGET_DOC_PUB_DIR)$(CLRRST)
    @if not exist "$(TARGET_DOC_PUB_DIR)" $(TOOL_CREATEPATH) $(TARGET_DOC_PUB_DIR)
    @$(TOOL_COPY) $(TARGET_DOCS) $(TARGET_DOC_PUB_DIR)
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
!ifdef SUBDIRS_TESTCASE
    @$(TOOL_DODIRS) "$(SUBDIRS_TESTCASE)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
!else
! ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) $@
! endif
!endif
!ifdef PREMAKEFILES_TESTCASE
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_TESTCASE)" $(TOOL_MAKE) $@
!else
! ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif
!ifdef POSTMAKEFILES_TESTCASE
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_TESTCASE)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif

!endif # BUILD_OWN_TESTCASE_RULE
!endif #!TESTCASE


# -----------------------------------------------------------------------------
# Pass x - The packing rule - traverse subdirs etc.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS_PACKING
_SUBDIRS_PACKING = _subdir_packing
$(_SUBDIRS_PACKING):
    @$(TOOL_DODIRS) "$(SUBDIRS_PACKING)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) packing
!else
! ifdef SUBDIRS
_SUBDIRS_PACKING = _subdir_packing
$(_SUBDIRS_PACKING):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) packing
! endif
!endif

!ifdef PREMAKEFILES_PACKING
_PREMAKEFILES_PACKING = _premakefiles_packing
$(PREMAKEFILES_PACKING):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES_PACKING)" $(TOOL_MAKE) packing
!else
! ifdef PREMAKEFILES
_PREMAKEFILES_PACKING = _premakefiles_packing
$(_PREMAKEFILES_PACKING):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) packing
! endif
!endif

!if "$(RULE_PACKING)" == "packing"
_packing: \
!else
packing: \
!endif
        $(_SUBDIRS_PACKING) $(_PREMAKEFILES_PACKING) $(RULE_PACKING)
!ifdef POSTMAKEFILES_PACKING
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES_PACKING)" $(TOOL_MAKE) $@
!else
! ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
! endif
!endif
    $(ECHO) .$(CLRRST)



# -----------------------------------------------------------------------------
# The target rule - Build the target.
#   NOTE! NO SUBDIRS OR POST/PREMAKED INVOLVED!
# -----------------------------------------------------------------------------
target: $(TARGET) $(TARGET_ILIB) $(_TARGET_EARLY_PUBLISH) $(OTHERS)
!if "$(TARGET)$(TARGET_ILIB)" != ""
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET) $(TARGET_ILIB)$(CLRRST)
!else
    @$(ECHO) .$(CLRRST)
!endif



# -----------------------------------------------------------------------------
# Others (experimental)
# -----------------------------------------------------------------------------
others: $(OTHERS)
!if "$(OTHERS)" != ""
    @$(ECHO) Successfully Built $(OTHERS)$(CLRRST)
!else
    @$(ECHO) .$(CLRRST)
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
$(TARGET): $(TARGET_OBJS) $(TARGET_RES) $(TARGET_DEF) $(TARGET_DEPS) $(TARGET_LIBS) \
           $(MAKEFILE) $(MAKE_INCLUDE_SETUP) $(MAKE_INCLUDE_LD_SETUP) $(MAKE_INCLUDE_LD_OPT) $(PATH_ROOT)\$(BUILD_SETUP_MAK) $(MAKE_INCLUDE_PROCESS) $(PATH_MAKE)\setup.tools.mk
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
!if "$(TARGET_DEF_LINK)" != "$(TARGET_DEF)"
! ifndef BUILD_QUIET
    @$(ECHO) Stamping deffile with build level info.$(CLRRST)
! endif
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BLDLEVEL) $(BUILD_BLDLEVEL_FLAGS) $(TARGET_BLDLEVEL_FLAGS) -R$(TARGET_DEF) $(TARGET_DEF) $(TARGET_DEF_LINK)
!endif
!ifndef TOOL_DEFCONV
    @$(TOOL_ECHOTXT) Creating Linker Input File $(CLRRST)<<$(TARGET_LNK)
$(LINK_LNK1)
$(LINK_LNK2)
$(LINK_LNK3)
$(LINK_LNK4)
$(LINK_LNK5)
<<KEEP
!else
    @$(ECHO) Creating Linker Input File $(CLRRST) $(TARGET_LNK)
    @$(TOOL_RM) "$(TARGET_LNK)"
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_DEFCONV) $(TARGET_DEF_LINK) $(TARGET_LNK) <<$(TARGET_LNK)2
$(LINK_LNK1)
$(LINK_LNK2)
$(LINK_LNK3)
$(LINK_LNK4)
$(LINK_LNK5)
<<keep
!endif
!ifdef BUILD_VERBOSE
    @type $(TARGET_LNK)
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
!if "$(TARGET_POSTLINKING_CMD)" != ""
	@$(ECHO) Executing custom postlinking step...$(CLRRST)
	$(TARGET_POSTLINKING_CMD)
!endif
!if "$(TARGET_RES)" != "" && "$(RL)" != ""
    @$(ECHO) Linking Resources $(CLRRST)
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(RL) $(RL_FLAGS) $(TARGET_RES) $(RL_EXE_OUT)$@
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
!if "$(BUILD_MODE)" == "RELEASE"
    \
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_COPY) "$@" "$(_TARGET_DEB)"
! if "$(TOOL_STRIP)" != ""
    \
!  ifndef BUILD_VERBOSE
    @ \
!  endif
    $(TOOL_STRIP) $@
! endif
!endif
!if "$(TARGET_POSTSTRIP_CMD)" != ""
	@$(ECHO) Executing custom poststrip step...$(CLRRST)
	$(TARGET_POSTSTRIP_CMD)
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
    @$(TOOL_RM) "$@"
    @$(AR_CMD)
!else
    $(TOOL_RM) "$@"
    $(AR_CMD)
!endif


#
# Lib parameter file.
#
$(TARGET_LNK):  $(MAKEFILE) $(MAKE_INCLUDE_SETUP) $(MAKE_INCLUDE_AR_SETUP) $(PATH_ROOT)\$(BUILD_SETUP_MAK)
    @$(TOOL_ECHOTXT) Creating Lib Input File $(CLRRST)<<$@
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
$(TARGET_ILIB): $(TARGET_IDEF) $(MAKEFILE) $(MAKE_INCLUDE_SETUP) $(MAKE_INCLUDE_AR_SETUP) $(PATH_MAKE)\setup.tools.mk
    @$(ECHO) Creating Import Library $(CLRFIL)$@ $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @if not exist $(@D) $(ECHO) Target implib path $(CLRFIL)$(@D)$(CLRTXT) does NOT exist. Creating. $(CLRRST)
!endif
    @if not exist $(@D) $(TOOL_CREATEPATH) $(@D)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(TARGET_IDEF)


#
# Conversion rule for converting the .def file before passing along to implib.
#
! if "$(TARGET_IDEF_ORG)" != "" && "$(TARGET_IDEF_ORG)" != "$(TARGET_IDEF)"
$(TARGET_IDEF): $(TARGET_IDEF_ORG)
    @$(ECHO) Making Import Definition File $(CLRFIL)$@ $(CLRRST)
    \
!ifndef BUILD_VERBOSE
    @if not exist $(@D) $(ECHO) Target .def path $(CLRFIL)$(@D)$(CLRTXT) does NOT exist. Creating. $(CLRRST)
!endif
    @if not exist $(@D) $(TOOL_CREATEPATH) $(@D)
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_IDEFCONV) $(TOOL_IDEFCONV_FLAGS) $(TARGET_IDEF_ORG) $@
! endif

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
#!  ifndef NODEP
#!   if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(TARGET_DEPEND) is missing.$(CLRRST)]
#!   endif
#!  endif
! endif
!else
! if %exists($(TARGET_DEPEND)) != 0
!  ifdef BUILD_VERBOSE
$(ECHO) Including dependency $(CLRFIL)$(TARGET_DEPEND)$(CLRRST)
!  endif
!  include $(TARGET_DEPEND)
! else
#!  ifndef NODEP
#$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(TARGET_DEPEND) is missing.$(CLRRST)
#!  endif
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


#
# Include system library dependency stubs.
#
!include $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)libs.mk


#
# Check if TARGET_ALWAYS needs to be forced built.
#
!if "$(TARGET_ALWAYS)" != ""
! ifndef BUILD_FORCED_BUILD_TEST
!  if [$(TOOL_MAKE) BUILD_FORCED_BUILD_TEST=1 -f $(MAKEFILE) -q $(TARGET) $(TARGET_ILIB) >nul 2>nul] != 0
$(TARGET_ALWAYS): .force
!  endif
! endif
!endif

