# $Id: makefile,v 1.4 2000-03-02 13:25:33 sandervl Exp $
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
         /DFX /DFX_V2 /DFX_SILENT /DBUILD_GL32

CFLAGS = -I.. -I..\GL $(CFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS) /Wcnd- /Tdp
CXXFLAGS = -I.. -I..\GL $(CXXFLAGS_WIN32APP) -I$(PDWIN32_INCLUDE) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS)

TARGET = OPENGL32

all: $(TARGET).DLL

OBJS =  fxapi.obj \
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
    vbindirect.obj vbrender.obj vbxform.obj vector.obj vertices.obj winpos.obj xform.obj zoom.obj \
    osmesa.obj

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

accum.obj: ..\accum.c
    $(CC) $(CFLAGS) -c ..\accum.c

alpha.obj: ..\alpha.c
    $(CC) $(CFLAGS) -c ..\alpha.c
alphabuf.obj: ..\alphabuf.c
    $(CC) $(CFLAGS) -c ..\alphabuf.c
api1.obj: ..\api1.c
    $(CC) $(CFLAGS) -c ..\api1.c
api2.obj: ..\api2.c
    $(CC) $(CFLAGS) -c ..\api2.c
apiext.obj: ..\apiext.c
    $(CC) $(CFLAGS) -c ..\apiext.c
attrib.obj: ..\attrib.c
    $(CC) $(CFLAGS) -c ..\attrib.c
bbox.obj: ..\bbox.c
    $(CC) $(CFLAGS) -c ..\bbox.c
bitmap.obj: ..\bitmap.c
    $(CC) $(CFLAGS) -c ..\bitmap.c
blend.obj: ..\blend.c
    $(CC) $(CFLAGS) -c ..\blend.c
clip.obj: ..\clip.c
    $(CC) $(CFLAGS) -c ..\clip.c
colortab.obj: ..\colortab.c
    $(CC) $(CFLAGS) -c ..\colortab.c
config.obj: ..\config.c
    $(CC) $(CFLAGS) -c ..\config.c
context.obj: ..\context.c
    $(CC) $(CFLAGS) -c ..\context.c
copypix.obj: ..\copypix.c
    $(CC) $(CFLAGS) -c ..\copypix.c
cva.obj: ..\cva.c
    $(CC) $(CFLAGS) -c ..\cva.c
depth.obj: ..\depth.c
    $(CC) $(CFLAGS) -c ..\depth.c
dlist.obj: ..\dlist.c
    $(CC) $(CFLAGS) -c ..\dlist.c
drawpix.obj: ..\drawpix.c
    $(CC) $(CFLAGS) -c ..\drawpix.c
enable.obj: ..\enable.c
    $(CC) $(CFLAGS) -c ..\enable.c
enums.obj: ..\enums.c
    $(CC) $(CFLAGS) -c ..\enums.c
eval.obj: ..\eval.c
    $(CC) $(CFLAGS) -c ..\eval.c
extensions.obj: ..\extensions.c
    $(CC) $(CFLAGS) -c ..\extensions.c
feedback.obj: ..\feedback.c
    $(CC) $(CFLAGS) -c ..\feedback.c
fog.obj: ..\fog.c
    $(CC) $(CFLAGS) -c ..\fog.c
get.obj: ..\get.c
    $(CC) $(CFLAGS) -c ..\get.c
glmisc.obj: ..\glmisc.c
    $(CC) $(CFLAGS) -c ..\glmisc.c
hash.obj: ..\hash.c
    $(CC) $(CFLAGS) -c ..\hash.c
image.obj: ..\image.c
    $(CC) $(CFLAGS) -c ..\image.c
light.obj: ..\light.c
    $(CC) $(CFLAGS) -c ..\light.c
lines.obj: ..\lines.c
    $(CC) $(CFLAGS) -c ..\lines.c
logic.obj: ..\logic.c
    $(CC) $(CFLAGS) -c ..\logic.c
masking.obj: ..\masking.c
    $(CC) $(CFLAGS) -c ..\masking.c
matrix.obj: ..\matrix.c
    $(CC) $(CFLAGS) -c ..\matrix.c
