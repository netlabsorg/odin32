# $Id: makefile,v 1.19 2000-09-17 22:28:55 davidr Exp $

#
# Odin32 API
#
#       ole32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = oleaut32rsrc.asm

!ifdef NODEBUGINFO
CFLAGS = $(CFLAGS) -DDEFAULT_LOGGING_OFF
CXXFLAGS = $(CXXFLAGS) -DDEFAULT_LOGGING_OFF
!endif


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\oleaut32.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\oList.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\olefont.obj \
$(OBJDIR)\variant.obj \
$(OBJDIR)\ole2disp.obj \
$(OBJDIR)\parsedt.obj \
$(OBJDIR)\iPicture.obj \
$(OBJDIR)\iPictureNone.obj \
$(OBJDIR)\iPictureBmp.obj \
$(OBJDIR)\iPictureMeta.obj \
$(OBJDIR)\iPictureIcon.obj \
$(OBJDIR)\iPictureEmf.obj \
$(OBJDIR)\typelib.obj \
$(OBJDIR)\itypelib.obj \
$(OBJDIR)\itypecomp.obj \
$(OBJDIR)\itypeinfo.obj \
$(OBJDIR)\hash.obj \
$(OBJDIR)\safearray.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\oleaut32rsrc.obj


# Target and original target names - names of the dll without extention and path
TARGET      = olautos2
ORGTARGET   = oleaut32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(ORGTARGET).lib $(PDWIN32_LIB)\$(ORGTARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(ORGTARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/pmwinx.lib
$(PDWIN32_LIB)/advapi32.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/ole32.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/$(ODINCRT).lib
OS2386.LIB
$(RTLLIB_O)
$(ORGTARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

