# $Id: makefile,v 1.4 2001-10-01 01:41:21 bird Exp $

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
$(OBJDIR)\amerror.obj \
$(OBJDIR)\amundoc.obj \
$(OBJDIR)\complist.obj \
$(OBJDIR)\fgclsid.obj \
$(OBJDIR)\fgidisp.obj \
$(OBJDIR)\fgraph.obj \
$(OBJDIR)\fmap.obj \
$(OBJDIR)\fmap2.obj \
$(OBJDIR)\ibasaud.obj \
$(OBJDIR)\ibasvid.obj \
$(OBJDIR)\ifgraph.obj \
$(OBJDIR)\ifmap.obj \
$(OBJDIR)\ifmap3.obj \
$(OBJDIR)\igrver.obj \
$(OBJDIR)\imcntl.obj \
$(OBJDIR)\imem.obj \
$(OBJDIR)\imesink.obj \
$(OBJDIR)\imevent.obj \
$(OBJDIR)\imfilter.obj \
$(OBJDIR)\impos.obj \
$(OBJDIR)\imseek.obj \
$(OBJDIR)\irclock.obj \
$(OBJDIR)\iunk.obj \
$(OBJDIR)\ividwin.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\memalloc.obj \
$(OBJDIR)\monprop.obj \
$(OBJDIR)\regsvr.obj \
$(OBJDIR)\seekpass.obj \
$(OBJDIR)\sysclock.obj \
$(OBJDIR)\devenum.obj \
$(OBJDIR)\devmon.obj \
$(OBJDIR)\enumunk.obj \
$(OBJDIR)\idevenum.obj \
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

