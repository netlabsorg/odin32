# $Id: makefile,v 1.4 2000-11-19 08:58:12 bird Exp $
#
# Odin32 API
#
#       swlibs\pcilib makefile
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
!include ..\..\sst1\3dfx.mak


#
# Flag overloads.
#
CINCLUDES = $(CINCLUDES) -I..\..\sst1\incsrc;..\fxmisc;..\..\sst1\init\initvg;..\..\sst1\glide;..\..\sst1\texus;..\..\sst1\init


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\fxos2.obj \
$(OBJDIR)\fxpci.obj \
$(OBJDIR)\sst1_pci.obj \
$(OBJDIR)\fxinfo.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = pcilib


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds depenencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\..\sst1\incsrc\*.h ..\fxmisc\*.h ..\..\sst1\init\initvg\*.h \
        ..\..\sst1\glide\*.h ..\..\sst1\texus\*.h ..\..\sst1\init\*.h

