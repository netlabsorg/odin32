# $Id: makefile,v 1.7 2001-10-01 01:40:56 bird Exp $

#
# Odin32 API
#
#       glu/tess makefile
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
!else
!ifdef WAT
C           = $(CXX)
CFLAGS      = $(CXXFLAGS)
!else
!error "Compiler is not yet supported."
!endif
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\dict.obj \
$(OBJDIR)\memalloc.obj \
$(OBJDIR)\mesh.obj \
$(OBJDIR)\normal.obj \
$(OBJDIR)\priorityq.obj \
$(OBJDIR)\render.obj \
$(OBJDIR)\sweep.obj \
$(OBJDIR)\tess.obj \
$(OBJDIR)\tessmono.obj \
$(OBJDIR)\geom.obj


#
# Target name - name of the library without extention and path.
#
TARGET = tess


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\..\mesa\*.h ..\*.h ..\include\*.h

