# $Id: lib-c.mak,v 1.1 2002-08-20 19:10:00 bird Exp $

# Testcase for makeing simple C programs.

# root & setup
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
TARGET_NAME = lib-c
TARGET_MODE = LIB
MAKEFILE    = lib-c.mak

# rules and more
!include $(MAKE_INCLUDE_PROCESS)

