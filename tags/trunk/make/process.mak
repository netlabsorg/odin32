# $Id: process.mak,v 1.11 2002-04-30 19:50:40 bird Exp $

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
!if "$(MAKE_SETUP_INCLUDED)" != "YES"
!error Fatal error: You must include setup.mak before process.mak in the makefile.
!endif
!if "$(ENV_STATUS)" != "OK"
!error Fatal error: The environment is not valid. Bad setup.mak?
!endif

!if "$(TARGET_NAME)" == ""
!error Fatal error: TARGET_NAME is not defined! Should be set in the makefile.
!endif

!ifdef TARGET_MODE
# Executable target mode.
! if "$(TARGET_MODE)" != "EXE"
# Dynamic Load Library target mode.
!  if "$(TARGET_MODE)" != "DLL"
# Dynamic Load Library target mode - Special variant for making custom C/C++ runtime DLL.
!   if "$(TARGET_MODE)" != "CRT"
# Drive (/ system software) target mode.
!    if "$(TARGET_MODE)" != "SYS"
# Installable File System Drive target mode. (Also called FSD, File System Driver.)
!     if "$(TARGET_MODE)" != "IFS"
# Virtual Device Driver target mode.
!      if "$(TARGET_MODE)" != "VDD"
# Object Library target mode.
!       if "$(TARGET_MODE)" != "LIB"
# Object Library target mode - Special variant which is to be linked with a SYS target.
!        if "$(TARGET_MODE)" != "SYSLIB"
# Object Library target mode - Special variant which is to be linked with an IFS target.
!         if "$(TARGET_MODE)" != "IFSLIB"
# Dummy/Hub/TopLevel empty makefile. This has no target.
!          if "$(TARGET_MODE)" != "EMPTY"
!           error Error: Bad TARGET_MODE="$(TARGET_MODE)". Valid ones are: EXE, DLL, CRT, EXE, SYS, IFS, VDD, LIB, SYSLIB, IFSLIB and EMPTY.
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
! if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
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
! ifndef TARGET_EXT
!error Internal Error: TARGET_EXT not set. Probably invalid TARGET_MODE. (TARGET_MODE="$(TARGET_MODE)")
! endif
!endif

# Default target path. (where all the generated stuff for this target goes)
!ifndef PATH_TARGET
PATH_TARGET = $(PATH_OBJ)\$(TARGET_NAME).$(TARGET_EXT)
!endif

# Default target file. (output)
!ifndef TARGET
! if "$(TARGET_MODE)" != "EMPTY"
TARGET      = $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)
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
! if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
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
!  if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
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

# Default makefile name.
!ifndef MAKEFILE
MAKEFILE = makefile
!endif

# Ignore linker warnings for some target modes.
!ifndef TARGET_IGNORE_LINKER_WARNINGS
! if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
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
!if [$(ECHO) Target is $(CLRFIL)$(TARGET)$(CLRRST)]
!endif
!endif


# -----------------------------------------------------------------------------
# Ensure the platform-specific target path exists
# -----------------------------------------------------------------------------

!if "$(TARGET_MODE)" != "EMPTY"
! if "$(PATH_TARGET)" != ""
!  if [$(TOOL_EXISTS) $(PATH_TARGET)] != 0
!   ifndef BUILD_QUIET
!    if [$(ECHO) Target path $(CLRFIL)$(PATH_TARGET)$(CLRTXT) does NOT exist. Creating. $(CLRRST)]
!    endif
!   endif
!   if [$(TOOL_CREATEPATH) $(PATH_TARGET)]
!    error Could not create $(PATH_TARGET)
!   endif
!  endif
! endif
!endif
# not 100% sure about the != EMPTY stuff, but this is way faster.



# -----------------------------------------------------------------------------
# Common inference rules
# -----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .$(EXT_OBJ) .c .cpp .asm .$(EXT_RES) .rc .pre-c .pre-cpp # .h .def


