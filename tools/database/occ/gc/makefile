# $Id: makefile,v 1.1.2.1 2001-09-20 03:07:54 bird Exp $


#
# Directory macro.
#
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Wanna make a library for .exe files.
#
LIBTARGET = 1
EXETARGET = 1


#
# include common definitions
#
!include ../../../../makefile.inc


#
# Addjustments and GC configuration.
#
CDEFINES = \
!ifdef DEBUG
    -DDEBUG \
!endif
    -DATOMIC_UNCOLLECTABLE \
    -DNO_SIGNALS \
    -DNO_EXECUTE_PERMISSION \
    -DALL_INTERIOR_POINTERS \
    -DSILENT \
    -DLARGE_CONFIG \
    -DOPERATOR_NEW_ARRAY \
    -DOLD_BLOCK_ALLOC \
    -DHBLKSIZE=1024
CFLAGS = $(CFLAGS) -Wpro-

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS=\
$(OBJDIR)\alloc.obj \
$(OBJDIR)\reclaim.obj \
$(OBJDIR)\allchblk.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\mach_dep.obj \
$(OBJDIR)\os_dep.obj \
$(OBJDIR)\mark_rts.obj \
$(OBJDIR)\headers.obj \
$(OBJDIR)\mark.obj \
$(OBJDIR)\obj_map.obj \
$(OBJDIR)\blacklst.obj \
$(OBJDIR)\finalize.obj \
$(OBJDIR)\new_hblk.obj \
$(OBJDIR)\dbg_mlc.obj \
$(OBJDIR)\malloc.obj \
$(OBJDIR)\stubborn.obj \
$(OBJDIR)\checksums.obj \
$(OBJDIR)\solaris_threads.obj \
$(OBJDIR)\irix_threads.obj \
$(OBJDIR)\linux_threads.obj \
$(OBJDIR)\typd_mlc.obj \
$(OBJDIR)\ptr_chck.obj \
$(OBJDIR)\mallocx.obj \
$(OBJDIR)\solaris_pthreads.obj


#
# Target name - name of the exe without extention and path.
#
TARGET = gc


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

