# $Id: makefile,v 1.4 2000-12-02 23:32:41 bird Exp $

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
LD2FLAGS = $(LD2FLAGS) /BASE:0x10000 /Stack:0xee00


#
# Interference rule. Note: ALP specific; enables listing.
# (For Debug)
#
#.asm{$(OBJDIR)}.obj:
#    $(AS) $(ASFLAGS) $< -Fo:$@ -Fl:$(OBJDIR)\$(*B).lst


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kRxa.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib \
rexx.lib


#
# Say that we actually have an definition file.
#
DEFFILE = $(TARGET).def


#
# Target name - name of the exe without extention and path.
#
TARGET  = kRx


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

