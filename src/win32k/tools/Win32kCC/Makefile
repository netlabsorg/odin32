# $Id: Makefile,v 1.1.2.1 2002-04-01 13:40:18 bird Exp $

#
# Odin32 API
#
#       Win32kCC.exe makefile
#


#
# Load the build setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_NAME  = Win32kCC
ALL_INCLUDES = -I../../include -I../../kKrnlLib/include
ALL_DEFINES  = -DRING3


#
# Libraries.
#
TARGET_LIBS = \
$(LIB_OS)\
$(LIB_C_OBJ)\
$(PATH_LIB)\Win32k.lib\


#
# Resources (OS2).
#
TARGET_RES = \
$(PATH_TARGET)\$(TARGET_NAME).res


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

