# $Id: winmmlib.mak,v 1.2 2001-07-29 19:00:53 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1
WRC_PREFIX_RESOURCE=1


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
$(OBJDIR)\midi.obj \
$(OBJDIR)\irtmidi.obj \
$(OBJDIR)\midistrm.obj \
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
# Target name - name of the library without extention and path.
#
TARGET  = winmmo


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

