# $Id: makefile,v 1.3 2000-11-19 08:58:11 bird Exp $
#
# Odin32 API
#
#       cvg\Init makefile
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin


#
# Tell buildenvironmet to build an object library and to invoke additional dep rule.
#
LIBTARGET = 1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk
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
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\glide\*.h ..\texus\*.h \
        ..\..\swlibs\pcilib\*.h

