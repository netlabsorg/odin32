# $Id: process.mak,v 1.3 2002-04-13 04:39:59 bird Exp $

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
!error Fatal error: The enironment is not valid. Bad setup.mak?
!endif

!if "$(TARGET_NAME)" == ""
!error Fatal error: TARGET_NAME is not defined! Should be set in the makefile.
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
! if "$(TARGET_MODE)" == "CRT" || "$(TARGET_MODE)" == "DLL"
TARGET_EXT  = $(EXT_DLL)
! endif
! if "$(TARGET_MODE)" == "SYS"
TARGET_EXT  = $(EXT_SYS)
! endif
! if "$(TARGET_MODE)" == "EXE"
TARGET_EXT  = $(EXT_EXE)
! endif
! if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB" || "$(TARGET_MODE)" == "SYSLIB"
TARGET_EXT  = $(EXT_LIB)
! endif
! if "$(TARGET_MODE)" == "EMPTY"
TARGET_EXT  = empty
! endif
! ifndef TARGET_EXT
!error Error: TARGET_EXT not set
! endif
!endif

# Default target path. (where all the generated stuff for this target goes)
!ifndef PATH_TARGET
PATH_TARGET = $(PATH_OBJ)\$(TARGET_NAME).$(TARGET_EXT)
!endif

# Default target file. (output)
!ifndef TARGET
TARGET      = $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)
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

# Default definition file for generating the import library. (input)
!ifndef TARGET_IDEF
TARGET_IDEF = $(TARGET_DEF)
!endif

# Default map file. (output)
!ifndef TARGET_MAP
TARGET_MAP  = $(PATH_TARGET)\$(TARGET_NAME).map
!endif

# Default link file. (output)
!ifndef TARGET_LNK
TARGET_LNK  = $(PATH_TARGET)\$(TARGET_NAME).lnk
!endif

# Default import library file. (output)
!ifndef TARGET_ILIB
! if "$(TARGET_MODE)" == "CRT" || "$(TARGET_MODE)" == "DLL"
TARGET_ILIB =$(PATH_LIB)\$(TARGET_NAME).$(EXT_ILIB)
! endif
!endif

# Default public library name. (output)
!ifndef TARGET_PUBLIB
! if "$(TARGET_MODE)" == "PUBLIB"
TARGET_PUBLIB=$(PATH_LIB)\$(TARGET_NAME).$(TARGET_EXT)
! else
TARGET_PUBLIB=
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
#
# BUGBUG/TODO/FIXME
# kso:  Not sure if this is the *right* way to represent it!
#       It can't be when we start changing it based on the
#       current build environment.
!ifndef TARGET_STACKSIZE
# check if 16-bit target compiler
! if "$(BUILD_ENV)" == "MSCV6"
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
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    @$(AS) $(AS_FLAGS_SYS) $< $(AS_OBJ_OUT)$@
!else
    @$(AS) $(AS_FLAGS) $< $(AS_OBJ_OUT)$@
!endif

.asm.$(EXT_OBJ):
    @$(ECHO) Assembling $(CLRFIL)$< $(CLRRST)
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    @$(AS) $(AS_FLAGS_SYS) $< $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!else
    @$(AS) $(AS_FLAGS) $< $(AS_OBJ_OUT)$(PATH_TARGET)\$(@F)
!endif


