# $Id: makefile,v 1.10 2000-07-19 19:05:13 sandervl Exp $

#
# Odin32 API
#
#       Glide\cvg makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


# Have extra clean rule.
CLEAN2 = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\initterm.obj


# Target name - name of the dll without extention and path.
TARGET = glide2x


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\Glide\Voodoo2\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib



# Libs rule - Make (sub) libraries. (Not import libraries!)
libs:
    cd ..\swlibs\pcilib
    $(MAKE_CMD) $@
    cd ..\..\cvg\init
    $(MAKE_CMD) $@
    cd ..\glide
    $(MAKE_CMD) $@
    cd ..\texus
    $(MAKE_CMD) $@
    cd ..


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
$(PDWIN32_LIB)/$(ODINCRT).lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Copy rule - copies the dll.
$(PDWIN32_BIN)\Glide\Voodoo2\$(TARGET).dll: $(OBJDIR)\$(TARGET).dll
    -mkdir $(PDWIN32_BIN)\Glide > nul 2>&1
    -mkdir $(PDWIN32_BIN)\Glide\Voodoo2 > nul 2>&1
    $(CP) $** $@
    $(CP) $** ..\..\..\..\bin\Glide\Voodoo2\$(TARGET).dll


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd ..\swlibs\pcilib
    $(MAKE_CMD) $@
    cd ..\..\cvg\init
    $(MAKE_CMD) $@
    cd ..\glide
    $(MAKE_CMD) $@
    cd ..\texus
    $(MAKE_CMD) $@
    cd ..


# Extra cleanrule.
clean2:
    cd ..\swlibs\pcilib
    $(MAKE_CMD) clean
    cd ..\..\cvg\glide
    $(MAKE_CMD) clean
    cd ..\init
    $(MAKE_CMD) clean
    cd ..\texus
    $(MAKE_CMD) clean
    cd ..


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post



