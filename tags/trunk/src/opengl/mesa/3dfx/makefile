# $Id: makefile,v 1.10 2000-07-19 19:05:16 sandervl Exp $
#
# Odin32 API
#
# Makefile for Mesa OpenGL
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk
MESADEFS= -DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DOS2_THREADS \
          -DUSE_X86_ASM -DUSE_MMX_ASM -DUSE_3DNOW_ASM_NOT \
          -DFX -DFX_V2 -DFX_SILENT -DBUILD_GL32 -DFAST_MATH

CFLAGS   = -I.. -I..\GL $(CFLAGS_WIN32APP) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS) /Wcnd- /Tdp
CXXFLAGS = -I.. -I..\GL $(CXXFLAGS_WIN32APP) -I..\..\glut -I..\..\glide\cvg\glide -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc $(MESADEFS)

ASFLAGS=$(ASFLAGS) -Fdo:$(OBJDIR)


#
# Interference rule for sources in the parent directory.
#
{..}.c{$(OBJDIR)}.obj:
    $(CC) $(CFLAGS) -c -Fo$(OBJDIR)\$(@B).obj $<

{..}.asm{$(OBJDIR)}.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $<


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\fxapi.obj \
$(OBJDIR)\fxcva.obj \
$(OBJDIR)\fxclip.obj \
$(OBJDIR)\fxdd.obj \
$(OBJDIR)\fxddspan.obj \
$(OBJDIR)\fxddtex.obj \
$(OBJDIR)\fxglidew.obj \
$(OBJDIR)\fxfastpath.obj \
$(OBJDIR)\fxpipeline.obj \
$(OBJDIR)\fxrender.obj \
$(OBJDIR)\fxsanity.obj \
$(OBJDIR)\fxsetup.obj \
$(OBJDIR)\fxtexman.obj \
$(OBJDIR)\fxtrifuncs.obj \
$(OBJDIR)\fxvsetup.obj \
$(OBJDIR)\fxwgl.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\fxmesa32.obj \
$(OBJDIR)\accum.obj \
$(OBJDIR)\alpha.obj \
$(OBJDIR)\alphabuf.obj \
$(OBJDIR)\glapi.obj \
$(OBJDIR)\glapinoop.obj \
$(OBJDIR)\attrib.obj \
$(OBJDIR)\bbox.obj \
$(OBJDIR)\bitmap.obj \
$(OBJDIR)\blend.obj \
$(OBJDIR)\clip.obj \
$(OBJDIR)\colortab.obj \
$(OBJDIR)\config.obj \
$(OBJDIR)\context.obj \
$(OBJDIR)\copypix.obj \
$(OBJDIR)\cva.obj \
$(OBJDIR)\depth.obj \
$(OBJDIR)\dlist.obj \
$(OBJDIR)\drawpix.obj \
$(OBJDIR)\enable.obj \
$(OBJDIR)\enums.obj \
$(OBJDIR)\eval.obj \
$(OBJDIR)\extensions.obj \
$(OBJDIR)\feedback.obj \
$(OBJDIR)\fog.obj \
$(OBJDIR)\get.obj \
$(OBJDIR)\hash.obj \
$(OBJDIR)\image.obj \
$(OBJDIR)\light.obj \
$(OBJDIR)\lines.obj \
$(OBJDIR)\logic.obj \
$(OBJDIR)\masking.obj \
$(OBJDIR)\matrix.obj \
$(OBJDIR)\mmath.obj \
$(OBJDIR)\glthread.obj \
$(OBJDIR)\os2thread.obj \
$(OBJDIR)\pb.obj \
$(OBJDIR)\pipeline.obj \
$(OBJDIR)\pixel.obj \
$(OBJDIR)\points.obj \
$(OBJDIR)\polygon.obj \
$(OBJDIR)\quads.obj \
$(OBJDIR)\rastpos.obj \
$(OBJDIR)\readpix.obj \
$(OBJDIR)\rect.obj \
$(OBJDIR)\scissor.obj \
$(OBJDIR)\shade.obj \
$(OBJDIR)\span.obj \
$(OBJDIR)\stages.obj \
$(OBJDIR)\stencil.obj \
$(OBJDIR)\teximage.obj \
$(OBJDIR)\texobj.obj \
$(OBJDIR)\texstate.obj \
$(OBJDIR)\texture.obj \
$(OBJDIR)\translate.obj \
$(OBJDIR)\triangle.obj \
$(OBJDIR)\aatriangle.obj \
$(OBJDIR)\varray.obj \
$(OBJDIR)\vb.obj \
$(OBJDIR)\vbcull.obj \
$(OBJDIR)\vbfill.obj \
$(OBJDIR)\vbindirect.obj \
$(OBJDIR)\vbrender.obj \
$(OBJDIR)\vbxform.obj \
$(OBJDIR)\vector.obj \
$(OBJDIR)\vertices.obj \
$(OBJDIR)\winpos.obj \
$(OBJDIR)\xform.obj \
$(OBJDIR)\zoom.obj \
$(OBJDIR)\osmesa.obj \
$(OBJDIR)\dispatch.obj \
$(OBJDIR)\imaging.obj \
$(OBJDIR)\state.obj \
$(OBJDIR)\hint.obj \
$(OBJDIR)\buffers.obj \
$(OBJDIR)\x86.obj \
$(OBJDIR)\x86a.obj \
$(OBJDIR)\common_x86.obj \
$(OBJDIR)\mmx_blend.obj \
$(OBJDIR)\vertex.obj


# Target name - name of the dll without extention and path.
TARGET = opengl32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\Glide\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    -4 $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/$(ODINCRT).lib
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\GDI32.LIB
$(PDWIN32_LIB)\USER32.LIB
$(PDWIN32_LIB)\glide2x.lib
$(RTLLIB_O)
OS2386.LIB
$(TARGET).def
<<keep


# Copy rule.
$(PDWIN32_BIN)\Glide\$(TARGET).dll: $(OBJDIR)\$(TARGET).dll
    $(CP) $** $@


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\..\glut -I..\GL -I.. -I$(PDWIN32_INCLUDE)\glide \
        *.c *.cpp *.h *.asm *.inc ..\*.c $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

