# $Id: StateUpd.mak,v 1.1 2001-04-17 00:24:09 bird Exp $

#
# Odin32 API
#
#       APIImport.exe makefile
#


#
# Macro which defines the tools common directory.
#
ODIN32_TCOMMON = ..\common
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Tell that we're producing an vio executable.
# Tell buildenvironment that we like to use static linked CRT.
# Tell that we're not to be copied to /bin.
# Tell that the makefile has another name.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1
MAKEFILE = StateUpd.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc
!include $(ODIN32_TCOMMON)/common.mk


#
# Addjustments..
#
CINCLUDES= -I$(ODIN32_INCLUDE) -I$(ODIN32_INCLUDE)\win -Igd -Imysql -I..\common
!ifdef DEBUG
CDEFINES = $(CDEFINES) -DNO_CLIENT_LONG_LONG -DDEBUG_ALLOC
!else
CDEFINES = $(CDEFINES) -DNO_CLIENT_LONG_LONG
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\StateUpd.obj \
$(OBJDIR)\db.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(COMMONLIB) \
$(RTLLIB) \
mysql\libmysqlclient.lib \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = StateUpd


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
!include $(ODIN32_TCOMMON_POST_INC)

