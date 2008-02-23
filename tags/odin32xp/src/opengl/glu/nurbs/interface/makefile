# $Id: makefile,v 1.8 2000-12-02 23:59:44 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/interface makefile
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


#
# Flag overloads.
#
CINCLUDES   = $(CINCLUDES) -I..\..\..\mesa\GL -I..\.. -I..\internals \
              -I..\nurbtess -I..\..\include  -I..\..\..\mesa
CDEFINES    = $(CDEFINES) -DNDEBUG


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\glcurveval.obj \
$(OBJDIR)\glinterface.obj \
$(OBJDIR)\glrenderer.obj \
$(OBJDIR)\glsurfeval.obj \
$(OBJDIR)\insurfeval.obj \
$(OBJDIR)\bezierPatch.obj \
$(OBJDIR)\bezierEval.obj \
$(OBJDIR)\bezierPatchMesh.obj \
$(OBJDIR)\incurveeval.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = interface


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\internals\*.h ..\nurbtess\*.h ..\..\..\mesa\GL\*.h

