# $Id: makefile,v 1.4 2000-03-07 11:12:31 bird Exp $

#
# Odin32 API
#
#       sst1\Glide makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\tools\bin


# Have extra clean rule.
CLEAN2 = 1

# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\initterm.obj


# Target name - name of the dll without extention and path.
TARGET = glide2x


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        libs \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\Glide\Voodoo1\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Libs rule - Make (sub) libraries. (Not import libraries!)
libs:
    cd ..\swlibs\pcilib
    $(MAKE_CMD) $@
    cd ..\..\sst1\init
    $(MAKE_CMD) $@
    cd ..\glide
    $(MAKE_CMD) $@
    cd ..\texus
    $(MAKE_CMD) $@
    cd ..


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) libs $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
..\swlibs\pcilib\$(OBJDIR)\pcilib.lib
texus\$(OBJDIR)\texus.lib
init\$(OBJDIR)\init.lib
glide\$(OBJDIR)\glide.lib
$(PDWIN32_LIB)\kernel32.lib
$(PDWIN32_LIB)\odincrt.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Copy rule.
$(PDWIN32_BIN)\Glide\Voodoo1\$(TARGET).dll: $(OBJDIR)\$(TARGET).dll
    $(CP) $** $@
    $(CP) $** ..\..\..\..\bin\Glide\Voodoo1\$(TARGET).dll


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd ..\swlibs\pcilib
    $(MAKE_CMD) $@
    cd ..\..\sst1\init
    $(MAKE_CMD) $@
    cd ..\glide
    $(MAKE_CMD) $@
    cd ..\texus
    $(MAKE_CMD) $@
    cd ..


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post


# Extra clean rule
clean2:
    cd ..\swlibs\pcilib
    $(MAKE_CMD) clean
    cd ..\..\sst1\init
    $(MAKE_CMD) clean
    cd ..\glide
    $(MAKE_CMD) clean
    cd ..\texus
    $(MAKE_CMD) clean
    cd ..

