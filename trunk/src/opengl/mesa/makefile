# $Id: makefile,v 1.10 2000-12-02 23:58:37 bird Exp $

#
# Odin32 API
#
#       Mesa Makefile - opengl.dll
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Overrides. We have to compile this as a WIN32APP.
#
CINCLUDES   = -I. -IGL $(CINCLUDES) -I..\glut
CDEFINES    = $(CDEFINES_WIN32APP) \
              -DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DOS2_THREADS \
              -DDIVE -DUSE_X86_ASM -DUSE_MMX_ASM -DUSE_3DNOW_ASM_NOT \
              -DFAST_MATH
CFLAGS      = $(CFLAGS_WIN32APP) /Wcnd- -Tdp
CXXFLAGS    = $(CFLAGS_WIN32APP) /Wcnd- -Tdp
!ifdef WAT
!error "Compiler not supported yet!"
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wgl.obj \
$(OBJDIR)\accum.obj \
$(OBJDIR)\alpha.obj \
$(OBJDIR)\alphabuf.obj \
$(OBJDIR)\dispatch.obj \
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
$(OBJDIR)\pb.obj \
$(OBJDIR)\pixel.obj \
$(OBJDIR)\pipeline.obj \
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
$(OBJDIR)\translate.obj \
$(OBJDIR)\triangle.obj \
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
$(OBJDIR)\x86.obj \
$(OBJDIR)\common_x86.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\texture.obj \
$(OBJDIR)\wmesa.obj \
$(OBJDIR)\x86a.obj \
$(OBJDIR)\vertex.obj \
$(OBJDIR)\mmx_blend.obj \
$(OBJDIR)\osmesa.obj \
$(OBJDIR)\mesadive.obj \
$(OBJDIR)\internal.obj \
$(OBJDIR)\mesarsrc.obj \
$(OBJDIR)\glapi.obj \
$(OBJDIR)\glapinoop.obj \
$(OBJDIR)\imaging.obj \
$(OBJDIR)\state.obj \
$(OBJDIR)\buffers.obj \
$(OBJDIR)\glthread.obj \
$(OBJDIR)\hint.obj \
$(OBJDIR)\mem.obj \
$(OBJDIR)\aatriangle.obj \
$(OBJDIR)\os2thread.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\gdi32.lib \
$(ODIN32_LIB)\user32.lib \
$(RTLLIB_O) \
os2386.lib \
mmpm2.lib


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
        .\gl\*.h ..\glut\*.h

