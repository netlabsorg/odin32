# $Id: makefile,v 1.18 2000-03-13 12:47:53 sandervl Exp $

#
# Odin32 API
#
#       dsound.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = dsoundrsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\dsound.obj \
$(OBJDIR)\os2dsound.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\os2sndbuffer.obj \
$(OBJDIR)\os2primbuff.obj \
$(OBJDIR)\os2notify.obj \
$(OBJDIR)\os23dlistener.obj \
$(OBJDIR)\os23dbuffer.obj \
$(OBJDIR)\dart.obj \
$(OBJDIR)\dsmixer.obj \
$(OBJDIR)\dsoundrsrc.obj


# Target name - name of the dll without extention and path.
TARGET = dsound


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
pmwinx.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/odincrt.lib
$(PDWIN32_LIB)/ole32.lib
mmpm2.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

