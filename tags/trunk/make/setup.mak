# $Id: setup.mak,v 1.9 2002-05-16 11:37:00 bird Exp $

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
!if "$(BUILD_ENV)" == "MSCV6-16"
SHT_BLDENV=mscv6-16
!endif
!if "$(BUILD_ENV)" == "WAT11C"
SHT_BLDENV=wat11
!endif
!if "$(BUILD_ENV)" == "WAT11C-16"
SHT_BLDENV=wat11-16
!endif
!if "$(SHT_BLDENV)" == ""
! error Fatal error: Env.var BUILD_ENV is either unspecified or incorrect. ($(BUILD_MODE)) Valid values: VAC308, VAC365, VAC4, EMX, MSCV6-16, WAT11C and WAT11C-16.
!endif


!ifdef BUILD_ENV_FORCE
SHT_BLDENVFRC=
!if "$(BUILD_ENV_FORCE)" == "VAC308"
SHT_BLDENVFRC=vac308
!endif
!if "$(BUILD_ENV_FORCE)" == "VAC365"
SHT_BLDENVFRC=vac365
!endif
!if "$(BUILD_ENV_FORCE)" == "VAC4"
SHT_BLDENVFRC=vac4
!endif
!if "$(BUILD_ENV_FORCE)" == "EMX"
SHT_BLDENVFRC=emx
!endif
!if "$(BUILD_ENV_FORCE)" == "MSCV6"
SHT_BLDENVFRC=mscv6
!endif
!if "$(BUILD_ENV_FORCE)" == "MSCV6-16"
SHT_BLDENVFRC=mscv6-16
!endif
!if "$(BUILD_ENV_FORCE)" == "WAT11C"
SHT_BLDENVFRC=wat11
!endif
!if "$(BUILD_ENV_FORCE)" == "WAT11C-16"
SHT_BLDENVFRC=wat11-16
!endif
!if "$(SHT_BLDENVFRC)" == ""
! error Fatal error: Var BUILD_ENV_FORCE is incorrect. ($(BUILD_ENV_FORCE)) Valid values: VAC308, VAC365, VAC4, EMX, MSCV6-16, WAT11C and WAT11C-16.
!endif
!else
BUILD_ENV_FORCE=$(BUILD_ENV)
SHT_BLDENVFRC=$(SHT_BLDENV)
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
BUILD_DEFINES           = -D__WIN32OS2__ -D__WINE__
BUILD_BLDLEVEL_FLAGS    = -V^"^#define=ODIN32_VERSION,$(PATH_ROOT)\include\odinbuild.h^" \
                          -M^"^#define=ODIN32_BUILD_NR,$(PATH_ROOT)\include\odinbuild.h^"
BUILD_PROJECT           = Odin32

# This is the process file to include at end of the makefile.
MAKE_INCLUDE_PROCESS    = $(PATH_MAKE)\process.mak



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
# Build the environments
# -----------------------------------------------------------------------------

# In the makefiles you're allowed to use the BUILD_ENVS_PRE,
# BUILD_ENV_FORCE, BUILD_ENVS_POST variables to make private changes to the
# environment. These are combined with the two base ones as follows:
#   $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(ENV_ENVS) $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST)
#
# BUILD_ENV_FORCE is used for changing the base compiler. Do *NOT* use
# BUILD_ENV for that! BUILD_ENV_FORCE isn't used directly but in the setup
# string above, but ENV_ENVS from the setup.[w]xyz.mk setup file is used.
#

# These strings are passed on to the BuildEnv.cmd script to setup the correct
# shell environment.
# TODO   Should these be overridable by setup.[w]xyz.mak ? (kso)

BUILD_ENVS_BASE_POST    = toolkit40
BUILD_ENVS_BASE_POST_16 =
BUILD_ENVS_BASE_PRE     = buildsetup emx cvs
BUILD_ENVS_BASE_PRE_16  = buildsetup emx cvs toolkit40 ddkbase


# Check if there is any change in the environment OR if the environment is
# uncertain (_BUILD_PROJECT not right).
# If there are Then we will have to forward all target commands to the
# correct shell environment
!if "$(_BUILD_PROJECT)" != "$(BUILD_PROJECT)" || "$(BUILD_ENV)" != "$(BUILD_ENV_FORCE)" || "$(BUILD_ENVS_PRE)" != "" || "$(BUILD_ENVS_POST)" != ""
MAKE_INCLUDE_PROCESS = $(PATH_MAKE)\process.forwarder.mak

# set the secret _build_project env.var.
!if "$(_BUILD_PROJECT)" != "$(BUILD_PROJECT)"
! if [set _BUILD_PROJECT=$(BUILD_PROJECT)]
! endif
!endif

# Compiler change or just environment change.
! if "$(BUILD_ENV)" != "$(BUILD_ENV_FORCE)"
MAKE_INCLUDE_SETUP_FORCE = $(PATH_MAKE)\setup.$(SHT_TRGPLTFRM)$(SHT_BLDMD)$(SHT_BLDENVFRC).mk
!  ifndef BUILD_QUIET
!   if [$(ECHO) Including forced platform setup file $(CLRFIL)$(MAKE_INCLUDE_SETUP_FORCE)$(CLRRST)]
!   endif
!  endif
!  include $(MAKE_INCLUDE_SETUP_FORCE)
!  ifdef ENV_16BIT
BUILD_ENVS_CHANGE = $(ENV_ENVS: =- )- $(BUILD_ENVS_BASE_PRE_16) $(BUILD_ENVS_PRE) $(ENV_ENVS_FORCE) $(BUILD_ENVS_BASE_POST_16) $(BUILD_ENVS_POST)
!  else
BUILD_ENVS_CHANGE = $(ENV_ENVS: =- )- $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(ENV_ENVS_FORCE) $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST)
!  endif
! else
!  ifdef ENV_16BIT
BUILD_ENVS_CHANGE = $(BUILD_ENVS_BASE_PRE_16) $(BUILD_ENVS_PRE) $(ENV_ENVS) $(BUILD_ENVS_BASE_POST_16) $(BUILD_ENVS_POST)
!  else
BUILD_ENVS_CHANGE = $(BUILD_ENVS_BASE_PRE) $(BUILD_ENVS_PRE) $(ENV_ENVS) $(BUILD_ENVS_BASE_POST) $(BUILD_ENVS_POST)
!  endif
! endif

!endif


!endif # MAKE_SETUP_INCLUDED

