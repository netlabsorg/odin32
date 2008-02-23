# $Id: process.forwarder.mak,v 1.18 2003-03-05 15:10:07 bird Exp $
#
# Generic Buildsystem
#
#   The common build process rules for all cases
#   where are forwarding the request to another environment setup.
#
#
# Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
#
# This file is part of Generic Buildsystem.
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
.SUFFIXES: .c .cpp .asm .$(EXT_OBJ) .rc .$(EXT_RES) .ii .s .ipp .ipf .$(EXT_INF) .$(EXT_HLP)


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
.rc{$(PATH_TARGET)}.$(EXT_RES):
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


# Compiling INFs.
.ipf{$(PATH_TARGET)}.$(EXT_INF):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.ipf.inf:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling INFs with pre-compiling
.ipp{$(PATH_TARGET)}.$(EXT_INF):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.ipp.inf:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling HLPs
.ipf{$(PATH_TARGET)}.$(EXT_HLP):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.ipf.hlp:
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@


# Compiling HLPs with pre-compiling
.ipp{$(PATH_TARGET)}.$(EXT_HLP):
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@

.ipp.hlp:
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
# Generic forwarder
# -----------------------------------------------------------------------------
build rebuild clean dep needed lib binary misc \
!if "$(TARGET_MODE)" == "DEPEND"
$(TARGET) \
!endif
$(TARGET_ILIB) \
$(RULES_FORWARD) \
$(OTHERS) \
publish publish_target testcase nothing target others packing \
pass1 pass2 pass3 pass4 pass5 pass6 quick \
# obsoletes:
miscellaneous executable :
    \
!ifndef BUILD_VERBOSE
    @ \
!endif
!ifndef _BUILD_PROJECT # workaround for buggy SET implementation in NMAKE.
    echo . && SET _BUILD_PROJECT=$(BUILD_PROJECT) && \
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
!ifndef _BUILD_PROJECT # workaround for buggy SET implementation in NMAKE.
    echo . && SET _BUILD_PROJECT=$(BUILD_PROJECT) && \
!endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(COMSPEC)



# -----------------------------------------------------------------------------
# The $(TARGET) rule - For EMPTY & DEPEND targets.
# -----------------------------------------------------------------------------
!if "$(TARGET_MODE)" == "DEPEND"
$(TARGET):
    @$(ECHO) .
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