# Compiling C++ source.
.cpp{$(PATH_TARGET)}.$(EXT_OBJ):
    @$(ECHO) C++ Compiler $(CLRFIL)$< $(CLRRST)
    @$(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB"  || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(CXX) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(CC) \
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB"
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
    @$(RC) $(RC_FLAGS) $< $@

.rc.res:
    @$(ECHO) RC Compiler $(CLRFIL)$< $(CLRRST)
    @$(RC) $(RC_FLAGS) $< $(PATH_TARGET)\$(@F)





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

build: $(SUBDIRS_BUILD) $(PREMAKEFILES_BUILD) $(TARGET) $(TARGET_ILIB) $(TARGET_PUBLIB)
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

lib: $(SUBDIRS_LIB)  $(TARGET_ILIB) $(TARGET_PUBLIB)
!ifdef POSTMAKEFILES
    @$(TOOL_DOMAKES) "$(POSTMAKEFILES)" $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The install rule - Copies target to main binary directory.
#   Installation order is not concidered vital, so subdirectories and
#   pre-makefiles are processed after this directory. This might be changed.
# -----------------------------------------------------------------------------
install:
!if "$(TARGET_MODE)" == "EXE"
    if exist $(TARGET) $(TOOL_COPY) $(TARGET) $(PATH_BIN)
!endif
!if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
    if exist $(TARGET) $(TOOL_COPY) $(TARGET) $(PATH_DLL)
!endif
!if "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "IFSLIB"
    if exist $(TARGET) $(TOOL_COPY) $(TARGET) $(PATH_SYS)
!endif
!if 0
# Nothing to do here currently. These are either private or they're allready where they should be.
#
# TODO/BUGBUG/FIXME:
#       The PUB stuff should be change to a separate variable.
#       It will make life easier to just state that this target,
#       what ever it is, should be public.
#
#       That's allow project to install targets during make without
#       running the install command by setting some target modes
#       public by default.
#   (kso)
#!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB" || "$(TARGET_MODE)" == "SYSLIB"
#    if exist $(TARGET) $(TOOL_COPY) $(TARGET) $(PATH_LIB)
#!endif
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
!ifndef BUILD_NO_TESTCASE
testcase: install
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
# The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
    @$(ECHO) Building dependencies $(CLRRST)
    @$(TOOL_DEP) $(TOOL_DEP_FLAGS) -o$$(PATH_TARGET) -d$(TARGET_DEPEND)\
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
        $(PATH_TARGET)\*.$(EXT_RES) \
        $(PATH_TARGET)\*.$(EXT_SYS) \
        $(PATH_TARGET)\*.$(EXT_LIB)
    $(TOOL_RM) \
        $(PATH_TARGET)\*.$(EXT_IFS) \
        $(PATH_TARGET)\*.map \
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
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS"
$(TARGET): $(TARGET_OBJS) $(TARGET_RES) $(TARGET_DEF) $(TARGET_LNK) $(TARGET_DEPS)
    @$(ECHO) Linking $(TARGET_MODE) $(CLRFIL)$@ $(CLRRST)
!ifdef TARGET_IGNORE_LINKER_WARNINGS
    -4 \
!endif
!if "$(TARGET_MODE)" == "EXE"
    $(LINK_CMD_EXE)
!endif
!if "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT"
    @$(LINK_CMD_DLL)
!endif
!if "$(TARGET_MODE)" == "SYS"
    @$(LINK_CMD_SYS)
!endif
!if "$(TARGET_MODE)" == "IFS"
    @$(LINK_CMD_IFS)
!endif
!if "$(TARGET_RES)" != "" && "$(RL)" != ""
    @$(ECHO) Linking Resources $(CLRRST)
    @$(RL) $(RL_FLAGS) $(TARGET_RES) $@
!endif
!if "$(TARGET_DLLRNAME)" != ""
    @$(ECHO) Dll Rename $(TARGET_DLLRNAME)
    $(TOOL_DLLRNAME) $(TARGET) $(TARGET_DLLRNAME)
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



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For LIB, PUBLIB, and SYSLIB targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "PUBLIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
$(TARGET): $(TARGET_OBJS) $(TARGET_LNK) $(TARGET_DEPS)
    @$(ECHO) Creating Library $(CLRFIL)$@ $(CLRRST)
    $(TOOL_RM) $@
    $(AR_CMD)


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
# Copy rule for public libraries.
#   BUGBUG/TODO/FIXME: get rid of the PUBLIB stuff. see commet in install.
#
!if "$(TARGET_MODE)" == "PUBLIB"
$(TARGET_PUBLIB): $(TARGET)
    @$(ECHO) Copying $(CLRFIL)$(TARGET)$(CLRTXT) to the Library Directory $(CLRRST)
    @$(TOOL_COPY) $** $@
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EMPTY targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EMPTY"
$(TARGET):
    @$(ECHO) .
!endif



# -----------------------------------------------------------------------------
# The $(TARGET_ILIB) rule - Make import library.
# -----------------------------------------------------------------------------
!ifdef TARGET_ILIB
$(TARGET_ILIB): $(TARGET_IDEF)
    @$(ECHO) Creating Import Library $(CLRFIL)$@ $(CLRRST)
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(TARGET_IDEF)
!endif
!endif



# -----------------------------------------------------------------------------
# The .force rule - Force a remake of something everytime.
# -----------------------------------------------------------------------------
.force:
    @$(ECHO) .



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

