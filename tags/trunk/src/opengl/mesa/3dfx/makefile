# $Id: makefile,v 1.14 2001-10-01 01:40:57 bird Exp $

#
# Odin32 API
#
#       Mesa/3dxf Makefile - opengl.dll.
#


#
# Tell buildenvironment that we should not copy this into /bin.
# Tell buildenvironment to make addtional target in the all rule.
#
NO_MAIN_BIN_COPY = 1
ADDITIONAL_ALL = $(ODIN32_BIN)\readme.txt


#
# Compiler, tools, and interference rules.
#
!include ../../../../makefile.inc


#
# Change bin directory.
#
ODIN32_BIN = $(ODIN32_BIN__)\Glide


#
# Overrides.
#
CINCLUDES   = -I.. -I..\GL $(CINCLUDES) -I..\..\glut -I..\..\glide\cvg\glide \
              -I..\..\glide\swlibs\fxmisc -I..\..\glide\cvg\incsrc
CDEFINES    = $(CDEFINES_WIN32APP) \
              -DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DOS2_THREADS \
              -DUSE_X86_ASM -DUSE_MMX_ASM -DUSE_3DNOW_ASM_NOT \
              -DFX -DFX_V2 -DFX_SILENT -DBUILD_GL32 -DFAST_MATH

!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS      = $(CFLAGS) /Wcnd- /Tdp
!else
!ifdef WAT
C           = $(CXX)
CFLAGS      = $(CXXFLAGS)
!else
!error "Compiler is not yet supported."
!endif
!endif


#
# Interference rule for sources in the parent directory.
#   This is BAD! Try make library in parent directory? Problem with different objects.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1" || ("$(WAT)" == "1" && defined(NMAKE))
{..}.c{$(OBJDIR)}.obj:
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj $<

{..}.asm{$(OBJDIR)}.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $<
!else
!error "Compiler not supported yet!"
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
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
$(OBJDIR)\vertex.obj \
$(OBJDIR)\mem.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/glide2x.lib \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = opengl32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\*.c ..\*.asm ..\*.h ..\GL\*.h ..\..\glut\*.h ..\..\glide\cvg\glide\*.h \
        ..\..\glide\swlibs\fxmisc\*.h ..\..\glide\cvg\incsrc\*.h


#
# Copyrule for the readme.txt file.
#
$(ODIN32_BIN)\readme.txt: readme.txt
    $(CP) readme.txt $@

