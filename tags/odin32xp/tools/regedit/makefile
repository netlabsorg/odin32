# $Id: makefile,v 1.3 2001-08-19 14:04:43 sandervl Exp $

#
# Odin32 API
#
#       Makefile for regedit.exe.
#
STACKSIZE = 0x10000
VIO = 1
EXETARGET = 1

#
# Directory macros.
#
ODIN32_BIN  = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're making an .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET   = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Set Win32APP flags.
#
CFLAGS   = $(CFLAGS_WIN32APP)
CXXFLAGS = $(CXXFLAGS_WIN32APP)
CDEFINES = $(CDEFINES_WIN32APP)
#LD2FLAGS = $(LD2FLAGS_WIN32APP)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\regapi.obj \
$(OBJDIR)\odinexe.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\advapi32.lib \
$(ODIN32_LIB)\user32.lib \
$(ODIN32_LIB)\gdi32.lib \
$(ODIN32_LIB)\$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the exe without extention and path.
#
TARGET = regapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


