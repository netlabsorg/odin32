# $Id: makefile,v 1.10 2000-12-02 23:43:27 bird Exp $

#
# Odin32 API
#
#       wsock32.dll makefile
#


# Compiler, tools, and interference rules.
OLD_STYLE=1
!include ../../../makefile.inc


# Flag overloads and local macros.
CLEANEXTRAS = wsock32rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\wsock32.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\wsa.obj \
$(OBJDIR)\wsastruct.obj \
$(OBJDIR)\wsock32rsrc.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\asyncapi.obj \
$(OBJDIR)\asyncthread.obj \
$(OBJDIR)\dbglocal.obj


# Target name - name of the dll without extention and path.
TARGET = wsock32

CXXFLAGS = $(CXXFLAGS) -DNEW_ASYNC


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(ODIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(ODIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(TARGET).def
tcp32dll.lib so32dll.lib
$(ODIN32_LIB)\kernel32.lib
$(ODIN32_LIB)/user32.lib
$(ODIN32_LIB)/odincrt.lib
OS2386.LIB
$(RTLLIB_O)
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(ODIN32_INCLUDE);$(ODIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(ODIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(ODIN32_POST_INC)

