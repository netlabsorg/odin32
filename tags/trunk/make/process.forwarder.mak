# $Id: process.forwarder.mak,v 1.5 2002-08-19 15:00:25 bird Exp $

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

!if "$(TARGET_NAME)" == "" && "$(TARGET_MODE)" != "EMPTY" && "$(TARGET_MODE)" != "DEPEND"
!error Fatal error: TARGET_NAME is not defined! Should be set in the makefile.
!endif

# -----------------------------------------------------------------------------
# Set necessary defaults.
# -----------------------------------------------------------------------------

# just make it shut up for now.
TARGET = process.forwarder.mak

# Default makefile names.
!ifndef BUILD_MAKEFILE
BUILD_MAKEFILE = Makefile
!endif

# Default makefile name.
!ifndef MAKEFILE
MAKEFILE = $(BUILD_MAKEFILE)
!endif


# -----------------------------------------------------------------------------
# Tell user what we're doing.
# -----------------------------------------------------------------------------
!ifndef BUILD_QUIET
! ifndef MAKEVER
!  if [$(ECHO) Forwarding to another (shell) environment setup...$(CLRRST)]
!  endif
! else
$(ECHO) Forwarding to another (shell) environment setup...$(CLRRST)
! endif
!endif


# -----------------------------------------------------------------------------
# Common inference rules
# -----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .cpp .asm .$(EXT_OBJ) .rc .$(EXT_RES) .ii .s


# Assembling assembly source.
.asm{$(PATH_TARGET)}.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.asm.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling C++ source.
.cpp{$(PATH_TARGET)}.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.cpp.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Pre-Compiling C++ source.
.cpp.ii:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiler C++ source to assembly.
.cpp.s:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling C source.
.c{$(PATH_TARGET)}.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.c.$(EXT_OBJ):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Pre-Compiling C source.
.c.ii:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiler C source to assembly.
.c.s:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling resources.
.rc{$(PATH_TARGET)}.res:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.rc.res:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@





# -----------------------------------------------------------------------------
# The all rule - The default one, as it's the first rule in the file.
# -----------------------------------------------------------------------------
all: build



# -----------------------------------------------------------------------------
# The build rule - Build the target.
# -----------------------------------------------------------------------------
build:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The lib rule - Make Public libraries.
# -----------------------------------------------------------------------------
lib:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The install rule - Copies target to main binary directory.
# -----------------------------------------------------------------------------
install:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The testcase rule - Execute testcases when present.
# -----------------------------------------------------------------------------
testcase:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The shell rule - Setup the correcte shell environment and start a shell.
# -----------------------------------------------------------------------------
shell:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(COMSPEC)



# -----------------------------------------------------------------------------
# The dep rule - Make dependencies.
# -----------------------------------------------------------------------------
dep:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The clean rule - Clean up output files.
#   The current setup doesn't clean the installed ones.
# -----------------------------------------------------------------------------
clean:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The nothing rule - Rule for testing the makefile structure.
# -----------------------------------------------------------------------------
nothing:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EXE, DLL, SYS and IFS targets
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "EXE" || "$(TARGET_MODE)" == "DLL" || "$(TARGET_MODE)" == "SYS" || "$(TARGET_MODE)" == "IFS" || "$(TARGET_MODE)" == "VDD"
$(TARGET):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For LIB, SYSLIB, and IFSLIB targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "LIB" || "$(TARGET_MODE)" == "SYSLIB" || "$(TARGET_MODE)" == "IFSLIB"
$(TARGET):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EMPTY & DEPEND targets.
# -----------------------------------------------------------------------------
# this doesn't work as we don't have a target name. Hence not needed.
#!if "$(TARGET_MODE)" == "EMPTY"
#$(TARGET):
#    @$(ECHO) .
#!endif
!if "$(TARGET_MODE)" == "DEPEND"
$(TARGET):
    @$(ECHO) .
!endif



# -----------------------------------------------------------------------------
# The $(TARGET_ILIB) rule - Make import library.
# -----------------------------------------------------------------------------
!ifdef TARGET_ILIB
$(TARGET_ILIB):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

!endif



# -----------------------------------------------------------------------------
# The .force rule - Force a remake of something everytime.
# -----------------------------------------------------------------------------
.force:
    \
!ifndef BUILD_VERBOSE
    @$(ECHO) .
!else
    @$(ECHO) . (force) .
!endif


