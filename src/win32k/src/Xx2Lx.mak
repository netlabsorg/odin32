# $Id: Xx2Lx.mak,v 1.1.2.2 2002-04-01 09:06:02 bird Exp $

#
# xx2lx makefile.
#
#


#
# Load the build setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE  = EXE
TARGET_NAME  = Xx2Lx
MAKEFILE     = Xx2Lx.mak
TARGET_STACKSIZE = 4096
ALL_INCLUDES = -I../include -I../kKrnlLib/include
ALL_DEFINES  = -DRING3 -DXX2LX -D__WIN32OS2__ -D__WINE__
CC_FLAGS_EXE = $(CC_FLAGS_SYS) -URING0


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
TARGET_OBJS =\
$(PATH_TARGET)\xx2lxmain.$(EXT_OBJ)\
$(PATH_TARGET)\modulebase.$(EXT_OBJ)\
$(PATH_TARGET)\pe2lx.$(EXT_OBJ)\
$(PATH_TARGET)\elf2lx.$(EXT_OBJ)\
#malloc.obj\
#smalloc_avl.obj\
#avl.obj\
#rmalloc_avl.obj\
#new.obj\
#stricmp.obj\
#vprintf.obj


#
# Libraries. One space before the '\'.
#
TARGET_LIBS =\
$(LIB_C_OBJ)\
$(LIB_OS)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

