# $Id: makefile,v 1.5 2000-12-03 00:26:13 bird Exp $

#
# Odin32 API
#
#       sst1\Init makefile
#


#
# Tell buildenvironmet to build an object library and to invoke additional dep rule.
#
LIBTARGET = 1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include ../../../../../makefile.inc
!include ..\3dfx.mak


#
# Flag overloads.
#
CINCLUDES   = $(CINCLUDES) -I..\incsrc;..\..\swlibs\fxmisc;initvg;..\glide;..\texus;..\..\swlibs\pcilib


#
# Subdirectories to travers.
#
SUBDIRS = \
initvg


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
INITSUB = initvg
OBJS = \
$(OBJDIR)\h3drvr.obj \
$(OBJDIR)\init.obj \
$(OBJDIR)\vg96drvr.obj \
$(OBJDIR)\vgdrvr.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = init


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds depenencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h initvg\*.h ..\glide\*.h \
        ..\texus\*.h ..\..\swlibs\pcilib\*.h

