# $Id: Extract.mak,v 1.1 2002-03-10 02:45:48 bird Exp $

#
# kKrnlLib/krnl makefile.
#
#

#
# Load the build setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE = LIB
TARGET_NAME = kKrnlLib_krnl_Extract
MAKEFILE    = Extract.mak
C_INCLUDES  = -I../include/kLib -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
AS_INCLUDES = -I../include/kLib -I../include -I$(PATH_DDKBASE)/inc
CC_DEFINES  = -DEXTRACT
CXX_DEFINES = -DEXTRACT


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\Extract32.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

