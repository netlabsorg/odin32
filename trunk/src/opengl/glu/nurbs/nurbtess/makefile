# $Id: makefile,v 1.6 2000-12-03 00:25:51 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/nurbtess makefile
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
CINCLUDES   = $(CINCLUDES) -I..\internals -I..\nurbtess -I..\..\..\mesa\GL -I..\..\include
CDEFINES    = $(CDEFINES)  -DNDEBUG


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\directedLine.obj \
$(OBJDIR)\gridWrap.obj \
$(OBJDIR)\monoTriangulation.obj \
$(OBJDIR)\partitionY.obj \
$(OBJDIR)\polyDBG.obj \
$(OBJDIR)\polyUtil.obj \
$(OBJDIR)\primitiveStream.obj \
$(OBJDIR)\quicksort.obj \
$(OBJDIR)\rectBlock.obj \
$(OBJDIR)\sampleComp.obj \
$(OBJDIR)\sampleCompBot.obj \
$(OBJDIR)\sampleCompRight.obj \
$(OBJDIR)\sampleCompTop.obj \
$(OBJDIR)\sampleMonoPoly.obj \
$(OBJDIR)\sampledLine.obj \
$(OBJDIR)\searchTree.obj \
$(OBJDIR)\partitionX.obj \
$(OBJDIR)\monoChain.obj \
$(OBJDIR)\monoPolyPart.obj


#
# Target name - name of the library without extention and path.
#
TARGET = nurbtess


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\..\mesa\*.h ..\nurbtess\*.h ..\internals\*.h ..\..\include\*.h

