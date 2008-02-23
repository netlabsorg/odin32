# $Id: winmm.mak,v 1.14 2004-12-30 18:45:47 sao2l02 Exp $

#
# Odin32 API
#
#       winmm.dll makefile
#


#
# Target name - name of the dll without extention and path.
#
TARGET = winmm


#
# Alternate makefile name.
#
MAKEFILE = winmm.mak


#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\os2timer.obj \
$(OBJDIR)\waveout.obj \
$(OBJDIR)\waveoutdart.obj \
$(OBJDIR)\waveoutbase.obj \
$(OBJDIR)\waveinoutbase.obj \
$(OBJDIR)\waveoutdaud.obj \
$(OBJDIR)\waveindart.obj \
$(OBJDIR)\wavein.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\auxiliary.obj \
$(OBJDIR)\auxos2.obj \
$(OBJDIR)\mixer.obj \
$(OBJDIR)\mixeros2.obj \
$(OBJDIR)\mixerdata.obj \
$(OBJDIR)\midi.obj \
$(OBJDIR)\irtmidi.obj \
$(OBJDIR)\midistrm.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initwinmm.obj \
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
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
