# $Id: makefile,v 1.17 2000-10-20 11:45:25 sandervl Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

# Tell that we're producing an executable
EXETARGET = 1

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = $(PDWIN32_BIN)\$(TARGET).exe


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = $(OBJDIR)\pe.obj
OBJS1= $(OBJDIR)\pec.obj

# Target name - name of the dll without extention and path.
TARGET  = pe
TARGET1 = pec

# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).exe \
        $(PDWIN32_BIN)\$(TARGET).exe \
        $(OBJDIR)\$(TARGET1).exe \
        $(PDWIN32_BIN)\$(TARGET1).exe


# Lib rule - dummy rule.
lib:


# Exe rule - builds the target exe.
$(OBJDIR)\$(TARGET).exe: $(OBJS) $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS_ODINCRT) @$(OBJDIR)\$(TARGET).lrf

$(OBJDIR)\$(TARGET1).exe: $(OBJS1) $(OBJDIR)\$(TARGET1).lrf
    $(LD2) $(LD2FLAGS_ODINCRT) @$(OBJDIR)\$(TARGET1).lrf

$(PDWIN32_BIN)\$(TARGET1).exe: $(OBJDIR)\$(TARGET1).exe
    cd $(OBJDIR)
    $(MAPSYM) $(TARGET1).map
    cd ..\..
    $(CP) $(OBJDIR)\$(TARGET1).sym $(PDWIN32_BIN)
    $(CP) $** $@
    -$(CP) $** $(@D)..\..\$(@F)
    -$(CP) $(OBJDIR)\$(TARGET1).sym $(@D)..\..

$(OBJDIR)\pec.obj:   pe.cpp
    $(CC) -C $(CXXFLAGS) -DCOMMAND_LINE_VERSION -Fo$@ pe.cpp

# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).exe
/MAP:$(OBJDIR)\$(TARGET).map
/PMTYPE:pm
/STACK:0x100000
/NOBASE
$(OBJS)
os2386.lib
<<keep


$(OBJDIR)\$(TARGET1).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET1).exe
/MAP:$(OBJDIR)\$(TARGET1).map
/PMTYPE:vio
/STACK:0x100000
/NOBASE
$(OBJS1)
os2386.lib
<<keep

# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