mmath.obj: ..\mmath.c
    $(CC) $(CFLAGS) -c ..\mmath.c
mthreads.obj: ..\mthreads.c
    $(CC) $(CFLAGS) -c ..\mthreads.c
pb.obj: ..\pb.c
    $(CC) $(CFLAGS) -c ..\pb.c
pipeline.obj: ..\pipeline.c
    $(CC) $(CFLAGS) -c ..\pipeline.c
pixel.obj: ..\pixel.c
    $(CC) $(CFLAGS) -c ..\pixel.c
pointers.obj: ..\pointers.c
    $(CC) $(CFLAGS) -c ..\pointers.c
points.obj: ..\points.c
    $(CC) $(CFLAGS) -c ..\points.c
polygon.obj: ..\polygon.c
    $(CC) $(CFLAGS) -c ..\polygon.c
quads.obj: ..\quads.c
    $(CC) $(CFLAGS) -c ..\quads.c
rastpos.obj: ..\rastpos.c
    $(CC) $(CFLAGS) -c ..\rastpos.c
readpix.obj: ..\readpix.c
    $(CC) $(CFLAGS) -c ..\readpix.c
rect.obj: ..\rect.c
    $(CC) $(CFLAGS) -c ..\rect.c
scissor.obj: ..\scissor.c
    $(CC) $(CFLAGS) -c ..\scissor.c
shade.obj: ..\shade.c
    $(CC) $(CFLAGS) -c ..\shade.c
span.obj: ..\span.c
    $(CC) $(CFLAGS) -c ..\span.c
stages.obj: ..\stages.c
    $(CC) $(CFLAGS) -c ..\stages.c
stencil.obj: ..\stencil.c
    $(CC) $(CFLAGS) -c ..\stencil.c
teximage.obj: ..\teximage.c
    $(CC) $(CFLAGS) -c ..\teximage.c
texobj.obj: ..\texobj.c
    $(CC) $(CFLAGS) -c ..\texobj.c
texstate.obj: ..\texstate.c
    $(CC) $(CFLAGS) -c ..\texstate.c
texture.obj: ..\texture.c
    $(CC) $(CFLAGS) -c ..\texture.c
translate.obj: ..\translate.c
    $(CC) $(CFLAGS) -c ..\translate.c
triangle.obj: ..\triangle.c
    $(CC) $(CFLAGS) -c ..\triangle.c
varray.obj: ..\varray.c
    $(CC) $(CFLAGS) -c ..\varray.c
vb.obj: ..\vb.c
    $(CC) $(CFLAGS) -c ..\vb.c
vbcull.obj: ..\vbcull.c
    $(CC) $(CFLAGS) -c ..\vbcull.c
vbfill.obj: ..\vbfill.c
    $(CC) $(CFLAGS) -c ..\vbfill.c
vbindirect.obj: ..\vbindirect.c
    $(CC) $(CFLAGS) -c ..\vbindirect.c
vbrender.obj: ..\vbrender.c
    $(CC) $(CFLAGS) -c ..\vbrender.c
vbxform.obj: ..\vbxform.c
    $(CC) $(CFLAGS) -c ..\vbxform.c
vector.obj: ..\vector.c
    $(CC) $(CFLAGS) -c ..\vector.c
vertices.obj: ..\vertices.c
    $(CC) $(CFLAGS) -c ..\vertices.c
winpos.obj: ..\winpos.c
    $(CC) $(CFLAGS) -c ..\winpos.c
xform.obj: ..\xform.c
    $(CC) $(CFLAGS) -c ..\xform.c
zoom.obj: ..\zoom.c
    $(CC) $(CFLAGS) -c ..\zoom.c
osmesa.obj: ..\osmesa.c
    $(CC) $(CFLAGS) -c ..\osmesa.c
wmesa.obj: ..\wmesa.obj
    $(CC) $(CFLAGS) -c ..\osmesa.obj
internal.obj: ..\internal.c
    $(CC) $(CFLAGS) -c ..\internal.c


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\..\glut -I..\GL -I.. -I$(PDWIN32_INCLUDE)\glide \
        *.c *.cpp *.h *.asm *.inc ..\*.c $(PDWIN32_INCLUDE)\*.h

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

