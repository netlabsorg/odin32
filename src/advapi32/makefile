# $Id: makefile,v 1.31 2002-08-24 04:18:50 bird Exp $

#
# Odin32 API
#
#       advapi32.dll makefile
#

!ifdef BUILD_SETUP_MAK

#
# Setup config.
#
ORC_NO_PREFIX   = 1
PATH_ROOT       = ..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config.
#
TARGET_NAME = advapi32
TARGET_IDEF_CONV = 1

TARGET_OBJS = \
$(PATH_TARGET)\advapi32.$(EXT_OBJ)\
$(PATH_TARGET)\advapi32rsrc.$(EXT_OBJ)\
$(PATH_TARGET)\lsa.$(EXT_OBJ)\
$(PATH_TARGET)\security.$(EXT_OBJ)\
$(PATH_TARGET)\security_odin.$(EXT_OBJ)\
$(PATH_TARGET)\systemfunction.$(EXT_OBJ)\
$(PATH_TARGET)\service.$(EXT_OBJ)\
$(PATH_TARGET)\eventlog.$(EXT_OBJ)\
$(PATH_TARGET)\crypt.$(EXT_OBJ)\
$(OBJ_DLLENTRY)\

TARGET_LIBS =\
$(PATH_LIB)\kernel32.$(EXT_LIB)\
$(PATH_LIB)\ntdll.$(EXT_LIB)\
$(LIB_ODINCRT)\
$(LIB_OS)\
$(LIB_C_RTDLL)\

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)


!else

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\advapi32.obj \
$(OBJDIR)\advapi32rsrc.obj \
$(OBJDIR)\lsa.obj \
$(OBJDIR)\security.obj \
$(OBJDIR)\security_odin.obj \
$(OBJDIR)\systemfunction.obj \
$(OBJDIR)\service.obj \
$(OBJDIR)\eventlog.obj \
$(OBJDIR)\crypt.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = advapi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

!endif
