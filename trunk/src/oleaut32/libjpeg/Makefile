# $Id: Makefile,v 1.1 2003-10-06 10:09:07 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


CDEFINES = $(CDEFINES) -I..\include

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#


OBJS = \
$(OBJDIR)\jcapimin.obj \
$(OBJDIR)\jcapistd.obj \
$(OBJDIR)\jccoefct.obj \
$(OBJDIR)\jccolor.obj \
$(OBJDIR)\jcdctmgr.obj \
$(OBJDIR)\jchuff.obj \
$(OBJDIR)\jcinit.obj \
$(OBJDIR)\jcmainct.obj \
$(OBJDIR)\jcmarker.obj \
$(OBJDIR)\jcmaster.obj \
$(OBJDIR)\jcomapi.obj \
$(OBJDIR)\jcparam.obj \
$(OBJDIR)\jcphuff.obj \
$(OBJDIR)\jcprepct.obj \
$(OBJDIR)\jcsample.obj \
$(OBJDIR)\jctrans.obj \
$(OBJDIR)\jdapimin.obj \
$(OBJDIR)\jdapistd.obj \
$(OBJDIR)\jdatadst.obj \
$(OBJDIR)\jdatasrc.obj \
$(OBJDIR)\jdcoefct.obj \
$(OBJDIR)\jdcolor.obj \
$(OBJDIR)\jddctmgr.obj \
$(OBJDIR)\jdhuff.obj \
$(OBJDIR)\jdinput.obj \
$(OBJDIR)\jdmainct.obj \
$(OBJDIR)\jdmarker.obj \
$(OBJDIR)\jdmaster.obj \
$(OBJDIR)\jdmerge.obj \
$(OBJDIR)\jdphuff.obj \
$(OBJDIR)\jdpostct.obj \
$(OBJDIR)\jdsample.obj \
$(OBJDIR)\jdtrans.obj \
$(OBJDIR)\jerror.obj \
$(OBJDIR)\jfdctflt.obj \
$(OBJDIR)\jfdctfst.obj \
$(OBJDIR)\jfdctint.obj \
$(OBJDIR)\jidctflt.obj \
$(OBJDIR)\jidctfst.obj \
$(OBJDIR)\jidctint.obj \
$(OBJDIR)\jidctred.obj \
$(OBJDIR)\jquant1.obj \
$(OBJDIR)\jquant2.obj \
$(OBJDIR)\jutils.obj \
$(OBJDIR)\jmemmgr.obj \
$(OBJDIR)\jmemnobs.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = jpeglib


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

