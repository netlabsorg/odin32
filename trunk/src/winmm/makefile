# $Id: makefile,v 1.20 2000-11-19 08:35:50 bird Exp $

#
# Odin32 API
#
#       winmm.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\os2timer.obj \
$(OBJDIR)\waveout.obj \
$(OBJDIR)\dwaveout.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\wavein.obj \
$(OBJDIR)\auxiliary.obj \
$(OBJDIR)\auxos2.obj \
$(OBJDIR)\mixer.obj \
$(OBJDIR)\midi.obj \
$(OBJDIR)\irtmidi.obj \
$(OBJDIR)\midistrm.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\mci.obj \
$(OBJDIR)\joy.obj \
$(OBJDIR)\mmio.obj \
$(OBJDIR)\driver.obj \
$(OBJDIR)\playsound.obj \
$(OBJDIR)\joyos2.obj \
$(OBJDIR)\winmmrsrc.obj \
$(OBJDIR)\dbglocal.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/pmwinx.lib \
mmpm2.lib \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = winmm


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