# Assembling assembly source.
.asm{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    $(AS) $(AS_FLAGS_SYS) $< $(AS_OBJ_OUT)$@
!else
    $(AS) $(AS_FLAGS) $< $(AS_OBJ_OUT)$@
!endif

.asm.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    $(AS) $(AS_FLAGS_SYS) $< $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!else
    $(AS) $(AS_FLAGS) $< $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!endif


# Compiling C++ source.
.cpp{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) C++ Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CXX_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CXX_FLAGS_CRT) \
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
!if "$(CXX_LST_OUT)" != ""
        $(CXX_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CXX_OBJ_OUT)$@ $<

.cpp.$(EXT_OBJ):
    @$(ECHO) C++ Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CXX_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CXX_FLAGS_CRT) \
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
!if "$(CXX_LST_OUT)" != ""
        $(CXX_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CXX_OBJ_OUT)$(PATH_TARGET)\$(@F) $<


# Pre-Compiling C++ source.
.cpp.pre-cpp:
    @$(ECHO) C++ Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CXX_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CXX_FLAGS_CRT) \
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
        $(CXX_PC_2_STDOUT) $< > $@


# Compiling C source.
.c{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) C Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CC_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CC_FLAGS_CRT) \
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
!if "$(CC_LST_OUT)" != ""
        $(CC_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CC_OBJ_OUT)$@ $<

.c.$(EXT_OBJ):
    @$(ECHO) C Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CC_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CC_FLAGS_CRT) \
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
!if "$(CC_LST_OUT)" != ""
        $(CC_LST_OUT)$(PATH_TARGET)\$(@B).s \
!endif
        $(CC_OBJ_OUT)$(PATH_TARGET)\$(@F) $<


# Pre-Compiling C source.
.c.pre-c:
    @$(ECHO) C PreCompiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"
        $(CC_FLAGS_EXE) \
!endif
!if "$(TARGET_MODE)" == "CRT"
        $(CC_FLAGS_CRT) \
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
        $(CC_PC_2_STDOUT) $< > $@


# Compiling resources.
.rc{$(PATH_TARGET)}.res:
    @$(ECHO) RC Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(RC) $(RC_FLAGS) $< $@

.rc.res:
    @$(ECHO) RC Compiler $(CLRFIL)$< $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(RC) $(RC_FLAGS) $< $(PATH_TARGET)\$(@F)





# -----------------------------------------------------------------------------
# The all rule - The default one, as it's the first rule in the file.
# -----------------------------------------------------------------------------
all: build



# -----------------------------------------------------------------------------
# The build rule - Build the target.
#   Must take into account any subdirectories and makefiles which is is to be
#   made before and after the target. That makes it kind of messy, sorry.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_BUILD = subbuild
$(SUBDIRS_BUILD):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) build
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_BUILD = premakefiles_build
$(PREMAKEFILES_BUILD):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) build
!endif

build: $(SUBDIRS_BUILD) $(PREMAKEFILES_BUILD) $(TARGET) $(TARGET_ILIB) $(TARGET_PUBNAME)
    @$(ECHO) Successfully Built $(CLRFIL)$(TARGET) $(TARGET_ILIB)$(CLRRST)
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The lib rule - Make Public libraries.
#   Must take into account any subdirectories and makefiles which is is to be
#   made before and after the target. That makes it kind of messy, sorry.
# -----------------------------------------------------------------------------
!ifdef SUBDIRS
SUBDIRS_LIB = subdir_lib
$(SUBDIRS_LIB):
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) lib
!endif

!ifdef PREMAKEFILES
PREMAKEFILES_LIB = premakefiles_lib
$(PREMAKEFILES_LIB):
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) lib
!endif

