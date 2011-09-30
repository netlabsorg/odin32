# $Id: ipf.mak,v 1.1 2002-09-20 03:21:03 bird Exp $

#
# Testcase for makeing hello world with HELP and BOOK.
#

#
# Setup config.
#
PATH_ROOT       = ..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)


#
# Target config.
#
TARGET_MODE     = EXE
TARGET_NAME     = helloworld
MAKEFILE        = ipf.mak
TARGET_LIBS     = $(LIB_C_OBJ) $(LIB_OS)    # use the static library!
TARGET_BOOKS    = $(PATH_TARGET)\book.$(EXT_INF) $(PATH_TARGET)\bookpre.$(EXT_INF)
TARGET_HELPS    = $(PATH_TARGET)\book.$(EXT_HLP) $(PATH_TARGET)\bookpre.$(EXT_HLP)


#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

