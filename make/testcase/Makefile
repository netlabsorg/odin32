# $Id: Makefile,v 1.1 2002-05-16 11:37:06 bird Exp $

#
# Makesystem Testcases
#

#
# Include Default Tools .
#
MAKEFILE = .\Makefile
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.tools.mk

#
# Configuration.
#

# Modes
MODES = \
    DEBUG \
    RELEASE \
    PROFILE \

# List of C environments
CC_ENVIRONMENTS = \
    WAT11C \
    WAT11C-16 \
    VAC308 \
    VAC365 \
    EMX \
# missing libraries   MSCV6 \
    MSCV6-16 \

# List of C++ environments
CXX_ENVIRONMENTS = \
    WAT11C \
    WAT11C-16 \
    VAC308 \
    VAC365 \
    EMX \

# List of C as C++ aware environments
CC_AS_CXX_ENVIRONMENTS = \
    WAT11C \
    WAT11C-16 \
    VAC308 \
    VAC365 \
    EMX \

# List of C++ as C aware environments
CXX_AS_CC_ENVIRONMENTS = \
    VAC308 \
    VAC365 \
    WAT11C \
    WAT11C-16 \
# not supported ??    EMX \


#
# All rule.
#
all: TempMakefile.mak
    $(TOOL_MAKE) -f TempMakefile.mak run-tests


TempMakefile.mak: $(MAKEFILE)
    $(ECHO) Creating temporary Makefile <<$@
# Temporary
# Temporary
# Temporary

run-tests:
<<KEEP
    @echo # >> $@
    @echo #   Programs >> $@
    @echo # >> $@
!if 1
    @echo #   CC_ENVIRONMENTS >> $@
    @for %%j in ($(MODES)) do $(COMSPEC) /C "echo   set BUILD_MODE=%%j >> $@ & \
        for %%%%i in ($(CC_ENVIRONMENTS)) do echo     $(TOOL_MAKE) -f prog-c.mak BUILD_ENV_FORCE=%%%%i clean dep lib all install test clean >> $@"
    @echo #   CXX_ENVIRONMENTS >> $@
    @for %%j in ($(MODES)) do $(COMSPEC) /C "echo   set BUILD_MODE=%%j >> $@ & \
        for %%%%i in ($(CXX_ENVIRONMENTS)) do echo     $(TOOL_MAKE) -f prog-cxx.mak BUILD_ENV_FORCE=%%%%i clean dep lib all install test clean >> $@"
    @echo #   CC_AS_CXX_ENVIRONMENTS >> $@
    @for %%j in ($(MODES)) do $(COMSPEC) /C "echo   set BUILD_MODE=%%j >> $@ & \
        for %%%%i in ($(CC_AS_CXX_ENVIRONMENTS)) do echo     $(TOOL_MAKE) -f prog-c-as-cxx.mak BUILD_ENV_FORCE=%%%%i clean dep lib all install test clean >> $@"
    @echo #   CXX_AS_CC_ENVIRONMENTS >> $@
    @for %%j in ($(MODES)) do $(COMSPEC) /C "echo   set BUILD_MODE=%%j >> $@ & \
        for %%%%i in ($(CXX_AS_CC_ENVIRONMENTS)) do echo     $(TOOL_MAKE) -f prog-cxx-as-c.mak BUILD_ENV_FORCE=%%%%i clean dep lib all install test clean >> $@"
!endif
    @echo # >> $@
    @echo #   DLLs >> $@
    @echo # >> $@
    @echo #   CC_ENVIRONMENTS >> $@
    @for %%j in ($(MODES)) do $(COMSPEC) /C "echo   set BUILD_MODE=%%j >> $@ & \
        for %%%%i in ($(CC_ENVIRONMENTS)) do echo     $(TOOL_MAKE) -f dll-prog-c.mak BUILD_ENV_FORCE=%%%%i clean dep lib all install test clean >> $@"

!ifdef BUILD_VERBOSE
    type $@
!endif

