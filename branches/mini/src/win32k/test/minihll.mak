# $Id: minihll.mak,v 1.1.2.3 2001-08-15 03:12:25 bird Exp $

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
!ifndef WAT
LD2FLAGS = $(LD2FLAGS) /BASE:0x10000 /Stack:0x1000
!endif

!ifdef WAT
CFLAGS  = -bt=os2v2 -e60 -5r -omlinears -s -w4 -ze -zl -zq -nt=CODEANDDATA
!if "$(%LIBC)" != ""
LIBC=1
!endif
!ifdef LIBC
CFLAGS += -dLIBC=1
!endif
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
!ifndef LIBC
$(OBJDIR)\minihll2.obj \
$(OBJDIR)\minihll.obj \
!else
$(OBJDIR)\minihll2.obj \
$(OBJDIR)\minihll.obj \
!endif


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
!ifndef WAT
    -8 $(LD2) /FORCE $(LD2FLAGS) /BASE:0x10000 /PACKCODE /PACKDATA /NOEXEPACK \
        /ALIGNMENT:1 @$(OBJDIR)\$(TARGET).lrf
!else
    wlink system os2v2 file {$(OBJS)} name $(OBJDIR)\.exe \
        option offset=0x10000 option alignment=1 option stack=4060 \
        option map=$(OBJDIR)\$(TARGET).map \
!ifndef LIBC
        import DosPutMessage MSG.5 \
        option start=minihll
!else
        import _printf_ansi_ LIBCN.83 \
        option start=minihll_
!endif
    mv $(OBJDIR)\.exe $@
!endif
    $(LXLITE) /AN:1 /ZS:1024 /ZX:1024 /MF3 /YXD /YND /ML1  $@

#    -8 ilink /NOFREE /FORCE /ALIGNMENT:1 /Map /BASE:0x10000 /PACKCODE /PACKDATA /EXEPACK:1 \
#        $(OBJS), $(OBJDIR)\$(TARGET).exe, $(OBJDIR)\$(TARGET).map, os2386.lib, mini.def;



#
# Exe: Linker file - creates the parameter file passed on to the linker.
#
!ifndef WAT
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

!else
$(OBJDIR)\$(TARGET).lrf: makefile
    echo $@
!endif




