# $Id: process.forwarder.mak,v 1.2 2002-04-22 02:21:51 bird Exp $

#
# Generic makefile system.
#
#   The common build process rules for the case
#   where we forward to a different compiler environment.
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


# -----------------------------------------------------------------------------
# Tell user what we're doing.
# -----------------------------------------------------------------------------
!ifndef BUILD_QUIET
!if [$(ECHO) Forwarding to another (shell) environment setup...$(CLRRST)]
!endif
!endif


# -----------------------------------------------------------------------------
# Common inference rules
# -----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .$(EXT_OBJ) .c .cpp .asm .$(EXT_RES) .rc .pre-c .pre-cpp # .h .def


# Assembling assembly source.
.asm{$(PATH_TARGET)}.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@

.asm.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@


# Compiling C++ source.
.cpp{$(PATH_TARGET)}.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@

.cpp.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@


# Pre-Compiling C++ source.
.cpp.pre-cpp:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@


# Compiling C source.
.c{$(PATH_TARGET)}.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@

.c.$(EXT_OBJ):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@


# Pre-Compiling C source.
.c.pre-c:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@


# Compiling resources.
.rc{$(PATH_TARGET)}.res:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@

.rc.res:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@





# -----------------------------------------------------------------------------
# The all rule - The default one, as it's the first rule in the file.
# -----------------------------------------------------------------------------
all: build



# -----------------------------------------------------------------------------
# The build rule - Build the target.
# -----------------------------------------------------------------------------
build:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The lib rule - Make Public libraries.
# -----------------------------------------------------------------------------
lib:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The install rule - Copies target to main binary directory.
# -----------------------------------------------------------------------------
install:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The testcase rule - Execute testcases when present.
# -----------------------------------------------------------------------------
testcase:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The shell rule - Setup the correcte shell environment and start a shell.
# -----------------------------------------------------------------------------
shell:
!ifndef BUILD_VERBOSE
    @ \
!endif
    -$(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(COMSPEC)



# -----------------------------------------------------------------------------
# The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The clean rule - Clean up output files.
#   The current setup doesn't clean the installed ones.
# -----------------------------------------------------------------------------
clean:
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EXE, DLL, SYS and IFS targets
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "CRT" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
$(TARGET):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For LIB, SYSLIB, and IFSLIB targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
$(TARGET):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@
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
$(TARGET_ILIB):
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) $@

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


