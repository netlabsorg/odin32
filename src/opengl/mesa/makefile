# $Id: makefile,v 1.1 2000-02-29 00:50:06 sandervl Exp $
#
# PD-Win32
#
# Makefile for Mesa OpenGL
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

!ifdef DEBUG
L=/CO
!endif

#MESADEFS=-DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DTHREADS -DDDRAW \
MESADEFS=-DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DTHREADS \
         -DUSE_X86_ASM_NOT -DUSE_MMX_ASM_NOT -DUSE_3DNOW_ASM_NOT

CFLAGS = -I. -IGL $(CFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\glut $(MESADEFS) /Tdp /Wcnd-
CXXFLAGS = -I. -IGL $(CXXFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\glut $(MESADEFS) /Tdp /Wcnd-

TARGET = OPENGL32

all: $(TARGET).DLL $(TARGET).LIB

OBJS = \
  wgl.obj \
  accum.obj \
  alpha.obj \
  alphabuf.obj \
  api1.obj \
  api2.obj \
  apiext.obj \
  attrib.obj \
  bbox.obj \
  bitmap.obj \
  blend.obj \
  clip.obj \
  colortab.obj \
  config.obj \
  context.obj \
  copypix.obj \
  cva.obj \
  depth.obj \
  dlist.obj \
  drawpix.obj \
  enable.obj \
  enums.obj \
  eval.obj \
  extensions.obj \
  feedback.obj \
  fog.obj \
  get.obj \
  glmisc.obj \
  hash.obj \
  image.obj \
  light.obj \
  lines.obj \
  logic.obj \
  masking.obj \
  matrix.obj \
  mmath.obj \
  pb.obj \
  pixel.obj \
  pipeline.obj \
  points.obj \
  pointers.obj \
  polygon.obj \
  quads.obj \
  rastpos.obj \
  readpix.obj \
  rect.obj \
  scissor.obj \
  shade.obj \
  span.obj \
  stages.obj \
  stencil.obj \
  teximage.obj \
  texobj.obj \
  texstate.obj \
  translate.obj \
  triangle.obj \
  varray.obj \
  vb.obj \
  vbcull.obj \
  vbfill.obj \
  vbindirect.obj \
  vbrender.obj \
  vbxform.obj \
  vector.obj \
  vertices.obj \
  winpos.obj \
  xform.obj \
  zoom.obj \
  x86.obj \
  common_x86.obj \
  initterm.obj \
  texture.obj \
  mthreads.obj \
  wmesa.obj

$(TARGET).DLL: $(OBJS) $(TARGET).lrf $(TARGET).def
    -ILINK /FREE /NOE $(L) @$(TARGET).lrf
    $(CP) $@ $(PDWIN32_BIN)

$(TARGET).lrf: makefile
  @echo Creating file <<$(@B).lrf
$(OBJS)
/OUT:$(TARGET).DLL
$(PDWIN32_LIB)\ODINCRT.LIB
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\GDI32.LIB
$(PDWIN32_LIB)\USER32.LIB
$(RTLLIB_O)
OS2386.LIB
$(TARGET).def
<<keep

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -IGL \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif

clean:
        $(RM) *.obj *.dll *.lib *.lrf
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

