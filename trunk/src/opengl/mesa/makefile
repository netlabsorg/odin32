# $Id: makefile,v 1.8 2000-07-19 19:05:15 sandervl Exp $

#
# Odin32 API
#
# Makefile for Mesa OpenGL
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk

# Flag overloads and local macros.
CLEANEXTRAS = lz32rsrc.asm


MESADEFS = -DNO_PARALLEL -DNO_STEREO -DGL_MESA_window_pos -DOS2_THREADS \
           -DDIVE -DUSE_X86_ASM -DUSE_MMX_ASM -DUSE_3DNOW_ASM_NOT \
           -DFAST_MATH -Tdp
CFLAGS   = -I. -IGL $(CFLAGS_WIN32APP)   -I..\glut $(MESADEFS) /Wcnd-
CXXFLAGS = -I. -IGL $(CXXFLAGS_WIN32APP) -I..\glut $(MESADEFS) /Wcnd-


# Object files. All objects should be prefixed with $(OBJDIR)!
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


# Target name - name of the dll without extention and path.
TARGET = opengl32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
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
$(RTLLIB_O)
OS2386.LIB
MMPM2.LIB
$(TARGET).def
<<keep



# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -IGL \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

