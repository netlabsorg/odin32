# $Id: comdlg32lib.mak,v 1.2 2001-07-29 19:00:13 sandervl Exp $

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
# Overrides.
#
!if "$(CCENV)" == "VAC36"
CDEFINES    = $(CDEFINES) -DNONAMELESSUNION
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\colordlg.obj \
$(OBJDIR)\fontdlg.obj \
$(OBJDIR)\initcomdlg32.obj \
$(OBJDIR)\cdlg32.obj \
$(OBJDIR)\filetitle.obj \
$(OBJDIR)\finddlg32.obj \
$(OBJDIR)\filedlg.obj \
$(OBJDIR)\filedlg95.obj \
$(OBJDIR)\comdlg32.obj \
$(OBJDIR)\filedlgbrowser.obj \
##$(OBJDIR)\printdlg.obj \
####$(OBJDIR)\comdlgguid.obj \
$(OBJDIR)\rsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = comdlg32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

