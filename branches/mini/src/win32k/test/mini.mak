# $Id: mini.mak,v 1.1.2.4 2001-08-14 22:14:34 bird Exp $

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
!if 1
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    wlink system os2v2 file {$(OBJS)} name $(OBJDIR)\.exe \
    import vprintf LIBCN.150 \
#    import DosPutMessage MSG.5   \
        option offset=0x0000 option alignment=1 option stack=4060
    mv $(OBJDIR)\.exe $@
#    link386 /ALIGNMENT:1 /NONULLSDOSSEG /NOSECTORALIGNCODE /BASE:0x10000 /PACKCODE /PACKDATA \
#        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;
#    -8 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 /PACKCODE /PACKDATA /EXEPACK:1 \
#        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;
    $(LXLITE) /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND $@
!else
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    -8 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 \
            /PACKCODE /PACKDATA /EXEPACK:1 /STACK:4000  \
            $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib libcmi.lib, mini.def;
    $(LXLITE) /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND $@
!endif

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