!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
lib: $(SUBDIRS_LIB) $(TARGET_ILIB) $(TARGET) $(TARGET_PUBNAME)
!else
lib: $(SUBDIRS_LIB) $(TARGET_ILIB)
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The install rule - Copies target to main binary directory.
#   Installation order is not concidered vital, so subdirectories and
#   pre-makefiles are processed after this directory. This might be changed.
# -----------------------------------------------------------------------------
install:
!if "$(TARGET_PUBLIC)" == ""
! if "$(TARGET_MODE)" == "EXE"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_BIN)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_BIN)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_BIN)
! endif
! if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_DLL)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_DLL)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_DLL)
! endif
! if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_SYS)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_SYS)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_SYS)
! endif
!if 1 # these targets are either TARGET_PUBLIC or all private.
!  if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
    @$(ECHO) Installing $(CLRFIL)$(TARGET)$(CLRTXT) in directory $(CLRFIL)$(PATH_LIB)$(CLRRST)
    @if not exist $(TARGET) $(ECHO) $(CLRERR)WARNING: $(CLRFIL)$(TARGET)$(CLRERR) doesn't exist. $(CLRRST)
    @if exist $(TARGET)     $(TOOL_COPY) $(TARGET)     $(PATH_LIB)
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(PATH_LIB)
!  endif
! endif
!endif
!if "$(TARGET_DOCS)" != ""
    $(TOOL_COPY) $(TARGET_DOCS) $(PATH_DOC)
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The testcase rule - Execute testcases when present.
#   Testcases are either a testcase.mak file or a testcase subdirectory.
# -----------------------------------------------------------------------------
!ifndef BUILD_OWN_TESTCASE_RULE
testcase:
!if [$(TOOL_EXISTS) testcase] == 0
    @$(TOOL_DODIRS) "testcase" $(TOOL_MAKE) $@
!endif
!if [$(TOOL_EXISTS) testcase.mak] == 0
    @$(TOOL_DOMAKES) "testcase.mak" $(TOOL_MAKE) $@
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif
!endif



# -----------------------------------------------------------------------------
# The shell rule - Setup the correcte shell environment and start a shell.
# -----------------------------------------------------------------------------
shell:
!ifndef BUILD_VERBOSE
    @ \
!endif
    -$(TOOL_BUILDENV) $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(ENV_ENVS) \
        $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST) * $(COMSPEC)



# -----------------------------------------------------------------------------
# The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
    @$(ECHO) Building dependencies $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_DEP) $(TOOL_DEP_FLAGS) -o$$(PATH_TARGET) -d$(TARGET_DEPEND)\
!ifdef TARGET_NO_DEP
        -x$(TARGET_NO_DEP: =;)\
!endif
        $(TOOL_DEP_FILES)
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) NODEP=1 $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) NODEP=1 $@
!endif



# -----------------------------------------------------------------------------
# The clean rule - Clean up output files.
#   The current setup doesn't clean the installed ones.
# -----------------------------------------------------------------------------
clean:
!if "$(PATH_TARGET)" != ""              # paranoia
    $(TOOL_RM) \
        $(PATH_TARGET)\*.$(EXT_OBJ) \
        $(PATH_TARGET)\*.$(EXT_ILIB) \
        $(PATH_TARGET)\*.$(EXT_EXE) \
        $(PATH_TARGET)\*.$(EXT_DLL) \
        $(PATH_TARGET)\*.$(EXT_RES)
    $(TOOL_RM) \
        $(PATH_TARGET)\*.$(EXT_SYS) \
        $(PATH_TARGET)\*.$(EXT_LIB) \
        $(PATH_TARGET)\*.$(EXT_IFS) \
        $(PATH_TARGET)\*.$(EXT_MAP) \
        $(PATH_TARGET)\*.$(EXT_SYM)
    $(TOOL_RM) \
        $(PATH_TARGET)\*.s \
        $(PATH_TARGET)\*.lst \
        $(PATH_TARGET)\*.lnk \
        $(PATH_TARGET)\*.pre-c \
        $(PATH_TARGET)\*.pre-cpp \
        $(PATH_TARGET)\.depend
!endif
!ifdef SUBDIRS
    @$(TOOL_DODIRS) "$(SUBDIRS)" $(TOOL_MAKE) $@
!endif
!ifdef PREMAKEFILES
    @$(TOOL_DOMAKES) "$(PREMAKEFILES)" $(TOOL_MAKE) $@
