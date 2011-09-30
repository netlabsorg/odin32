# $Id: genericexe.mak,v 1.2 2001-03-20 22:49:24 bird Exp $

#
# Odin32 API
#
#       Sample makefile for Odin32 Exe files.
#


#
# Build environment configuration.
#
NO_ALL_RULE=
NO_LIB_RULE=1
EXETARGET = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CFLAGS   = $(CFLAGS_WIN32APP)
CXXFLAGS = $(CXXFLAGS_WIN32APP)
CDEFINES = $(CDEFINES_WIN32APP)
STACKSIZE = 0x100000


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\generic.obj \
$(OBJDIR)\odinexe.obj \
$(OBJDIR)\resource.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/comdlg32.lib \
$(ODIN32_LIB)/shlwapi.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/winspool.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/version.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/shell32.lib \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = generic


#
# Our own All rule which doesn't copy stuff to global bin directories.
#
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).exe \
        $(OBJDIR)\$(TARGET).sym


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

