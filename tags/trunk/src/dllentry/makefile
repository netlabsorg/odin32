# $Id: makefile,v 1.4 2000-03-06 23:38:35 bird Exp $

#
# Odin32 API
#
#       dllentry library makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = lz32rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\dllentry.obj


# Target name - name of the obj without extention and path.
TARGET = dllentry


# All rule - build objs and copies obj to lib.
all: $(OBJDIR) $(PDWIN32_LIB)\$(TARGET).obj


# Lib rule - same as all.
lib: all


# Build objs and copies obj to lib.
$(PDWIN32_LIB)\$(TARGET).obj: $(OBJS)
    $(CP) $(OBJS) $(PDWIN32_LIB)


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

