# $Id: makefile,v 1.3 2000-03-01 14:50:17 sandervl Exp $
#
# PD-Win32
#
# Makefile for Mesa OpenGL
#

PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

!ifdef DEBUG
L=/CO
!endif

MESADEFS=-DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DTHREADS \
         -DUSE_X86_ASM_NOT -DUSE_MMX_ASM_NOT -DUSE_3DNOW_ASM_NOT \
         /DFX /DFX_V2 /DFX_SILENT /DMESA_FX_DDRAW /DBUILD_GL32

CFLAGS = -I.. -I..\GL $(CFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS) /Wcnd- /Tdp
CXXFLAGS = -I.. -I..\GL $(CXXFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS)

TARGET = OPENGL32

all: $(TARGET).DLL

OBJS = 	fxapi.obj \
	fxcva.obj \
	fxclip.obj \
	fxdd.obj \
	fxddspan.obj \
	fxddtex.obj \
	fxglidew.obj \
	fxfastpath.obj \
	fxpipeline.obj \
	fxrender.obj \
	fxsanity.obj \
	fxsetup.obj \
	fxtexman.obj \
	fxtrifuncs.obj \
	fxvsetup.obj \
	fxwgl.obj initterm.obj resource.obj \
        accum.obj alpha.obj alphabuf.obj api1.obj api2.obj apiext.obj \
        attrib.obj bbox.obj bitmap.obj blend.obj clip.obj colortab.obj config.obj context.obj \
	copypix.obj cva.obj depth.obj dlist.obj drawpix.obj enable.obj enums.obj eval.obj \
        extensions.obj feedback.obj fog.obj get.obj glmisc.obj hash.obj image.obj light.obj \
        lines.obj logic.obj masking.obj matrix.obj mmath.obj mthreads.obj pb.obj pipeline.obj \
        pixel.obj pointers.obj points.obj polygon.obj quads.obj rastpos.obj readpix.obj \
        rect.obj scissor.obj shade.obj span.obj stages.obj stencil.obj teximage.obj texobj.obj texstate.obj \
        texture.obj translate.obj triangle.obj varray.obj vb.obj vbcull.obj vbfill.obj \
        vbindirect.obj vbrender.obj vbxform.obj vector.obj vertices.obj winpos.obj xform.obj zoom.obj 

$(TARGET).DLL: $(OBJS) $(TARGET).lrf $(TARGET).def
    -ILINK /FREE /NOE $(L) @$(TARGET).lrf
    $(CP) $@ $(PDWIN32_BIN)\Glide

$(TARGET).lrf: makefile
  @echo Creating file <<$(@B).lrf
$(OBJS)
/OUT:$(TARGET).DLL
$(PDWIN32_LIB)\ODINCRT.LIB
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\GDI32.LIB
$(PDWIN32_LIB)\USER32.LIB
$(PDWIN32_LIB)\glide2x.lib
$(RTLLIB_O)
OS2386.LIB
$(TARGET).def
<<keep

resource.asm: fxmesa32.rc
    $(RC) $(RCFLAGS) -o resource.asm fxmesa32.rc

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\..\glut -I..\GL -I.. -I$(PDWIN32_INCLUDE)\glide \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h  

!ifndef NODEP
!include .depend
!endif

clean:
        $(RM) *.obj 
        $(RM) *.dll 
        $(RM) *.lib 
        $(RM) *.lrf
        $(RM) resource.asm
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