!endif
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EXE, DLL, SYS and IFS targets
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
$(TARGET): $(TARGET_OBJS) $(TARGET_RES) $(TARGET_DEF_LINK) $(TARGET_LNK) $(TARGET_DEPS)
    @$(ECHO) Linking $(TARGET_MODE) $(CLRFIL)$@ $(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
!ifdef TARGET_IGNORE_LINKER_WARNINGS
    -4 \
!endif
!if "$(TARGET_MODE)" == "EXE"
    $(LINK_CMD_EXE)
!endif
!if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
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
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(RL) $(RL_FLAGS) $(TARGET_RES) $@
!endif
!if "$(TARGET_DLLRNAME)" != ""
    @$(ECHO) Dll Rename $(TARGET_DLLRNAME)
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_DLLRNAME) $(TARGET) $(TARGET_DLLRNAME)
!endif
!if "$(TOOL_MAPSYM)" != "" && "$(TARGET_SYM)" != "" && "$(TARGET_MAP)" != ""
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_MAPSYM) $(TARGET_MAP) $(TARGET_SYM)
!endif


#
# Linker parameter file.
#
$(TARGET_LNK): $(MAKE_INCLUDE_PROCESS) $(MAKE_INCLUDE_SETUP) $(PATH_MAKE)\setup.mak $(MAKEFILE)
    @$(ECHO) Creating Linker Input File $(CLRRST)<<$@
$(LINK_LNK1)
$(LINK_LNK2)
$(LINK_LNK3)
$(LINK_LNK4)
$(LINK_LNK5)
<<KEEP
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
    @$(ECHO) Creating Lib Input File $(CLRRST)<<$@
$(AR_LNK1)
$(AR_LNK2)
$(AR_LNK3)
$(AR_LNK4)
$(AR_LNK5)
<<KEEP
!endif


#
# Copy rule for public targets.
#
!if "$(TARGET_PUBNAME)" != ""
$(TARGET_PUBNAME): $(TARGET)
    @$(ECHO) Copying $(CLRFIL)$(TARGET)$(CLRTXT) to $(CLRFIL)$(@D)$(CLRRST)
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_COPY) $** $@
    @if exist $(TARGET_SYM) $(TOOL_COPY) $(TARGET_SYM) $(@R).sym
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EMPTY targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EMPTY"
#$(TARGET):
#    @$(ECHO) .
!endif



# -----------------------------------------------------------------------------
# The $(TARGET_ILIB) rule - Make import library.
# -----------------------------------------------------------------------------
!ifdef TARGET_ILIB
$(TARGET_ILIB): $(TARGET_IDEF)
    @$(ECHO) Creating Import Library $(CLRFIL)$@ $(CLRRST)
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

!if "$(TARGET_MODE)" != "EMPTY"

#
# Read dependency file for current directory
#
!if [$(TOOL_EXISTS) $(TARGET_DEPEND)] == 0
! ifndef BUILD_QUIET
!  if [$(ECHO) Including dependency $(CLRFIL)$(TARGET_DEPEND)$(CLRRST)]
!  endif
! endif
! include $(TARGET_DEPEND)
!else
! ifndef NODEP
!   if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(TARGET_DEPEND) is missing.$(CLRRST)]
!   endif
! endif
!endif


#
# Read global dependency files.
#
!ifdef BUILD_DEPEND1
! if [$(TOOL_EXISTS) $(BUILD_DEPEND1)] == 0
!  ifndef BUILD_QUIET
!   if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND1)$(CLRRST)]
!   endif
!  endif
!  include $(BUILD_DEPEND1)
! else
!  ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND1) is missing.$(CLRRST)]
!    endif
!  endif
! endif
!endif

!ifdef BUILD_DEPEND2
! if [$(TOOL_EXISTS) $(BUILD_DEPEND2)] == 0
!  ifndef BUILD_QUIET
!   if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND2)$(CLRRST)]
!   endif
!  endif
!  include $(BUILD_DEPEND2)
! else
!  ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND2) is missing.$(CLRRST)]
!    endif
!  endif
! endif
!endif


!ifdef BUILD_DEPEND3
! if [$(TOOL_EXISTS) $(BUILD_DEPEND3)] == 0
!  ifndef BUILD_QUIET
!   if [$(ECHO) Including dependency $(CLRFIL)$(BUILD_DEPEND3)$(CLRRST)]
!   endif
!  endif
!  include $(BUILD_DEPEND3)
! else
!  ifndef NODEP
!    if [$(ECHO) $(CLRERR)WARNING: Please make dependencies first. $(BUILD_DEPEND3) is missing.$(CLRRST)]
!    endif
!  endif
! endif
!endif


!endif

