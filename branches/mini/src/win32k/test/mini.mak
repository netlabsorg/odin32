# $Id: mini.mak,v 1.1.2.1 2001-08-14 03:32:08 bird Exp $

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
# Interference rule. Note: ALP specific; enables listing.
# (For Debug)
#
.asm{$(OBJDIR)}.obj:
!ifdef NORMAL
    $(AS) $(ASFLAGS) -D:NORMAL=1 $< -Fo:$@ -Fl:$(OBJDIR)\$(*B).lst \
!else
    $(AS) $(ASFLAGS) $< -Fo:$@ -Fl:$(OBJDIR)\$(*B).lst
#    masm $<, $@,$(OBJDIR)\$(*B).lst,$(OBJDIR)\$(*B).crf;

!endif



#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\mini.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib


#
# Say that we actually have an definition file.
#
DEFFILE = $(TARGET).def


#
# Target name - name of the exe without extention and path.
#
TARGET  = mini


#
# Main rule
#
!ifdef NORMAL
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    -4 $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf
    $(LXLITE) $@

!else

$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    link $(OBJS), $(OBJDIR)\$(TARGET)ne.exe, $(OBJDIR)\$(TARGET).map;
    myexe2bin $(OBJDIR)\$(TARGET)ne.exe $@ 512

#    exe2bin $(OBJDIR)\$(TARGET)ne.exe $@

!endif


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




