# $Id: makefile,v 1.4 2000-12-03 00:26:13 bird Exp $

#
# Odin32 API
#
#       sst1/Glide makefile
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
CINCLUDES   = $(CINCLUDES) -I..\incsrc;..\..\swlibs\fxmisc;..\init\initvg;..\init;..\texus;..\..\swlibs\pcilib;


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\cpudtect.obj \
$(OBJDIR)\ddgump.obj \
$(OBJDIR)\diglide.obj \
$(OBJDIR)\digutex.obj \
$(OBJDIR)\disst.obj \
$(OBJDIR)\ditex.obj \
$(OBJDIR)\g3df.obj \
$(OBJDIR)\gaa.obj \
$(OBJDIR)\gbanner.obj \
$(OBJDIR)\gdraw.obj \
$(OBJDIR)\gerror.obj \
$(OBJDIR)\gglide.obj \
$(OBJDIR)\glfb.obj \
$(OBJDIR)\gmovie.obj \
$(OBJDIR)\gpci.obj \
$(OBJDIR)\gsplash.obj \
$(OBJDIR)\gsst.obj \
$(OBJDIR)\gtex.obj \
$(OBJDIR)\gtexdl.obj \
$(OBJDIR)\gu.obj \
$(OBJDIR)\guclip.obj \
$(OBJDIR)\gump.obj \
$(OBJDIR)\gutex.obj \
$(OBJDIR)\gxdraw.obj \
$(OBJDIR)\p6fence.obj \
$(OBJDIR)\sst96.obj \
$(OBJDIR)\glidersrc.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = glide


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\init\initvg\*.h \
        ..\init\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h

