# $Id: minihll.mak,v 1.1.2.1 2001-08-14 22:56:05 bird Exp $

#
# Odin32 API
#
#       kRx.exe makefile
#


#
# Tell the buildenvironment to make an VIO exe.
#
VIO = 0
EXETARGET=1


#
# Compiler, tools, and interference rules.
#
!include ..\..\..\makefile.inc


#
# Tools and Flags Addjustments.
#
LD2FLAGS = $(LD2FLAGS) /BASE:0x10000 /Stack:0x1000


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\minihll.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET  = minihll


#
# Main rule
#
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    -8 $(LD2) /FORCE $(LD2FLAGS) /BASE:0x10000 /PACKCODE /PACKDATA /NOEXEPACK \
        /ALIGNMENT:1 @$(OBJDIR)\$(TARGET).lrf
    $(LXLITE) /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND $@

#    -8 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 /PACKCODE /PACKDATA /EXEPACK:1 \
#        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;


#
# Exe: Linker file - creates the parameter file passed on to the linker.
#
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).exe
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS:  =^
)
$(LIBS:  =^
)
$(DEFFILE)
<<keep




