# $Id: makefile,v 1.5 2001-10-06 08:54:26 sandervl Exp $

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
!ifndef WMAKE
CDEFINES    = $(CDEFINES) -DICOM_CINTERFACE -DCINTERFACE
!else
CDEFINES   += -DICOM_CINTERFACE -DCINTERFACE
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\amundoc.obj \
$(OBJDIR)\audren.obj \
$(OBJDIR)\basefilt.obj \
$(OBJDIR)\basepin.obj \
$(OBJDIR)\complist.obj \
$(OBJDIR)\devenum.obj \
$(OBJDIR)\devmon.obj \
$(OBJDIR)\enumunk.obj \
$(OBJDIR)\fgevent.obj \
$(OBJDIR)\fgidisp.obj \
$(OBJDIR)\fgpass.obj \
$(OBJDIR)\fgraph.obj \
$(OBJDIR)\fmap.obj \
$(OBJDIR)\fmap2.obj \
$(OBJDIR)\ifgraph.obj \
$(OBJDIR)\igconfig.obj \
$(OBJDIR)\igrver.obj \
$(OBJDIR)\imcntl.obj \
$(OBJDIR)\imfilter.obj \
$(OBJDIR)\impos.obj \
$(OBJDIR)\imseek.obj \
$(OBJDIR)\iunk.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\memalloc.obj \
$(OBJDIR)\mtype.obj \
$(OBJDIR)\regsvr.obj \
$(OBJDIR)\sample.obj \
$(OBJDIR)\seekpass.obj \
$(OBJDIR)\sysclock.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initquartz.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\quartzrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/oleaut32.lib \
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

