# $Id: makefile,v 1.5 2000-11-19 08:58:15 bird Exp $

#
# Odin32 API
#
#       glu/util makefile
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


#
# Tell buildenvironmet to build an object library and to invoke additional dep rule.
#
LIBTARGET = 1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk


#
# Flag overloads.
#
CINCLUDES   = $(CINCLUDES) -I.. -I..\..\mesa -I..\include -I..\..\mesa\GL
CDEFINES    = $(CDEFINES) -DNDEBUG
CFLAGS      = $(CFLAGS) -Tdp
CXXFLAGS    = $(CXXFLAGS) -Tdp
!ifdef WAT
!error "Compiler not supported yet!"
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\error.obj \
$(OBJDIR)\glue.obj \
$(OBJDIR)\mipmap.obj \
$(OBJDIR)\project.obj \
$(OBJDIR)\quad.obj \
$(OBJDIR)\registry.obj


#
# Target name - name of the library without extention and path.
#
TARGET = util


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\include\*.h ..\..\mesa\*.h ..\*.h

