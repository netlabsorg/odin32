# $Id: lib-c#.mak,v 1.1 2002-08-20 19:10:00 bird Exp $

# Testcase for makeing simple C programs.

# root & setup
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
#TARGET_NAME = lib-c^# # doesn't work because the include of the .depend file then failes. NMAKE bug! :-/
TARGET_NAME = lib-c12
TARGET_MODE = LIB
MAKEFILE    = lib-c^#.mak
!if [echo $(MAKEFILE)]
!endif
TARGET_OBJS =\
$(PATH_TARGET)\lib-c^#1.$(EXT_OBJ) \
$(PATH_TARGET)\lib-c^#2.$(EXT_OBJ)\

# rules and more
!include $(MAKE_INCLUDE_PROCESS)

