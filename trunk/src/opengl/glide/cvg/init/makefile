# $Id: makefile,v 1.4 2000-12-03 00:26:12 bird Exp $

#
# Odin32 API
#
#       cvg\Init makefile
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
CINCLUDES = $(CINCLUDES) -I..\incsrc;..\..\swlibs\fxmisc;..\glide;..\texus;..\..\swlibs\pcilib


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dac.obj \
$(OBJDIR)\gamma.obj \
$(OBJDIR)\gdebug.obj \
$(OBJDIR)\info.obj \
$(OBJDIR)\parse.obj \
$(OBJDIR)\print.obj \
$(OBJDIR)\sli.obj \
$(OBJDIR)\sst1init.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\video.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = init


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\glide\*.h ..\texus\*.h \
        ..\..\swlibs\pcilib\*.h

