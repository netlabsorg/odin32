# $Id: winmm.mak,v 1.12 2003-08-08 15:54:57 sandervl Exp $

#
# Odin32 API
#
#       winmm.dll makefile
#


#
# Alternate makefile name.
#
!if "$(DEBUG)" == "1"
DEFFILE    = winmmdbg.def
ORGDEFFILE = winmm.def
!endif
WRC_PREFIX_RESOURCE=1

MAKEFILE = winmm.mak


#
# Compiler, tools, and interference rules.
#
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
!ifdef DEBUG
$(OBJDIR)\dbgwrap.obj \
!endif
$(OBJDIR)\dbglocal.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/libwrap.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = winmm


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

