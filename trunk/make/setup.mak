# $Id: setup.mak,v 1.4 2002-04-22 00:28:46 bird Exp $

#
# Generic makefile system.
#
#   Setting up the build environment variables
#
#   Many of the variables are a subject to change from project to project.
#


!ifndef MAKE_SETUP_INCLUDED
MAKE_SETUP_INCLUDED=YES


# -----------------------------------------------------------------------------
# Validate the build the requested environment setup.
# -----------------------------------------------------------------------------

SHT_TRGPLTFRM=
!if "$(BUILD_PLATFORM)" == "OS2"
SHT_TRGPLTFRM=os2
!endif
!if "$(BUILD_PLATFORM)" == "WIN32"
SHT_TRGPLTFRM=win32
!endif
!if "$(SHT_TRGPLTFRM)" == ""
! error Fatal error: Env.var BUILD_PLATFORM is either unspecified or incorrect. ($(BUILD_PLATFORM)) Valid values: OS2 and WIN32
!endif


SHT_BLDMD=
!if "$(BUILD_MODE)" == "RELEASE"
SHT_BLDMD=rel
!endif
!if "$(BUILD_MODE)" == "DEBUG"
SHT_BLDMD=deb
!endif
!if "$(BUILD_MODE)" == "PROFILE"
SHT_BLDMD=prf
!endif
!if "$(SHT_BLDMD)" == ""
! error Fatal error: Env.var BUILD_MODE is either unspecified or incorrect. ($(BUILD_MODE)) Valid values: RELEASE, DEBUG and PROFILE
!endif


SHT_BLDENV=
!if "$(BUILD_ENV)" == "VAC308"
SHT_BLDENV=vac308
!endif
!if "$(BUILD_ENV)" == "VAC365"
SHT_BLDENV=vac365
!endif
!if "$(BUILD_ENV)" == "VAC4"
SHT_BLDENV=vac4
!endif
!if "$(BUILD_ENV)" == "EMX"
SHT_BLDENV=emx
!endif
!if "$(BUILD_ENV)" == "MSCV6"
SHT_BLDENV=mscv6
!endif
!if "$(BUILD_ENV)" == "WAT11"
SHT_BLDENV=wat11
!endif
!if "$(BUILD_ENV)" == "WATCOM"
SHT_BLDENV=wat11
!endif
!if "$(SHT_BLDENV)" == ""
! error Fatal error: Env.var BUILD_ENV is either unspecified or incorrect. ($(BUILD_MODE)) Valid values: VAC308, VAC365, VAC4, EMX, MSCV6 and WATCOM
!endif



# -----------------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------------

# current directory.
PATH_CURRENT    = $(MAKEDIR)
# Where build system files are located. (like this file)
PATH_MAKE       = $(PATH_ROOT)\make
# Where the bulid system and other tools are located
PATH_TOOLS      = $(PATH_ROOT)\tools\bin
# Where platform-specific files are located. (like the .def files)
# (default) PATH_DEF        = $(SHT_TRGPLTFRM)
PATH_DEF        = .
# Where the include files are located.
PATH_INCLUDES   = $(PATH_ROOT)\include;$(PATH_ROOT)\include\win

# Where the temporary files goes.
PATH_OBJ    = $(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)$(SHT_BLDENV)
# Where the executable binaries goes.
PATH_BIN    = $(PATH_ROOT)\bin\$(BUILD_MODE)
# Where the public libraries goes.
PATH_LIB    = $(PATH_ROOT)\lib\$(BUILD_MODE)
# Where the dynamic link libraries goes.
PATH_DLL    = $(PATH_ROOT)\bin\$(BUILD_MODE)
# Where the drivers goes. (common for IFS and SYS.)
PATH_SYS    = $(PATH_ROOT)\bin\$(BUILD_MODE)
# Where the virtual dos drivers goes.
PATH_VDD    = $(PATH_ROOT)\bin\$(BUILD_MODE)
# Where the documentation goes.
PATH_DOC    = $(PATH_ROOT)\bin\$(BUILD_MODE)
# Where the helpfiles goes.
PATH_HLP    = $(PATH_ROOT)\bin\$(BUILD_MODE)


