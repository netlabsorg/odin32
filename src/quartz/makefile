# $Id: makefile,v 1.2 2001-08-23 18:34:22 sandervl Exp $

#
# Odin32 API
#
#       quartz.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WAT
CDEFINES    = $(CDEFINES) -DICOM_CINTERFACE -DCINTERFACE
!else
CDEFINES   += -DICOM_CINTERFACE -DCINTERFACE
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\amerror.obj \
$(OBJDIR)\complist.obj \
$(OBJDIR)\fgraph.obj \
$(OBJDIR)\ifgraph.obj \
$(OBJDIR)\imem.obj \
$(OBJDIR)\ibasaud.obj \
$(OBJDIR)\ibasvid.obj \
$(OBJDIR)\imcntl.obj \
$(OBJDIR)\imevent.obj \
$(OBJDIR)\impos.obj \
$(OBJDIR)\imseek.obj \
$(OBJDIR)\ividwin.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initquartz.obj \
$(OBJDIR)\irclock.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\iunk.obj \
$(OBJDIR)\memalloc.obj \
$(OBJDIR)\sysclock.obj \
$(OBJDIR)\quartzrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = quartz

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

