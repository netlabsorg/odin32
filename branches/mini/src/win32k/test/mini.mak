# $Id: mini.mak,v 1.1.2.9 2001-08-20 19:11:36 bird Exp $

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
# Target name - name of the exe without extention and path.
#
!ifdef NORMAL
TARGET  = minic
!else
TARGET  = minio
!endif
DEFFILE = mini.def



!ifdef NORMAL
#
# Main rule for the high octane category.
#
!if 1
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
!ifdef WATCOM # 274 bytes
    wlink system os2v2 file {$(OBJS)} name $(OBJDIR)\.exe \
    import vprintf LIBCN.150 \
#    import DosPutMessage MSG.5   \
        option offset=0x0000 option alignment=1 option stack=4060
# skip this step if it aint allowed, but then LXLITE has to get $(OBJDIR)\.exe as input. :-)
    mv $(OBJDIR)\.exe $@
!else
!if 0
    link386 /ALIGNMENT:1 /NONULLSDOSSEG /NOSECTORALIGNCODE /BASE:0x10000 /PACKCODE /PACKDATA \
        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;
!else
!ifndef VAC36 # 273 bytes
    -12 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 /PACKCODE /PACKDATA /NOEXEPACK \
        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;
!else
    -12 ilink /FORCE /ALIGNFILE:1 /Map /BASE:0x10000 /FIXED /PACKCODE /PACKDATA /NOEXEPACK /STACK:4060 \
        $(OBJS) /OUT:$(OBJDIR)\$(TARGET).exe /MAP:$(OBJDIR)\$(TARGET).map mini.def \
        /section:CODE32,RW
!endif
    exehdr /RESETERROR $@
!endif
!endif
    $(LXLITE) /MR1 /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND $@
!else
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    -8 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 \
            /PACKCODE /PACKDATA /EXEPACK:1 /STACK:4000  \
            $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib libcmi.lib, mini.def;
    $(LXLITE) /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND $@
!endif

!else

#
# Builds the custom 196 bytes mini.exe.
#
$(OBJDIR)\$(TARGET).exe: $(OBJS)  $(DEFFILE) myexe2bin.exe
    link $(OBJS), $(OBJDIR)\$(TARGET)ne.exe, $(OBJDIR)\$(TARGET).map;
    myexe2bin $(OBJDIR)\$(TARGET)ne.exe $@ 512

# help tool, custom exe2bin.
myexe2bin.exe myexe2bin: myexe2bin.c
!ifndef WATCOM
    icc myexe2bin.c
!else
    wcl386 myexe2bin.c
!endif
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




