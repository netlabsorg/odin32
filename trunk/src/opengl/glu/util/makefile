# $Id: makefile,v 1.6 2000-12-02 23:59:06 bird Exp $

#
# Odin32 API
#
#       glu/util makefile
#


#
# Tell buildenvironmet to build an object library and to invoke additional dep rule.
#
LIBTARGET = 1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include ../../../../makefile.inc


#
# Flag overloads.
#
CINCLUDES   = $(CINCLUDES) -I.. -I..\..\mesa -I..\include -I..\..\mesa\GL
CDEFINES    = $(CDEFINES) -DNDEBUG
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS      = $(CFLAGS) -Tdp
CXXFLAGS    = $(CXXFLAGS) -Tdp
!else
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
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\include\*.h ..\..\mesa\*.h ..\*.h

