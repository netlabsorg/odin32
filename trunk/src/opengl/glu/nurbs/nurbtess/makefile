# $Id: makefile,v 1.5 2000-11-19 08:58:14 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/nurbtess makefile
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
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\..\mesa\*.h ..\nurbtess\*.h ..\internals\*.h ..\..\include\*.h

