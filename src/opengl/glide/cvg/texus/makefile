# $Id: makefile,v 1.3 2000-11-19 08:58:11 bird Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\..\tools\bin


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
CINCLUDES   = $(CINCLUDES) -I..\incsrc;..\..\swlibs\fxmisc;..\init;..\glide;..\..\swlibs\pcilib


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\3df.obj \
$(OBJDIR)\clamp.obj \
$(OBJDIR)\dequant.obj \
$(OBJDIR)\diffuse.obj \
$(OBJDIR)\gldutil.obj \
$(OBJDIR)\mipmap.obj \
$(OBJDIR)\ncc.obj \
$(OBJDIR)\nccnnet.obj \
$(OBJDIR)\pal256.obj \
$(OBJDIR)\ppm.obj \
$(OBJDIR)\quantize.obj \
$(OBJDIR)\read.obj \
$(OBJDIR)\resample.obj \
$(OBJDIR)\rgt.obj \
$(OBJDIR)\texuslib.obj \
$(OBJDIR)\tga.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\view.obj \
$(OBJDIR)\write.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = texus


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds depenencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc*.h ..\glide\*.h ..\init\*.h \
        ..\..\swlibs\pcilib\*.h