# Note: Makefiles are supposed to set the correct *RELATIVE* path to the
#       projects root. Using '\' slashes please. No trailing slash.
#
# Example:
#       PATH_ROOT= ..\..\..
# Assert PATH_ROOT
!if "$(PATH_ROOT)" == ""
!error fatal error: PATH_ROOT empty or undefined.
!endif


# -----------------------------------------------------------------------------
# Common variables / Project variables
# -----------------------------------------------------------------------------

# The default definitions.
BUILD_DEFINES = -D__WIN32OS2__ -D__WINE__ -D__i386__

# This is the process file to include at end of the makefile.
MAKE_INCLUDE_PROCESS    = $(PATH_MAKE)\process.mak



# -----------------------------------------------------------------------------
# Build the environments
# -----------------------------------------------------------------------------

# EXPERIMENTAL
!if 0
# These strings are passed on to the BuildEnv.cmd script to setup
# the correct environment.
BUILD_ENVS_BASE_POST    = toolkit40
BUILD_ENVS_BASE_PRE     =

# Check for forced change in default compiler environment.
!ifdef BUILD_ENV_FORCE
!if "$(BUILD_ENV)" != "$(BUILD_ENV_FORCE)" || "$(BUILD_ENVS_PRE)" != "" || "$(BUILD_ENVS_POST)" != ""
#
# Remove old compiler and insert new compiler into the environment.
# The individual makefiles requests changes using BUILD_ENVS_[PRE|POST].
#
!if [echo call $(PATH_TOOLS)\BuildEnv.cmd $(BUILD_ENV)- $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(BUILD_ENV_FORCE) $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST)]
!endif
!if [call $(PATH_TOOLS)\BuildEnv.cmd $(BUILD_ENV)- $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(BUILD_ENV_FORCE) $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST)]
!endif
#BUILD_ENV = $(BUILD_ENV_FORCE)
!endif
!endif

!endif



# -----------------------------------------------------------------------------
# Include the setup.
#   First the default common tools setup is included.
#   The the environment specific setup.
# -----------------------------------------------------------------------------

!include $(PATH_MAKE)\setup.tools.mk

MAKE_INCLUDE_SETUP = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)$(SHT_BLDENV).mk
!ifndef BUILD_QUIET
! if [$(ECHO) Including platform setup file $(CLRFIL)$(MAKE_INCLUDE_SETUP)$(CLRRST)]
! endif
!endif
!include $(MAKE_INCLUDE_SETUP)


# -----------------------------------------------------------------------------
# Verify the environment setups.
# -----------------------------------------------------------------------------

!ifndef ENV_NAME
!error No environment signature has been defined ($(NAME_COMPLETE))
!endif

!ifndef MAKE_INCLUDE_SETUP
!error No setup to include has been determined (MAKE_INCLUDE_SETUP)
!endif

!if "$(ENV_STATUS)" != "OK"
!error Environment $(ENV_NAME) does work yet (ENV_STATUS is not OK).
!endif

!ifndef CC
!error Environment $(ENV_NAME) does not define variable (CC).
!endif

!ifndef CC_FLAGS_EXE
!error Environment $(ENV_NAME) does not define variable (CC_FLAGS_EXE).
!endif

!ifndef LINK
!error Environment $(ENV_NAME) does not define variable (LINK).
!endif

!ifndef LINK_FLAGS_EXE
!error Environment $(ENV_NAME) does not define variable (LINK_FLAGS_EXE).
!endif


# -----------------------------------------------------------------------------
# Ensure the output path exists
# -----------------------------------------------------------------------------
!if "$(PATH_OBJ)" != ""
! if [$(TOOL_EXISTS) $(PATH_OBJ)] != 0
!  ifndef BUILD_QUIET
!   if [$(ECHO) Target path $(PATH_OBJ) does NOT exist. Creating. $(CLRRST)]
!   endif
!  endif
!  if [$(TOOL_CREATEPATH) $(PATH_OBJ)]
!   error Fatal error: Could not create $(PATH_OBJ).
!  endif
! endif
!endif


!endif MAKE_SETUP_INCLUDED

