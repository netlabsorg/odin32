# $Id: makefile,v 1.4 2000-12-03 00:26:12 bird Exp $

#
# Odin32 API
#
#       Glide makefile
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
!include ../3dfx.mak


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
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds depenencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc*.h ..\glide\*.h ..\init\*.h \
        ..\..\swlibs\pcilib\*.h

