# $Id: makefile,v 1.5 2000-11-19 08:58:11 bird Exp $

#
# Odin32 API
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
CINCLUDES   = $(CINCLUDES) -I..\incsrc -I..\..\swlibs\fxmisc -I..\init\initvg -I..\init -I..\texus -I..\..\swlibs\pcilib
CFLAGS      = $(CFLAGS)   /Os-
CXXFLAGS    = $(CXXFLAGS) /Os-


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
$(OBJDIR)\fifo.obj \
$(OBJDIR)\glidersrc.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = glide


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds depenencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\init\initvg\*.h \
        ..\init\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h

